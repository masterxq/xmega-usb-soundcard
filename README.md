# XMega USB sound card

## Requirements
* XMega µC with USB support
* 8KB ram (Currently configured with buffer 4KB buffer so 8KB should work)(Should be possible to get it to work with 4KB for lower bitrates <=41 KHz)
* An amplifier for the DAC outputs. A standard OPAMP should do it for the start.

## Features
* Stereo Sound
* Bitrates tested up to 96 KHz
* FIFO-USB Stackcalls
* Fully DMA support
* Clock calibration on start
* Clock calibration over time
* Endpoint size for ISONCHRONOUS endpoints up to 1023 bytes
* Acceptable sound quality
* Special and very fast implementation for the USB-Stack
* Precompiled hex files provided

## Limitations

### 12bit DAC Sample cut
The xmega has a 12bit DAC Only and we get 16bit data from the host. It is configured to ignore the 4 lsb.

### No hardware Volume control
It is not possible to control the volume in hardware without additional external components. So if the audio volume is lowered we will lose resolution on the DAC.

## Instructions
### Clock
The clockrate will be auto calibrated against the host. It will take a few seconds before calibration is finished and the audio playback starts. I measure it on PORTC 7 set as CLK Output. Remember that the USB start of frame calibrates this clock.

### UART
The UART C0 is configured to give some debug outputs at 1000000 baud. This can be used for debugging. The standard out is redirected to a buffer and the buffer will be printed in "idle" in the main loop. This is done, because else the output needs too much time in the interrupts. The main loop is not really idle, because we get the signed audio samples from the host. And every second byte needs to be touched to convert the data from signed into unsigned to make them compatible to the DAC. This is also done in main loop. So we do not have endless time there.

### 96 KHz
As mentioned in [Uart](#uart) the data is converted in the main loop. If you want to use a soundcard with 96 KHz you possibly need to disable the outputs in the main loop. Else, the convertion of the sample can be too slow and we get a buffer underflow for the dac. These high sample rates are only tested with overclocking to 60 MHz or more. Maybe it also works with 32 MHz or 48 Mhz.

### Devices with low memory
If you want to try to use a device with small internal SRAM (4 KB), you can try to disable all printf outputs or change them to print_p(PSTR("string")). The strings are in RAM permanently to assure improved processing speed. Settings about the buffer can be made in usb_sound_cfh.h. For very small buffers. Possibly, an adjustment of the audio.c necessary.

### Stripped down too the minimum
For make it easy to use, the includes i normaly use are stripped down to a minimum from a large avr library i wrote. So if something looks unnessesary it possibly a remnant of the complete library. It is absolutely possible that it is really unnessesary for this use case.

### build
On linux, the build of the Firmware should be done with _make_ after installing the avr lib and compiler. It can be programmed with _make program_. If you write your own Makefile for it pay attention the compiler flag _-fno-jump-tables_. Is sadly needed for some sections of the code.

### Outputs
* PORTC 7 is used for CPU CLK out.
* PORTB 2+3 Stereo Out.
* PORTC 2+3 USARTC0 RX,TX.

## Thank you
**Thanks to Dean and mojo-chun**

Audio descriptor taken from [LUFA][LUFALink]. Inspiration, some headers and some code snippets from the [Random Number Generator][RNGLink] by mojo-chun.

[LUFALink]:http://www.fourwalledcubicle.com/LUFA.php
[RNGLink]:https://github.com/kuro68k/xrng

## Help needed
I need a makefile for Windows or whatever is needed there and I need somebody who can test this on Windows. If you'd like to do that and share your work I would be very thankful and I could share it here.
