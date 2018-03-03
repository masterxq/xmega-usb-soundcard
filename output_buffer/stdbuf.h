#ifndef __STD_BUF_H__
#define __STD_BUF_H__ 1

#include <stdint.h>

typedef void (*stdbuf_out_func)(uint8_t out);


typedef struct stdbuf_mem
{
	volatile uint8_t readpos;
	volatile uint8_t writepos;
	uint8_t buffer[256];
} stdbuf_mem_t;

extern stdbuf_mem_t stdbuf_memory;

extern void stdbuf_init(stdbuf_out_func out_func);
extern void stdbuf_work(void);
#endif
