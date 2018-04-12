/* vim: set sw=2 ts=2 si et: */
/*********************************************
* Analog conversion
*
* Modified by: Clancy Palmer
* Original Author: Guido Socher
* License: GPL
* Copyright: GPL
**********************************************/

#include <avr/io.h>
#include "analog.h"
#include "uart.h"

#define VTARGET_ADC_CHANNEL 0

// Return analog value of a given channel w/out interrupts 
unsigned int convertanalog(unsigned char channel, bool debug) 
{
  unsigned char msg_buf[16];

  /* Configure ADC reference and channel
   * REFS1 = 1, REFS0 = 1 - Internal 1.1V reference w/ external capacitor on AREF pin
   * MUX3, MUX2, MUX1, MUX0 = Channel
   */
	ADMUX = (1 << REFS1) | (1 << REFS0) | (channel & 0x0f);

  /* Configure ADC enabled with clock prescaler of 128 (Clock = 18.432MHz) to stay in
   * the recommended 50-200kHz range and no interrupts.
   * ADEN = 1 - ADC Enabled
   * ADPS2 = 1, ADPS1 = 1, ADPS0 = 1 - Clock prescaler of 128
   */
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

  // Start conversion
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1 << ADSC)); // Wait for result 
	unsigned char adlow = ADCL;   // Read low first 
	unsigned char adhigh = ADCH;  // Then read high

  if (debug) {
    uart_sendstr_p(PSTR("ADCL: "));
    utoa(adlow, (char *)msg_buf, 10);
    uart_sendstr((char *)msg_buf);
    uart_sendchar('\x1B');
    uart_sendchar('E');
    uart_sendstr_p(PSTR("ADCH: "));
    utoa(adhigh, (char *)msg_buf, 10);
    uart_sendstr((char *)msg_buf);
    uart_sendchar('\x1B');
    uart_sendchar('E');
  }

  uint16_t ret = (unsigned int)((adhigh << 8) | (adlow & 0xFF));
  
  if (debug) {
    uart_sendstr_p(PSTR("ADC: "));
    utoa(ret, (char *)msg_buf, 10);
    uart_sendstr((char *)msg_buf);
    uart_sendchar('\x1B');
    uart_sendchar('E');
  }

  return ret;
}


unsigned char analog2v(unsigned int aval)
{
  // VTGT = 5.0V
  // VADCPIN(47k/220k divider) = 5.0 * (47 / (47 + 220)) = 0.88014V
  // AVAL(1.1V ref, 10-bit ADC) = (0.88014 / 1.1) * 1024 = 819.339 = 819
  // 
  // AVAL = 819
  // VADCPIN = (819 / 1024) * 1.1 = 0.879785 - Percentage of 1.1V reference
  // VTGT = 0.879785 * ((47 + 220) / 47) = 4.99792 - Scaled back up from 47/220 divider
  // VTGT(*10 multiplier) = 4.99792 * 10 = 49.9792 - Multiplied by 10 for spec
  // RET(VTGT rounded) = ((49.9792 * 10) + 5) / 10 = 50.479 = 50 (matches 5.0V)
  //
  // VTGT = (AVAL / 1024 * 1.1) * (267 / 47) * 10
  // VTGT = AVAL * 1.1 * 10 * 267 / 47 / 1024 - Multiplication first to keep precision
  // VTGTx10 = (AVAL * 1.1 * 10 * 267 * 10) / (47 * 1024) - Scaled by 10 to keep precision
  // VTGTx10 = AVAL * 293700 / 48128 = AVAL * 73425 / 12032 = AVAL * 6.102476728
  // VTGTx10(approx) = AVAL * 6.1025 = AVAL * 6 * 41/400 - Close approximation to prevent overflow
  // VTGT(rounded) = ((AVAL * 6 * 41 / 400) + 5) / 10 - Rounded and scaled back down

  uint32_t r = ((aval * 6 * 41 / 400) + 5) / 10;
  return (unsigned char)(r & 0xff);
}

// Returns 1 if valid and 0 if invalid
unsigned char vtarget_valid(void)
{
  // 74ACH125 chip will handle 2-7V but
  // < 3.0V will not register logic 1 on MISO pin
  // > 5.5V will overvoltage MISO 
  unsigned char vtrg = vtarget_voltage();
  return (vtrg >= 30 && vtrg <= 55); // 3.0V - 5.5V
}

// Returns target voltage * 10 (ie. 3.3V = 33)
unsigned char vtarget_voltage(bool debug)
{
  return analog2v(convertanalog(VTARGET_ADC_CHANNEL));
}
unsigned char vtarget_voltage(void) {
  return vtarget_voltage(false);
}