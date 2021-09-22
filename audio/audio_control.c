#include "audio_control.h"
#include "audio_speaker.h"
#include "audio_mic.h"

#include <usb/xmega_usb.h>
#include <usb/usb_standard.h>
#include <usb/descriptor.h>


#include <stdio.h>

audio_memory_t audio_mem;


void audio_init()
{
	for(uint8_t i = 0; i < AUDIO_BUFFER_INSTANCES; i++)
	{
		audio_mem.buffer[0].size = 0;
	}
	AUDIO_TIME_COUNTER.CNT = 0;
	AUDIO_TIME_COUNTER.PER = -1;
	AUDIO_TIME_COUNTER.CTRLA = TC_CLKSEL_DIV1024_gc;
	AUDIO_TIME_COUNTER.CTRLB = TC_WGMODE_NORMAL_gc;
	AUDIO_TIME_COUNTER.INTCTRLA = TC_OVFINTLVL_OFF_gc;

	audio_mem.mcu_clock = F_CPU;
	audio_mem.mcu_synced = false;

	audio_mic_init();
	audio_mem.speaker_sampleRate = 48000;
	audio_mem.speaker_teoriticalCLKsPerSample = (F_CPU/48000) - 1;


	audio_speaker_init();
	audio_mem.mic_sampleRate = 48000;
	audio_mem.mic_teoriticalCLKsPerSample = (F_CPU/48000) - 1;
}



bool audio_setup_out(void)
{
	switch (usb_mem.setup_pkg.bRequest)
	{
		/* USB_REQ_SetInterface handles the enable and disable of
		 * a Terminal. This is controlled by setting the alternative
		 * Interface. If the empty interface is selected the snd card
		 * is disabled...
		 */
		case USB_REQ_SetInterface:
			if(usb_mem.setup_pkg.bmRequestType == (REQDIR_HOSTTODEVICE | USB_REQTYPE_STANDARD | USB_RECIPIENT_INTERFACE))
			{
				//####### Compare the interface Number ############
				//####### Then get the alternative instruction ####
				switch(usb_mem.setup_pkg.wIndex)
				{
					
					
					
					case INTERFACE_ID_AudioStream_Speaker:
						if(usb_mem.setup_pkg.wValue)
						{
							audio_speaker_interface_enable();
						}
						else
						{
							audio_speaker_interface_disable();
						}
						break;
						
						
						
						
					case INTERFACE_ID_AudioStream_Mic:
						if(usb_mem.setup_pkg.wValue)
						{
							audio_mic_interface_enable();
						}
						else
						{
							audio_mic_interface_disable();
						}
						break;
		
					default:
						audio_unknown_setup(PSTR("if"));
						return false;
				}
				usb_ep0_in(0);
				usb_ep0_clear_setup();
				printf("set interface %x %x\n", usb_mem.setup_pkg.wIndex, usb_mem.setup_pkg.wValue);
				return true;
			}
			audio_unknown_setup(PSTR("if"));
			return false;
		case AUDIO_REQ_SetCurrent:
			if(usb_mem.setup_pkg.bmRequestType == (REQDIR_HOSTTODEVICE | USB_REQTYPE_CLASS | USB_RECIPIENT_ENDPOINT))
			{
// 				audio_mem.sampleRate = (((uint32_t)usb_mem.ep0_out_buf[sizeof(USB_setup_package)] << 16) | ((uint32_t)usb_mem.ep0_out_buf[sizeof(USB_setup_package) + 1] << 8) | (uint32_t)usb_mem.ep0_out_buf[sizeof(USB_setup_package) + 2]);
				usb_mem.callback.ep[0].out = &audio_out_setup_samplerate;
				usb_ep0_in(0);
				usb_ep0_clear_setup();

				return true;
			}
			audio_unknown_setup(PSTR("setSampleRate"));
			return false;
		case AUDIO_REQ_GetCurrent:
			if(usb_mem.setup_pkg.bmRequestType == (REQDIR_DEVICETOHOST | USB_REQTYPE_CLASS | USB_RECIPIENT_ENDPOINT))
			{
				uint8_t EndpointAddress = (uint8_t)usb_mem.setup_pkg.wIndex;
				uint8_t EndpointControl = (usb_mem.setup_pkg.wValue >> 8);

				/* Only handle GET CURRENT requests to the audio endpoint's sample frequency property */
				if(EndpointControl == AUDIO_EPCONTROL_SamplingFreq)
				{
					uint8_t *s_addr = (uint8_t *) &audio_mem.mic_sampleRate;
					if (EndpointAddress == AUDIO_STREAM_SPEAKER_EPADDR)
					{
						s_addr = (uint8_t *)&audio_mem.speaker_sampleRate;
					}

						/* Convert the sampling rate value into the 24-bit format the host expects for the property */
						usb_mem.ep0_in_buf[2] = s_addr[2];
						usb_mem.ep0_in_buf[1] = s_addr[1];
						usb_mem.ep0_in_buf[0] = s_addr[0];
						usb_ep0_clear_setup();
						usb_ep0_in(3);
						printf("rate sended %ld ep: %x\n", *(uint32_t *)s_addr, usb_mem.setup_pkg.wIndex);
						return true;
					}
			}
			audio_unknown_setup(PSTR("getCurrent"));
	}
	printf("audio out bm: %x b: %x\n", usb_mem.setup_pkg.bmRequestType, usb_mem.setup_pkg.bRequest);
	return false;
}

