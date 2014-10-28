s_display_board
=====================
This project illustrates the use of the display (3.2", 4.3" and 7" are
supported). The application has four options, controlled with buttons
SW2, SW3, SW4 and SW5. Press SW3 to display fading bars of red, green,
and blue. Press SW4 to gradually have the display backlight get darker
and then brighter again. Press SW5 to calibrate the touch screen and 
then check the calibration by drawing on the surface.


The project makes use of code from the following library projects:
- Lib_CMSIS          : for CMSIS files relevant to LPC408x
- Lib_Displays       : for display support
- Lib_LPC            : for window support (swim)
- Lib_MCU        	 : for LPC408x peripheral driver files
- Lib_OemBaseBoard   : for Embedded Artists OEM Base Board peripheral drivers
- Lib_OemBoard       : for Embedded Artists OEM Board drivers


These library projects must exist in the same workspace in order
for the project to successfully build.

