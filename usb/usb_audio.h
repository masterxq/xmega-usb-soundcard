#ifndef __USB_AUDIO_H__
#define __USB_AUDIO_H__ 1

#include <stdint.h>
#include "usb_standard.h"
//Audio stuff

#define AUDIO_SAMPLE_FREQ(freq)           {.Byte1 = ((uint32_t)freq & 0xFF), .Byte2 = (((uint32_t)freq >> 8) & 0xFF), .Byte3 = (((uint32_t)freq >> 16) & 0xFF)}

enum Audio_Descriptor_ClassSubclassProtocol_t
{
	AUDIO_CSCP_AudioClass                     = 0x01, /**< Descriptor Class value indicating that the device or
																											*   interface belongs to the USB Audio 1.0 class.
																											*/
	AUDIO_CSCP_ControlSubclass                = 0x01, /**< Descriptor Subclass value indicating that the device or
																											*   interface belongs to the Audio Control subclass.
																											*/
	AUDIO_CSCP_ControlProtocol                = 0x00, /**< Descriptor Protocol value indicating that the device or
																											*   interface belongs to the Audio Control protocol.
																											*/
	AUDIO_CSCP_AudioStreamingSubclass         = 0x02, /**< Descriptor Subclass value indicating that the device or
																											*   interface belongs to the MIDI Streaming subclass.
																											*/
	AUDIO_CSCP_MIDIStreamingSubclass          = 0x03, /**< Descriptor Subclass value indicating that the device or
																											*   interface belongs to the Audio streaming subclass.
																											*/
	AUDIO_CSCP_StreamingProtocol              = 0x00, /**< Descriptor Protocol value indicating that the device or
																											*   interface belongs to the Streaming Audio protocol.
																											*/
};

enum Audio_CSEndpoint_SubTypes_t
{
	AUDIO_DSUBTYPE_CSEndpoint_General         = 0x01, /**< Audio class specific endpoint general descriptor. */
};

/** Audio class specific interface description subtypes, for the Audio Streaming interface. */
enum Audio_CSInterface_AS_SubTypes_t
{
	AUDIO_DSUBTYPE_CSInterface_General        = 0x01, /**< Audio class specific streaming interface general descriptor. */
	AUDIO_DSUBTYPE_CSInterface_FormatType     = 0x02, /**< Audio class specific streaming interface format type descriptor. */
	AUDIO_DSUBTYPE_CSInterface_FormatSpecific = 0x03, /**< Audio class specific streaming interface format information descriptor. */
};

typedef struct
{
	uint8_t Size; /**< Size of the descriptor, in bytes. */
	uint8_t Type; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
									*   given by the specific class.
									*/
	uint8_t                 Subtype; /**< Sub type value used to distinguish between audio class-specific descriptors,
																*   a value from the \ref Audio_CSInterface_AS_SubTypes_t enum.
																*/

	uint8_t                 TerminalLink; /**< ID value of the output terminal this descriptor is describing. */

	uint8_t                 FrameDelay; /**< Delay in frames resulting from the complete sample processing from input to output. */
	uint16_t                AudioFormat; /**< Format of the audio stream, see Audio Device Formats specification. */
} __attribute__ ((packed)) USB_Audio_Descriptor_Interface_AS_t;

typedef struct
{
	uint8_t Size; /**< Size of the descriptor, in bytes. */
	uint8_t Type; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
									*   given by the specific class.
									*/
	uint8_t                 Subtype; /**< Sub type value used to distinguish between audio class-specific descriptors,
																		*   a value from the \ref Audio_CSInterface_AS_SubTypes_t enum.
																		*/

	uint16_t                ACSpecification; /**< Binary Coded Decimal value, indicating the supported Audio Class specification version.
																						*
																						*   \see \ref VERSION_BCD() utility macro.
																						*/
	uint16_t                TotalLength; /**< Total length of the Audio class-specific descriptors, including this descriptor. */

	uint8_t                 InCollection; /**< Total number of Audio Streaming interfaces linked to this Audio Control interface (must be 1). */
	uint8_t                 InterfaceNumber; /**< Interface number of the associated Audio Streaming interface. */
} __attribute__ ((packed)) USB_Audio_Descriptor_Interface_AC_t;

