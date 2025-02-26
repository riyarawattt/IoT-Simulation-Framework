#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "client.h"
#include <ctype.h>

// sends appropriate messages which are ON, OFF, STOP when required.
void handleMessage (char *message)
{
  if (!strcmp(message, OFF)){
    printf("OFF Command received!\nHandling...\n\n");
  }else if(!strcmp(message, ON)){
    printf("ON Command received!\nHandling...\n\n");
  }else {
    printf("STOP Command received!\nTurning off Actuator...\n");
  }
}

int main ()
{
  int controller_fifo_fd, act_fifo_fd;
  int read_res;
  char act_fifo[256];
  
  // Opening Server FIFO
  controller_fifo_fd = open(CONTROLLER_FIFO_NAME, O_WRONLY);
  if (controller_fifo_fd == -1) {
	  fprintf(stderr, "Sorry, no server\n");
	  exit(EXIT_FAILURE);
  }
  
  // Writing to Server FIFO
  struct data_to_pass_to_controller sending_data;
  sending_data.client_pid = getpid();
  sprintf(sending_data.deviceType, ACTUATOR);
  sprintf(sending_data.my_data, "Data sent from %d", sending_data.client_pid);
  write(controller_fifo_fd, &sending_data, sizeof(sending_data));

  // Creating Actuator FIFO
  mkfifo(ACT_FIFO_NAME, 0777);
  
  // Reading Actuator FIFO
 struct data_to_pass_to_device receiving_data;
  
  act_fifo_fd = open(ACT_FIFO_NAME, O_RDONLY);

  if (act_fifo_fd == -1) {
	  fprintf(stderr, "Sorry, no act fifo\n");
	  exit(EXIT_FAILURE);
  }

  do {
    read_res = read(act_fifo_fd, &receiving_data, sizeof(receiving_data));
    if (read_res > 0) {
      printf("Message received from Controller pid: %d\n", receiving_data.client_pid);
      handleMessage(receiving_data.message);
    }
  } while (strcmp(receiving_data.message, STOP));

  close(act_fifo_fd);
  unlink(act_fifo);
  exit(EXIT_SUCCESS);
}
