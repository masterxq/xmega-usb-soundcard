#include <uart/uart.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

receive_f_ptr uart_receive_func = 0;

#if _USARTC0_EN
int _c0_char_out(char c_out, FILE* stream);
FILE _c0_std_file_out = FDEV_SETUP_STREAM( _c0_char_out, NULL, _FDEV_SETUP_WRITE );
receive_f_ptr uart_c0_rx;
#endif
#if _USARTC1_EN
int _c1_char_out(char c_out, FILE* stream);
FILE _c1_std_file_out = FDEV_SETUP_STREAM( _c1_char_out, NULL, _FDEV_SETUP_WRITE );
receive_f_ptr uart_c1_rx;
#endif

#if _USARTD0_EN
int _d0_char_out(char c_out, FILE* stream);
FILE _d0_std_file_out = FDEV_SETUP_STREAM( _d0_char_out, NULL, _FDEV_SETUP_WRITE );
receive_f_ptr uart_d0_rx;
#endif
#if _USARTD1_EN
int _d1_char_out(char c_out, FILE* stream);
FILE _d1_std_file_out = FDEV_SETUP_STREAM( _d1_char_out, NULL, _FDEV_SETUP_WRITE );
receive_f_ptr uart_d1_rx;
#endif

#if _USARTE0_EN
int _e0_char_out(char c_out, FILE* stream);
FILE _e0_std_file_out = FDEV_SETUP_STREAM( _e0_char_out, NULL, _FDEV_SETUP_WRITE );
receive_f_ptr uart_e0_rx;
#endif
#if _USARTE1_EN
int _e1_char_out(char c_out, FILE* stream);
FILE _e1_std_file_out = FDEV_SETUP_STREAM( _e1_char_out, NULL, _FDEV_SETUP_WRITE );
receive_f_ptr uart_e1_rx;
#endif

#if _USARTF0_EN
int _f0_char_out(char c_out, FILE* stream);
FILE _f0_std_file_out = FDEV_SETUP_STREAM( _f0_char_out, NULL, _FDEV_SETUP_WRITE );
receive_f_ptr uart_f0_rx;
#endif
#if _USARTF1_EN
int _f1_char_out(char c_out, FILE* stream);
FILE _f1_std_file_out = FDEV_SETUP_STREAM( _f1_char_out, NULL, _FDEV_SETUP_WRITE );
receive_f_ptr uart_f1_rx;
#endif


//USART_RXCINTLVL_HI_gc, USART_RXCINTLVL_MED_gc, USART_RXCINTLVL_LO_gc, USART_RXCINTLVL_OFF_gc
void uart_init(USART_t * usart_ptr, uint32_t baud, USART_RXCINTLVL_t irq_lvl, receive_f_ptr rx_f_ptr, bool redirect_stdout, bool redirect_stderr)
{
  FILE *out;
  switch((uint16_t)usart_ptr)
  {
    //UART on port C
#if _USARTC0_EN
    case (uint16_t)&USARTC0:
      PORTC.DIRCLR = PIN2_bm;
      PORTC.DIRSET = PIN3_bm;
      out = &_c0_std_file_out;
      break;
#endif
#if _USARTC1_EN
    case (uint16_t)&USARTC1:
      PORTC.DIRCLR = PIN6_bm;
      PORTC.DIRSET = PIN7_bm;
      out = &_c1_std_file_out;
      break;
#endif

#if _USARTD0_EN
    case (uint16_t)&USARTD0:
      PORTD.DIRCLR = PIN2_bm;
      PORTD.DIRSET = PIN3_bm;
      out = &_d0_std_file_out;
      break;
#endif
#if _USARTD1_EN
    case (uint16_t)&USARTD1:
      PORTD.DIRCLR = PIN6_bm;
      PORTD.DIRSET = PIN7_bm;
      out = &_d1_std_file_out;
      break;
#endif

#if _USARTE0_EN
    case (uint16_t)&USARTE0:
      PORTE.DIRCLR = PIN2_bm;
      PORTE.DIRSET = PIN3_bm;
      out = &_e0_std_file_out;
      break;
#endif
#if _USARTE1_EN
    case (uint16_t)&USARTE1:
      PORTE.DIRCLR = PIN6_bm;
      PORTE.DIRSET = PIN7_bm;
      out = &_e1_std_file_out;
      break;
#endif

#if _USARTF0_EN
    case (uint16_t)&USARTF0:
      PORTE.DIRCLR = PIN2_bm;
      PORTE.DIRSET = PIN3_bm;
      out = &_f0_std_file_out;
      break;
#endif
#if _USARTF1_EN
    case (uint16_t)&USARTF1:
      PORTE.DIRCLR = PIN6_bm;
      PORTE.DIRSET = PIN7_bm;
      out = &_f1_std_file_out;
      break;
#endif
    default:
      //TODO: Change to debug out...
      fprintf_P(stderr, PSTR("USART Not available or not provided\n"));
      return;
  }
  
  uart_set_receive_func(usart_ptr, irq_lvl, rx_f_ptr);
  
  uint16_t val = (((uint32_t)F_CPU * 2) + baud*8)/(baud*16) - 1;
  usart_ptr->BAUDCTRLA = val & 0xFF;                                                           //Baudrate einstellen
  usart_ptr->BAUDCTRLB = val >> 8;        
  usart_ptr->CTRLB = USART_TXEN_bm | USART_RXEN_bm | USART_CLK2X_bm;                                //RX+TX Enable CLK
  usart_ptr->CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc;                            //8N1
  
  if(redirect_stdout)
  {
    stdout = out;
  }
  
  if(redirect_stderr)
  {
    stderr = out;
  }
}

