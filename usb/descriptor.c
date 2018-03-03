#include "descriptor.h"

#include "usb/xmega_usb.h"

#include <audio.h>

#include <avr/pgmspace.h>

#include <string.h>


// Notes:
// Fill in VID/PID in device_descriptor
// Fill in msft_extended for WCID
// WCID request ID can be changed below
// Other options in usb.h
// Additional compiler flags: -std=gnu99 -fno-strict-aliasing -Wstrict-prototypes -fno-jump-tables

#define WCID_REQUEST_ID			0x22
#define WCID_REQUEST_ID_STR		u"\x22"

// const USB_DeviceDescriptor PROGMEM device_descriptor = {
// 	.bLength = sizeof(USB_DeviceDescriptor),
// 	.bDescriptorType = USB_DTYPE_Device,
// 
// 	.bcdUSB                 = 0x0200,
// 	.bDeviceClass           = USB_CSCP_VendorSpecificClass,
// 	.bDeviceSubClass        = USB_CSCP_NoDeviceSubclass,
// 	.bDeviceProtocol        = USB_CSCP_NoDeviceProtocol,
// 
// 	.bMaxPacketSize0        = 64,
// 	.idVendor               = 0x8282,
// 	.idProduct              = 0x9999,
// 	.bcdDevice              = 0x0100,
// 
// 	.iManufacturer          = 0x01,
// 	.iProduct               = 0x02,
// 	.iSerialNumber          = 0,
// 
// 	.bNumConfigurations     = 1
// };
// 
// typedef struct ConfigDesc {
// 	USB_ConfigurationDescriptor Config;
// 	USB_InterfaceDescriptor Interface0;
// 	USB_EndpointDescriptor DataInEndpoint;
// 	USB_EndpointDescriptor DataOutEndpoint;
// 
// } ConfigDesc;
// 
// const PROGMEM ConfigDesc configuration_descriptor = {
// 	.Config = {
// 		.bLength = sizeof(USB_ConfigurationDescriptor),
// 		.bDescriptorType = USB_DTYPE_Configuration,
// 		.wTotalLength  = sizeof(ConfigDesc),
// 		.bNumInterfaces = 1,
// 		.bConfigurationValue = 1,
// 		.iConfiguration = 0,
// 		.bmAttributes = USB_CONFIG_ATTR_RESERVED,
// 		.bMaxPower = USB_CONFIG_POWER_MA(500)
// 	},
// 	.Interface0 = {
// 		.bLength = sizeof(USB_InterfaceDescriptor),
// 		.bDescriptorType = USB_DTYPE_Interface,
// 		.bInterfaceNumber = 0,
// 		.bAlternateSetting = 0,
// 		.bNumEndpoints = 2,
// 		.bInterfaceClass = USB_CSCP_VendorSpecificClass,
// 		.bInterfaceSubClass = 0x00,
// 		.bInterfaceProtocol = 0x00,
// 		.iInterface = 0
// 	},
// 	.DataInEndpoint = {
// 		.bLength = sizeof(USB_EndpointDescriptor),
// 		.bDescriptorType = USB_DTYPE_Endpoint,
// 		.bEndpointAddress = 0x81,
// 		.bmAttributes = (USB_EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
// 		.wMaxPacketSize = 64,
// 		.bInterval = 0x00
// 	},
// 	.DataOutEndpoint = {
// 		.bLength = sizeof(USB_EndpointDescriptor),
// 		.bDescriptorType = USB_DTYPE_Endpoint,
// 		.bEndpointAddress = 0x2,
// 		.bmAttributes = (USB_EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
// 		.wMaxPacketSize = 64,
// 		.bInterval = 0x00
// 	},
// };

const USB_DeviceDescriptor PROGMEM device_descriptor = {
	.bLength = sizeof(USB_DeviceDescriptor),
	.bDescriptorType = USB_DTYPE_Device,

	.bcdUSB                 = 0x0200,
	.bDeviceClass           = USB_CSCP_NoDeviceClass,
	.bDeviceSubClass        = USB_CSCP_NoDeviceSubclass,
	.bDeviceProtocol        = USB_CSCP_NoDeviceProtocol,

	.bMaxPacketSize0        = 64,
	.idVendor               = 0x9999,
	.idProduct              = 0x1337,
	.bcdDevice              = 0x0100,

	.iManufacturer          = 0x01,
	.iProduct               = 0x02,
	.iSerialNumber          = 0,

	.bNumConfigurations     = 1
};

// #ifndef F_USB
// #define __flash
// #endif



