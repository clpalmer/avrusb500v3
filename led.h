/* vim: set sw=2 ts=2 si et: */
/*********************************************
* LED helper defines
*
* Modified by: Clancy Palmer
* Original Author: Guido Socher
* License: GPL
* Copyright: GPL
**********************************************/

#ifndef LED_H
#define LED_H
#include <avr/io.h>
#define LED_PORT    PORTB
#define LED_DIR     DDRB
#define LED_PIN     PB1
#define LED_INIT    LED_DIR |= (1 << LED_PIN)
#define LED_ON      LED_PORT &= ~(1 << LED_PIN)
#define LED_OFF     LED_PORT |= (1 << LED_PIN)
#define LED_TOGGLE  LED_PORT ^= (1 << LED_PIN)
#endif //LED_H
