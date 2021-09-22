#ifndef Q_AUDIO_CONTROL_H__
#define Q_AUDIO_CONTROL_H__ 1

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <cfg/usb_sound_cfg.h>

typedef struct audio_buffer
{
	uint8_t audio_buffer[AUDIO_STREAM_SPEAKER_EPSIZE];
	volatile uint16_t size;
} audio_buffer_t;

typedef struct audio_memory
{
	audio_buffer_t buffer[AUDIO_BUFFER_INSTANCES];
	uint8_t mic_buffer0[AUDIO_STREAM_MIC_EPSIZE];
	uint8_t mic_buffer1[AUDIO_STREAM_MIC_EPSIZE];
	uint16_t mcu_sync_counter;

	bool mcu_synced;
	uint32_t mcu_clock;
	
	volatile uint32_t mic_sampleRate;
	volatile uint16_t mic_teoriticalCLKsPerSample;
	volatile uint16_t mic_middleCLKsPerSample;
	
	volatile uint32_t speaker_sampleRate;
	volatile uint16_t speaker_teoriticalCLKsPerSample;
	volatile uint16_t speaker_middleCLKsPerSample;

	//For the fifo
	volatile uint8_t last_completed_bank; // = AUDIO_BUFFER_INSTANCES - 1
	volatile uint8_t last_converted_bank; // = AUDIO_BUFFER_INSTANCES - 1
	volatile uint8_t last_free_bank; // = AUDIO_BUFFER_INSTANCES - 1
	
	//For status
	volatile bool speaker_interface_enabled;
	volatile bool mic_interface_enabled;
	
	volatile uint8_t _bank_in_work1;
	volatile uint8_t _bank_in_work0;
} audio_memory_t;


extern audio_memory_t audio_mem;

extern void audio_init(void);

extern bool audio_setup_out(void);

extern void audio_unknown_setup(const char *where);

extern void audio_out_setup_samplerate(USB_EP_t *ep);

extern void audio_work(void);

extern void audio_data_out_sync_clk0(USB_EP_t *ep);
extern void audio_data_out_sync_clk1(USB_EP_t *ep);

#endif
