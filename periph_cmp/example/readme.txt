Comparator example

Example description
The CMP example demonstrates using the analog comparator.

This example configures the positive voltage input as the voltage
found on P1.7, which is trimmed through the potentiometer.  Adjust the
POT up and down to adjust the voltage into the analog comparator. When the
voltage crosses the negative voltage input, a CMP IRQ bit is set.  Based
on which side of the voltage is in reference to the bandgap, the LED state
will change.

Special connection requirements
Connect P0.25 (J5.24) to P1.7 (J5.29) to input the comparator 0

Build procedures:
Visit the LPCOpen quickstart guides at link "http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides"
to get started building LPCOpen projects.

