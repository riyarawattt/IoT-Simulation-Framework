# Emulation of Internet of Things (IoT) Using Software Processes

This project simulates a basic Internet of Things (IoT) system using software processes that communicate with each other. It consists of multiple components—**controller, actuator, cloud, and sensor**—which interact via **Inter-Process Communication (IPC)** mechanisms such as message queues and FIFOs.

The objective is to demonstrate how different IoT components work together in a system, mimicking real-world IoT device interactions.

## How to Run the Program

1. **Run the Shell Script**

   Execute the provided script to compile and set up the processes:

   
sh
   ./run_script.sh


2. **Start the Software Processes**

   Run each process in a separate terminal in the following order:

   
sh
   ./controller
   ./actuator
   ./cloud
   ./sensor <deviceType> <thresholdValue>
   ./sensor <deviceType> <thresholdValue>


   The sensor process can be executed multiple times with different parameters to simulate multiple IoT sensors.

This setup ensures proper communication between all components in the system.
 
