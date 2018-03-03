# XMega usb Soundcard

## Requirements
* XMega µC with USB support
* 8KB ram (Currently configured with big buffer so 16KB without changing the configuration)(Should be possible to get it work with 4KB for lower bitrates <=41 KHz)
* An amplifier for the DAC outputs. An standard OPAMP should do it for the start.

## Features
* Stereo Sound
* Bitrates tested up to 96 KHz
* FIFO-USB Stackcalls
* Fully DMA support
* Clock calibration over time
* Endpoint size for ISONCHRONOUS endpoints up to 1023 bytes
* Acceptable sound Quality

## Limitations

### 12bit DAC Sample cut
The xmega has an 12bit DAC Only and we get 16bit data from the host. It is configured to ignore the 4 lsb. So if the signal is very quiet you will not hear anything.

### No hardware Volume control
Its not possible to control the Volume in hardware without additionaly external components. So if the audio volume is lowered we will lose resolution on the DAC.

## Instructions
### Clock
The clockrate should be set in the Makefile_cfg. Set it so precise as possible. Because if it drift to hard, it can happen that you get buffer over and underflows before the clock can be corrected from the clock calibration, this dont need to be a problem but can be a problem. I measure it on PORTC 7 set as CLK Output.

### UART
The UART C0 is configured to give some debug output at 1000000 baud. This is can be used for debugging. The standard out is redirected to a buffer and the buffer will be printed in "idle" in the main loop. This is done because else the output need to many time in interrupts. The main loop is not really idle. Because we get the signed audio samples from the host. And the every second byte needs to be touched for convert the data from signed into unsigned for making them compatible to the dac. This is also done in main loop. So we have not endless time there.

### 96 KHz
As mentioned in [Uart](#uart) the data are converted in the main loop. If you want to use Soundcard with 96 KHz you posible need to disable the outputs in the main loop. Else the convertion of the sample can be to slow and we get an buffer underflow for the dac. This high samplerates are only tested with overclocking to 60 MHz or more. Maybe it also works with 32 MHz or 48 Mhz.

### Devices with low memory
If you want to try to use a Device with small internal SRAM (4 KB). You can try to disable all printf outputs or change them to print_p(PSTR("string")). The strings are permanently in RAM for being faster.

## Thank you
**Thanks to Dean and mojo-chun**

Audio descriptor token from [LUFA][LUFALink]. Inspiration, some headers and some code snippets from the [Random Number Generator][RNGLink] from mojo-chun.

[LUFALink]:http://www.fourwalledcubicle.com/LUFA.php
[RNGLink]:https://github.com/kuro68k/xrng
