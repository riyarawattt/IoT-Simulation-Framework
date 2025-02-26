#include "client.h"
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include "controller.h"



// adding the pids 
int addPid(pid_t pid, pid_t *arrPid, int index)
{
  if (index > DEVICES){
    return 0;
  }
  *(arrPid + index) = pid;
  return 1;
}

// value returned if current value is greater then threshold value.
int ifValueExceedsThres(struct data_to_pass_to_controller sending_data)
{
  return sending_data.currentValue > sending_data.thresholdValue;
}


// sends a message to the specified device type and opens the fifo
void sendingMessage( pid_t pid_val1, char *message, char *deviceType)
{   
    int client_fifo_fd;
    char client_fifo[256];
    struct data_to_pass_to_device sent_data;
    sent_data.client_pid = getpid();
    sprintf(sent_data.message, message);


    if (!strcmp(deviceType, SENSOR)){
        sprintf(client_fifo, CLIENT_FIFO_NAME, pid_val1);
        client_fifo_fd = open(client_fifo, O_WRONLY);
    } else if (!strcmp(deviceType, CLOUD)){
        client_fifo_fd = open(CLOUD_FIFO_NAME, O_WRONLY);
    } else if (!strcmp(deviceType, ACTUATOR)){
        client_fifo_fd = open(CLOUD_FIFO_NAME, O_WRONLY);
    }

    printf("Sending Message %s to device type %s\n", message, deviceType);
    if (client_fifo_fd != -1) {
        fprintf(stderr, "Sorry, no server for device: %s\n", deviceType);
    }
    write(client_fifo_fd, &sent_data, sizeof(sent_data)); 
 
}

int valueFoundInArr(pid_t pid, pid_t *arrPid, int currentPid)
{
  for (int i = 0; i < currentPid; i++){
    if (arrPid[i] == pid){
      return 1;
    }
  }
  return 0;
}



// Returns 1 if threshold is exceeded, elese return 0.
int getData(struct data_to_pass_to_controller my_data, pid_t *arrPid, int *pidKnown, pid_t *actuPid, pid_t *cloudPid, int *num_sensors)
{
  if(!valueFoundInArr(my_data.client_pid, arrPid, *pidKnown)){
    printf("Data Received from new device!\nDevice Type: %s, Pid: %d\n", my_data.deviceType, my_data.client_pid);
    addPid(my_data.client_pid, arrPid, *pidKnown);
    *pidKnown += 1;

    if(!strcmp(my_data.deviceType, SENSOR)){
      printf("New sensor registered! thresholdValue: %d\n", my_data.thresholdValue);
      sendingMessage(my_data.deviceType, my_data.client_pid, GO);
      *num_sensors+=1;
    } else if(!strcmp(my_data.deviceType, ACTUATOR)){
      printf("Actuator registered!\n\n");
      *actuPid = my_data.client_pid;
    } else {
      printf("Cloud Registered!\n");
      *cloudPid = my_data.client_pid;
    }
    printf("Controller has knowledge of %d devices\n\n", *pidKnown);
    return -1;
  } else {
    printf("Data received from registered Sensor, Pid: %d\n", my_data.client_pid);
    printf("Value is %d, Threshold is %d\n", my_data.currentValue, my_data.thresholdValue);
  }

  return ifValueExceedsThres(my_data);
}


// Sends signal to the parent and kills the process
void sendSignalToParent(int sig)
{
  kill(getppid(), sig);
}


static int turnOn = 0;
void receiveSignal(int sig)
{
    turnOn = sig == SIGUSR1;
}

int main()
{
    int controller_fifo_fd, client_fifo_fd, actuator_fifo_fd;
    int read_res;
    int deviceCount = 0;
    int thresholdReached;
    int num_sensors = 0;
    char actuator_fifo[256];
   
	  pid_t knownPids[DEVICES];
    pid_t actuPid;
    pid_t cloudPid;
    pid_t forkPid;


  struct data_to_pass_to_controller receiving_data;   
  struct sigaction act;
	act.sa_handler = receiveSignal;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
		

  printf("Runs the Fork Program \n");
  forkPid = fork();

    switch(forkPid)
	{
	case -1:
		perror("fork failed");
		exit(1);
    //child process
	
    case 0:
        mkfifo(CONTROLLER_FIFO_NAME, 0777);
        controller_fifo_fd = open(CONTROLLER_FIFO_NAME, O_RDONLY);
        if (controller_fifo_fd == -1) {
            fprintf(stderr, "Server fifo failure\n");
            exit(EXIT_FAILURE);
        }
        
        sleep(5); /* lets clients queue for demo purposes */

        do {
            read_res = read(controller_fifo_fd, &receiving_data, sizeof(receiving_data));
            if (read_res > 0) {
            thresholdReached = getData(receiving_data, knownPids, &deviceCount, &actuPid, &cloudPid, &num_sensors);
            //if threshold reached is greater than 0 ,sends sensor.c "ACK" message... device starts operatons
            //if threshold is not negative, sends "STOP" message... device stops operation and exit
            
                if(thresholdReached > 0){
             printf("Threshold passed sending stop to device, pid: %d\n", receiving_data.client_pid);
             sendingMessage(receiving_data.deviceType, receiving_data.client_pid, STOP);
             printf("Sending OFF message to actuator, pid: %d\n\n", receiving_data.client_pid);
            sendingMessage(ACTUATOR, actuPid, STOP);
             sendSignalToParent(SIGUSR2);
                }else if(thresholdReached != -1){
            printf("Threshold not passed sending ACK to device %d\n\n", receiving_data.client_pid);
             sendingMessage(receiving_data.deviceType, receiving_data.client_pid, ACK);
                }
            }
         } while (read_res > 0);
         break;
    
    
    //parent process
		//recieves signal (ON or OFF)
	  // communicates with cloud -> sends current state
    default:
    //ready to receive the signal 
        sigaction(SIGUSR1, &act, 0);
        sigaction(SIGUSR2, &act, 0);
        
    for(int i = 0; i < SENSORS; i++) {
      pause();
      printf("Signal received from child.\nHandling...\n");
      struct data_to_pass_to_device sending_data;
      sending_data.client_pid = getpid();

      if(turnOn){
        printf("Sending ON to Cloud\n");
        sendingMessage(CLOUD, cloudPid, ON);
      } else{
        printf("Sending OFF to Cloud\n");
        sendingMessage(CLOUD, cloudPid, OFF);
      }
      printf("\n");
    }

    printf("The process of controller is performed, all devices are now paused\n");
    sendingMessage(CLOUD, cloudPid, STOP);
    sendingMessage(ACTUATOR, actuPid, STOP);
    
    break;
  }
  close(controller_fifo_fd);
  unlink(CONTROLLER_FIFO_NAME);
  exit(EXIT_SUCCESS);
}














