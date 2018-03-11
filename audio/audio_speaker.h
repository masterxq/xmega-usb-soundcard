#ifndef Q_AUDIO_SPEAKER_H__
#define Q_AUDIO_SPEAKER_H__ 1

#include <avr/io.h>
// #define DEBUG_SYSTEM 1

#define DEBUG_FILTER 1


extern void audio_speaker_interface_enable(void);
extern void audio_speaker_interface_disable(void);
extern void audio_speaker_reset(void);
extern void audio_speaker_init(void);

extern void audio_speaker_start_dac_dma(void);
extern void audio_speaker_work(void);
extern void audio_speaker_data_out0(USB_EP_t *ep);
extern void audio_speaker_data_out1(USB_EP_t *ep);
#endif
