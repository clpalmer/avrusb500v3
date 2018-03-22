/* vim: set sw=2 ts=2 si et: */
/*********************************************
* UART interface without interrupts
*
* Modified by: Clancy Palmer
* Original Author: Guido Socher
* License: GPL
* Copyright: GPL
**********************************************/

#ifndef UART_H
#define UART_H
#include <avr/pgmspace.h>

extern void uart_init(void);
extern void uart_sendchar(char c);
extern void uart_sendstr(char *s);
extern void uart_sendstr_p(const char *progmem_s);
extern unsigned char uart_getchar(unsigned char kickwd);
extern void uart_flushRXbuf(void);
extern unsigned char prg_state_get(void);
extern void prg_state_set(unsigned char p);

#endif /* UART_H */
