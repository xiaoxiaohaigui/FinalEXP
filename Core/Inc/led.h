#ifndef LED_H
#define LED_H

#include <stdint.h>

void LED_Flow(uint8_t state);

void LED_Binary_Display(uint16_t value);

void LED_Turn_off_All(void);

#endif // LED_H