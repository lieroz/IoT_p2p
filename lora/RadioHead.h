#ifndef RadioHead_h
#define RadioHead_h

#define RH_VERSION_MAJOR 1
#define RH_VERSION_MINOR 67

#define RH_PLATFORM_RASPI 9
#define RH_PLATFORM RH_PLATFORM_RASPI

#define RH_HAVE_HARDWARE_SPI
#define RH_HAVE_SERIAL
#define PROGMEM

#include "RasPi.h"
#include <string.h>
#define SS 8

#define YIELD

#define NOT_AN_INTERRUPT -1
#define digitalPinToInterrupt(p) (p)

#undef abs
#undef round
#undef double

#define htons(x) (((x) << 8) | (((x) >> 8) & 0xFF))
#define ntohs(x) htons(x)
#define htonl(x) (((x) << 24 & 0xFF000000UL) | \
                   ((x) << 8 & 0x00FF0000UL) | \
                   ((x) >> 8 & 0x0000FF00UL) | \
                  ((x) >> 24 & 0x000000FFUL))
#define ntohl(x) htonl(x)

#define ATOMIC_BLOCK_START
#define ATOMIC_BLOCK_END

#define RH_BROADCAST_ADDRESS 0xff

#endif