typedef struct
{
	uint8_t Size; /**< Size of the descriptor, in bytes. */
	uint8_t Type;
	uint8_t                 Subtype; /**< Sub type value used to distinguish between audio class-specific descriptors,
																		*   must be \ref AUDIO_DSUBTYPE_CSInterface_InputTerminal.
																		*/

	uint8_t                 TerminalID; /**< ID value of this terminal unit - must be a unique value within the device. */
	uint16_t                TerminalType; /**< Type of terminal, a \c TERMINAL_* mask. */
	uint8_t                 AssociatedOutputTerminal; /**< ID of associated output terminal, for physically grouped terminals
																											*   such as the speaker and microphone of a phone handset.
																											*/
	uint8_t                 TotalChannels; /**< Total number of separate audio channels within this interface (right, left, etc.) */
	uint16_t                ChannelConfig; /**< \c CHANNEL_* masks indicating what channel layout is supported by this terminal. */

	uint8_t                 ChannelStrIndex; /**< Index of a string descriptor describing this channel within the device. */
	uint8_t                 TerminalStrIndex; /**< Index of a string descriptor describing this descriptor within the device. */
} __attribute__ ((packed)) USB_Audio_Descriptor_InputTerminal_t;

typedef struct
{
	uint8_t Size; /**< Size of the descriptor, in bytes. */
	uint8_t Type;
	uint8_t                 Subtype; /**< Sub type value used to distinguish between audio class-specific descriptors,
																		*   must be \ref AUDIO_DSUBTYPE_CSInterface_OutputTerminal.
																		*/

	uint8_t                 TerminalID; /**< ID value of this terminal unit - must be a unique value within the device. */
	uint16_t                TerminalType; /**< Type of terminal, a \c TERMINAL_* mask. */
	uint8_t                 AssociatedInputTerminal; /**< ID of associated input terminal, for physically grouped terminals
																											*   such as the speaker and microphone of a phone handset.
																											*/
	uint8_t                 SourceID; /**< ID value of the unit this terminal's audio is sourced from. */

	uint8_t                 TerminalStrIndex; /**< Index of a string descriptor describing this descriptor within the device. */
} __attribute__ ((packed)) USB_Audio_Descriptor_OutputTerminal_t;

typedef struct
{
	uint8_t  bLength; /**< Size of the descriptor, in bytes. */
	uint8_t  bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
															*   given by the specific class.
															*/

	uint8_t  bDescriptorSubtype; /**< Sub type value used to distinguish between audio class-specific descriptors,
																*   a value from the \ref Audio_CSInterface_AS_SubTypes_t enum.
																*/

	uint8_t  bTerminalLink; /**< ID value of the output terminal this descriptor is describing. */

	uint8_t  bDelay; /**< Delay in frames resulting from the complete sample processing from input to output. */
	uint16_t wFormatTag; /**< Format of the audio stream, see Audio Device Formats specification. */
} __attribute__ ((packed)) USB_Audio_StdDescriptor_Interface_AS_t;


typedef struct
{
	uint8_t Size; /**< Size of the descriptor, in bytes. */
	uint8_t Type;
	uint8_t                 Subtype; /**< Sub type value used to distinguish between audio class-specific descriptors,
																		*   must be \ref AUDIO_DSUBTYPE_CSInterface_FormatType.
																		*/

	uint8_t                 FormatType; /**< Format of the audio stream, see Audio Device Formats specification. */
	uint8_t                 Channels; /**< Total number of discrete channels in the stream. */

	uint8_t                 SubFrameSize; /**< Size in bytes of each channel's sample data in the stream. */
	uint8_t                 BitResolution; /**< Bits of resolution of each channel's samples in the stream. */

	uint8_t                 TotalDiscreteSampleRates; /**< Total number of discrete sample frequencies supported by the device. When
																											*   zero, this must be followed by the lower and upper continuous sampling
																											*   frequencies supported by the device; otherwise, this must be followed
																											*   by the given number of discrete sampling frequencies supported.
																											*/
} __attribute__ ((packed)) USB_Audio_Descriptor_Format_t;

typedef struct
{
	uint8_t Byte1; /**< Lowest 8 bits of the 24-bit value. */
	uint8_t Byte2; /**< Middle 8 bits of the 24-bit value. */
	uint8_t Byte3; /**< Upper 8 bits of the 24-bit value. */
} __attribute__ ((packed)) USB_Audio_SampleFreq_t;

