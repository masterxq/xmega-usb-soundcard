#include "xmega_usb.h"
#include "usb_standard.h"
#include "descriptor.h"

#include <stdio.h>
#include <string.h>

#include <avr/pgmspace.h>

bool usb_handle_control_in_complete(void) {
	if ((usb_mem.setup_pkg.bmRequestType & USB_REQTYPE_TYPE_MASK) == USB_REQTYPE_STANDARD)
	{
		switch (usb_mem.setup_pkg.bRequest)
		{
			case USB_REQ_SetAddress:
				USB.ADDR = usb_mem.setup_pkg.wValue & 0x7F;
				printf_P(PSTR("Setting addr %d\n"), USB.ADDR);
				
// 			case USB_REQ_GetDescriptor:
// 				if(usb_mem.end_pkg)
// 				{
// 					usb_ep0_out();
// 					usb_mem.end_pkg = false;
// 					printf("ended!!\n");
// 				}
		}
		return true;
// 		printf_P(PSTR("std request %d "), usb_mem.setup_pkg.bRequest);
	}
// 	printf_P(PSTR("Unknown handle in complete\n"));
	return false;
}

bool usb_handle_setup(void){
	if ((usb_mem.setup_pkg.bmRequestType & USB_REQTYPE_TYPE_MASK) == USB_REQTYPE_STANDARD){
		switch (usb_mem.setup_pkg.bRequest){
			case USB_REQ_GetStatus:
				usb_mem.ep0_in_buf[0] = 0;
				usb_mem.ep0_in_buf[1] = 0;
				stage = PSTR("getStatus");
				usb_ep0_clear_setup();
				usb_ep0_in(2);
				printf_P(PSTR("get status\n"));
				return true;

			case USB_REQ_ClearFeature:
			case USB_REQ_SetFeature:
				stage = PSTR("SetFeature");
// 				usb_ep0_ack_out();
				usb_ep0_clear_setup();
				printf_P(stage);
				return true;

			case USB_REQ_SetAddress:
				usb_ep0_set_addr();
				printf_P(PSTR("SetAddress: %d\n"), usb_mem.setup_pkg.wValue);
				return true;

			case USB_REQ_GetDescriptor: {
				uint8_t type = (usb_mem.setup_pkg.wValue >> 8);
				uint8_t index = (usb_mem.setup_pkg.wValue & 0xFF);
				const uint8_t* descriptor = 0;
				uint16_t size = usb_cb_get_descriptor(type, index, &descriptor);

				if (size && descriptor)
				{
					if (size > usb_mem.setup_pkg.wLength)
					{
						size = usb_mem.setup_pkg.wLength;
					}

					if (descriptor != usb_mem.ep0_in_buf)
					{
						memcpy(usb_mem.ep0_in_buf, descriptor, size);
					}
					usb_ep0_in(size);
					usb_ep0_clear_setup();
					
					

					if(size > sizeof(usb_mem.ep0_in_buf))
						printf_P(PSTR("getDescriptor: %d size: %d\n"), type, size);
					
					return true;
				} else {
					usb_ep0_stall();
					return true;
				}
			}
			case USB_REQ_GetConfiguration:
				printf_P(PSTR("Getconfig\n"));
				usb_mem.ep0_in_buf[0] = usb_mem.usb_configuration;
				usb_ep0_clear_setup();
				usb_ep0_in(1);
// 				usb_ep0_out();
				return true;

			case USB_REQ_SetConfiguration:
				if (usb_mem.setup_pkg.wValue == 1) {
					usb_ep0_in(0);
					usb_mem.usb_configuration = (uint8_t)(usb_mem.setup_pkg.wValue);
					usb_ep0_out();
					printf_P(PSTR("SetConfig\n"));
					return true;
				}
				return false;
// 				} else {
// 					usb_ep0_stall();
// 					return true;
// 				}

// 			case USB_REQ_SetInterface:
// 				if (usb_cb_set_interface(usb_mem.setup_pkg.wIndex, usb_mem.setup_pkg.wValue)) {
// 					usb_ep0_in(0);
// 					usb_ep0_out();
// 					printf_P(PSTR("Set If\n"));
// 					return true;
// 				} else {
// 					usb_ep0_stall();
// 					return true;
// 				}
		}
	}
	
// 	printf_P(PSTR("cb\n"));
	if(usb_mem.unhandled_setup_out())
		return true;
	return false;
}
