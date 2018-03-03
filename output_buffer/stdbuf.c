#include "stdbuf.h"
#include <stdio.h>


stdbuf_mem_t stdbuf_memory;

int stdbuf_printf(char c_out, FILE *stream)
{
	stdbuf_memory.buffer[stdbuf_memory.writepos++] = c_out;
	return 0;
}

stdbuf_out_func external_func = NULL;

FILE stdbuf_out = FDEV_SETUP_STREAM(stdbuf_printf, NULL, _FDEV_SETUP_WRITE );

void stdbuf_work(void)
{
	while(stdbuf_memory.readpos != stdbuf_memory.writepos)
	{
		external_func(stdbuf_memory.buffer[stdbuf_memory.readpos++]);
	}
}

void stdbuf_init(stdbuf_out_func out_func)
{
	stdout = &stdbuf_out;
	external_func = out_func;
	stdbuf_memory.writepos = 0;
	stdbuf_memory.readpos  = 0;
}
