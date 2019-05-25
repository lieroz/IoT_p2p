#include "RasPi.h"

#include <sys/time.h>
#include <time.h>

#include "RadioHead.h"

#ifndef NOT_A_PIN
 #define NOT_A_PIN 0xFF
#endif

timeval RHStartTime;

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

unsigned long millis()
{
    //Declare a variable to store current time
    struct timeval RHCurrentTime;
    //Get current time
    gettimeofday(&RHCurrentTime, NULL);
    //Calculate the difference between our start time and the end time
    unsigned long difference = ((RHCurrentTime.tv_sec - RHStartTime.tv_sec) * 1000);
    difference += ((RHCurrentTime.tv_usec - RHStartTime.tv_usec) / 1000);

    //Return the calculated value
    return difference;
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
            printf("%c", buff[i]);
        }
        else
        {
            printf(" %02X", buff[i]);
        }
    }
}

