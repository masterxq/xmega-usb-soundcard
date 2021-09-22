#include "audio_mic.h"

#include "audio_control.h"

#include <Q/Qdef_types.h>

#include <usb/xmega_usb.h>
#include <cfg/usb_sound_cfg.h>

#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <stdio.h>



void audio_mic_interface_enable()
{
	printf("Mic en\n");
	ADCB.CTRLA |= ADC_ENABLE_bm; //| ADC_DMASEL_CH01_gc;
	AUDIO_MIC_ADC_SAMPLE_TIMER.PERBUF = audio_mem.mcu_clock/audio_mem.mic_sampleRate - 1;
}



void audio_mic_interface_disable()
{
	ADCB.CTRLA &= ~ADC_ENABLE_bm; //| ADC_DMASEL_CH01_gc;
}


void audio_mic_reset()
{
	printf("mic res\n");
	ep_config_isochronous(AUDIO_STREAM_MIC_EPADDR, audio_mem.mic_buffer0, audio_mem.mic_buffer1, AUDIO_STREAM_MIC_EPSIZE, &audio_mic_callback0, &audio_mic_callback0);
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & 0x7F].ep[0].CNT = 200;
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & 0x7F].ep[1].CNT = 200;
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & 0x7F].ep[0].STATUS = ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & 0x7F].ep[1].STATUS = ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
}

void audio_mic_callback0(USB_EP_t *ep)
{
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].DATAPTR = (uint16_t)&audio_mem.mic_buffer0;
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].AUXDATA = 0;
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].CNT = AUDIO_STREAM_EPSIZE;
// 
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_TRNCOMPL0_bm | USB_EP_OVF_bm);
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].DATAPTR = (uint16_t)&audio_mem.mic_buffer1;
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].AUXDATA = 0;
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].CNT = AUDIO_STREAM_EPSIZE;
// // 	printf("3:%X\n", usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].STATUS & (USB_EP_BUSNACK0_bm | USB_EP_TRNCOMPL0_bm | USB_EP_OVF_bm));
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].STATUS &= ~(USB_EP_BUSNACK1_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].STATUS &= ~(USB_EP_BUSNACK1_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
// 	ep->CNT = 200;
// 	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & 0x7F].ep[0].STATUS = ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
// 	usb_ep0_in(2);
// 	printf("!%x\n", usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].STATUS);
// 	printf("!%d\n",usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].CNT);
}



void audio_mic_adc_init()
{
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;  
	ADCB.CALL = pgm_read_byte(&PRODSIGNATURES_ADCBCAL0); 
  ADCB.CALH = pgm_read_byte(&PRODSIGNATURES_ADCBCAL1);

	NVM_CMD = NVM_CMD_NO_OPERATION_gc;

	
	//ADCB.CTRLA = ADC_DMASEL_CH01_gc; //For stereo
  ADCB.CTRLB = /*ADC_IMPMODE_bm |*/ ADC_CONMODE_bm | ADC_RESOLUTION_LEFT12BIT_gc; //ADC_FREERUN_bm
	ADCB.REFCTRL = ADC_REFSEL_INTVCC_gc;                                    //Reference Voltage
	ADCB.EVCTRL = ADC_SWEEP_0_gc | ADC_EVSEL_7_gc | ADC_EVACT_CH0_gc;       //You need to change this if you want stereo
	ADCB.PRESCALER = ADC_PRESCALER_DIV512_gc;                               //For 96kHz you maybe need faster conversion
  
	
	ADCB.CH0.CTRL = ADC_CH_INPUTMODE_DIFF_gc;
	ADCB.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN0_gc | ADC_CH_MUXNEG_PIN1_gc; //PB0 + PB1
	ADCB.CH0.INTCTRL = ADC_CH_INTLVL_OFF_gc;
}


