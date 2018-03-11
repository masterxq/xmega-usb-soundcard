#ifndef Q_AUDIO_MIC_H__
#define Q_AUDIO_MIC_H__ 1

#include <avr/io.h>

extern void audio_mic_interface_enable(void);
extern void audio_mic_interface_disable(void);

extern void audio_mic_reset(void);

extern void audio_mic_init(void);

extern void audio_mic_callback0(USB_EP_t *ep);

#endif
