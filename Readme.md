# Emulation of Internet of Things(IoT) Using Software Processes

The project imitates a simple Internet of Things (IoT) system. It uses various software processes that communicate with each other to simulate the basic behavior of IoT devices and systems. This system consists of multiple processes (controller, actuator, cloud, and sensor) that interact via Inter-Process Communication (IPC) mechanisms such as message queues and FIFOs. The goal of this project is to demonstrate how different IoT components (controller, actuator, cloud, and sensor devices) can communicate and function together.

## How to Run the Program

### Run the Shell Script found in this directory:

<code>./run_script.sh</code>

Run the software processes given below in individual terminals:
Make sure that the c files run in the following order:

<code>./controller</code>
<code>./actuator</code>
<code>./cloud</code>
<code>./sensor <deviceType\> <thresholdValue\> <code>
<code>./sensor <deviceType\> <thresholdValue\> <code>

The code will be compiled using the script file provided.
