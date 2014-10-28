s_memreg
=====================
This project contains a an example using the memory mapped
register. Part of the register is mapped onto a few LEDs on
the OEM Base Board which are blinking in the 0x55AA and 0xAA55
patterns.


The project makes use of code from the following library projects:
- Lib_CMSIS          : for CMSIS files relevant to LPC408x
- Lib_OemBaseBoard   : for Embedded Artists OEM Base Board peripheral drivers


These library projects must exist in the same workspace in order
for the project to successfully build.

