#ifndef __QDEF_TYPES_H__
#define __QDEF_TYPES_H__ 1

#include <stdint.h>

// #define Qfalse 0
// #define Qtrue 1

typedef union uint16_u
{
  uint16_t u16;
  uint8_t  u8[2];
} uint16_u;

typedef union uint24_u
{
	__int24 u24;
	uint8_t u8[3];
} uint24_u;

typedef enum { Qfalse = 0, Qtrue = 1, } Qbool;

typedef volatile Qbool QVbool;

#define Qnull 0

#endif
