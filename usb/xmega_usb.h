#ifndef __XMEGA_USB_H__
#define __XMEGA_USB_H__ 1
#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>

#ifndef F_USB
#include <avr/iox256a3u.h>
#endif

#define NUM_EP 2

//This functions types of functions will be called if the transaction is complete on the coresponding endpoint.
typedef void (*ep_callback_action)(USB_EP_t *ep);

typedef bool (*setup_callback_action)(void);

extern const char *stage;

typedef union USB_EP_pair{
	union{
		struct{
			USB_EP_t out;
			USB_EP_t in;
		};
		USB_EP_t ep[2];
	};
} __attribute__((packed)) USB_EP_pair_t;

typedef struct  {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} __attribute__ ((packed)) USB_setup_package;


typedef struct usb_memory
{
	USB_EP_t *ptr[NUM_EP * 2];
	USB_EP_pair_t ep[NUM_EP];
	uint8_t ep0_in_buf[128];
	uint8_t ep0_out_buf[64]; //For holding bigger endpoint configurations and send them as multipackage the size is set to 128
	union {
		struct {
			volatile ep_callback_action out;
			volatile ep_callback_action in;
		} ep[NUM_EP];
		volatile ep_callback_action outin[NUM_EP * 2];
	} callback;
	USB_setup_package setup_pkg;
	uint8_t usb_configuration;
	volatile setup_callback_action unhandled_setup_out;
	volatile setup_callback_action unhandled_setup_in;
} __attribute__((packed)) usb_memory_t;

extern usb_memory_t usb_mem;

extern void usb_init(setup_callback_action unhandled_setup_out, setup_callback_action unhandled_setup_in);
extern void usb_reset(void);
extern bool ep_config(uint8_t ep_num, void *in_buf, void *out_buf, USB_EP_TYPE_t ep_type, uint16_t ep_size, ep_callback_action callback_in, ep_callback_action callback_out);
extern bool ep_config_isochronous(uint8_t ep_num, void* buf0, void* buf1, uint16_t ep_size, ep_callback_action callback_out, ep_callback_action callback_in);
extern void usb_ep0_clear_setup(void);
extern void usb_ep0_ack_out(void);
extern void usb_ep0_ack_in(void);
extern void usb_ep0_set_addr(void);
extern void usb_ep_start_in(uint8_t ep, const uint8_t* data, uint16_t size, bool zlp);
extern void usb_ep0_in(uint8_t size);
extern void usb_ep0_out(void);
// extern void usb_ep0_clear_in(void);
extern void usb_ep0_clear_out(void);
extern void usb_ep0_stall(void);

//Unwanted
extern const uint8_t* usb_ep0_from_progmem(const uint8_t* addr, uint16_t size);

#define GCC_FORCE_ALIGN_2  __attribute__((section (".data,\"aw\",@progbits\n.p2align 1;")))

#endif
