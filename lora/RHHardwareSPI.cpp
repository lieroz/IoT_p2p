#include "RHHardwareSPI.h"

RHHardwareSPI hardware_spi;

#define SPI_CLOCK_DIV16 (VARIANT_MCK/84000000) // 1MHz
#define SPI_CLOCK_DIV8  (VARIANT_MCK/42000000) // 2MHz
#define SPI_CLOCK_DIV4  (VARIANT_MCK/21000000) // 4MHz
#define SPI_CLOCK_DIV2  (VARIANT_MCK/10500000) // 8MHz
#define SPI_CLOCK_DIV1  (VARIANT_MCK/5250000)  // 16MHz

RHHardwareSPI::RHHardwareSPI(Frequency frequency, BitOrder bitOrder, DataMode dataMode)
    :
    RHGenericSPI(frequency, bitOrder, dataMode)
{
}

uint8_t RHHardwareSPI::transfer(uint8_t data) 
{
    return SPI.transfer(data);
}

void RHHardwareSPI::attachInterrupt() 
{
}

void RHHardwareSPI::detachInterrupt() 
{
}
    
void RHHardwareSPI::begin() 
{
  uint8_t dataMode;
  if (_dataMode == DataMode0)
    dataMode = BCM2835_SPI_MODE0;
  else if (_dataMode == DataMode1)
    dataMode = BCM2835_SPI_MODE1;
  else if (_dataMode == DataMode2)
    dataMode = BCM2835_SPI_MODE2;
  else if (_dataMode == DataMode3)
    dataMode = BCM2835_SPI_MODE3;

  uint8_t bitOrder;
  if (_bitOrder == BitOrderLSBFirst)
    bitOrder = BCM2835_SPI_BIT_ORDER_LSBFIRST;
  else
    bitOrder = BCM2835_SPI_BIT_ORDER_MSBFIRST;

  uint32_t divider;
  switch (_frequency)
  {
    case Frequency1MHz:
    default:
      divider = BCM2835_SPI_CLOCK_DIVIDER_256;
      break;
    case Frequency2MHz:
      divider = BCM2835_SPI_CLOCK_DIVIDER_128;
      break;
    case Frequency4MHz:
      divider = BCM2835_SPI_CLOCK_DIVIDER_64;
      break;
    case Frequency8MHz:
      divider = BCM2835_SPI_CLOCK_DIVIDER_32;
      break;
    case Frequency16MHz:
      divider = BCM2835_SPI_CLOCK_DIVIDER_16;
      break;
  }
  SPI.begin(divider, bitOrder, dataMode);
}

void RHHardwareSPI::end() 
{
    SPI.end();
}

