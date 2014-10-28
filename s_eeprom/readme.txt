s_eeprom
=====================
This project contains a an example reading from and writing
to an I2C attached eeprom memory. The example support both
the eeprom on the OEM Board and the eeprom on the OEM Base 
Board.

To select which EEPROM is to be tested modify the USE_OEM_EEPROM
define in main.c.

A successful run should result in these printouts on the UART:

EEPROM: Data written
EEPROM: Reading
EEPROM: Verifing
EEPROM: OK


The project makes use of code from the following library projects:
- Lib_CMSIS          : for CMSIS files relevant to LPC408x
- Lib_MCU        	 : for LPC408x peripheral driver files
- Lib_OemBaseBoard   : for Embedded Artists OEM Base Board peripheral drivers
- Lib_OemBoard       : for Embedded Artists OEM Board drivers


These library projects must exist in the same workspace in order
for the project to successfully build.