const PROGMEM ConfigDesc configuration_descriptor = {
	.Config = {
		.bLength = sizeof(USB_ConfigurationDescriptor),
		.bDescriptorType = USB_DTYPE_Configuration,
		.wTotalLength  = sizeof(ConfigDesc),
		.bNumInterfaces = 2,
		
		.bConfigurationValue = 1,
		.iConfiguration = 0,
		
		.bmAttributes = (USB_CONFIG_ATTR_RESERVED),
		.bMaxPower = USB_CONFIG_POWER_MA(500)
	},

		.Audio_ControlInterface = {
			.bLength = sizeof(USB_InterfaceDescriptor),
			.bDescriptorType = USB_DTYPE_Interface,
			
			.bInterfaceNumber = INTERFACE_ID_AudioControl,
			.bAlternateSetting = 0,

			.bNumEndpoints = 0,
			.bInterfaceClass = AUDIO_CSCP_AudioClass,
			.bInterfaceSubClass = AUDIO_CSCP_ControlSubclass,
			.bInterfaceProtocol = AUDIO_CSCP_ControlProtocol,
			.iInterface = 0
		},
		.Audio_ControlInterface_SPC = {
			.Size = sizeof(USB_Audio_Descriptor_Interface_AC_t),
			.Type = USB_DTYPE_CSInterface,
			.Subtype = AUDIO_DSUBTYPE_CSInterface_Header,
			.ACSpecification = 0x0100,
			.TotalLength = (sizeof(USB_Audio_Descriptor_Interface_AC_t)
			              + sizeof(USB_Audio_Descriptor_InputTerminal_t)
			              + sizeof(USB_Audio_Descriptor_OutputTerminal_t)),
			.InCollection = 1, /**< Total number of Audio Streaming interfaces linked to this Audio Control interface (must be 1). */
			.InterfaceNumber = INTERFACE_ID_AudioStream, /**< Interface number of the associated Audio Streaming interface. */
		},
		.Audio_InputTerminal = {
			.Size = sizeof(USB_Audio_Descriptor_InputTerminal_t),
			.Type = USB_DTYPE_CSInterface,
			.Subtype = AUDIO_DSUBTYPE_CSInterface_InputTerminal,
			.TerminalID = 0x01,
			.TerminalType = AUDIO_TERMINAL_STREAMING,
			.AssociatedOutputTerminal = 0x00,

			.TotalChannels            = 2,
			.ChannelConfig            = (AUDIO_CHANNEL_LEFT_FRONT | AUDIO_CHANNEL_RIGHT_FRONT),

			.ChannelStrIndex          = 0,
			.TerminalStrIndex         = 0
		},
		.Audio_OutputTerminal =
		{
			.Size = sizeof(USB_Audio_Descriptor_OutputTerminal_t),
			.Type = USB_DTYPE_CSInterface,
			.Subtype                  = AUDIO_DSUBTYPE_CSInterface_OutputTerminal,

			.TerminalID               = 0x02,
			.TerminalType             = AUDIO_TERMINAL_OUT_SPEAKER,
			.AssociatedInputTerminal  = 0x00,

			.SourceID                 = 0x01,

			.TerminalStrIndex         = 0
		},
	
	
	.Audio_StreamInterface_Alt0 = {
		.bLength = sizeof(USB_InterfaceDescriptor),
		.bDescriptorType = USB_DTYPE_Interface,
		.bInterfaceNumber = INTERFACE_ID_AudioStream,
		.bAlternateSetting = 0,

		.bNumEndpoints = 0,
		.bInterfaceClass = AUDIO_CSCP_AudioClass,
		.bInterfaceSubClass = AUDIO_CSCP_AudioStreamingSubclass,
		.bInterfaceProtocol = AUDIO_CSCP_ControlProtocol,
		.iInterface = 0
	},
	
	.Audio_StreamInterface_Alt1 = {
		.bLength = sizeof(USB_InterfaceDescriptor),
		.bDescriptorType = USB_DTYPE_Interface,
		
		.bInterfaceNumber = INTERFACE_ID_AudioStream,
		.bAlternateSetting = 1,

		.bNumEndpoints = 1,

		.bInterfaceClass = AUDIO_CSCP_AudioClass,
		.bInterfaceSubClass = AUDIO_CSCP_AudioStreamingSubclass,
		.bInterfaceProtocol = AUDIO_CSCP_ControlProtocol,
		.iInterface = 0
	},
	
		.Audio_StreamInterface_SPC =
		{
			.Size = sizeof(USB_Audio_Descriptor_Interface_AS_t),
			.Type = USB_DTYPE_CSInterface,
			.Subtype                  = AUDIO_DSUBTYPE_CSInterface_General,

			.TerminalLink             = 0x01,

			.FrameDelay               = 1,
			.AudioFormat              = 0x0001
		},

		.Audio_AudioFormat =
		{
			.Size = sizeof(USB_Audio_Descriptor_Format_t) +
			        sizeof(configuration_descriptor.Audio_AudioFormatSampleRates),
			.Type = USB_DTYPE_CSInterface,
			.Subtype                  = AUDIO_DSUBTYPE_CSInterface_FormatType,

			.FormatType               = 0x01,
			.Channels                 = 0x02,

			.SubFrameSize             = 0x02,
			.BitResolution            = 16,

			.TotalDiscreteSampleRates = (sizeof(configuration_descriptor.Audio_AudioFormatSampleRates) / sizeof(USB_Audio_SampleFreq_t)),
		},
		
		.Audio_AudioFormatSampleRates =
		{
			AUDIO_SAMPLE_FREQ(8000),
			AUDIO_SAMPLE_FREQ(11025),
			AUDIO_SAMPLE_FREQ(22050),
			AUDIO_SAMPLE_FREQ(44100),
			AUDIO_SAMPLE_FREQ(48000),
			AUDIO_SAMPLE_FREQ(64000),
			AUDIO_SAMPLE_FREQ(88200),
			AUDIO_SAMPLE_FREQ(96000),
		},
		
		.Audio_StreamEndpoint =
		{
			.Endpoint =
				{
					.bLength = sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t),
					.bDescriptorType = USB_DTYPE_Endpoint,

					.bEndpointAddress     = AUDIO_STREAM_EPADDR,
					.bmAttributes          = (USB_EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_SYNC | ENDPOINT_USAGE_DATA),
					.wMaxPacketSize        = AUDIO_STREAM_EPSIZE,
					.bInterval   = 0x01
				},

			.Refresh                  = 0,
			.SyncEndpointNumber       = 0
		},
		
		.Audio_StreamEndpoint_SPC =
		{
			.Size = sizeof(USB_Audio_Descriptor_StreamEndpoint_Spc_t),
			.Type = USB_DTYPE_CSEndpoint,
			.Subtype                  = AUDIO_DSUBTYPE_CSEndpoint_General,

			.Attributes               = (AUDIO_EP_ACCEPTS_SMALL_PACKETS | AUDIO_EP_SAMPLE_FREQ_CONTROL),

			.LockDelayUnits           = 0x00,
			.LockDelay                = 0x0000
		}
};

