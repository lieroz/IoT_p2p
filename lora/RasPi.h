#pragma once

#include <stdint.h>

#ifndef OUTPUT
 #define OUTPUT BCM2835_GPIO_FSEL_OUTP
#endif

#ifndef INPUT
 #define INPUT BCM2835_GPIO_FSEL_INPT
#endif

namespace tools
{
void pinMode(unsigned char pin, unsigned char mode);

void digitalWrite(unsigned char pin, unsigned char value);

unsigned char digitalRead(unsigned char pin) ;

void delay (unsigned long delay);

void printbuffer(uint8_t buff[], int len);
}
