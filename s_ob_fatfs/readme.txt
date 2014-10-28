s_ob_fatfs
=====================
This project contains an example demonstrating
the use of the SD/MMC interface. Code from the Open Source
FatFs module has been used.

http://elm-chan.org/fsw/ff/00index_e.html

NOTE: This project currently doesn't compile with GNU/CodeRed tools.
The Lib_MCU/readfifo.s must be ported to the GNU compiler.

A successful run will look like this:

   FatFS Demo
   Opening '/'
   Listing content of '/'
   IP.TXT
   TESTFILE.TXT
   APPEXT.BIN

   Opening a file
   Reading content of the file
   192.168.5.236
   Closing file

   Opening a file for writing
   Writing  to the file
   Closing file


The project makes use of code from the following library projects:
- Lib_CMSIS          : for CMSIS files relevant to LPC408x
- Lib_Displays       : for display support
- Lib_MCU        	 : for LPC408x peripheral driver files
- Lib_OemBaseBoard   : for Embedded Artists OEM Base Board peripheral drivers


These library projects must exist in the same workspace in order
for the project to successfully build.

