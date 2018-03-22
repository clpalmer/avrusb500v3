/* vim: set sw=8 ts=8 si et: */
/*
* Timer for timeout supervision of the stk 500 protocol
* Author: Guido Socher, Copyright: GPL V2
*/

#include <util/delay.h>
#include "timeout.h"

/* delay for a minimum of <ms> */
void delay_ms(unsigned int ms)
{
  // Calibrated macro that is more accurate and not as compiler dependent as self made code.
  while (ms) {
    _delay_ms(0.96);
    ms--;
  }
}
