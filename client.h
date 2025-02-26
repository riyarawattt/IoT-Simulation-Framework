#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CONTROLLER_FIFO_NAME "/tmp/serv_fifo"
#define CLIENT_FIFO_NAME "/tmp/cli_%d_fifo"
#define CLOUD_FIFO_NAME "/tmp/cloud_fifo"
#define ACT_FIFO_NAME "tmp/act_fifo"


#define GO "GO"
#define STOP "STOP"
#define ACK "ACK"
#define OFF "OFF"
#define ON "ON"

#define SENSOR "SENSOR"
#define ACTUATOR "ACTUATOR"
#define CLOUD "CLOUD"

#define BUFFER_SIZE 20

struct data_to_pass_to_controller {
    pid_t  client_pid;
    char my_data[BUFFER_SIZE - 1];
    int currentValue;
	int thresholdValue;
    char deviceType[BUFFER_SIZE - 1];
};

struct data_to_pass_to_device {
    pid_t  client_pid;
    char   message[BUFFER_SIZE - 1];
};
