#include "audio.h"

#include <usb/xmega_usb.h>

#include <stdio.h>

#include <usb/usb_audio.h>
#include <usb/usb_standard.h>
#include "Qdef_types.h"

#include <avr/interrupt.h>

audio_memory_t audio_mem;

/* This driver uses multiply buffers for audio usb->dac transfer.
 * it stores them into rotating buffers converting the data from
 * signed to non-signed (with cpu) for making them compatible to
 * the dac and using timer triggered double buffered dma for
 * getting them on dac. There are many other options to manage
 * this but this i think this is a very fast option.
 */

void audio_callback_in(USB_EP_t *ep)
{
	printf("Audio in");
}

void audio_callback_out(USB_EP_t *ep)
{
	printf("Audio in");
}




void audio_data_out1(USB_EP_t *ep) //normaly in.. 1
{
	uint16_t bytes = usb_mem.ep[AUDIO_STREAM_EPADDR].ep[1].CNT;
	
// 	audio_mem.lastDownloadCompleteCLK = TCD0.CNT; //TODO: this is a problem, because its not absolut.
// 	audio_mem.lastDownloadCompleteSampleNum += bytes;


	uint8_t wanted_bank = (audio_mem.last_completed_bank + 3) & AUDIO_BUFFER_INSTANCES_MASK;
// 	uint8_t delta_to_last_free = (audio_mem.last_free_bank - wanted_bank) & AUDIO_BUFFER_INSTANCES_MASK;
	
	if(wanted_bank == ((audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK)) //TODO this could be important and should be checked
	{
		printf("over1 wb: %d, lf: %d\n", wanted_bank, audio_mem.last_free_bank);
		usb_mem.ep[AUDIO_STREAM_EPADDR].ep[1].CNT = 0;
		usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
		return;
	}
	
	audio_mem.buffer[audio_mem._bank_in_work1].size = bytes;
	audio_mem._bank_in_work1 = wanted_bank;
	audio_mem.last_completed_bank = (audio_mem.last_completed_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK;

#ifdef DEBUG_SYSTEM
	printf("down1: %d, %d\n", audio_mem._bank_in_work1, bytes);
#endif
	usb_mem.ep[AUDIO_STREAM_EPADDR].ep[1].DATAPTR = (uint16_t) audio_mem.buffer[wanted_bank].audio_buffer;
	usb_mem.ep[AUDIO_STREAM_EPADDR].ep[1].CNT = 0;
	usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);




// 	printf("1b: %d\n", audio_mem._bank_in_work1);
// 	printf("##%x %d\n", status, (ep == &usb_mem.ep[AUDIO_STREAM_EPADDR].ep[1]));
}

void audio_data_out0(USB_EP_t *ep) //out 0
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
	 * Hint: &usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0] == &usb_mem.ep[AUDIO_STREAM_EPADDR].out == ep
	 * but direct static access is much faster.
	 */
	uint16_t bytes = usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0].CNT;
	
// 	audio_mem.lastDownloadCompleteCLK = TCD0.CNT; //TODO: this is a problem, because its not absolut.
// 	audio_mem.lastDownloadCompleteSampleNum += bytes;


	uint8_t wanted_bank = (audio_mem.last_completed_bank + 3) & AUDIO_BUFFER_INSTANCES_MASK;
// 	uint8_t delta_to_last_free = (audio_mem.last_free_bank - wanted_bank) & AUDIO_BUFFER_INSTANCES_MASK;

	if(wanted_bank == ((audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK)) //TODO this could be important and should be checked
	{
		printf("over0 wb: %d, lf: %d\n", wanted_bank, audio_mem.last_free_bank);
		usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0].CNT = 0;
		usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
		return;
	}
	
	audio_mem.buffer[audio_mem._bank_in_work0].size = bytes;
	audio_mem._bank_in_work0 = wanted_bank;
	audio_mem.last_completed_bank = (audio_mem.last_completed_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK;

#ifdef DEBUG_SYSTEM
// 	printf("down0: %d, %d\n", audio_mem._bank_in_work0, bytes);
#endif
	usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0].DATAPTR = (uint16_t) audio_mem.buffer[wanted_bank].audio_buffer;
	usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0].CNT = 0;
	usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
	
