s_FreeRTOS_Demo
=====================
This project contains a simple example for the operating system
FreeRTOS. 

The program has two tasks: Rx and Tx. Tx will send a message to
Rx every 500ms. Each time Rx receives a message it will toggle
the state of two LEDs. 


The project makes use of code from the following library projects:
- Lib_CMSIS          : for CMSIS files relevant to LPC408x
- Lib_Lib_FreeRTOS   : FreeRTOS library


These library projects must exist in the same workspace in order
for the project to successfully build.

