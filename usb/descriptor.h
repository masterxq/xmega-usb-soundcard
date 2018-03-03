#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__ 1

#include "usb_standard.h"
#include "usb_audio.h"

#include <stdint.h>
#include <stdbool.h>

#include <avr/pgmspace.h>

typedef struct ConfigDesc {
	USB_ConfigurationDescriptor Config;
		USB_InterfaceDescriptor Audio_ControlInterface;
			USB_Audio_Descriptor_Interface_AC_t Audio_ControlInterface_SPC;
			USB_Audio_Descriptor_InputTerminal_t Audio_InputTerminal;
			USB_Audio_Descriptor_OutputTerminal_t Audio_OutputTerminal;
		USB_InterfaceDescriptor                Audio_StreamInterface_Alt0;
		USB_InterfaceDescriptor                Audio_StreamInterface_Alt1;
			USB_Audio_Descriptor_Interface_AS_t       Audio_StreamInterface_SPC;
			USB_Audio_Descriptor_Format_t             Audio_AudioFormat;
			USB_Audio_SampleFreq_t                    Audio_AudioFormatSampleRates[5];
			USB_Audio_Descriptor_StreamEndpoint_Std_t Audio_StreamEndpoint;
			USB_Audio_Descriptor_StreamEndpoint_Spc_t Audio_StreamEndpoint_SPC;

} ConfigDesc;

enum InterfaceDescriptors_t
{
	INTERFACE_ID_AudioControl = 0, /**< Audio control interface descriptor ID */
	INTERFACE_ID_AudioStream  = 1, /**< Audio stream interface descriptor ID */
};



extern const PROGMEM ConfigDesc configuration_descriptor;

extern bool usb_cb_control_setup(void);

extern uint16_t usb_cb_get_descriptor(uint8_t type, uint8_t index, const uint8_t** ptr);
extern bool usb_cb_set_configuration(uint8_t config);
extern bool usb_cb_set_interface(uint16_t interface, uint16_t altsetting);
#endif