void audio_mic_init()
{
	uint16_u target;
	//###################### Setup Mic ################################
	
	//###################### Mic Timer ################################
	AUDIO_MIC_ADC_SAMPLE_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
	AUDIO_MIC_ADC_SAMPLE_TIMER.CNT = 0;
	AUDIO_MIC_ADC_SAMPLE_TIMER.PER = 3000;       //Timerlimit
	AUDIO_MIC_ADC_SAMPLE_TIMER.CCA = 100;
	AUDIO_MIC_ADC_SAMPLE_TIMER.INTCTRLA = TC_OVFINTLVL_OFF_gc;
	AUDIO_MIC_ADC_SAMPLE_TIMER.CTRLB = TC_WGMODE_NORMAL_gc;
	
	//TODO: get this from my sample
	
	//Event channel for trigger the DMA
	EVSYS.CH2MUX                     = EVSYS_CHMUX_ADCB_CH0_gc;
	EVSYS.CH2CTRL                    = 0;
	
	//Event channel for trigger the ADC
	EVSYS.CH7MUX                     = EVSYS_CHMUX_TCC1_OVF_gc; //Route Timer through eventcontroller for clearing timer interrupt
	EVSYS.CH7CTRL                    = 0;

  audio_mic_adc_init();
	
// 	static uint16_t s0 = 0xf0f0;
// 	static uint16_t s1 = 0xAA55;
	uint16_u source;
// 	source.u16 = (uint16_t) &s0;
	source.u16 = (uint16_t) &ADCB.CH0RES;

	DMA.CTRL = DMA_ENABLE_bm | DMA_DBUFMODE_CH01_gc;

	DMA.CH2.CTRLA = DMA_CH_BURSTLEN_2BYTE_gc | DMA_CH_SINGLE_bm | DMA_CH_REPEAT_bm;
	DMA.CH2.CTRLB = DMA_CH_TRNINTLVL_MED_gc | DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;
	
	DMA.CH3.CTRLA = DMA_CH_BURSTLEN_2BYTE_gc | DMA_CH_SINGLE_bm | DMA_CH_REPEAT_bm;

	DMA.CH3.CTRLB = DMA_CH_TRNINTLVL_MED_gc | DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;
	
	DMA.CH2.TRIGSRC = DMA_CH_TRIGSRC_EVSYS_CH2_gc;
	DMA.CH3.TRIGSRC = DMA_CH_TRIGSRC_EVSYS_CH2_gc;

	target.u16 = (uint16_t) audio_mem.mic_buffer0;
	DMA.CH2.DESTADDR0 = target.u8[0];
	DMA.CH2.DESTADDR1 = target.u8[1];
	DMA.CH2.DESTADDR2 = 0;
	DMA.CH2.SRCADDR0 = source.u8[0];
	DMA.CH2.SRCADDR1 = source.u8[1];
  DMA.CH2.SRCADDR2 = 0;

// 	source.u16 = (uint16_t) &s1;
	target.u16 = (uint16_t) audio_mem.mic_buffer1;
	DMA.CH3.DESTADDR0 = target.u8[0];
	DMA.CH3.DESTADDR1 = target.u8[1];
	DMA.CH3.DESTADDR2 = 0;
	DMA.CH3.SRCADDR0 = source.u8[0];
	DMA.CH3.SRCADDR1 = source.u8[1];
  DMA.CH3.SRCADDR2 = 0;



	DMA.CH2.TRFCNT                   = AUDIO_STREAM_MIC_EPSIZE;
	DMA.CH2.REPCNT                   = 0;
	DMA.CH3.TRFCNT                   = AUDIO_STREAM_MIC_EPSIZE;
	DMA.CH3.REPCNT                   = 0;

	DMA.CH2.ADDRCTRL                 = DMA_CH_SRCRELOAD_BURST_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_TRANSACTION_gc | DMA_CH_DESTDIR_INC_gc;
	DMA.CH3.ADDRCTRL                 = DMA_CH_SRCRELOAD_BURST_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_TRANSACTION_gc | DMA_CH_DESTDIR_INC_gc;

	DMA.CH2.CTRLA                   |= DMA_CH_ENABLE_bm;
	audio_mic_reset();
}


ISR(DMA_CH2_vect)
{
	DMA.CH2.CTRLB |= DMA_CH_TRNIF_bm;
	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].DATAPTR = (uint16_t) &audio_mem.mic_buffer0;
	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].AUXDATA = 0;
	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].CNT = AUDIO_STREAM_MIC_EPSIZE;

// 	printf("2:%X\n", usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].STATUS & (USB_EP_BUSNACK0_bm | USB_EP_TRNCOMPL0_bm | USB_EP_OVF_bm));
	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_TRNCOMPL0_bm | USB_EP_OVF_bm | USB_EP_BUSNACK1_bm | USB_EP_TRNCOMPL1_bm);
}

ISR(DMA_CH3_vect)
{
	DMA.CH3.CTRLB |= DMA_CH_TRNIF_bm;
	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].DATAPTR = (uint16_t) &audio_mem.mic_buffer1;
	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].AUXDATA = 0;
	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].CNT = AUDIO_STREAM_MIC_EPSIZE;
// 	printf("3:%X\n", usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[0].STATUS & (USB_EP_BUSNACK0_bm | USB_EP_TRNCOMPL0_bm | USB_EP_OVF_bm));
	usb_mem.ep[AUDIO_STREAM_MIC_EPADDR & ~0x80].ep[1].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_TRNCOMPL0_bm | USB_EP_OVF_bm | USB_EP_BUSNACK1_bm | USB_EP_TRNCOMPL1_bm);
}
