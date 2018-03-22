/* vim: set sw=2 ts=2 si et: */
/*********************************************
* Serial Peripheral Interface for avrusb500 v2
* We do all spi operations in software. This
* gives more flexibility. The ATMega88 is at 18.432MHz is fast
* enough to do this. The C-code here is highly optimized
* for speed.
*
* Modified by: Clancy Palmer
* Original Author: Guido Socher
* License: GPL
* Copyright: GPL
**********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "timeout.h"
#include "spi.h"

static unsigned char sck_dur = 1;
static unsigned char d_sck_dur = 9;

void spi_disable(void)
{
  // Just to be sure:
  DDRD |= (1 << DDD2); // SCK is output
  SCK_LOW;

  RST_HIGH;               // +5V, reset off

  // All other pins as input:
  DDRD &= ~(1 << DDD3);   // MISO is input
  PORTD &= ~(1 << PIND3); // Pullup off
  DDRD &= ~(1 << DDD2);   // SCK as input
  PORTD &= ~(1 << PIND2); // Pullup off
  DDRD &= ~(1 << DDD4);   // MOSI as input
  PORTD &= ~(1 << PIND4); // Pullup off

  delay_ms(20);

  // Now we can also set reset high impedence:
  DDRB &= ~(1 << DDB0);   // Reset as input
  PORTB &= ~(1 << PINB0); // Pullup off

  // Disable SN74ACH125N outputs
  DDRD &= ~(1 << DDD5);   // PD5 is input
  PORTD &= ~(1 << PIND5); // Pullup off

}

void spi_init(void)
{
  // The connections for the sotware only SPI are as follows:
  // Reset = PB0
  // MOSI  = PD4
  // MISO  = PD3
  // SCK   = PD2

  // Configure target reset pin
  DDRB |= (1 << DDB0);    // Reset pin as output
  RST_HIGH;               // +5V, reset off

  // Configure SPI pins
  DDRD &= ~(1 << DDD3);   // MISO is input
  DDRD |= (1 << DDD4);    // MOSI is output
  MOSI_LOW;               // MOSI low in case target uses as output
  DDRD |= (1 << DDD2);    // SCK is output
  SCK_LOW;                // SCK low in case target uses as output
  
  // Enable SN74ACH125N outputs
  DDRD |= (1 << DDD5);    // PD5 is output
  PORTD &= ~(1 << PIND5); // Low to enable outputs

  delay_ms(10);           // discharge MOSI/SCK
  RST_LOW;                // Reset = low, stay active
  delay_ms(20);           // stab delay
  spi_reset_pulse();
}

// the mapping between SCK freq and SCK_DURATION is for avrisp as follows:
// this table is for 14.7 MHz:
// sck_dur       sck-freq   d_sck_dur
// 0              921kHz     1
//                520kHz     2
//                370kHz     4
//                280kHz     6
// 1              230kHz     7
//                100kHz     22
// 2              57kHz      50
// 3              28kHz      80
// 4              27kHz      90
// 5              26kHz      100
// 7              20kHz      120
// 8              18kHz      140
// 10             15kHz      160
// 15             10kHz      240
// 19             9 kHz      254
// 25             6 kHz
// 50             3 kHz
// 100            1.5kHz
unsigned char  spi_set_sck_duration(unsigned char dur)
{
  if (dur >= 10) {
    // 10Khz
    sck_dur = 15;
    d_sck_dur = 253;
  } else if (dur >= 7) {
    // 20Khz
    sck_dur = 7;
    d_sck_dur = 150;
  } else if (dur >= 3) {
    // 28KHz
    sck_dur = 3;
    d_sck_dur = 100;
  } else if (dur >= 2) {
    // 57KHz
    sck_dur = 2;
    d_sck_dur = 60;
  } else if (dur >= 1) {
    // 230KHz
    sck_dur = 1;
    d_sck_dur = 9;
  } else if (dur < 1 ) {
    // 900KHz
    sck_dur = 0;
    d_sck_dur = 2;
  } else {
    // we should never come here
    sck_dur = 1;
    d_sck_dur = 9;
  }

  return (sck_dur);
}

unsigned char spi_get_sck_duration(void)
{
  return (sck_dur);
}

void spi_sck_pulse(void)
{
  SCK_LOW;
  SCK_DELAY;
  SCK_DELAY;
  SCK_HIGH;
  SCK_DELAY;
  SCK_DELAY;
  SCK_LOW;
}

void spi_reset_pulse(void)
{
  /* give a positive RESET pulse, we can't guarantee
   * that SCK was low during power up (see Atmel's
   * data sheets, search for "Serial Downloading in e.g atmega8
   * data sheet):
   * "... the programmer can not guarantee that SCK is held low during
   * Power-up. In this case, RESET must be given a positive pulse of at least two
   * CPU clock cycles duration after SCK has been set to 0."
   * */
  RST_HIGH; // reset = high = not active
  SCK_DELAY;
  SCK_DELAY;
  RST_LOW;  // reset = low, stay active
  delay_ms(20); // min stab delay
}

// Send 8 bits, no receive
unsigned char spi_xmit(unsigned char data, unsigned char read)
{
  unsigned char i = 128;
  unsigned char rval = 0;
  // software spi
  while (i != 0) {
    // Transmit a 1 or 0
    if (data & i) {
      MOSI_HIGH;
    } else {
      MOSI_LOW;
    }
    SCK_DELAY;

    if (read) {
      if (MISO_READ) {
        rval |= i;
      }
    } else {
      _delay_loop_1(1); // make pulse len equal to read case
    }

    SCK_HIGH;
    SCK_DELAY;
    _delay_loop_1(3);// make low and high pulse equaly long:
    SCK_LOW;
    i = i >> 1;
  }

  return rval;
}

void spi_mastertransmit_nr(unsigned char data)
{
  spi_xmit(data, 0);
}
// Send 8 bits, return received byte
unsigned char spi_mastertransmit(unsigned char data)
{
  return spi_xmit(data, 1);
}

// Send 16 bit, no read
void spi_mastertransmit_16_nr(unsigned int data)
{
  SCK_LOW;
  spi_mastertransmit_nr((data >> 8) & 0xFF);
  spi_mastertransmit_nr(data & 0xFF);
}

// Send 32 bit, return last read byte
unsigned char spi_mastertransmit_32(unsigned long data)
{
  SCK_LOW;
  spi_mastertransmit_nr((data >> 24) & 0xFF);
  spi_mastertransmit_nr((data >> 16) & 0xFF);
  spi_mastertransmit_nr((data >> 8) & 0xFF);
  return spi_mastertransmit(data & 0xFF);
}
