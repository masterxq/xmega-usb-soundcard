#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>
#include <stdio.h>

#include <Qdef_types.h>

#include <uart/uart.h>
#include <osc/osc.h>
#include "stdbuf.h"

#include <cfg/QA_uart_cfg.h>

#include <usb/xmega_usb.h>

#include "audio.h"

int main(void)
{
	int2x_USB48(12);

//   int48_ppl_USB48();
	

	
	PORTCFG.CLKEVOUT = PORTCFG_CLKOUTSEL_CLK1X_gc | PORTCFG_CLKOUT_PC7_gc;
	PORTC.DIRSET = PIN7_bm;
	_delay_ms(800);
  uart_init(&USARTC0, 1000000, USART_RXCINTLVL_OFF_gc, NULL, false, true);

	stdbuf_init(&uart_c0_byte_out);

	PMIC.CTRL |= PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;

	PORTE.DIRSET = 0xff;

  printf("booting %d\n", USB.INTFLAGSBCLR);

  _delay_ms(1000);
	
	stdbuf_work();
	
	usb_init(&audio_setup_out, NULL);
	audio_init();
	sei();

	
  uint16_t i = 0;

	
  while(1)
  {
		if(i == 0)
		{
// 			usb_mem.ep[1].in.STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
// 			usb_mem.ep[1].out.STATUS &= ~(USB_EP_BUSNACK0_bm | USB_EP_BUSNACK1_bm | USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_TRNCOMPL1_bm | USB_EP_OVF_bm);
			static uint8_t j = 0;
			if(j == 0)
			{
				printf("0x%X\n", usb_mem.ep[1].out.STATUS);
			}
			j++;
		}
		i++;
    stdbuf_work();
		audio_work();
  }
}
