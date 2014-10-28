s_nand
=====================
This project contains an example reading from and 
writing to the NAND flash on the OEM Board

The program takes a while to complete, but the printouts should look like these:

   Checking blocks
   DONE

   Erasing...
   Writing...
   Reading...
   Verifing...
   Done


The project makes use of code from the following library projects:
- Lib_CMSIS          : for CMSIS files relevant to LPC178x
- Lib_MCU        	 : for LPC408x peripheral driver files
- Lib_OemBaseBoard   : for Embedded Artists OEM Base Board peripheral drivers
- Lib_OemBoard       : for Embedded Artists OEM Board drivers


These library projects must exist in the same workspace in order
for the project to successfully build.

