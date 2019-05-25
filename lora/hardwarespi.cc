#include "hardwarespi.h"

#include <bcm2835.h>

RHHardwareSPI::RHHardwareSPI()
    : divider(BCM2835_SPI_CLOCK_DIVIDER_256),
    bitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST),
    dataMode(BCM2835_SPI_MODE0)
{
}

void RHHardwareSPI::begin()
{
    setClockDivider(divider);
    setBitOrder(bitOrder);
    setDataMode(dataMode);

    bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);
    bcm2835_spi_begin();
}

uint8_t RHHardwareSPI::transfer(uint8_t data)
{
    bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);
    return bcm2835_spi_transfer(data);
}

void RHHardwareSPI::end()
{
    bcm2835_spi_end();
}

void RHHardwareSPI::setClockDivider(uint16_t rate)
{
    divider = rate;
    bcm2835_spi_setClockDivider(rate);
}

void RHHardwareSPI::setBitOrder(uint8_t order)
{
    bitOrder = order;
    bcm2835_spi_setBitOrder(bitOrder);
}

void RHHardwareSPI::setDataMode(uint8_t mode)
{
    dataMode = mode;
    bcm2835_spi_setDataMode(dataMode);
}