// 	printf("##%x %u %u\n", status, (uint16_t)(cnt_value - old_cnt_value), pkg_cnt);
}

static inline void sync_mcu_freq(uint16_t size, uint8_t time)
{
	
	static uint32_t clks_total;
	static uint32_t size_total;
	if(audio_mem.mcu_sync_counter < 4000)
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

		if(audio_mem.mcu_sync_counter == 4000)
		{
			uint64_t frq = clks_total * 1024;
			frq = frq * (uint64_t)audio_mem.sampleRate;
			frq = frq / (uint64_t)(size_total >> 1);
			usb_mem.callback.ep[AUDIO_STREAM_EPADDR].out = &audio_data_out0; //TODO: Dirty. Should be done by xmega_usb.c
			usb_mem.callback.ep[AUDIO_STREAM_EPADDR].in = &audio_data_out1;
			audio_mem.mcu_clock = frq;
			audio_mem.teoriticalCLKsPerSample = (audio_mem.mcu_clock/audio_mem.sampleRate) - 1;
			audio_mem.middleCLKsPerSample = 0;
			audio_mem.mcu_synced = true;
			uint16_t val = (((uint32_t)audio_mem.mcu_clock * 2) + 1000000UL*8)/(1000000UL*16) - 1; //TODO: Very dirty should be done somewhere but not here
			USARTC0.BAUDCTRLA = val & 0xFF;
			USARTC0.BAUDCTRLB = val >> 8; 
			printf("cpu freq is: %lu %lu, %u\n", (uint32_t)frq, size_total, time);
		}
	}
}

void audio_data_out_sync_clk0(USB_EP_t *ep)
{
	uint16_t size = ep->CNT;
	ep->CNT = 0;
	usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);

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
	usb_mem.ep[AUDIO_STREAM_EPADDR].ep[0].STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);

	static uint16_t old_time = 0;
	uint16_t new_time = AUDIO_TIME_COUNTER.CNT;
	uint16_t time = new_time - old_time;

	sync_mcu_freq(size, time);

	old_time = new_time;
}

void audio_init_dma(void)
{
	EVSYS.CH0MUX                     = EVSYS_CHMUX_TCC0_OVF_gc; //Route Timer through eventcontroller for clearing timer interrupt
	EVSYS.CH0CTRL                    = 0;

	DACB.CTRLA = DAC_CH0EN_bm | DAC_CH1EN_bm;
	DACB.CTRLB = DAC_CHSEL_DUAL_gc;
	DACB.CTRLC = DAC_REFSEL_INT1V_gc | DAC_LEFTADJ_bm;
	DACB.CTRLA |= DAC_ENABLE_bm;
	DACB.EVCTRL = DAC_EVSEL0_bm;
	
	AUDIO_DMA_SAMPLE_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
	AUDIO_DMA_SAMPLE_TIMER.CNT = 0;
	AUDIO_DMA_SAMPLE_TIMER.PER = 3000;       //Timerlimit
	AUDIO_DMA_SAMPLE_TIMER.INTCTRLA = TC_OVFINTLVL_OFF_gc;
	AUDIO_DMA_SAMPLE_TIMER.CTRLB = TC_WGMODE_NORMAL_gc;
	
	uint16_u target;
	target.u16 = ((uint16_t) &DACB.CH0DATA);
// 	target.u16 = ((uint16_t) ((uint8_t *)&targetbuf[1]) + 1);

	DMA.CTRL = DMA_ENABLE_bm | DMA_DBUFMODE_CH01_gc;

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
	
// 	static uint32_t dummy;
// 	target.u16 = (uint16_t)&dummy;
// 	DMA.CH1.DESTADDR0 = target.u8[0];
// 	DMA.CH1.DESTADDR1 = target.u8[1];
// 	DMA.CH1.DESTADDR2 = 0;
// 	DMA.CH0.DESTADDR0 = target.u8[0];
// 	DMA.CH0.DESTADDR1 = target.u8[1];
// 	DMA.CH0.DESTADDR2 = 0;
// 	target.u16 = (uint16_t) audio_mem.buffer[0].audio_buffer;
// 	DMA.CH1.SRCADDR0 = target.u8[0];
// 	DMA.CH1.SRCADDR1 = target.u8[1];
// 	DMA.CH1.SRCADDR2 = 0;
// 	DMA.CH0.SRCADDR0 = target.u8[0];
// 	DMA.CH0.SRCADDR1 = target.u8[1];
// 	DMA.CH0.SRCADDR2 = 0;
	

	DMA.CH0.TRFCNT                   = 64; //This should not be nesseary it will be overwritten.
	DMA.CH0.REPCNT                   = 0;
	DMA.CH1.TRFCNT                   = 64;
	DMA.CH1.REPCNT                   = 0;

	DMA.CH0.ADDRCTRL                 = DMA_CH_SRCRELOAD_TRANSACTION_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_BURST_gc | DMA_CH_DESTDIR_INC_gc;
	DMA.CH1.ADDRCTRL                 = DMA_CH_SRCRELOAD_TRANSACTION_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_BURST_gc | DMA_CH_DESTDIR_INC_gc;
	
// 	DMA.CH0.CTRLA                   |= DMA_CH_ENABLE_bm;
}


