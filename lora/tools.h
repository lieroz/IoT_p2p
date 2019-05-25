#pragma once

#include <stdint.h>

#ifndef OUTPUT
 #define OUTPUT BCM2835_GPIO_FSEL_OUTP
#endif

#ifndef INPUT
 #define INPUT BCM2835_GPIO_FSEL_INPT
#endif

#define RF_LED_PIN RPI_V2_GPIO_P1_16 // Led on GPIO23 so P1 connector pin #16
#define RF_CS_PIN  RPI_V2_GPIO_P1_24 // Slave Select on CE0 so P1 connector pin #24
#define RF_IRQ_PIN RPI_V2_GPIO_P1_22 // IRQ on GPIO25 so P1 connector pin #22
#define RF_RST_PIN RPI_V2_GPIO_P1_15 // IRQ on GPIO22 so P1 connector pin #15

namespace tools
{
void pinMode(unsigned char pin, unsigned char mode);

void digitalWrite(unsigned char pin, unsigned char value);

unsigned char digitalRead(unsigned char pin) ;

void delay (unsigned long delay);

void printbuffer(uint8_t buff[], int len);
}
