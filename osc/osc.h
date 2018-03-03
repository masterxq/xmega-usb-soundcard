#ifndef __OSC_H_
#define __OSC_H_

#include <avr/eeprom.h>

#include <stdint.h>

void ext_osc_init(void);

void CCPWrite( volatile uint8_t * address, uint8_t value );


//Good clocks
void int2x_USB48(uint8_t x6_P2_x12);
void int2x_ppl_USB48(uint8_t x2);



#endif
