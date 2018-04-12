/* vim: set sw=2 ts=2 si et: */
/*********************************************
* Analog conversion
*
* Modified by: Clancy Palmer
* Original Author: Guido Socher
* License: GPL
* Copyright: GPL
**********************************************/

#ifndef ANALOG_H
#define ANALOG_H

#include <stdint.h>

extern unsigned char vtarget_valid(void);         // Return 1 if target voltage is valid, 0 otherwise
extern unsigned char vtarget_voltage(void);       // Return target voltage * 10
extern unsigned char vtarget_voltage(bool debug); // Return target voltage, optionally printing debug traces

#endif /* ANALOG_H */
