/* vim: set sw=2 ts=2 si et: */
/*********************************************
* UART interface without interrupts
*
* Modified by: Clancy Palmer
* Original Author: Guido Socher
* License: GPL
* Copyright: GPL
**********************************************/

#include <avr/interrupt.h>
#include <string.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include "timeout.h"
#include "uart.h"
#include "analog.h"
#include "led.h"

static unsigned char prg_state = 0;  // 0 = Idle, 1 = Programming

unsigned char prg_state_get(void)
{
  return prg_state;
}

void prg_state_set(unsigned char p)
{
  prg_state = p;
}

void uart_init(void)
{
  // baud=9=115.2K with an external 18.4320MHz crystal
  unsigned int baud = 9;

  UBRR0H = (unsigned char) (baud >> 8);
  UBRR0L = (unsigned char) (baud & 0xFF);
  /* enable tx/rx and no interrupt on tx/rx */
  UCSR0B =  (1 << RXEN0) | (1 << TXEN0);
  /* format: asynchronous, 8N1 */
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

/* send one character to the rs232 */
void uart_sendchar(char c)
{
  /* wait for empty transmit buffer */
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}
/* send string to the rs232 */
void uart_sendstr(char *s)
{
  while (*s) {
    uart_sendchar(*s);
    s++;
  }
}

void uart_sendstr_p(const char *progmem_s)
/* print string from program memory on rs232 */
{
  char c;
  while ((c = pgm_read_byte(progmem_s++))) {
    uart_sendchar(c);
  }
}

/* get a byte from rs232. This function does a blocking read */
unsigned char uart_getchar(unsigned char kickwd)
{
  unsigned char l = 1;
  while (!(UCSR0A & (1 << RXC0))) {
    // we can not aford a watchdog timeout because this is a blocking function
    if (kickwd) {
      wdt_reset();
    }

    if (prg_state_get()) {
      // Programming is ongoing
      continue;
    }

    if (l == 0) {
      // Once every 256th loop, l will wrap at 8 bit
      if (vtarget_valid()) {
        LED_ON;
      } else {
        LED_OFF;
      }
    }

    l++;
  }
  return (UDR0);
}
/* read and discard any data in the receive buffer */
void uart_flushRXbuf(void)
{
  // __attribute__ to avoid compiler warning
  unsigned char tmp __attribute__((unused));
  while (UCSR0A & (1 << RXC0)) {
    tmp = UDR0;
  }
}