inline void _uart_char_out(USART_t *usart_ptr, uint8_t c_out)
{
  if(c_out == '\n')
  {
    while(!(usart_ptr->STATUS & USART_DREIF_bm));
      usart_ptr->DATA = '\r';
  }
  while(!(usart_ptr->STATUS & USART_DREIF_bm));
    usart_ptr->DATA = c_out;
}

void uart_char_out(USART_t *usart_ptr, uint8_t c_out)
{
  _uart_char_out(usart_ptr, c_out);
}

void uart_byte_out(USART_t *usart_ptr, uint8_t c_out)
{
  while(!(usart_ptr->STATUS & USART_DREIF_bm));
    usart_ptr->DATA = c_out;

}

void uart_var_out(USART_t *usart_ptr, int x)
{
  char VarinDec[10];
  itoa(x, VarinDec, 10);
  uart_string_out(usart_ptr, VarinDec);
}


void uart_string_out(USART_t *usart_ptr, char *s)
{
  while (*s)
  {
    uart_char_out(usart_ptr, *s);
    s++;
  }
}

void uart_block_out(USART_t *usart_ptr, void *s, size_t size)
{
  size_t i;
  uint8_t *data = s;
  for(i = 0; i < size; i++)
  {
    uart_byte_out(usart_ptr, *data);
    data++;
  }
}

void uart_string_out_E(USART_t* usart_ptr, unsigned char* eep_ptr)
{
  unsigned char byte = eeprom_read_byte(eep_ptr);
  while(byte != '\0')
  {
    uart_char_out(usart_ptr, byte);
    eep_ptr++;
    byte = eeprom_read_byte(eep_ptr);
  }
}

void uart_string_out_P(USART_t* usart_ptr, const unsigned char* pgm_ptr)
{
  unsigned char byte = pgm_read_byte(pgm_ptr);
  while(byte != '\0')
  {
    uart_char_out(usart_ptr, byte);
    pgm_ptr++;
    byte = pgm_read_byte(pgm_ptr);
  }
}


uint8_t uart_read_byte(USART_t *usart_ptr)
{
  while(!(usart_ptr->STATUS & USART_RXCIF_bm))
    ;
  return usart_ptr->DATA;
}

void uart_print_bin64(USART_t *usart_ptr, int64_t n)
{
  uint8_t i;
  for(i = 0; i < 64; i++)
  {
    if(n < 0)
    {
      uart_byte_out(usart_ptr, '1');
    }
    else
    {
      uart_byte_out(usart_ptr, '0');
    }
    n = n << 1;
  }
}

void uart_print_bin8(USART_t *usart_ptr, int8_t n)
{
  uint8_t i;
  for(i = 0; i < 8; i++)
  {
    if(n < 0)
    {
      uart_byte_out(usart_ptr, '1');
    }
    else
    {
      uart_byte_out(usart_ptr, '0');
    }
    n = n << 1;
  }
}

