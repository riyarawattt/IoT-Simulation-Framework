#include "client.h"
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main()
{
    int server_fifo_fd, client_fifo_fd;
    struct data_to_pass_to_controller my_data;
    struct data_to_pass_to_device receiving_data;
    int read_res;
    char client_fifo[256];
    char *tmp_char_ptr;

    //relays message between controller.
    //connects clients controller
    //prints current state recieved from parent process
    my_data.client_pid = getpid();
    sprintf(my_data.deviceType, CLOUD);
    server_fifo_fd = open(CONTROLLER_FIFO_NAME, O_WRONLY);
    if (server_fifo_fd == -1){
        fprintf(stderr, "Error");
    }
    write(server_fifo_fd, &my_data, sizeof(my_data));

    mkfifo(CLOUD_FIFO_NAME, 0777);
    server_fifo_fd = open(CLOUD_FIFO_NAME, O_RDONLY);
    if (server_fifo_fd == -1) {
        fprintf(stderr, "Server fifo failure\n");
        exit(EXIT_FAILURE);
    }

    //when update recieved, sends to device to notify user
    //print statement 
    do {
        read_res = read(CLOUD_FIFO_NAME, &receiving_data, sizeof(receiving_data));
        if (read_res > 0) {
            printf("Data Recieved: %d, %s\n", receiving_data.client_pid, receiving_data.message);
            sprintf(client_fifo, CLIENT_FIFO_NAME, my_data.client_pid);
        }
    } while (read_res > 0);
    close(server_fifo_fd);
    unlink(CLOUD_FIFO_NAME);
    exit(EXIT_SUCCESS);
    
}