typedef struct
{
	USB_EndpointDescriptor    Endpoint; /**< Standard endpoint descriptor describing the audio endpoint. */

	uint8_t                   Refresh; /**< Always set to zero for Audio class devices. */
	uint8_t                   SyncEndpointNumber; /**< Endpoint address to send synchronization information to, if needed (zero otherwise). */
} __attribute__ ((packed)) USB_Audio_Descriptor_StreamEndpoint_Std_t;


typedef struct
{
	uint8_t Size; /**< Size of the descriptor, in bytes. */
	uint8_t Type;
	uint8_t                 Subtype; /**< Sub type value used to distinguish between audio class-specific descriptors,
																		*   a value from the \ref Audio_CSEndpoint_SubTypes_t enum.
																		*/

	uint8_t                 Attributes; /**< Audio class-specific endpoint attributes, such as \ref AUDIO_EP_FULL_PACKETS_ONLY. */

	uint8_t                 LockDelayUnits; /**< Units used for the LockDelay field, see Audio class specification. */
	uint16_t                LockDelay; /**< Time required to internally lock endpoint's internal clock recovery circuitry. */
} __attribute__ ((packed)) USB_Audio_Descriptor_StreamEndpoint_Spc_t;

enum Audio_ClassRequests_t
{
	AUDIO_REQ_SetCurrent    = 0x01, /**< Audio class-specific request to set the current value of a parameter within the device. */
	AUDIO_REQ_SetMinimum    = 0x02, /**< Audio class-specific request to set the minimum value of a parameter within the device. */
	AUDIO_REQ_SetMaximum    = 0x03, /**< Audio class-specific request to set the maximum value of a parameter within the device. */
	AUDIO_REQ_SetResolution = 0x04, /**< Audio class-specific request to set the resolution value of a parameter within the device. */
	AUDIO_REQ_SetMemory     = 0x05, /**< Audio class-specific request to set the memory value of a parameter within the device. */
	AUDIO_REQ_GetCurrent    = 0x81, /**< Audio class-specific request to get the current value of a parameter within the device. */
	AUDIO_REQ_GetMinimum    = 0x82, /**< Audio class-specific request to get the minimum value of a parameter within the device. */
	AUDIO_REQ_GetMaximum    = 0x83, /**< Audio class-specific request to get the maximum value of a parameter within the device. */
	AUDIO_REQ_GetResolution = 0x84, /**< Audio class-specific request to get the resolution value of a parameter within the device. */
	AUDIO_REQ_GetMemory     = 0x85, /**< Audio class-specific request to get the memory value of a parameter within the device. */
	AUDIO_REQ_GetStatus     = 0xFF, /**< Audio class-specific request to get the device status. */
};

enum Audio_CSInterface_AC_SubTypes_t
{
	AUDIO_DSUBTYPE_CSInterface_Header         = 0x01, /**< Audio class specific control interface header. */
	AUDIO_DSUBTYPE_CSInterface_InputTerminal  = 0x02, /**< Audio class specific control interface Input Terminal. */
	AUDIO_DSUBTYPE_CSInterface_OutputTerminal = 0x03, /**< Audio class specific control interface Output Terminal. */
	AUDIO_DSUBTYPE_CSInterface_Mixer          = 0x04, /**< Audio class specific control interface Mixer Unit. */
	AUDIO_DSUBTYPE_CSInterface_Selector       = 0x05, /**< Audio class specific control interface Selector Unit. */
	AUDIO_DSUBTYPE_CSInterface_Feature        = 0x06, /**< Audio class specific control interface Feature Unit. */
	AUDIO_DSUBTYPE_CSInterface_Processing     = 0x07, /**< Audio class specific control interface Processing Unit. */
	AUDIO_DSUBTYPE_CSInterface_Extension      = 0x08, /**< Audio class specific control interface Extension Unit. */
};

