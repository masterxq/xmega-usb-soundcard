#include "audio_speaker.h"
#include "audio_control.h"

#include <usb/xmega_usb.h>
#include <Q/Qdef_types.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <stdio.h>


void audio_speaker_interface_enable()
{
	audio_speaker_reset();
	audio_mem.speaker_interface_enabled = true;
	printf("Spk en\n");
}


void audio_speaker_interface_disable()
{
	audio_mem.speaker_interface_enabled = false;
	DMA.CH0.CTRLA &= ~DMA_CH_ENABLE_bm;
	DMA.CH1.CTRLA &= ~DMA_CH_ENABLE_bm;
}



void audio_speaker_reset()
{
	audio_mem.mcu_sync_counter = 0;
	audio_mem.last_completed_bank = AUDIO_BUFFER_INSTANCES - 1;
	audio_mem.last_converted_bank = AUDIO_BUFFER_INSTANCES - 1;
	audio_mem.last_free_bank = AUDIO_BUFFER_INSTANCES - 1;
// 	audio_mem.dac_stopped = true;
	audio_mem.speaker_middleCLKsPerSample = 0;
	audio_mem._bank_in_work0 = 0;
	audio_mem._bank_in_work1 = 1;
	if(audio_mem.mcu_synced)
		ep_config_isochronous(AUDIO_STREAM_SPEAKER_EPADDR, audio_mem.buffer[0].audio_buffer, audio_mem.buffer[1].audio_buffer, AUDIO_STREAM_SPEAKER_EPSIZE, &audio_speaker_data_out0, &audio_speaker_data_out1);
	else
		ep_config_isochronous(AUDIO_STREAM_SPEAKER_EPADDR, audio_mem.buffer[0].audio_buffer, audio_mem.buffer[1].audio_buffer, AUDIO_STREAM_SPEAKER_EPSIZE, &audio_data_out_sync_clk0, &audio_data_out_sync_clk1);
}

void audio_speaker_init()
{
	//########################################################
	//################## Setup Speakers ######################
	//############### Speaker Event Channel ##################
	EVSYS.CH0MUX                     = EVSYS_CHMUX_TCC0_OVF_gc; //Route Timer through eventcontroller for clearing timer interrupt
	EVSYS.CH4MUX                     = EVSYS_CHMUX_TCC0_CCA_gc;
	EVSYS.CH0CTRL                    = 0;
	EVSYS.CH4CTRL                    = 0;
	//################## Speaker Output ######################
	DACB.CTRLA = DAC_CH0EN_bm | DAC_CH1EN_bm;
	DACB.CTRLB = DAC_CHSEL_DUAL_gc | DAC_CH0TRIG_bm | DAC_CH1TRIG_bm;
	DACB.CTRLC = DAC_REFSEL_INT1V_gc | DAC_LEFTADJ_bm;
	DACB.CTRLA |= DAC_ENABLE_bm;
	DACB.EVCTRL = DAC_EVSEL_4_gc;
	
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;  
	DACB.CH0GAINCAL = pgm_read_byte(&PRODSIGNATURES_DACB0GAINCAL);
	DACB.CH0OFFSETCAL= pgm_read_byte(&PRODSIGNATURES_DACB0OFFCAL);
	DACB.CH1GAINCAL = pgm_read_byte(&PRODSIGNATURES_DACB1GAINCAL);
	DACB.CH1OFFSETCAL= pgm_read_byte(&PRODSIGNATURES_DACB1OFFCAL);
  ADCB.CALH = pgm_read_byte(&PRODSIGNATURES_ADCBCAL1);
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;
	
	//############### Speaker sample timer ###################
	AUDIO_SPEAKER_DMA_SAMPLE_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
	AUDIO_SPEAKER_DMA_SAMPLE_TIMER.CNT = 0;
	AUDIO_SPEAKER_DMA_SAMPLE_TIMER.PER = 3000;       //Timerlimit
	AUDIO_SPEAKER_DMA_SAMPLE_TIMER.CCA = 60;
	AUDIO_SPEAKER_DMA_SAMPLE_TIMER.INTCTRLA = TC_OVFINTLVL_OFF_gc;
	AUDIO_SPEAKER_DMA_SAMPLE_TIMER.CTRLB = TC_WGMODE_NORMAL_gc;
	
	uint16_u target;
	target.u16 = ((uint16_t) &DACB.CH0DATA);

	//############### Speaker DMA Transfer ###################
	DMA.CTRL = DMA_ENABLE_bm | DMA_DBUFMODE_CH01CH23_gc; //Enable both mic and speaker

	DMA.CH0.CTRLA = DMA_CH_BURSTLEN_4BYTE_gc | DMA_CH_SINGLE_bm | DMA_CH_REPEAT_bm;
	DMA.CH0.CTRLB = DMA_CH_TRNINTLVL_MED_gc | DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;
	
	DMA.CH1.CTRLA = DMA_CH_BURSTLEN_4BYTE_gc | DMA_CH_SINGLE_bm | DMA_CH_REPEAT_bm;
	
	
	DMA.CH1.CTRLB = DMA_CH_TRNINTLVL_MED_gc | DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;
	
	DMA.CH0.TRIGSRC = DMA_CH_TRIGSRC_EVSYS_CH0_gc;
	DMA.CH1.TRIGSRC = DMA_CH_TRIGSRC_EVSYS_CH0_gc;
	
	DMA.CH0.DESTADDR0 = target.u8[0];
	DMA.CH0.DESTADDR1 = target.u8[1];
	DMA.CH0.DESTADDR2 = 0;

  DMA.CH0.SRCADDR2 = 0;

	DMA.CH1.DESTADDR0 = target.u8[0];
	DMA.CH1.DESTADDR1 = target.u8[1];
	DMA.CH1.DESTADDR2 = 0;
  DMA.CH1.SRCADDR2 = 0;
	

	DMA.CH0.TRFCNT                   = 64; //This should not be nesseary it will be overwritten.
	DMA.CH0.REPCNT                   = 0;
	DMA.CH1.TRFCNT                   = 64;
	DMA.CH1.REPCNT                   = 0;

	DMA.CH0.ADDRCTRL                 = DMA_CH_SRCRELOAD_TRANSACTION_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_BURST_gc | DMA_CH_DESTDIR_INC_gc;
	DMA.CH1.ADDRCTRL                 = DMA_CH_SRCRELOAD_TRANSACTION_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_BURST_gc | DMA_CH_DESTDIR_INC_gc;

	audio_speaker_reset();
	audio_mem.speaker_interface_enabled = false;
}


