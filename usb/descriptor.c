#include "descriptor.h"

#include "usb/xmega_usb.h"

#include <avr/pgmspace.h>

#include <string.h>
#include <cfg/usb_sound_cfg.h>

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



const PROGMEM ConfigDesc configuration_descriptor = {
	.Config = {
		.bLength = sizeof(USB_ConfigurationDescriptor),
		.bDescriptorType = USB_DTYPE_Configuration,
		.wTotalLength  = sizeof(ConfigDesc),
		.bNumInterfaces = 3, //One control, one input (alt0 + alt1), one output (alt0 + alt1)
		
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
			.Size = sizeof(USB_Audio_Descriptor_Interface_AC_t)
			      + sizeof(configuration_descriptor.interfaceList),
			.Type = USB_DTYPE_CSInterface,
			.Subtype = AUDIO_DSUBTYPE_CSInterface_Header,
			.ACSpecification = 0x0100,
			.TotalLength = (sizeof(USB_Audio_Descriptor_Interface_AC_t)
			              + sizeof(USB_Audio_Descriptor_InputTerminal_t)
			              + sizeof(USB_Audio_Descriptor_OutputTerminal_t)
			              + sizeof(USB_Audio_Descriptor_InputTerminal_t)
			              + sizeof(USB_Audio_Descriptor_OutputTerminal_t)
			              + sizeof(configuration_descriptor.interfaceList)),
			.InCollection = 2, /**< Total number of Audio Streaming interfaces linked to this Audio Control interface (must be number of interfaces). */
// 			.InterfaceNumber = INTERFACE_ID_AudioStream, /**< Interface number of the associated Audio Streaming interface. */
		},
		.interfaceList = {INTERFACE_ID_AudioStream_Speaker, INTERFACE_ID_AudioStream_Mic},


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
		.Audio_InputTerminal2 = {
			.Size = sizeof(USB_Audio_Descriptor_InputTerminal_t),
			.Type = USB_DTYPE_CSInterface,
			.Subtype = AUDIO_DSUBTYPE_CSInterface_InputTerminal,
			.TerminalID               = 0x03,
			.TerminalType             = AUDIO_TERMINAL_IN_MIC,
			.AssociatedOutputTerminal = 0x00,

			.TotalChannels            = 1,
			.ChannelConfig            = 0,

			.ChannelStrIndex          = 0,
			.TerminalStrIndex         = 0
		},
		.Audio_OutputTerminal2 =
		{
			.Size = sizeof(USB_Audio_Descriptor_OutputTerminal_t),
			.Type = USB_DTYPE_CSInterface,
			.Subtype                  = AUDIO_DSUBTYPE_CSInterface_OutputTerminal,

			.TerminalID               = 0x04,
			.TerminalType             = AUDIO_TERMINAL_STREAMING,
			.AssociatedInputTerminal  = 0x00,

			.SourceID                 = 0x03,

			.TerminalStrIndex         = 0
		},
		


	.Audio_StreamInterface_Alt0 = {
		.bLength = sizeof(USB_InterfaceDescriptor),
		.bDescriptorType = USB_DTYPE_Interface,
		.bInterfaceNumber = INTERFACE_ID_AudioStream_Speaker,
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
		
		.bInterfaceNumber = INTERFACE_ID_AudioStream_Speaker,
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
// 			AUDIO_SAMPLE_FREQ(64000), //If you want to set this you need to change the size for the field in the descriptor.h from 5 to 8
// 			AUDIO_SAMPLE_FREQ(88200),
// 			AUDIO_SAMPLE_FREQ(96000),
		},
		
		.Audio_StreamEndpoint =
		{
			.Endpoint =
				{
					.bLength = sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t),
					.bDescriptorType = USB_DTYPE_Endpoint,

					.bEndpointAddress     = AUDIO_STREAM_SPEAKER_EPADDR,
					.bmAttributes          = (USB_EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_ASYNC | ENDPOINT_USAGE_DATA),
					.wMaxPacketSize        = AUDIO_STREAM_SPEAKER_EPSIZE,
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

			.Attributes               = (AUDIO_EP_SAMPLE_FREQ_CONTROL), //AUDIO_EP_ACCEPTS_SMALL_PACKETS

			.LockDelayUnits           = 0x00,
			.LockDelay                = 0x0000
		},

		//Microphone
		.Audio_MicStreamInterface_Alt0 =
		{
			.bLength                  = sizeof(USB_InterfaceDescriptor),
			.bDescriptorType          = USB_DTYPE_Interface,

			.bInterfaceNumber         = INTERFACE_ID_AudioStream_Mic,
			.bAlternateSetting        = 0,

			.bNumEndpoints            = 0,

			.bInterfaceClass          = AUDIO_CSCP_AudioClass,
			.bInterfaceSubClass       = AUDIO_CSCP_AudioStreamingSubclass,
			.bInterfaceProtocol       = AUDIO_CSCP_StreamingProtocol,

			.iInterface               = 0,
		},

	.Audio_MicStreamInterface_Alt1 =
		{
			.bLength                  = sizeof(USB_InterfaceDescriptor),
			.bDescriptorType          = USB_DTYPE_Interface,

			.bInterfaceNumber         = INTERFACE_ID_AudioStream_Mic,
			.bAlternateSetting        = 1,

			.bNumEndpoints            = 1,

			.bInterfaceClass          = AUDIO_CSCP_AudioClass,
			.bInterfaceSubClass       = AUDIO_CSCP_AudioStreamingSubclass,
			.bInterfaceProtocol       = AUDIO_CSCP_StreamingProtocol,

			.iInterface               = 0
		},

		.Audio_MicStreamInterface_SPC =
		{
			.Size                     = sizeof(USB_Audio_Descriptor_Interface_AS_t), 
			.Type                     = USB_DTYPE_CSInterface,
			.Subtype                  = AUDIO_DSUBTYPE_CSInterface_General,

			.TerminalLink             = 0x04,

			.FrameDelay               = 1,
			.AudioFormat              = 0x0001
		},

		.Audio_MicAudioFormat =
			{
				.Size                     = sizeof(USB_Audio_Descriptor_Format_t) +
				                            sizeof(configuration_descriptor.Audio_MicAudioFormatSampleRates),
				.Type                     = USB_DTYPE_CSInterface,
				.Subtype                  = AUDIO_DSUBTYPE_CSInterface_FormatType,

				.FormatType               = 0x01,
				.Channels                 = 0x01,

				.SubFrameSize             = 0x02,
				.BitResolution            = 16,

				.TotalDiscreteSampleRates = (sizeof(configuration_descriptor.Audio_MicAudioFormatSampleRates) / sizeof(USB_Audio_SampleFreq_t))
			},

		.Audio_MicAudioFormatSampleRates =
			{
				AUDIO_SAMPLE_FREQ(8000),
				AUDIO_SAMPLE_FREQ(11025),
				AUDIO_SAMPLE_FREQ(22050),
				AUDIO_SAMPLE_FREQ(44100),
				AUDIO_SAMPLE_FREQ(48000),
			},
// 
		.Audio_MicStreamEndpoint =
		{
			.Endpoint =
			{
				.bLength             = sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t), 
				.bDescriptorType     = USB_DTYPE_Endpoint,

				.bEndpointAddress    = AUDIO_STREAM_MIC_EPADDR,
				.bmAttributes        = (USB_EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_SYNC | ENDPOINT_USAGE_DATA),
				.wMaxPacketSize      = AUDIO_STREAM_MIC_EPSIZE,
				.bInterval           = 0x01
			},

			.Refresh               = 0,
			.SyncEndpointNumber    = 0
		},

	.Audio_MicStreamEndpoint_SPC =
		{
			.Size                     = sizeof(USB_Audio_Descriptor_StreamEndpoint_Spc_t),
			.Type                     = USB_DTYPE_CSEndpoint,
			.Subtype                  = AUDIO_DSUBTYPE_CSEndpoint_General,

			.Attributes               = (AUDIO_EP_SAMPLE_FREQ_CONTROL),

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



void usb_cb_control_in_completion(void) {

}

void usb_cb_control_out_completion(void) {

}

bool usb_cb_set_interface(uint16_t __attribute__((unused)) interface, uint16_t __attribute__((unused)) altsetting) {
	return false;
}
