#ifndef Q_AUDIO_H__
#define Q_AUDIO_H__ 1

extern void audio_init(void);

extern void audio_reset(void);

extern void audio_reset_mic(void);

extern void audio_start_dac_dma(void);

extern void audio_work(void);

extern void audio_init_dma(void);

#endif
