#include "RHHardwareSPI.h"

RHHardwareSPI::RHHardwareSPI(Frequency frequency,
        BitOrder bitOrder, DataMode dataMode)
    : RHGenericSPI(frequency, bitOrder, dataMode)
{
}

uint8_t setAndGetDataMode()
{
    uint8_t dataMode = 0;

    switch (_dataMode)
    {
        case DataMode0:
            dataMode = BCM2835_SPI_MODE0;
            break;
        case DataMode1:
            dataMode = BCM2835_SPI_MODE1;
            break;
        case DataMode2:
            dataMode = BCM2835_SPI_MODE2;
            break;
        case DataMode3:
            dataMode = BCM2835_SPI_MODE3;
            break;
        default:
            break;
    }

    return dataMode;
}

uint8_t setAndGetBitOrder()
{
    uint8_t bitOrder = 0;

    if (_bitOrder == BitOrderLSBFirst)
    {
        bitOrder = BCM2835_SPI_BIT_ORDER_LSBFIRST;
    }
    else
    {
        bitOrder = BCM2835_SPI_BIT_ORDER_MSBFIRST;
    }

    return bitOrder;
}

uint32_t setAndGetDivider()
{
    switch (_frequency)
    {
    case Frequency1MHz:
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
    default:
        break;
    }

    return divider;
}

uint8_t RHHardwareSPI::transfer(uint8_t data)
{
    return SPI.transfer(data);
}

void RHHardwareSPI::begin()
{
    uint8_t dataMode = setAndGetDataMode();
    uint8_t bitOrder = setAndGetBitOrder();
    uint32_t divider = setAndGetDivider();
    SPI.begin(divider, bitOrder, dataMode);
}

void RHHardwareSPI::end()
{
    SPI.end();
}

