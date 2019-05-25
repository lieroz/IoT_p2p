#include "RasPi.h"

#include <iostream>

#include <bcm2835.h>

#ifndef NOT_A_PIN
 #define NOT_A_PIN 0xFF
#endif

namespace tools
{
void pinMode(unsigned char pin, unsigned char mode)
{
    if (pin != NOT_A_PIN)
    {
        mode == OUTPUT
            ? bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP)
            : bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    }
}

void digitalWrite(unsigned char pin, unsigned char value)
{
    if (pin != NOT_A_PIN)
    {
        bcm2835_gpio_write(pin, value);
    }
}

unsigned char digitalRead(unsigned char pin)
{
    return pin == NOT_A_PIN ? 0 : bcm2835_gpio_lev(pin);
}

void delay(unsigned long ms)
{
    bcm2835_delay(ms);
}

void printbuffer(uint8_t buff[], int len)
{
    bool ascii = true;

    for (int i = 0; i < len; i++)
    {
        if (buff[i] < 32 || buff[i] > 127)
        {
            if (buff[i] != 0 || i != len - 1)
            {
                ascii = false;
                break;
            }
        }
    }

    for (int i = 0; i < len; i++)
    {
        if (ascii)
        {
            std::cout << buff[i];
        }
        else
        {
            std::cout << std::hex << buff[i] << std::dec;
        }
    }
}
}