void audio_work(void)
{
	if(audio_mem.speaker_interface_enabled)
		audio_speaker_work();
}


//######################################### SETUP PACKAGES #############################################

void audio_out_setup_samplerate(USB_EP_t *ep)
{
	if(ep->CNT == 3) //Check size of the package
	{
		//convert size
		uint32_t sample_rate =  (((uint32_t)usb_mem.ep0_out_buf[2] << 16) | ((uint32_t)usb_mem.ep0_out_buf[1] << 8) | (uint32_t)usb_mem.ep0_out_buf[0]);
		
		
		//Mic or alternative speaker
		if(usb_mem.setup_pkg.wIndex == AUDIO_STREAM_MIC_EPADDR)
		{
			audio_mem.mic_sampleRate = sample_rate;
			audio_mem.mic_teoriticalCLKsPerSample = (audio_mem.mcu_clock/audio_mem.mic_sampleRate) - 1;
		}
		else if(usb_mem.setup_pkg.wIndex == AUDIO_STREAM_SPEAKER_EPADDR)
		{
			audio_mem.speaker_sampleRate = sample_rate;
			audio_mem.speaker_teoriticalCLKsPerSample = (audio_mem.mcu_clock/audio_mem.speaker_sampleRate) - 1;
			AUDIO_SPEAKER_DMA_SAMPLE_TIMER.PERBUF = audio_mem.speaker_teoriticalCLKsPerSample;
		}
		
		
		usb_ep0_clear_setup();
		printf("sampleRate: %ld %d ep: %X\n", sample_rate, audio_mem.speaker_teoriticalCLKsPerSample, usb_mem.setup_pkg.wIndex);
	}
	else
	{
		//TODO: activate this...
// 		usb_ep0_stall();
	}
// 	usb_mem.callback.ep[0].out = &audio_callback_out;
}

//######################################## CLOCK SYNCRONISATION ##########################################
static inline void sync_mcu_freq(uint16_t size, uint8_t time)
{
	
	static uint32_t clks_total;
	static uint32_t size_total;
	if(audio_mem.mcu_sync_counter < 2048)
	{
		audio_mem.mcu_sync_counter++;
		if(audio_mem.mcu_sync_counter < 512)
		{
			clks_total = 0;
			size_total = 0;
			return;
		}

		clks_total += time;
		size_total += size;

		if(audio_mem.mcu_sync_counter == 2048)
		{
			uint64_t frq = clks_total * 1024;
			frq = frq * (uint64_t)audio_mem.speaker_sampleRate;
			frq = frq / (uint64_t)(size_total >> 1);
			usb_mem.callback.ep[AUDIO_STREAM_SPEAKER_EPADDR].out = &audio_speaker_data_out0; //TODO: Dirty. Should be done by xmega_usb.c
			usb_mem.callback.ep[AUDIO_STREAM_SPEAKER_EPADDR].in = &audio_speaker_data_out1;
			audio_mem.mcu_clock = frq;
			//Setup speaker sample rate
			audio_mem.speaker_teoriticalCLKsPerSample = (audio_mem.mcu_clock/audio_mem.speaker_sampleRate) - 1;
			audio_mem.speaker_middleCLKsPerSample = 0;
			//Setup mic sample rate
			audio_mem.mic_teoriticalCLKsPerSample = (audio_mem.mcu_clock/audio_mem.mic_sampleRate) - 1;
			AUDIO_MIC_ADC_SAMPLE_TIMER.PERBUF = audio_mem.mic_teoriticalCLKsPerSample;
			audio_mem.mic_middleCLKsPerSample = 0;
			audio_mem.mcu_synced = true;
// 			uint16_t val = (((uint32_t)audio_mem.mcu_clock * 2) + 1000000UL*8)/(1000000UL*16) - 1; //TODO: Very dirty should be done somewhere but not here
// 			USARTC0.BAUDCTRLA = val & 0xFF;
// 			USARTC0.BAUDCTRLB = val >> 8; 
			printf("cpu freq is: %lu %lu, %u\n", (uint32_t)frq, size_total, time);
		}
	}
}

void audio_data_out_sync_clk0(USB_EP_t *ep)
{
	uint16_t size = ep->CNT;
	ep->CNT = 0;
	usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);

	static uint16_t old_time = 0;
	uint16_t new_time = AUDIO_TIME_COUNTER.CNT;
	uint16_t time = new_time - old_time;

	sync_mcu_freq(size, time);

	old_time = new_time;
}

void audio_data_out_sync_clk1(USB_EP_t *ep)
{
	uint16_t size = ep->CNT;
	ep->CNT = 0;
	usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);

	static uint16_t old_time = 0;
	uint16_t new_time = AUDIO_TIME_COUNTER.CNT;
	uint16_t time = new_time - old_time;

	sync_mcu_freq(size, time);

	old_time = new_time;
}

void audio_unknown_setup(const char *where)
{
	printf("Unk set pkg: %S\n", where);
}
