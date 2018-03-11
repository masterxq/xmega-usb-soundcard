#include "xmega_usb.h"
#include "xmega_macros.h"
#include "usb_default_request.h"
#include "usb_standard.h"

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <QA/uart/uart.h>


usb_memory_t GCC_FORCE_ALIGN_2 usb_mem;

ep_callback_action backup_ep0_ep_callback_action_out;
ep_callback_action backup_ep0_ep_callback_action_in;

const char PROGMEM stage_unknown[] = "Unknown";

const char *stage = stage_unknown;

bool empty_setup_action(void)
{
	return false;
}

void empty_ep_func(USB_EP_t __attribute__((unused)) *ep)
{
	printf("ep_empty_func!!!\n");
}

void usb_init(setup_callback_action unhandled_setup_out, setup_callback_action unhandled_setup_in)
{
	NVM.CMD  = NVM_CMD_READ_CALIB_ROW_gc;
	USB.CAL0 = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBCAL0));
	NVM.CMD  = NVM_CMD_READ_CALIB_ROW_gc;
	USB.CAL1 = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBCAL1));

	NVM.CMD = NVM_CMD_NO_OPERATION_gc;
	if(!unhandled_setup_in)
		unhandled_setup_in = &empty_setup_action;

	if(!unhandled_setup_out)
		unhandled_setup_out = &empty_setup_action;
	
	usb_mem.unhandled_setup_in = unhandled_setup_in;
	usb_mem.unhandled_setup_out = unhandled_setup_out;
	
	USB.INTCTRLA = /*USB_SOFIE_bm |*/ USB_BUSEVIE_bm | USB_INTLVL_MED_gc;
	USB.INTCTRLB = USB_TRNIE_bm | USB_SETUPIE_bm;
	
	for(uint8_t i = 1; i < NUM_EP; i++)
	{
		if(!usb_mem.callback.ep[i].in)
			usb_mem.callback.ep[i].in = &empty_ep_func;
		if(!usb_mem.callback.ep[i].out)
			usb_mem.callback.ep[i].out = &empty_ep_func;
	}
	
	usb_reset();
}


// //Send ack in via callback
// void ep0_callback_action_ack_in(USB_EP_t *ep);
// void ep0_callback_action_ack_in(USB_EP_t *ep)
// {
// 	usb_mem.ep[0].in.STATUS &= ~(USB_EP_TRNCOMPL0_bm | USB_EP_BUSNACK0_bm | USB_EP_OVF_bm);
// 	usb_mem.callback.ep[0].in = backup_ep0_ep_callback_action_in;
// }
// 
// void usb_ep0_ack_in(void)
// {
// 	backup_ep0_ep_callback_action_in = usb_mem.callback.ep[0].in;
// 	usb_mem.callback.ep[0].in = &ep0_callback_action_ack_in;
// 	printf("Send ACK in\n");
// }


//Send ack out via callback
void ep0_callback_action_ack_out(USB_EP_t *ep);
void ep0_callback_action_ack_out(USB_EP_t *ep)
{
	usb_mem.ep[0].out.STATUS &= ~(USB_EP_TRNCOMPL0_bm | USB_EP_BUSNACK0_bm | USB_EP_OVF_bm);
	usb_mem.callback.ep[0].out = backup_ep0_ep_callback_action_out;
}

void usb_ep0_ack_out(void)
{
	backup_ep0_ep_callback_action_out = usb_mem.callback.ep[0].out;
	usb_mem.callback.ep[0].out = &ep0_callback_action_ack_out;
	printf("Send ACK out\n");
}


//Set address and ack via callback
void ep0_callback_action_set_address_in(USB_EP_t *ep);
void ep0_callback_action_set_address_in(USB_EP_t *ep)
{
	USB.ADDR = usb_mem.setup_pkg.wValue & 0x7F;
	usb_mem.callback.ep[0].in = backup_ep0_ep_callback_action_in;
	stage = stage_unknown;
	printf("Addr:%d\n", USB.ADDR);
}


//out set ADDRESS => clear setup => wait out => ack out => wait for in => set addres
void usb_ep0_set_addr(void)
{
	stage = PSTR("Addr");
	usb_ep0_clear_setup();
	backup_ep0_ep_callback_action_in = usb_mem.callback.ep[0].in;
	usb_mem.callback.ep[0].in = &ep0_callback_action_set_address_in;
	usb_ep0_in(0);
}




//Clearing
void usb_ep0_clear_setup()
{
	usb_mem.ep[0].out.STATUS &= ~(USB_EP_SETUP_bm | USB_EP_TRNCOMPL0_bm | USB_EP_BUSNACK0_bm | USB_EP_OVF_bm);
	usb_mem.ep[0].out.STATUS |= USB_EP_TOGGLE_bm;
	
	usb_mem.ep[0].in.STATUS |= USB_EP_TOGGLE_bm;
}

