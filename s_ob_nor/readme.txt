s_ob_nor
=====================
This project contains an example demonstrating
reading from and writing to the NOR flash on the OEM Base Board.

NOTE: The NOR flash API includes functions to write and lock the user
      security information on the chip but those function are not used
      in the example and have been commented out in the code to prevent
      accidental writing/locking.


The project makes use of code from the following library projects:
- Lib_CMSIS          : for CMSIS files relevant to LPC408x
- Lib_MCU        	 : for LPC408x peripheral driver files
- Lib_OemBaseBoard   : for Embedded Artists OEM Base Board peripheral drivers


These library projects must exist in the same workspace in order
for the project to successfully build.