void audio_speaker_start_dac_dma(void)
{
	DMA.CH0.CTRLB |= DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;
	DMA.CH1.CTRLB |= DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;
	DMA.INTFLAGS = DMA_CH0ERRIF_bm | DMA_CH1ERRIF_bm | DMA_CH0TRNIF_bm | DMA_CH1TRNIF_bm;
	AUDIO_SPEAKER_DMA_SAMPLE_TIMER.CNT = 0;
	DMA.CH0.CTRLA |= DMA_ENABLE_bm;
	DMA.CTRL |= DMA_ENABLE_bm;
#ifdef DEBUG_SYSTEM
	printf("dac st\n");
#endif
}


 void audio_speaker_setup_dma_ch(DMA_CH_t *channel, uint8_t buf_num)
{
	uint16_u source_addr;
	source_addr.u16 = (uint16_t) audio_mem.buffer[buf_num].audio_buffer;
	channel->TRFCNT = audio_mem.buffer[buf_num].size; //divide with for because 16 bit stereo == 4 bytes per trigger
	channel->SRCADDR0 = source_addr.u8[0];
	channel->SRCADDR1 = source_addr.u8[1];
	channel->CTRLB |= DMA_CH_TRNIF_bm;
	uint8_t buffer_delta = (buf_num - audio_mem.last_completed_bank) & AUDIO_BUFFER_INSTANCES_MASK;
	static uint16_t m_count = 0;
	static uint32_t m_sum = 0;
	static uint16_t clks;
	static int8_t linear = 13;
	static uint16_t measureas = 2048;
	static int8_t max_offset = -127;
	static int8_t min_offset = 127;
	if(!audio_mem.speaker_middleCLKsPerSample)
	{
		audio_mem.speaker_middleCLKsPerSample = audio_mem.speaker_teoriticalCLKsPerSample;
		linear = 13;
		measureas = 2048;
		m_sum = 0;
		m_count = 0;
		min_offset = 127;
		max_offset = -127;
	}


	clks = audio_mem.speaker_middleCLKsPerSample;
	int8_t from_idle = buffer_delta - AUDIO_WISHED_OFFSET;
	if(from_idle < min_offset)
	{
		min_offset = from_idle;
	}
	else if(from_idle > max_offset)
	{
		max_offset = from_idle;
	}

	if(from_idle < -1)
	{
		clks = audio_mem.speaker_middleCLKsPerSample + ((from_idle + 1) * linear);
// 		printf("neg\n");
	}

	if(from_idle > 1)
	{
		int8_t delta = from_idle - 1;
		clks = audio_mem.speaker_middleCLKsPerSample + delta * linear;
// 		printf("pos %d\n", delta);
	}
	
	
// 	printf("extreme %d\n", from_idle);
	if(from_idle < -3)
	{
		clks = clks + (from_idle + 5) * 15;
// 		printf("cmp%d%d:%d\n", audio_mem.last_completed_bank, audio_mem.last_converted_bank, audio_mem.last_free_bank);
	}
	if(from_idle > 3)
	{
		clks = clks + (from_idle - 5) * 15;
// 		printf("extreme %d\n", from_idle);
	}

	AUDIO_SPEAKER_DMA_SAMPLE_TIMER.PERBUF = clks;
	m_count++;
	m_sum += clks;
// 	static uint8_t i = 0;
// 	if(audio_mem.buffer[0].audio_buffer[1] != 0x00 && audio_mem.buffer[0].audio_buffer[1] != 0xFF)
// 		printf("%X %X\n", DACB.CH0DATA, DACB.CH1DATA);
// 	i++;
	if(m_count == measureas)
	{
		audio_mem.speaker_middleCLKsPerSample = m_sum / measureas - 1;
		printf("clks: %u %u mm: %d;%d size: %d\n", audio_mem.speaker_middleCLKsPerSample, linear, min_offset, max_offset, audio_mem.buffer[buf_num].size);
		if(measureas < 17000)
		{
			linear -= 3; //
			measureas <<= 1; //multiply with 2
			min_offset = 127;
			max_offset = -127;
		}
		m_sum = 0;
		m_count = 0;
		
	}
#ifdef DEBUG_FILTER
// 	printf("out-com %d\n", buffer_delta);
#endif
#ifdef DEBUG_SYSTEM
	printf("feed b: %d %d\n", buf_num, audio_mem.buffer[buf_num].size);
#endif
}