// void usb_ep0_clear_in(void)
// {
// 	usb_mem.ep[0].in.STATUS &= ~(USB_EP_TRNCOMPL0_bm | USB_EP_BUSNACK0_bm | USB_EP_OVF_bm);
// }

void usb_ep0_clear_out(void)
{
	usb_mem.ep[0].out.STATUS &= ~(USB_EP_TRNCOMPL0_bm | USB_EP_BUSNACK0_bm | USB_EP_OVF_bm);
}


void ep0_out_action(USB_EP_t *ep) //Non setup out package
{
// 	printf_P(PSTR("unfetched in, bm: %x, b: %x s: %S\n"), usb_mem.setup_pkg.bmRequestType, usb_mem.setup_pkg.bRequest, stage);
// 	if((uint16_t) ep != (uint16_t) &usb_mem.ep[0].out)
// 		printf("not equal\n");
// 	else
// 		printf("good\n");
// 		printf("got: %02X%02X%02X", usb_mem.ep0_out_buf[0], usb_mem.ep0_out_buf[1], usb_mem.ep0_out_buf[2]);
// 	memcpy(&usb_mem.setup_pkg, usb_mem.ep0_out_buf, sizeof(USB_setup_package));
// 
// 	if(usb_mem.setup_pkg.bmRequestType == 0x0b)
// 	{
// 		fprintf(stderr,"pgk. type:%x req:%x val:%x index:%x len:%d\n",usb_mem.setup_pkg.bmRequestType, usb_mem.setup_pkg.bRequest, usb_mem.setup_pkg.wValue, usb_mem.setup_pkg.wIndex, usb_mem.setup_pkg.wLength);
// 	}
// 	usb_ep0_out();
// 	static uint8_t i = 0;
// 	if((i & 0b111) == 0)
// 	{
// 		USB_setup_package *pkg = (USB_setup_package * ) usb_mem.ep0_out_buf;
// // 		if(pkg->bmRequestType == 0x22)
// 			fprintf(stderr,"pgk. type:%x req:%x val:%x index:%x len:%d truelen:%d\n",pkg->bmRequestType, pkg->bRequest, pkg->wValue, pkg->wIndex, pkg->wLength, ep->CNT);
// 		i++;
// 	}
}

void ep0_in_action(USB_EP_t *ep)
{
// 	if((usb_mem.setup_pkg.bmRequestType & USB_REQTYPE_TYPE_MASK) == USB_REQTYPE_STANDARD && usb_mem.setup_pkg.bRequest == USB_REQ_SetAddress)
// 	{
// 			USB.ADDR = usb_mem.setup_pkg.wValue & 0x7F;
// 	}
	


// 	printf_P(PSTR("unfetched in, bm: %x, b: %x s: %S\n"), usb_mem.setup_pkg.bmRequestType, usb_mem.setup_pkg.bRequest, stage);
// 	memcpy(&usb_mem.setup_pkg, usb_mem.ep0_out_buf, sizeof(USB_setup_package));
// 	usb_mem.ep[0].in.STATUS &= ~(USB_EP_TRNCOMPL0_bm | USB_EP_BUSNACK0_bm | USB_EP_OVF_bm);
// 	uint8_t status = ep->STATUS;
// 	if(status & USB_EP_SETUP_bm)
// 	{
// 		memcpy(&usb_mem.setup_pkg, usb_mem.ep0_out_buf, sizeof(USB_setup_package));
// 		LACR16(&(usb_mem.ep[0].out.STATUS), USB_EP_TRNCOMPL0_bm | USB_EP_SETUP_bm);
// 		usb_handle_setup();
// 		printf("Setup handled set: %d cmpl: %d\n",  & USB_EP_SETUP_bm, status & USB_EP_TRNCOMPL0_bm);
// 	}
// 	printf("Got in packet\n");
}

void usb_reset(void)
{
	USB.EPPTR = (unsigned) &usb_mem.ep[0].ep[0];
	USB.ADDR = 0;
	
	usb_mem.ep[0].out.STATUS = 0;
	usb_mem.ep[0].out.CTRL = USB_EP_TYPE_CONTROL_gc | USB_EP_BUFSIZE_64_gc;
	usb_mem.ep[0].out.DATAPTR = (unsigned) usb_mem.ep0_out_buf;
	usb_mem.ep[0].in.STATUS = USB_EP_BUSNACK0_bm;
	usb_mem.ep[0].in.CTRL = USB_EP_TYPE_CONTROL_gc | USB_EP_MULTIPKT_bm | USB_EP_BUFSIZE_64_gc;
	usb_mem.ep[0].in.DATAPTR = (unsigned) &usb_mem.ep0_in_buf;
	usb_mem.callback.ep[0].in = &ep0_in_action;
	usb_mem.callback.ep[0].out = &ep0_out_action;
	USB.CTRLA = USB_ENABLE_bm | USB_SPEED_bm | USB_FIFOEN_bm | (NUM_EP - 1);
	USB.CTRLB |= USB_ATTACH_bm;
}



