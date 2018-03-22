/* vim: set sw=2 ts=2 si et: */
/*********************************************
* Timer for timeout supervision of the stk 500 protocol
*
* Modified by: Clancy Palmer
* Original Author: Guido Socher
* License: GPL
* Copyright: GPL
**********************************************/

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
