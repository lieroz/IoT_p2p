#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <bcm2835.h>

#ifndef OUTPUT
 #define OUTPUT BCM2835_GPIO_FSEL_OUTP
#endif

#ifndef INPUT
 #define INPUT BCM2835_GPIO_FSEL_INPT
#endif

#ifndef NOT_A_PIN
 #define NOT_A_PIN 0xFF
#endif

void RasPiSetup();
void pinMode(unsigned char pin, unsigned char mode);
void digitalWrite(unsigned char pin, unsigned char value);
unsigned char digitalRead(unsigned char pin) ;
unsigned long millis();
void delay (unsigned long delay);
long random(long min, long max);
void printbuffer(uint8_t buff[], int len);

