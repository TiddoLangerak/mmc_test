s_ob_audio_i2s
=====================
This project contains a an example using the UDA1380 audio codec
and the I2S to play an audio file. The audio will be output
on the audio output connectors on the OEM Base Board.

This is the same code as NXP's I2S_Audio example with 
changed audio data.

http://sw.lpcware.com


The project makes use of code from the following library projects:
- Lib_CMSIS		 : for CMSIS files relevant to LPC178x
- Lib_MCU        	 : for LPC178x peripheral driver files
- Lib_OemBaseBoard   : for Embedded Artists OEM Base Board peripheral drivers


These library projects must exist in the same workspace in order
for the project to successfully build.