void audio_speaker_work()
{

	//trigger dac/dma if we had an underflow
	if(!((DMA.CH0.CTRLA | DMA.CH1.CTRLA) & DMA_CH_ENABLE_bm)) //Maybe this will be cleared to late. But i think its dont matter.
	{
		if(((audio_mem.last_converted_bank - audio_mem.last_free_bank) & AUDIO_BUFFER_INSTANCES_MASK) > (AUDIO_BUFFER_INSTANCES/2) - 1)
		{
			audio_speaker_setup_dma_ch(&DMA.CH0, (audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK);
			audio_speaker_setup_dma_ch(&DMA.CH1, (audio_mem.last_free_bank + 2) & AUDIO_BUFFER_INSTANCES_MASK);
			audio_speaker_start_dac_dma();
		}
	}
	else
	{

	}

	//Start converting if we are ready.
	if(audio_mem.last_converted_bank != audio_mem.last_completed_bank)
	{
		uint8_t next_bank = (audio_mem.last_converted_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK;
		audio_buffer_t *buf_ptr = &audio_mem.buffer[next_bank];

		for(uint16_t i = 1; i <= buf_ptr->size; i+=2)
		{
			buf_ptr->audio_buffer[i] ^= (1 << 7);
		}
#ifdef DEBUG_SYSTEM
		printf("con b: %d\n", next_bank);
#endif
		audio_mem.last_converted_bank = next_bank;
	}

}








void audio_speaker_data_out1(USB_EP_t *ep) //normaly in.. 1
{
	uint16_t bytes = usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[1].CNT;
	
// 	audio_mem.lastDownloadCompleteCLK = TCD0.CNT; //TODO: this is a problem, because its not absolut.
// 	audio_mem.lastDownloadCompleteSampleNum += bytes;


	uint8_t wanted_bank = (audio_mem.last_completed_bank + 3) & AUDIO_BUFFER_INSTANCES_MASK;
// 	uint8_t delta_to_last_free = (audio_mem.last_free_bank - wanted_bank) & AUDIO_BUFFER_INSTANCES_MASK;
	
	if(wanted_bank == ((audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK)) //TODO this could be important and should be checked
	{
// 		printf("over1 wb: %d, lf: %d\n", wanted_bank, audio_mem.last_free_bank);
		usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[1].CNT = 0;
		usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
		return;
	}
	
	audio_mem.buffer[audio_mem._bank_in_work1].size = bytes;
	audio_mem._bank_in_work1 = wanted_bank;
	audio_mem.last_completed_bank = (audio_mem.last_completed_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK;

#ifdef DEBUG_SYSTEM
	printf("down1: %d, %d\n", audio_mem._bank_in_work1, bytes);
#endif
	usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[1].DATAPTR = (uint16_t) audio_mem.buffer[wanted_bank].audio_buffer;
	usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[1].CNT = 0;
	usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);



// 	printf("1b: %d\n", audio_mem._bank_in_work1);
// 	printf("##%x %d\n", status, (ep == &usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[1]));
}

void audio_speaker_data_out0(USB_EP_t *ep) //out 0
{
	/* This function reports the complete download
	 * of a audio data package. It triggers rotating
	 * with the oder. Its buffer is located in:
	 * ep->DATAPTR and its size in: ep->CNT.
	 * I set a new buffer and set it immidatly as free again.
	 * it is also possible to work with the data and set it
	 * free again if the work is done.
	 * If you want to hack this driver it should be the only
	 * informationen you need.
	 *
	 * 
	 * It is stored on:
	 * 1. Save the size of the downloaded bank.
	 * 2. See if there is a new bank for downloading available
	 * or if we get an overflow.
	 * 3. If overflow just use same bank again.
	 * 4. Set the one bank as completed.
	 * Its possible that the completed bank
	 * not is the real completed. But this should
	 * if its apper, only apear once in the beginning.
	 * 
	 * 3. Log the time and the bytenum, for statistics.
	 * 
	 * 
	 * 
	 * 5. Take the bank
	 * 
	 * Hint: &usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0] == &usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].out == ep
	 * but direct static access is faster.
	 */
	uint16_t bytes = usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0].CNT;

	uint8_t wanted_bank = (audio_mem.last_completed_bank + 3) & AUDIO_BUFFER_INSTANCES_MASK;

	if(wanted_bank == ((audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK)) //TODO this could be important and should be checked
	{
// 		printf("over0 wb: %d, lf: %d\n", wanted_bank, audio_mem.last_free_bank);
		usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0].CNT = 0;
		usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
		return;
	}
	
	audio_mem.buffer[audio_mem._bank_in_work0].size = bytes;
	audio_mem._bank_in_work0 = wanted_bank;
	audio_mem.last_completed_bank = (audio_mem.last_completed_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK;

#ifdef DEBUG_SYSTEM
// 	printf("down0: %d, %d\n", audio_mem._bank_in_work0, bytes);
#endif
	usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0].DATAPTR = (uint16_t) audio_mem.buffer[wanted_bank].audio_buffer;
	usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0].CNT = 0;
	usb_mem.ep[AUDIO_STREAM_SPEAKER_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
	
// 	printf("##%x %u %u\n", status, (uint16_t)(cnt_value - old_cnt_value), pkg_cnt);
}




//########################### Interrupt service routines ######################################
ISR(DMA_CH0_vect)
{
	DMA.CH0.CTRLB |= DMA_CH_TRNIF_bm;
	if(((audio_mem.last_converted_bank - audio_mem.last_free_bank) & AUDIO_BUFFER_INSTANCES_MASK) <= 2)
	{
// 		printf("u0%d%d:%d\n", audio_mem.last_completed_bank, audio_mem.last_converted_bank, audio_mem.last_free_bank);
// 		printf("buffer underflow");
		return;
	}
	
	//We can prepare the buffer
	audio_speaker_setup_dma_ch(&DMA.CH0, (audio_mem.last_free_bank + 3) & AUDIO_BUFFER_INSTANCES_MASK);
	if(DMA.CH0.CTRLA & DMA_CH_ENABLE_bm)//When both channels are ready we possibly had an underflow and will disable the dma.
	{
		printf("dma call to slow\n");
		return;
	}
	
	//Free the complete buffer
	audio_mem.last_free_bank = (audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK;
#ifdef DEBUG_SYSTEM
	printf("dma0: %d\n", audio_mem.last_free_bank);
#endif
}

ISR(DMA_CH1_vect)
{
	DMA.CH1.CTRLB |= DMA_CH_TRNIF_bm;
	if(((audio_mem.last_converted_bank - audio_mem.last_free_bank) & AUDIO_BUFFER_INSTANCES_MASK) <= 2)
	{
// 		printf("u1%d%d:%d\n", audio_mem.last_completed_bank, audio_mem.last_converted_bank, audio_mem.last_free_bank);
// 		printf("buffer underflow");
		return;
	}
	
	//We can prepare the buffer
	audio_speaker_setup_dma_ch(&DMA.CH1, (audio_mem.last_free_bank + 3) & AUDIO_BUFFER_INSTANCES_MASK);
	if(DMA.CH1.CTRLA & DMA_CH_ENABLE_bm)//When both channels are ready we possibly had an underflow and will disable the dma.
	{
		//TODO: nothings happens
// 		printf("dma call to slow\n");
		return;
	}
	
	//Free the complete buffer
	audio_mem.last_free_bank = (audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK;
#ifdef DEBUG_SYSTEM
	printf("dma0: %d\n", audio_mem.last_free_bank);
#endif
}