void audio_init(void)
{
	for(uint8_t i = 0; i < AUDIO_BUFFER_INSTANCES; i++)
	{
		audio_mem.buffer[0].size = 0;
	}
	audio_mem.sampleRate = 48000;
	audio_mem.mcu_synced = false;
	audio_mem.mcu_clock = F_CPU;
	audio_mem.teoriticalCLKsPerSample = (F_CPU/48000) - 1;
	audio_reset();
	AUDIO_TIME_COUNTER.CNT = 0;
	AUDIO_TIME_COUNTER.PER = -1;
	AUDIO_TIME_COUNTER.CTRLA = TC_CLKSEL_DIV1024_gc;
	AUDIO_TIME_COUNTER.CTRLB = TC_WGMODE_NORMAL_gc;
	AUDIO_TIME_COUNTER.INTCTRLA = TC_OVFINTLVL_OFF_gc;
	
// 	for(uint8_t i = 2; i < 8; i++)
// 	{
// 		ep_config(i, audio_buffer[i -1][0], audio_buffer[i - 1][1], USB_EP_TYPE_ISOCHRONOUS, 64, &audio_callback_in, &audio_callback_out);
// 	}
}

static inline void dma_setup_channel(DMA_CH_t *channel, uint8_t buf_num)
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
	if(!audio_mem.middleCLKsPerSample)
	{
		audio_mem.middleCLKsPerSample = audio_mem.teoriticalCLKsPerSample;
		linear = 13;
		measureas = 2048;
		m_sum = 0;
		m_count = 0;
		min_offset = 127;
		max_offset = -127;
	}


	clks = audio_mem.middleCLKsPerSample;
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
		clks = audio_mem.middleCLKsPerSample + ((from_idle + 1) * linear);