bool ep_config(uint8_t ep_num, void* in_buf, void* out_buf, USB_EP_TYPE_t ep_type, uint16_t ep_size, ep_callback_action callback_in, ep_callback_action callback_out)
{
	if(ep_num >= NUM_EP)
	{
		printf_P(PSTR("Invalid ep: %d, max: %d\n"), ep_num, NUM_EP);
		return false;
	}
	//If the user not gives an endpoint complete function we will add an empty so we can call without checking if its zero.
	if(!callback_in)
		callback_in = &empty_ep_func;

	if(!callback_out)
		callback_out = &empty_ep_func;
		 
	//Setup Out endpoint.
	USB_EP_t *ep_ptr = &usb_mem.ep[ep_num].out;
	ep_ptr->DATAPTR = (unsigned) out_buf;
	ep_ptr->CTRL = USB_EP_size_to_gc(ep_size) | ep_type;
	ep_ptr->STATUS = USB_EP_BUSNACK0_bm;
	
	//Setup In endpoint.
	ep_ptr = &usb_mem.ep[ep_num].in;
	ep_ptr->DATAPTR = (unsigned) in_buf;
	ep_ptr->CTRL = USB_EP_size_to_gc(ep_size) | ep_type;
	ep_ptr->STATUS = USB_EP_BUSNACK0_bm;

	usb_mem.callback.ep[ep_num].in = callback_in;
	usb_mem.callback.ep[ep_num].out = callback_out;
	return true;
}

bool ep_config_isochronous(uint8_t ep_num, void* buf0, void* buf1, uint16_t ep_size, ep_callback_action callback_out, ep_callback_action callback_in)
{
	ep_num &= ~(1 << 7);
	if(ep_num >= NUM_EP)
	{
		printf_P(PSTR("Invalid ep: %d, max: %d\n"), ep_num, NUM_EP);
		return false;
	}
	//If the user not gives an endpoint complete function we will add an empty so we can call without checking if its zero.
	if(!callback_in)
		callback_in = &empty_ep_func;

	if(!callback_out)
		callback_out = &empty_ep_func;
		 
	//Setup Out endpoint.
	usb_mem.callback.ep[ep_num].in = callback_in;
	usb_mem.callback.ep[ep_num].out = callback_out;
	USB_EP_t *ep_ptr = &usb_mem.ep[ep_num].out;

	ep_ptr->DATAPTR = (unsigned) buf0;
	ep_ptr->CNT = 0;
	ep_ptr->CTRL = USB_EP_size_to_gc(ep_size) | USB_EP_MULTIPKT_bm | USB_EP_PINGPONG_bm | USB_EP_TYPE_ISOCHRONOUS_gc; //Dont sure if pingpong is needed if isochronous
// 	printf("CTRL: 0x%02X\n", ep_ptr->CTRL);
	ep_ptr->STATUS = 0;

	//Setup In endpoint.
	ep_ptr = &usb_mem.ep[ep_num].in;
	ep_ptr->DATAPTR = (unsigned) buf1;
	ep_ptr->CNT = 0;
	ep_ptr->CTRL = USB_EP_size_to_gc(ep_size) | USB_EP_MULTIPKT_bm | USB_EP_PINGPONG_bm | USB_EP_TYPE_ISOCHRONOUS_gc;
	ep_ptr->STATUS = 0;


	return true;
}



void usb_ep0_in(uint8_t size)
{
	usb_mem.ep[0].in.AUXDATA = 0;
	usb_mem.ep[0].in.CNT = size | (1 << 15);
	LACR16(&(usb_mem.ep[0].in.STATUS), USB_EP_BUSNACK0_bm | USB_EP_TRNCOMPL0_bm | USB_EP_OVF_bm);
}

void usb_ep0_out(void)
{
	LACR16(&usb_mem.ep[0].out.STATUS, USB_EP_SETUP_bm | USB_EP_BUSNACK0_bm | USB_EP_TRNCOMPL0_bm | USB_EP_OVF_bm);
}


void usb_ep0_stall(void) {

	usb_mem.ep[0].out.CTRL |= USB_EP_STALL_bm;
	usb_mem.ep[0].in.CTRL  |= USB_EP_STALL_bm;
	LACR16(&usb_mem.ep[0].out.STATUS, USB_EP_TRNCOMPL0_bm | USB_EP_OVF_bm);
}