enum Audio_EndpointControls_t
{
	AUDIO_EPCONTROL_SamplingFreq = 0x01, /**< Sampling frequency adjustment of the endpoint. */
	AUDIO_EPCONTROL_Pitch        = 0x02, /**< Pitch adjustment of the endpoint. */
};

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_LEFT_FRONT           (1 << 0)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_RIGHT_FRONT          (1 << 1)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_CENTER_FRONT         (1 << 2)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_LOW_FREQ_ENHANCE     (1 << 3)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_LEFT_SURROUND        (1 << 4)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_RIGHT_SURROUND       (1 << 5)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_LEFT_OF_CENTER       (1 << 6)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_RIGHT_OF_CENTER      (1 << 7)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_SURROUND             (1 << 8)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_SIDE_LEFT            (1 << 9)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_SIDE_RIGHT           (1 << 10)

/** Supported channel mask for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_CHANNEL_TOP                  (1 << 11)
//@}

/** \name Audio Feature Masks */
//@{
/** Supported feature mask for an Audio class feature unit descriptor. See the Audio class specification for more details. */
#define AUDIO_FEATURE_MUTE                 (1 << 0)

/** Supported feature mask for an Audio class feature unit descriptor. See the Audio class specification for more details. */
#define AUDIO_FEATURE_VOLUME               (1 << 1)

/** Supported feature mask for an Audio class feature unit descriptor. See the Audio class specification for more details. */
#define AUDIO_FEATURE_BASS                 (1 << 2)

/** Supported feature mask for an Audio class feature unit descriptor. See the Audio class specification for more details. */
#define AUDIO_FEATURE_MID                  (1 << 3)

/** Supported feature mask for an Audio class feature unit descriptor. See the Audio class specification for more details. */
#define AUDIO_FEATURE_TREBLE               (1 << 4)

/** Supported feature mask for an Audio class feature unit descriptor. See the Audio class specification for more details. */
#define AUDIO_FEATURE_GRAPHIC_EQUALIZER    (1 << 5)

/** Supported feature mask for an Audio class feature unit descriptor. See the Audio class specification for more details. */
#define AUDIO_FEATURE_AUTOMATIC_GAIN       (1 << 6)

/** Supported feature mask for an Audio class feature unit descriptor. See the Audio class specification for more details. */
#define AUDIO_FEATURE_DELAY                (1 << 7)

/** Supported feature mask for an Audio class feature unit descriptor. See the Audio class specification for more details. */
#define AUDIO_FEATURE_BASS_BOOST           (1 << 8)

/** Supported feature mask for an Audio class feature unit descriptor. See the Audio class specification for more details. */
#define AUDIO_FEATURE_BASS_LOUDNESS        (1 << 9)


/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_UNDEFINED           0x0100

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_STREAMING           0x0101

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_VENDOR              0x01FF

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_IN_UNDEFINED        0x0200

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_IN_MIC              0x0201

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_IN_DESKTOP_MIC      0x0202

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_IN_PERSONAL_MIC     0x0203

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_IN_OMNIDIR_MIC      0x0204

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_IN_MIC_ARRAY        0x0205

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_IN_PROCESSING_MIC   0x0206

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_IN_OUT_UNDEFINED    0x0300

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_OUT_SPEAKER         0x0301

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_OUT_HEADPHONES      0x0302

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_OUT_HEAD_MOUNTED    0x0303

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_OUT_DESKTOP         0x0304

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_OUT_ROOM            0x0305

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_OUT_COMMUNICATION   0x0306

/** Terminal type constant for an Audio class terminal descriptor. See the Audio class specification for more details. */
#define AUDIO_TERMINAL_OUT_LOWFREQ         0x0307

#define AUDIO_EP_FULL_PACKETS_ONLY        (1 << 7)

/** Mask for the attributes parameter of an Audio class-specific Endpoint descriptor, indicating that the endpoint
	*  will accept partially filled endpoint packets of audio samples.
	*/
#define AUDIO_EP_ACCEPTS_SMALL_PACKETS    (0 << 7)

/** Mask for the attributes parameter of an Audio class-specific Endpoint descriptor, indicating that the endpoint
	*  allows for sampling frequency adjustments to be made via control requests directed at the endpoint.
	*/
#define AUDIO_EP_SAMPLE_FREQ_CONTROL      (1 << 0)

/** Mask for the attributes parameter of an Audio class-specific Endpoint descriptor, indicating that the endpoint
	*  allows for pitch adjustments to be made via control requests directed at the endpoint.
	*/
#define AUDIO_EP_PITCH_CONTROL            (1 << 1)


#endif

