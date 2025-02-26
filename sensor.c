#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include "client.h"
#include <ctype.h>




// checks if sensor is running
void stop(int *isRunning)
{
  *isRunning = 0;
  printf("Device has been Stopped\n");
}

// checks if the device type and the threshold value is specified or not.
int checkArguments(int argc, char *argv[])
{
  if (argc < 3){
    printf("Missing %d arguments\n", 3-argc);
    return 0;
  }
  int testThresholdValue;
  testThresholdValue = atoi(argv[2]);

  if(!testThresholdValue){
    printf("Invalid Second Argument for thresholdValue expected Number got Char\n");
    return 0;
  }
  return 1;
}



int main(int argc, char *argv[])
{
    if(!checkArguments(argc, argv)){
        return EXIT_FAILURE;

    }
    
    int controller_fifo_fd, client_fifo_fd;
    char *deviceName;
    int thresholdValue;
    char client_fifo[256];
    int running = 0;
    
    deviceName = malloc(strlen(argv[1]));
    strcpy(deviceName, argv[1]);
    thresholdValue = atoi(argv[2]);
    struct data_to_pass_to_controller sending_data;
    struct data_to_pass_to_device receiving_data;



    // Opening Controller Fifo
    controller_fifo_fd = open(CONTROLLER_FIFO_NAME, O_WRONLY);
    if (controller_fifo_fd == -1) {
        fprintf(stderr, "Sorry, no server\n");
        exit(EXIT_FAILURE);
    }

    // Writing to Server FIFO.
    sending_data.client_pid = getpid();
    sending_data.thresholdValue = thresholdValue;
    sending_data.currentValue = 0;
    sprintf(sending_data.deviceType, SENSOR);
    sprintf(sending_data.my_data, "Data sent from %d", sending_data.client_pid);
    write(controller_fifo_fd, &sending_data, sizeof(sending_data));

    // Making sensor FIFO

    sending_data.client_pid = getpid();
    sprintf(client_fifo, CLIENT_FIFO_NAME, sending_data.client_pid);
    if (mkfifo(client_fifo, 0777) == -1) {
        fprintf(stderr, "Sorry, can't make %s\n", client_fifo);
        exit(EXIT_FAILURE);
    }

// For each of the five loops, the client data is sent to the server.
// Then the client FIFO is opened (read-only, blocking mode) and the data read back.
// Finally, the server FIFO is closed and the client FIFO removed from memory.

    
    // Reading your own FIFO
    client_fifo_fd = open(client_fifo, O_RDONLY);
    if (client_fifo_fd != -1) {
    	if (read(client_fifo_fd, &receiving_data, sizeof(receiving_data)) > 0) {
            if (!strcmp(receiving_data.message, "GO")){
                running = 1;
                //Set is Running True/Start Run the loop
            }
            running = 1;

             printf("received: %s\n", receiving_data.message);
        }
        close(client_fifo_fd);
    }
    
    //Sending the data  
    while(running){
        sending_data.currentValue = rand () % 50;
        printf("Sending Data to Controller: currentValue %d,thresholdValue %d\n", 
        sending_data.currentValue, sending_data.thresholdValue);
        write(controller_fifo_fd, &sending_data, sizeof(sending_data));
        read(client_fifo_fd, &receiving_data, sizeof(receiving_data));
        printf("Received DATA: %s\n", receiving_data.message);
        if (!strcmp(receiving_data.message, STOP)) running = 0;
        

    }
  
}




