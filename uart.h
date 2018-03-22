/* vim: set sw=8 ts=8 si et: */
/*************************************************************************
 Title:   C include file for uart
 Target:    atmega8
 Copyright: GPL
***************************************************************************/
#ifndef UART_H
#define UART_H
#include <avr/pgmspace.h>

extern void uart_init(void);
extern void uart_sendchar(char c);
extern void uart_sendstr(char *s);
extern void uart_sendstr_p(const char *progmem_s);
extern unsigned char uart_getchar(unsigned char kickwd);
extern void uart_flushRXbuf(void);

#endif /* UART_H */
