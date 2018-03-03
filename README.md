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

### 12bit DAC
#### Sample cut
The xmega has an 12bit DAC Only and we get 16bit data from the host. It is configured to ignore the 4 lsb. So if the signal is very quiet you will not hear anything.

#### No hardware Volume control
Its not possible to control the Volume in hardware without additionaly external components. So if the audio volume is lowered we will lose resolution on the DAC.

Thanks to Dean and mojo-chun.
Audio descriptor token from [LUFA][LUFALink]. Inspiration, some headers and some code snippets from the [Random Number Generator][RNGLink] from mojo-chun.

[LUFALink]:http://www.fourwalledcubicle.com/LUFA.php
[RNGLink]:https://github.com/kuro68k/xrng

Thank you!
