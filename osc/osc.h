#ifndef __OSC_H_
#define __OSC_H_

#include <avr/eeprom.h>

void ext_osc_init(void);

void int32_osc_init(void);

void int48_ppl_overclock(void);

void int48_ppl_overclock_PER2_2x(void);

void int6x_ppl_overclock_PER2_2x_USB48(uint8_t x);

void int2x_ppl_USB48(uint8_t x);
void int2x_ppl_USB48_2(uint8_t x);

void int2x_USB48(uint8_t x6_P2_x12);

void int32_ppl_PER2_2x(void);

void CCPWrite( volatile uint8_t * address, uint8_t value );

void int48_ppl_USB48(void);

#endif
