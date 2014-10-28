s_display_exp_board
===================
This project illustrates the use of the display expansion board. 
The application has four options, controlled with buttons
SW2, SW3, and SW4. Press SW2 to show the menu. Press SW3 to display 
fading bars of red, green, and blue. Press SW4 to rotate between the
different resolutions (VGA, SVGA, 480p and 576p).


The project makes use of code from the following library projects:
- Lib_CMSIS          : for CMSIS files relevant to LPC178x
- Lib_Displays       : for display support
- Lib_LPC            : for window support (swim)
- Lib_MCU        	 : for LPC178x peripheral driver files
- Lib_OemBaseBoard   : for Embedded Artists OEM Base Board peripheral drivers
- Lib_OemBoard       : for Embedded Artists OEM Board drivers


These library projects must exist in the same workspace in order
for the project to successfully build.