const PROGMEM USB_StringDescriptor language_string = {
	.bLength = USB_STRING_LEN(1),
	.bDescriptorType = USB_DTYPE_String,
	.bString = {USB_LANGUAGE_EN_US},
};

const PROGMEM USB_StringDescriptor manufacturer_string = {
	.bLength = USB_STRING_LEN(7),
	.bDescriptorType = USB_DTYPE_String,
	.bString = u"MasterQ"
};

const PROGMEM USB_StringDescriptor product_string = {
	.bLength = USB_STRING_LEN(15),
	.bDescriptorType = USB_DTYPE_String,
	.bString = u"Audio Amplifier"
};





const PROGMEM USB_StringDescriptor msft_string = {
	.bLength = 18,
	.bDescriptorType = USB_DTYPE_String,
	.bString = u"MSFT100" WCID_REQUEST_ID_STR
};


__attribute__((__aligned__(4))) const USB_MicrosoftCompatibleDescriptor msft_compatible = {
	.dwLength = sizeof(USB_MicrosoftCompatibleDescriptor) +
				1*sizeof(USB_MicrosoftCompatibleDescriptor_Interface),
	.bcdVersion = 0x0100,
	.wIndex = 0x0004,
	.bCount = 1,
	.reserved = {0, 0, 0, 0, 0, 0, 0},
	.interfaces = {
		{
			.bFirstInterfaceNumber = 0,
			.reserved1 = 0x01,
			.compatibleID = "WINUSB\0\0",
			.subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
			.reserved2 = {0, 0, 0, 0, 0, 0},
		},
	}
};

__attribute__((__aligned__(4))) const USB_MicrosoftExtendedPropertiesDescriptor msft_extended = {
	.dwLength = sizeof(USB_MicrosoftExtendedPropertiesDescriptor),
	.bcdVersion = 0x0100,
	.wIndex = 0x0005,
	.wCount = 2,

	.dwPropLength = 132,
	.dwType = 1,
	.wNameLength = 40,
	.name = L"DeviceInterfaceGUID\0",
	.dwDataLength = 78,
	.data = L"{d1ef5aba-506a-4ec6-94af-280f9ead82d5}\0",

	.dwPropLength2 = 14 + (6*2) + (5*2),
	.dwType2 = 1,
	.wNameLength2 = 6*2,
	.name2 = L"Label\0",
	.dwDataLength2 = 5*2,
	.data2 = L"XRNG\0",
};


