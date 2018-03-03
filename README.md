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
* Special and very fast implementation for the USB-Stack

## Limitations

### 12bit DAC Sample cut
The xmega has an 12bit DAC Only and we get 16bit data from the host. It is configured to ignore the 4 lsb. So if the signal is very quiet you will not hear anything.

### No hardware Volume control
Its not possible to control the Volume in hardware without additionaly external components. So if the audio volume is lowered we will lose resolution on the DAC.

## Instructions
### Clock
The clockrate should be set in the Makefile_cfg. Set it so precise as possible. Because if it drift to hard, it can happen that you get buffer over and underflows before the clock can be corrected from the clock calibration, this dont need to be a problem but can be a problem. I measure it on PORTC 7 set as CLK Output. Remember the USB start of frame are calibrate this clock. So the device needs to be connected to USB for getting the correct clockrate.

### UART
The UART C0 is configured to give some debug output at 1000000 baud. This is can be used for debugging. The standard out is redirected to a buffer and the buffer will be printed in "idle" in the main loop. This is done because else the output need to many time in interrupts. The main loop is not really idle. Because we get the signed audio samples from the host. And the every second byte needs to be touched for convert the data from signed into unsigned for making them compatible to the dac. This is also done in main loop. So we have not endless time there.

### 96 KHz
As mentioned in [Uart](#uart) the data are converted in the main loop. If you want to use Soundcard with 96 KHz you posible need to disable the outputs in the main loop. Else the convertion of the sample can be to slow and we get an buffer underflow for the dac. This high samplerates are only tested with overclocking to 60 MHz or more. Maybe it also works with 32 MHz or 48 Mhz.

### Devices with low memory
If you want to try to use a Device with small internal SRAM (4 KB). You can try to disable all printf outputs or change them to print_p(PSTR("string")). The strings are permanently in RAM for being faster. Settings about the buffer can directly made in audio.h for very small buffers there is possibly an adjustment of the audio.c necessary.

### Striped down to minimum
For make it easy to use the includes i normaly use are striped down to a minimum from a large avr library i wrote. So if something looks unnessesary it possibly a remaining of the complete library. This means it is absolutely possible that it is really unnessesary for this use case.

### build
On linux the build of the Firmware should be done with make after installing the avr lib and compiler. It can be programmed with make program. If you write your own Makefile for it take attention the compiler flag -fno-jump-tables is sadly needed for some sections of the code. The audio controller is untestet on Windows. If you testes it drop me a message please.

## Thank you
**Thanks to Dean and mojo-chun**

Audio descriptor token from [LUFA][LUFALink]. Inspiration, some headers and some code snippets from the [Random Number Generator][RNGLink] from mojo-chun.

[LUFALink]:http://www.fourwalledcubicle.com/LUFA.php
[RNGLink]:https://github.com/kuro68k/xrng

## Help needed
I need a makefile for windows or whatever needed there. I need somebody who can test this on windows. If you like do that and if you like to share your work I would be very thankfully and share you it here for everybody.