// 		printf("neg\n");
	}

	if(from_idle > 1)
	{
		int8_t delta = from_idle - 1;
		clks = audio_mem.middleCLKsPerSample + delta * linear;
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

	AUDIO_DMA_SAMPLE_TIMER.PERBUF = clks;
	m_count++;
	m_sum += clks;
// 	static uint8_t i = 0;
// 	if(audio_mem.buffer[0].audio_buffer[1] != 0x00 && audio_mem.buffer[0].audio_buffer[1] != 0xFF)
// 		printf("%X %X\n", DACB.CH0DATA, DACB.CH1DATA);
// 	i++;
	if(m_count == measureas)
	{
		audio_mem.middleCLKsPerSample = m_sum / measureas;
		printf("clks: %u %u mm: %d;%d size: %d\n", audio_mem.middleCLKsPerSample, linear, min_offset, max_offset, audio_mem.buffer[buf_num].size);
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



// static inline void dma_check_ready(void)
// {
// 	if(DMA.CH0.CTRLB & DMA_CH_TRNIF_bm) //The channel has made his work
// 	{
// 		if(((audio_mem.last_converted_bank - audio_mem.last_free_bank) & AUDIO_BUFFER_INSTANCES_MASK) <= 2) //If we have 2 or less buffers ready we can not continue, because all ready buffers are in work.
// 		{
// #ifdef DEBUG_SYSTEM
// 			printf("dma nr\n");
// #endif
// 			return; //we can return the error case that both buffers are ready will be handled in the next iteration.
// 		}
// 		//We can prepare the buffer
// 		dma_setup_channel(&DMA.CH0, (audio_mem.last_free_bank + 3) & AUDIO_BUFFER_INSTANCES_MASK);
// 		if(DMA.CH1.CTRLB & DMA_CH_TRNIF_bm)//When both channels are ready we possibly had an underflow and will disable the dma.
// 		{
// 			printf("dma uf\n");
// 			//No matter let us clear the 2 completed buffers so we and clear the memory.
// 			audio_mem.last_free_bank += 2;
// 			DMA.CH0.CTRLB |= DMA_CH_TRNIF_bm; //Clear the flags
// 			DMA.CH1.CTRLB |= DMA_CH_TRNIF_bm;
// 			DMA.CH0.CTRLA &= ~DMA_CH_ENABLE_bm; //Disable dma
// 			DMA.CH1.CTRLA &= ~DMA_CH_ENABLE_bm; //Disable dma
// 			return;
// 		}
// 		
// 		//Free the complete buffer
// 		audio_mem.last_free_bank = (audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK;
// #ifdef DEBUG_SYSTEM
// 		printf("dma0: %d\n", audio_mem.last_free_bank);
// #endif
// 	}
// 	
// 	if(DMA.CH1.CTRLB & DMA_CH_TRNIF_bm)
// 	{
// 		if(((audio_mem.last_converted_bank - audio_mem.last_free_bank) & AUDIO_BUFFER_INSTANCES_MASK) <= 2) //If we have 2 or less buffers ready we can not continue, because all ready buffers are in work.
// 		{
// #ifdef DEBUG_SYSTEM
// 			printf("dma nr\n");
// #endif
// 			return; //we can return the error case that both buffers are ready will be handled in the next iteration. Or we can continue here next time.
// 		}
// 		//We can prepare the buffer
// 		dma_setup_channel(&DMA.CH1, (audio_mem.last_free_bank + 3) & AUDIO_BUFFER_INSTANCES_MASK);
// 		if(DMA.CH0.CTRLB & DMA_CH_TRNIF_bm)//When both channels are ready we possibly had an underflow and will disable the dma.
// 		{
// 			printf("dma uf\n");
// 			//No matter let us clear the 2 completed buffers so we and clear the memory.
// 			audio_mem.last_free_bank += 2;
// 			DMA.CH0.CTRLB |= DMA_CH_TRNIF_bm; //Clear the flags
// 			DMA.CH1.CTRLB |= DMA_CH_TRNIF_bm;
// 			DMA.CH0.CTRLA &= ~DMA_CH_ENABLE_bm; //Disable dma
// 			DMA.CH1.CTRLA &= ~DMA_CH_ENABLE_bm; //Disable dma
// 			return;
// 		}
// 		//Free the complete buffer
// 		audio_mem.last_free_bank = (audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK;
// #ifdef DEBUG_SYSTEM
// 		printf("dma1: %d\n", audio_mem.last_free_bank);
// #endif
// 		DMA.CH1.CTRLB |= DMA_CH_TRNIF_bm;
// 	}
// }

void audio_reset(void)
{
	audio_mem.mcu_sync_counter = 0;
	audio_mem.last_completed_bank = AUDIO_BUFFER_INSTANCES - 1;
	audio_mem.last_converted_bank = AUDIO_BUFFER_INSTANCES - 1;
	audio_mem.last_free_bank = AUDIO_BUFFER_INSTANCES - 1;
// 	audio_mem.dac_stopped = true;
	audio_mem.interface_enabled = false;
	audio_mem.middleCLKsPerSample = 0;
	audio_mem._bank_in_work0 = 0;
	audio_mem._bank_in_work1 = 1;
	if(audio_mem.mcu_synced)
		ep_config_isochronous(1, audio_mem.buffer[0].audio_buffer, audio_mem.buffer[1].audio_buffer, AUDIO_STREAM_EPSIZE, &audio_data_out0, &audio_data_out1);
	else
		ep_config_isochronous(1, audio_mem.buffer[0].audio_buffer, audio_mem.buffer[1].audio_buffer, AUDIO_STREAM_EPSIZE, &audio_data_out_sync_clk0, &audio_data_out_sync_clk1);
	audio_init_dma();
}

void audio_work(void)
{
	if(audio_mem.interface_enabled)
	{
		
		//trigger dac/dma if we had an underflow
		if(!((DMA.CH0.CTRLA | DMA.CH1.CTRLA) & DMA_CH_ENABLE_bm)) //Maybe this will be cleared to late. But i think its dont matter.
		{
			if(((audio_mem.last_converted_bank - audio_mem.last_free_bank) & AUDIO_BUFFER_INSTANCES_MASK) > (AUDIO_BUFFER_INSTANCES/2) - 1)
			{
				dma_setup_channel(&DMA.CH0, (audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK);
				dma_setup_channel(&DMA.CH1, (audio_mem.last_free_bank + 2) & AUDIO_BUFFER_INSTANCES_MASK);
				audio_start_dac_dma();
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
// 				buf_ptr->audio_buffer[i] = buf_ptr->audio_buffer[i + 1];
// 				buf_ptr->audio_buffer[i + 1] = tmp;
// 				tmp = buf_ptr->audio_buffer[i + 1] ^ (1 << 7);
			}
#ifdef DEBUG_SYSTEM
			printf("con b: %d\n", next_bank);
#endif
			audio_mem.last_converted_bank = next_bank;
		}
	}
}

void audio_start_dac_dma(void)
{
	DMA.CH0.CTRLB |= DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;
	DMA.CH1.CTRLB |= DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;
	DMA.INTFLAGS = DMA_CH0ERRIF_bm | DMA_CH1ERRIF_bm | DMA_CH0TRNIF_bm | DMA_CH1TRNIF_bm;
	AUDIO_DMA_SAMPLE_TIMER.CNT = 0;
	DMA.CH0.CTRLA |= DMA_ENABLE_bm;
	DMA.CTRL |= DMA_ENABLE_bm;
#ifdef DEBUG_SYSTEM
	printf("dac st\n");
#endif
}

void audio_control_setSampleRate(USB_EP_t *ep)
{
	printf("ep cnt: %d\n", ep->CNT);
	if(ep->CNT == 3)
	{
		audio_mem.sampleRate = (((uint32_t)usb_mem.ep0_out_buf[2] << 16) | ((uint32_t)usb_mem.ep0_out_buf[1] << 8) | (uint32_t)usb_mem.ep0_out_buf[0]);
		usb_ep0_clear_setup();
		audio_mem.teoriticalCLKsPerSample = (audio_mem.mcu_clock/audio_mem.sampleRate) - 1;

		printf("sampleRate: %ld %d\n", audio_mem.sampleRate, audio_mem.teoriticalCLKsPerSample);
		AUDIO_DMA_SAMPLE_TIMER.PERBUF = audio_mem.teoriticalCLKsPerSample;
// 		usb_mem.ep[0].out.STATUS &= ~(/*USB_EP_TRNCOMPL0_bm |*/ USB_EP_BUSNACK0_bm | USB_EP_OVF_bm);
		usb_ep0_clear_setup();
	}
// 	usb_mem.callback.ep[0].out = &audio_callback_out;
}

bool audio_setup_out(void)
{
	switch (usb_mem.setup_pkg.bRequest)
	{
		case USB_REQ_SetInterface:
			if(usb_mem.setup_pkg.bmRequestType == (REQDIR_HOSTTODEVICE | USB_REQTYPE_STANDARD | USB_RECIPIENT_INTERFACE))
			{
				if(usb_mem.setup_pkg.wValue) //Enable or disable
				{
						if(audio_mem.mcu_synced)
							ep_config_isochronous(1, audio_mem.buffer[0].audio_buffer, audio_mem.buffer[1].audio_buffer, AUDIO_STREAM_EPSIZE, &audio_data_out0, &audio_data_out1);
						else
							ep_config_isochronous(1, audio_mem.buffer[0].audio_buffer, audio_mem.buffer[1].audio_buffer, AUDIO_STREAM_EPSIZE, &audio_data_out_sync_clk0, &audio_data_out_sync_clk1);
					usb_mem.ep[1].out.STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
// 					usb_mem.ep[1].in.STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
					audio_mem.usb_stopped = false;
					audio_mem.interface_enabled = true;
				}
				else
				{
					audio_reset();
					audio_mem.usb_stopped = true;
					audio_mem.interface_enabled = false;
					DMA.CH0.CTRLA &= ~DMA_CH_ENABLE_bm;
					DMA.CH1.CTRLA &= ~DMA_CH_ENABLE_bm;
				}
				usb_ep0_in(0);
				usb_ep0_clear_setup();
				printf("set interface %d\n", usb_mem.setup_pkg.wValue);
				return true;
			}
			return false;
		case AUDIO_REQ_SetCurrent:
			if(usb_mem.setup_pkg.bmRequestType == (REQDIR_HOSTTODEVICE | USB_REQTYPE_CLASS | USB_RECIPIENT_ENDPOINT))
			{
// 				audio_mem.sampleRate = (((uint32_t)usb_mem.ep0_out_buf[sizeof(USB_setup_package)] << 16) | ((uint32_t)usb_mem.ep0_out_buf[sizeof(USB_setup_package) + 1] << 8) | (uint32_t)usb_mem.ep0_out_buf[sizeof(USB_setup_package) + 2]);
				usb_mem.callback.ep[0].out = &audio_control_setSampleRate;
				usb_ep0_in(0);
				usb_ep0_clear_setup();

				return true;
			}
			return false;
		case AUDIO_REQ_GetCurrent:
			if(usb_mem.setup_pkg.bmRequestType == (REQDIR_DEVICETOHOST | USB_REQTYPE_CLASS | USB_RECIPIENT_ENDPOINT))
			{
				uint8_t EndpointAddress = (uint8_t)usb_mem.setup_pkg.wIndex;
				uint8_t EndpointControl = (usb_mem.setup_pkg.wValue >> 8);

				/* Only handle GET CURRENT requests to the audio endpoint's sample frequency property */
				if ((EndpointAddress == AUDIO_STREAM_EPADDR) && (EndpointControl == AUDIO_EPCONTROL_SamplingFreq))
				{
	

					/* Convert the sampling rate value into the 24-bit format the host expects for the property */
					usb_mem.ep0_in_buf[2] = (audio_mem.sampleRate >> 16);
					usb_mem.ep0_in_buf[1] = (audio_mem.sampleRate >> 8);
					usb_mem.ep0_in_buf[0] = (audio_mem.sampleRate &  0xFF);
					usb_ep0_clear_setup();
					usb_ep0_in(3);
					printf("Sample rate sended\n");
					return true;
				}
			}
	}
	printf("audio out bm: %x b: %x\n", usb_mem.setup_pkg.bmRequestType, usb_mem.setup_pkg.bRequest);
	return false;
}

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
	dma_setup_channel(&DMA.CH0, (audio_mem.last_free_bank + 3) & AUDIO_BUFFER_INSTANCES_MASK);
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
	dma_setup_channel(&DMA.CH1, (audio_mem.last_free_bank + 3) & AUDIO_BUFFER_INSTANCES_MASK);
	if(DMA.CH1.CTRLA & DMA_CH_ENABLE_bm)//When both channels are ready we possibly had an underflow and will disable the dma.
	{
// 		printf("dma call to slow\n");
		return;
	}
	
	//Free the complete buffer
	audio_mem.last_free_bank = (audio_mem.last_free_bank + 1) & AUDIO_BUFFER_INSTANCES_MASK;
#ifdef DEBUG_SYSTEM
	printf("dma0: %d\n", audio_mem.last_free_bank);
#endif
}
