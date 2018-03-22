/* vim: set sw=8 ts=8 si et: */
/*************************************************************************
 Title:   C include file for spi
 Target:    atmega8
 Copyright: GPL
***************************************************************************/
#ifndef SPI_H
#define SPI_H

extern void spi_init(void);
#define spi_scklow() PORTD&=~(1<<PD2)
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
