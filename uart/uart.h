#ifndef __UART_H_
#define __UART_H_ 1
#include <stdio.h>
#include <stdint.h>
#include <avr/eeprom.h>
#include <stdbool.h>
#include <cfg/QA_uart_cfg.h>

// #ifndef __AVR_ATxmega192A3U__
//  #include <avr/iox192a3u.h>
// #endif

typedef void (*receive_f_ptr)(uint8_t data);

/*////////////////////////////////
//Funktions Ruempfe		//
////////////////////////////////*/

#ifdef USARTC0
  #define _USARTC0_EN USARTC0_PROVIDED
  extern void uart_c0_byte_out(uint8_t byte);
#else
  #define _USARTC0_EN 0
#endif

#ifdef USARTC1
  #define _USARTC1_EN USARTC1_PROVIDED
#else
  #define _USARTC1_EN 0
#endif

#ifdef USARTD0
  #define _USARTD0_EN USARTD0_PROVIDED
#else
  #define _USARTD0_EN 0
#endif

#ifdef USARTD1
  #define _USARTD1_EN USARTD1_PROVIDED
#else
  #define _USARTD1_EN 0
#endif

#ifdef USARTE0
  #define _USARTE0_EN USARTE0_PROVIDED
#else
  #define _USARTE0_EN 0
#endif

#ifdef USARTE1
  #define _USARTE1_EN USARTE1_PROVIDED
#else
  #define _USARTE1_EN 0
#endif

#ifdef USARTF0
  #define _USARTF0_EN USARTF0_PROVIDED
#else
  #define _USARTF0_EN 0
#endif

#ifdef USARTF1
  #define _USARTF1_EN USARTF1_PROVIDED
#else
  #define _USARTF1_EN 0
#endif

void uart_init(USART_t* usart_ptr, uint32_t baud, USART_RXCINTLVL_t irq_lvl, receive_f_ptr rx_f_ptr, bool redirect_stdout, bool redirect_stderr);

void uart_byte_out(USART_t* usart_ptr, uint8_t c_out);
void uart_char_out(USART_t *usart_ptr, uint8_t c_out);
void uart_var_out(USART_t* usart_ptr, int x);
void uart_string_out(USART_t* usart_ptr, char* s);
void uart_string_out_E(USART_t* usart_ptr, unsigned char* eep_ptr);
void uart_string_out_P(USART_t* usart_ptr, const unsigned char* pgm_ptr);

void uart_block_out(USART_t *usart_ptr, void *s, size_t size);

uint8_t uart_read_byte(USART_t* usart_ptr);
void uart_print_bin64(USART_t* usart_ptr, int64_t n);

void uart_print_bin8(USART_t* usart_ptr, int8_t n);








void uart_set_receive_func(USART_t* usart_ptr, USART_RXCINTLVL_t irq_lvl, receive_f_ptr receive_func);

#ifndef __QA_UART_CFG_H_
#error No configuration loaded. Please add an uart Configuration before loading QA_uart. Take a look at QA/uart/QA_uart_cfg_template.h for a sample config.
#else

/*////////////////////////////////
//Die berechnung der		//
//Baudrate, ist die Fehler	//
//Quote hoeher als 1%, bricht	//
//der compiler ab.		//
////////////////////////////////*/


// #define F_CPU_2 F_CPU * 2	// Falls ein double up für uart configuriert wurde mit 2 Multiplizieren
// // Berechnungen
// 
// #define UBRR_VAL ((F_CPU_2+uart_baud*8)/(uart_baud*16)-1)   // clever runden
// #define BAUD_REAL (F_CPU_2/(16*(UBRR_VAL+1)))     // Reale Baudrate
// #define BAUD_ERROR ( ( BAUD_REAL * 1000 ) / uart_baud) // Fehler in Promille, 1000 = kein Fehler.
// 
// #if (( BAUD_ERROR < 990 ) || ( BAUD_ERROR > 1010 ))
//   #error Systematischer Fehler der Baudrate grösser 1% und damit zu hoch!
// #endif
//////////////////////////////////
#endif




// /*////////////////////////////////
// // Uart empfangs Pufferund co   //
// ////////////////////////////////*/
// volatile unsigned char uart_rx_buffer[UART_BUFFER_SIZE + 1];
// volatile unsigned char uart_rx_index;
// volatile unsigned char uart_rx_complete;
// //////////////////////////////////







#endif
