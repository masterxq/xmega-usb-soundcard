#ifndef __USB_SOUND_CFG_H__
#define __USB_SOUND_CFG_H__ 1


#define AUDIO_STREAM_EPADDR   1            //For this application it should stay 1
#define AUDIO_BUFFER_INSTANCES 16          //min 4 next 8... The needed buffersize is AUDIO_BUFFER_INSTANCES * AUDIO_STREAM_EPSIZE
#define AUDIO_BUFFER_INSTANCES_MASK 0b1111 //4 = 0b11, 8 = 0b111, 16 = 0b1111... It will be used instead of modulo (%)
#define AUDIO_WISHED_OFFSET 7              /* The software will try to output the buffer with an offset of 8. 
                                            * If buffer 2 just received buffer 10 will be put on the dac.
                                            * To avoid over and underflows this this allways be something
                                            * like AUDIO_BUFFER_INSTANCES/2.
                                            */
#define AUDIO_STREAM_EPSIZE 256            //Size of each buffer

#define AUDIO_DMA_SAMPLE_TIMER TCC0        //If this is changed evsys needs to be changed manually.
#define AUDIO_TIME_COUNTER TCD0            //This timer is used for calibrating the clock.

#define UART_BAUD 1000000                  //This should be choisen as high as possible. Else it will need too much time.


#endif
