/* vim: set sw=8 ts=8 si et: */
/*
* Serial Peripheral Interface for avrusb500 v2
* We do all spi operations in software. This
* gives more flexibility. The atmega8 is at 14MHz fast
* enough to do this. The C-code here is highly optimized
* for speed.
* Check the result with: avr-objdump -d main.out
* Author: Guido Socher, Copyright: GPL V2
*/
#include <avr/io.h>
#include <util/delay.h>
#include "timeout.h"
#include "spi.h"

static unsigned char sck_dur = 1;
static unsigned char d_sck_dur = 9;

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
    goto RET_RESULT;
  }
  if (dur >= 7) {
    // 20Khz
    sck_dur = 7;
    d_sck_dur = 150;
    goto RET_RESULT;
  }
  if (dur >= 3) {
    // 28KHz
    sck_dur = 3;
    d_sck_dur = 100;
    goto RET_RESULT;
  }
  if (dur >= 2) {
    // 57KHz
    sck_dur = 2;
    d_sck_dur = 60;
    goto RET_RESULT;
  }
  if (dur >= 1) {
    // 230KHz
    sck_dur = 1;
    d_sck_dur = 9;
    goto RET_RESULT;
  }
  if (dur < 1 ) {
    // 900KHz
    sck_dur = 0;
    d_sck_dur = 2;
    goto RET_RESULT;
  }
  // we should never come here
  sck_dur = 1;
  d_sck_dur = 9;
  //
RET_RESULT:
  return (sck_dur);
}

unsigned char spi_get_sck_duration(void)
{
  return (sck_dur);
}

void spi_sck_pulse(void)
{
  PORTD &= ~(1 << PD2); // SCK low
  _delay_loop_1(d_sck_dur);
  _delay_loop_1(d_sck_dur);
  PORTD |= (1 << PD2); // SCK high
  _delay_loop_1(d_sck_dur);
  _delay_loop_1(d_sck_dur);
  PORTD &= ~(1 << PD2); // SCK low
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
  PORTB |= (1 << PB0); // reset = high = not active
  _delay_loop_1(d_sck_dur);
  _delay_loop_1(d_sck_dur);
  PORTB &= ~(1 << PB0); // reset = low, stay active
  delay_ms(20); // min stab delay
}

void spi_init(void)
{
  // the connections for the sotware only spi are as follows:
  // reset=pb0
  // mosi =pd4
  // miso =pd3
  // sck  =pd2
  //
  DDRB |= (1 << DDB0); // reset as output
  PORTB |= (1 << PB0); // +5V, reset off
  //
  DDRD &= ~(1 << DDD3); // MISO is input
  // now output pins low in case somebody used it as output in his/her circuit
  DDRD |= (1 << DDD4); // MOSI is output
  PORTD &= ~(1 << PD4); // MOSI low
  DDRD |= (1 << DDD2); // SCK is output
  PORTD &= ~(1 << PD2); // SCK low
  delay_ms(10); // discharge MOSI/SCK
  //
  PORTB &= ~(1 << PB0); // reset = low, stay active
  delay_ms(20); // stab delay
  spi_reset_pulse();
}

// send 8 bit, no return value
void spi_mastertransmit_nr(unsigned char data)
{
  unsigned char i = 128;
  // software spi
  while (i != 0) {
    // MOSI
    if (data & i) {
      PORTD |= 1 << PD4;
    } else {
      // trans mit a zero
      PORTD &= ~(1 << PD4);
    }
    _delay_loop_1(d_sck_dur);
    _delay_loop_1(1); // make pulse len equal to spi_mastertransmit
    PORTD |= (1 << PD2); // SCK high
    _delay_loop_1(d_sck_dur);
    // make low and high pulse equaly long:
    _delay_loop_1(3);
    i = i >> 1;
    PORTD &= ~(1 << PD2); // SCK low
  }
}

// send 8 bit, return received byte
unsigned char spi_mastertransmit(unsigned char data)
{
  unsigned char i = 128;
  unsigned char rval = 0;
  // software spi
  while (i != 0) {
    // MOSI
    if (data & i) {
      PORTD |= 1 << PD4;
    } else {
      // trans mit a zero
      PORTD &= ~(1 << PD4);
    }
    _delay_loop_1(d_sck_dur);
    // read MISO
    if (PIND & (1 << PIND3)) {
      rval |= i;
    }
    PORTD |= (1 << PD2); // SCK high
    _delay_loop_1(d_sck_dur);
    _delay_loop_1(3); // make low and high pulse equaly long
    i = i >> 1;
    PORTD &= ~(1 << PD2); // SCK low
  }
  return (rval);
}

// send 16 bit, return last rec byte
void spi_mastertransmit_16_nr(unsigned int data)
{
  PORTD &= ~(1 << PD2); // SCK low
  spi_mastertransmit_nr((data >> 8) & 0xFF);
  spi_mastertransmit_nr(data & 0xFF);
}

// send 32 bit, return last rec byte
unsigned char spi_mastertransmit_32(unsigned long data)
{
  PORTD &= ~(1 << PD2); // SCK low
  spi_mastertransmit_nr((data >> 24) & 0xFF);
  spi_mastertransmit_nr((data >> 16) & 0xFF);
  spi_mastertransmit_nr((data >> 8) & 0xFF);
  return (spi_mastertransmit(data & 0xFF));
}

void spi_disable(void)
{
  // just to be sure:
  DDRD |= (1 << DDD2); // SCK is output
  PORTD &= ~(1 << PD2); // SCK low
  //
  PORTB |= (1 << PB0); // +5V, reset off
  // all other pins as input:
  DDRD &= ~(1 << DDD3); // MISO is input
  PORTD &= ~(1 << PIND3); // pullup off
  //
  DDRD &= ~(1 << DDD2); // SCK as input, high
  PORTD &= ~(1 << PIND2); // pullup off
  //
  DDRD &= ~(1 << DDD4); // MOSI as input, high
  PORTD &= ~(1 << PIND4); // pullup off
  delay_ms(20);
  // now we can also set reset high impedence:
  DDRB &= ~(1 << DDB0); // reset as input, high
  PORTB &= ~(1 << PINB0); // pullup off
}

