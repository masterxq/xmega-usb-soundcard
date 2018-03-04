#ifndef __AUDIO_H__
#define __AUDIO_H__ 1

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <cfg/usb_sound_cfg.h>

// #define DEBUG_SYSTEM 1

#define DEBUG_FILTER 1

enum audio_buffer_state
{
	audio_buffer_empty = 0,
	audio_buffer_filled = 1,
	audio_buffer_converted = 2,
};

typedef struct audio_buffer
{
	uint8_t audio_buffer[AUDIO_STREAM_EPSIZE];
	volatile uint16_t size;
} audio_buffer_t;

typedef struct audio_memory
{
	audio_buffer_t buffer[AUDIO_BUFFER_INSTANCES];
	uint16_t mcu_sync_counter;
// 	uint16_t lastDownloadCompleteSampleNum;
// 	uint16_t lastDownloadCompleteCLK;
// 	uint16_t lastPlayedCompleteSampleNum;
// 	uint16_t lastPlayedCompleteCLK;

	bool mcu_synced;
	uint32_t mcu_clock;
	
	uint32_t sampleRate;

	volatile uint16_t teoriticalCLKsPerSample;
	volatile uint16_t middleCLKsPerSample;

	//For the fifo
	volatile uint8_t last_completed_bank; // = AUDIO_BUFFER_INSTANCES - 1
	volatile uint8_t last_converted_bank; // = AUDIO_BUFFER_INSTANCES - 1
	volatile uint8_t last_free_bank; // = AUDIO_BUFFER_INSTANCES - 1
	
	volatile bool usb_stopped;
	volatile bool interface_enabled;
	
	volatile uint8_t _bank_in_work1;
	volatile uint8_t _bank_in_work0;
} audio_memory_t;


extern audio_memory_t audio_mem;

extern bool audio_setup_out(void);

extern void audio_init(void);

extern void audio_reset(void);

extern void audio_start_dac_dma(void);

extern void audio_work(void);

extern void audio_init_dma(void);

#endif
