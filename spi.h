/* vim: set sw=2 ts=2 si et: */
/*********************************************
* Serial Peripheral Interface Include
*
* Modified by: Clancy Palmer
* Original Author: Guido Socher
* License: GPL
* Copyright: GPL
**********************************************/

#ifndef SPI_H
#define SPI_H

#define RST_LOW           PORTB &= ~(1 << PB0)        // Target reset pin low
#define RST_HIGH          PORTB |= (1 << PB0)         // Target reset pin high
#define SCK_DELAY         _delay_loop_1(d_sck_dur)    // Delay for d_sck_dur ms
#define SCK_LOW           PORTD &= ~(1 << PD2)        // SCK pin low
#define SCK_HIGH          PORTD |= (1 << PD2)         // SCK pin high
#define MISO_READ         PIND & (1 << PIND3)         // MISO pin value
#define MOSI_LOW          PORTD &= ~(1 << PD4)        // MOSI pin low
#define MOSI_HIGH         PORTD |= (1 << PD4)         // MOSI pin high

extern void spi_init(void);
extern unsigned char spi_set_sck_duration(unsigned char dur);
extern unsigned char spi_get_sck_duration(void);
extern void spi_mastertransmit_nr(unsigned char data);
extern unsigned char spi_mastertransmit(unsigned char data);
extern void spi_mastertransmit_16_nr(unsigned int data);
extern unsigned char spi_mastertransmit_32(unsigned long data);
extern void spi_disable(void);
extern void spi_reset_pulse(void);
extern void spi_sck_pulse(void);

#endif /* SPI_H */