/*
__attribute__((__aligned__(4))) const USB_MicrosoftExtendedPropertiesDescriptor msft_extended = {
	.dwLength = sizeof(USB_MicrosoftExtendedPropertiesDescriptor),
	.dwLength = 142,
	.bcdVersion = 0x0100,
	.wIndex = 0x0005,
	.wCount = 1,
	.dwPropLength = 132,
	.dwType = 1,
	.wNameLength = 40,
	.name = L"DeviceInterfaceGUID\0",
	.dwDataLength = 78,
	.data = L"{42314231-5A81-49F0-BC3D-A4FF138216D7}\0",
};
*/
/*
__attribute__((__aligned__(4))) const USB_MicrosoftExtendedPropertiesDescriptor msft_extended = {
	.dwLength = sizeof(USB_MicrosoftExtendedPropertiesDescriptor),
	.dwLength = 146,
	.bcdVersion = 0x0100,
	.wIndex = 0x0005,
	.wCount = 1,
	.dwPropLength = 136,
	.dwType = 7,
	.wNameLength = 42,
	.name = L"DeviceInterfaceGUIDs\0",
	.dwDataLength = 80,
	.data = L"{42314231-5A81-49F0-BC3D-A4FF138216D7}\0\0",
};
*/



uint16_t usb_cb_get_descriptor(uint8_t type, uint8_t index, const uint8_t** ptr) {
	const void* address = NULL;
	uint16_t size    = 0;

	switch (type) {
		case USB_DTYPE_Device:
			address = &device_descriptor;
			size    = sizeof(USB_DeviceDescriptor);
			break;
		case USB_DTYPE_Configuration:
			address = &configuration_descriptor;
			size    = sizeof(ConfigDesc);
			break;
		case USB_DTYPE_String:
			switch (index) {
				case 0x00:
					address = &language_string;
					break;
				case 0x01:
					address = &manufacturer_string;
					break;
				case 0x02:
					address = &product_string;
					break;
				case 0xEE:
					address = &msft_string;
					break;
			}
			size = pgm_read_byte(&((USB_StringDescriptor*)address)->bLength);
			break;
	}

	*ptr = usb_ep0_from_progmem(address, size);
// 	memcpy_P(usb_mem.ep0_in_buf, address, size);
// 	*ptr = usb_mem.ep0_in_buf;
	return size;
}

void usb_cb_reset(void) {

}

bool usb_cb_set_configuration(uint8_t config) {
	if (config <= 1) {
		return true;
	} else {
		return false;
	}
}

void usb_cb_completion(void) {

}



bool handle_msft_compatible(void) {
	const uint8_t *data;
	uint16_t len;
	if (usb_mem.setup_pkg.wIndex == 0x0005) {
		len = msft_extended.dwLength;
		data = (const uint8_t *)&msft_extended;
	} else if (usb_mem.setup_pkg.wIndex == 0x0004) {
		len = msft_compatible.dwLength;
		data = (const uint8_t *)&msft_compatible;
	} else {
		return false;
	}
	if (len > usb_mem.setup_pkg.wLength) {
		len = usb_mem.setup_pkg.wLength;
	}
	memcpy(usb_mem.ep0_in_buf, data, len);
	usb_ep0_in(len);
	usb_ep0_out();
	return true;
}

bool usb_cb_control_setup(void) {
	uint8_t recipient = usb_mem.setup_pkg.bmRequestType & USB_REQTYPE_RECIPIENT_MASK;
	if (recipient == USB_RECIPIENT_DEVICE) {
		switch(usb_mem.setup_pkg.bRequest) {
			case WCID_REQUEST_ID:
				return handle_msft_compatible();
		}
	} else if (recipient == USB_RECIPIENT_INTERFACE) {
		switch(usb_mem.setup_pkg.bRequest) {
			case WCID_REQUEST_ID:
				return handle_msft_compatible();
		}
	}
	return false;
}

void usb_cb_control_in_completion(void) {

}

void usb_cb_control_out_completion(void) {

}

bool usb_cb_set_interface(uint16_t __attribute__((unused)) interface, uint16_t __attribute__((unused)) altsetting) {
	return false;
}