//TODO:This should only be temporaly...
const uint8_t* usb_ep0_from_progmem(const uint8_t* addr, uint16_t size) {
	NVM.CMD = NVM_CMD_NO_OPERATION_gc;
// 	while (size--){
// 		*buf++ = pgm_read_byte(addr++);
// 	}
	memcpy_P(usb_mem.ep0_in_buf, addr, size);
	return usb_mem.ep0_in_buf;
}

// void usb_ep_start_in(uint8_t ep, const uint8_t* data, uint16_t size, bool zlp) {
// 	_USB_EP(ep);
// 	e->DATAPTR = (unsigned) data;
// 	e->AUXDATA = 0;	// for multi-packet
// 	e->CNT = size | (zlp << 15);
// 	LACR16(&(e->STATUS), USB_EP_BUSNACK0_bm | USB_EP_TRNCOMPL0_bm);
// 	return 0;
// }





ISR(USB_TRNCOMPL_vect)
{
// 	printf("irq");
// 	printf("irq %x\n", USB.INTFLAGSBCLR);
	int8_t fifo_read_pos;
	if(USB.INTFLAGSBCLR & USB_TRNIF_bm)
	{
		/* The way the FIFO works is special. It is a fifo for all in and out eps.
		 * and stores every transaction complete.
		 * If all transactions are read the interrupt flag TRNIF will be cleared automatically.
		 */
		fifo_read_pos = USB.FIFORP; //If we read FIFORP it will automatically go to next pos.
		
		/*The endpoint that triggered the action need be read from
		 * the memory before USB.EPPTR. This is the fifo mem. The fifo
		 * mem contains pointers to the endpoint address in sram.
		 * As we dont want the address but the endpoint num we need to
		 * calculate it. The fiforp is a negative number. And contains
		 * the offset to the EPPTR pointer, where the pointer to the ep
		 * is stored.
		*/
		uint16_t ep_location = *(uint16_t*)(USB.EPPTR + (fifo_read_pos * 2)); //Pointer size == 2 bytes
		uint8_t ep_num = (ep_location - USB.EPPTR) >> 3;                      //We divide with 8 == sizeof(USB_EP_t)
		if(ep_num >= 2 * NUM_EP)                                              //Just for the case something went wrong.
		{
			printf("ep callback out of range!!!!!");
		}

		usb_mem.callback.outin[ep_num]((USB_EP_t *)ep_location);              //The coresponding EP function pointer is called.

		/* The return is in most cases faster and will work because if
		 * there are still complete transactions in fifo. The TRNIF flag
		 * is not cleared and the interrupt is called again immediately.
		 */
// 		fprintf(stdout, "offset %d\n", ep_location - USB.EPPTR);

// 		fprintf(stdout, "ep %s num: %d s: %d\n", (ep_num&1)?"in":"out", ep_num >> 1, ((USB_EP_t *)ep_location)->CNT) ;
		return;
	}

	if(USB.INTFLAGSBCLR & USB_SETUPIE_bm)
	{
		USB.INTFLAGSBCLR = USB_SETUPIE_bm; //Clear this first so we know if we get a new package.
		LACR16(&(usb_mem.ep[0].out.STATUS), USB_EP_TRNCOMPL0_bm | USB_EP_SETUP_bm);
		memcpy(&usb_mem.setup_pkg, usb_mem.ep0_out_buf, sizeof(USB_setup_package));
		if(!usb_handle_setup())
			usb_ep0_stall();
		return;
// 		printf("Setup handled set: %d cmpl: %d\n", status & USB_EP_SETUP_bm, status & USB_EP_TRNCOMPL0_bm);
	}
}







ISR(USB_BUSEVENT_vect)
{
	printf_P(PSTR("Busevent "));
	if (USB.INTFLAGSACLR & USB_SOFIF_bm){
		USB.INTFLAGSACLR = USB_SOFIF_bm;
	}
	if (USB.INTFLAGSACLR & (USB_CRCIF_bm | USB_UNFIF_bm | USB_OVFIF_bm)){
		USB.INTFLAGSACLR = (USB_CRCIF_bm | USB_UNFIF_bm | USB_OVFIF_bm);
		printf_P(PSTR("of\n"));
	}
	if (USB.INTFLAGSACLR & USB_STALLIF_bm)
	{
		USB.INTFLAGSACLR = USB_STALLIF_bm;
		printf_P(PSTR("stall\n"));
	}
	else
	{
		USB.INTFLAGSACLR = USB_SUSPENDIF_bm | USB_RESUMEIF_bm | USB_RSTIF_bm;
		if (USB.STATUS & USB_RSTIF_bm){
			USB.STATUS &= ~USB_RSTIF_bm;
			usb_reset();
			printf_P(PSTR("Performed reset\n"));
		}
	}
}
