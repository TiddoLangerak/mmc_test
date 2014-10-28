ea_demo
=======
This project contains a set of features to show of the LPC4088
Oem Board when used with the Oem Base Board. If a display is
attached then that will be used as well.

The project makes use of code from the following library projects:
- Lib_CMSIS          : for CMSIS files relevant to LPC408x
- Lib_Displays       : for display support
- Lib_LPC            : for heap and window support (swim)
- Lib_MCU        	 : for LPC408x peripheral driver files
- Lib_OemBaseBoard   : for Embedded Artists OEM Base Board peripheral drivers
- Lib_OemBoard       : for Embedded Artists OEM Board drivers

These library projects must exist in the same workspace in order
for the project to successfully build.


=======
The pre flashed demo application shows off a number of techniques:

1) The accelerometer and memory mapped registers. By tilting the board 
   the readout from the accelerometer changes and that is reflected on
   the list of LEDs (LED16 to LED1)
2) Timers, buttons and the PCA9532 port expander. 8 LEDS are available through 
   the port expander and starts of as a pattern of running lights. As the
   buttons (also on the port expander) are pressed the displayed pattern
   will change.
3) Joystick and GPIO. The 5-key joystick controls the running lights. 
   UP increases the speed, CENTER returns it to normal and DOWN decreases
   the speed. LEFT changes to the previous pattern for the running lights
   and RIGHT changes to the next pattern.
4) Speaker, DAC and DMA. The melody played at startup demonstrates the use
   of DAC and DMA to output a sinus wave at different frequencies.
5) A message is printed on the UART. It will look like this:
 
			***************************************************
			*                                                 *
			* Demo Application for the LPC4088 OEM Board...   *
			* (C) Embedded Artists AB 2001-2012               *
			*                                                 *
			*                                                 *
			*     Thank you for buying Embedded Artists'      *
			*     LPC4088 Developer's Kit                     *
			*                                                 *
			*     ... and congratulations on your choice      *
			*     of microcontroller!                         *
			*                                                 *
			***************************************************
			Version: 1.0, (build Mar 13 2012) 

6) The program will detect if a display is connected on the LCD Expansion
   Connector and display an image on it.

          Image will be the one found in the src/media/ folder

				 