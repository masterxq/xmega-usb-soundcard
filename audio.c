#include "audio.h"

#include <usb/xmega_usb.h>

#include <stdio.h>

#include <usb/usb_audio.h>
#include <usb/usb_standard.h>
#include "Qdef_types.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>



/* This driver uses multiply buffers for audio usb->dac transfer.
 * it stores them into rotating buffers converting the data from
 * signed to non-signed (with cpu) for making them compatible to
 * the dac and using timer triggered double buffered dma for
 * getting them on dac. There are many other options to manage
 * this but this i think this is a very fast option.
 */

// void audio_callback_in(USB_EP_t *ep)
// {
// 	printf("Audio in");
// }
// 
// void audio_callback_out(USB_EP_t *ep)
// {
// 	printf("Audio in");
// }









void audio_init(void)
{

	
// 	for(uint8_t i = 2; i < 8; i++)
// 	{
// 		ep_config(i, audio_buffer[i -1][0], audio_buffer[i - 1][1], USB_EP_TYPE_ISOCHRONOUS, 64, &audio_callback_in, &audio_callback_out);
// 	}
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

}