#if _USARTC0_EN
int _c0_char_out(char c_out, FILE *stream)
{
  _uart_char_out(&USARTC0, c_out);
  return 0;
}
void uart_c0_byte_out(uint8_t byte)
{
	if(byte == '\n')
  {
    while(!(USARTC0.STATUS & USART_DREIF_bm));
      USARTC0.DATA = '\r';
  }
  while(!(USARTC0.STATUS & USART_DREIF_bm));
    USARTC0.DATA = byte;
}
#endif
#if _USARTC1_EN
int _c1_char_out(char c_out, FILE *stream)
{
  _uart_char_out(&USARTC1, c_out);
  return 0;
}
#endif

#if _USARTD0_EN
int _d0_char_out(char c_out, FILE *stream)
{
  _uart_char_out(&USARTD0, c_out);
  return 0;
}
#endif
#if _USARTD1_EN
int _d1_char_out(char c_out, FILE *stream)
{
  _uart_char_out(&USARTD1, c_out);
  return 0;
}
#endif

#if _USARTE0_EN
int _e0_char_out(char c_out, FILE *stream)
{
  _uart_char_out(&USARTE0, c_out);
  return 0;
}
#endif
#if _USARTE1_EN
int _e1_char_out(char c_out, FILE *stream)
{
  _uart_char_out(&USARTE1, c_out);
  return 0;
}
#endif

#if _USARTF0_EN
int _f0_char_out(char c_out, FILE *stream)
{
  _uart_char_out(&USARTF0, c_out);
  return 0;
}
#endif
#if _USARTF1_EN
int _f1_char_out(char c_out, FILE *stream)
{
  _uart_char_out(&USARTF1, c_out);
  return 0;
}
#endif

void uart_set_receive_func(USART_t* usart_ptr, USART_RXCINTLVL_t irq_lvl,  receive_f_ptr receive_func)
{
  switch((uint16_t)usart_ptr)
  {
    //UART on port C
#if _USARTC0_EN
    case (uint16_t)&USARTC0:
      uart_c0_rx = receive_func;
      break;
#endif
#if _USARTC1_EN
    case (uint16_t)&USARTC1:
      uart_c1_rx = receive_func;
      break;
#endif

#if _USARTD0_EN
    case (uint16_t)&USARTD0:
      uart_d0_rx = receive_func;
      break;
#endif
#if _USARTD1_EN
    case (uint16_t)&USARTD1:
      uart_d1_rx = receive_func;
      break;
#endif

#if _USARTE0_EN
    case (uint16_t)&USARTE0:
      uart_e0_rx = receive_func;
      break;
#endif
#if _USARTE1_EN
    case (uint16_t)&USARTE1:
      uart_e1_rx = receive_func;
      break;
#endif

#if _USARTF0_EN
    case (uint16_t)&USARTF0:
      uart_f0_rx = receive_func;
      break;
#endif
#if _USARTF1_EN
    case (uint16_t)&USARTF1:
      uart_f1_rx = receive_func;
      break;
#endif
    default:
      //TODO: Change to debug out...
      fprintf_P(stderr, PSTR("USART Not available or not provided\n"));
      return;
  }
  usart_ptr->CTRLA = irq_lvl;
}


#if _USARTC0_EN
ISR(USARTC0_RXC_vect)
{
  uart_c0_rx(USARTC0.DATA);
}
#endif

#if _USARTC1_EN
ISR(USARTC1_RXC_vect)
{
  uart_c1_rx(USARTC1.DATA);
}
#endif


#if _USARTD0_EN
ISR(USARTD0_RXC_vect)
{
  uart_d0_rx(USARTD0.DATA);
}
#endif

#if _USARTD1_EN
ISR(USARTD1_RXC_vect)
{
  uart_d1_rx(USARTD1.DATA);
}
#endif


#if _USARTE0_EN
ISR(USARTE0_RXC_vect)
{
  uart_e0_rx(USARTE0.DATA);
}
#endif

#if _USARTE1_EN
ISR(USARTE1_RXC_vect)
{
  uart_e1_rx(USARTE1.DATA);
}
#endif


#if _USARTF0_EN
ISR(USARTF0_RXC_vect)
{
  uart_f0_rx(USARTF0.DATA);
}
#endif

#if _USARTF1_EN
ISR(USARTF1_RXC_vect)
{
  uart_f1_rx(USARTF1.DATA);
}
#endif


