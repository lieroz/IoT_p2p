#include "RHSPIDriver.h"

RHSPIDriver::RHSPIDriver(uint8_t slaveSelectPin)
    : _slaveSelectPin(slaveSelectPin)
{
}

bool RHSPIDriver::init()
{
    _spi.begin();

    pinMode(_slaveSelectPin, OUTPUT);
    digitalWrite(_slaveSelectPin, HIGH);

    return true;
}

uint8_t RHSPIDriver::spiRead(uint8_t reg)
{
    uint8_t val;
    rpiCe0Ce1Fix();

    digitalWrite(_slaveSelectPin, LOW);
    _spi.transfer(reg & ~RH_SPI_WRITE_MASK);
    val = _spi.transfer(0);
    digitalWrite(_slaveSelectPin, HIGH);

    return val;
}

uint8_t RHSPIDriver::spiWrite(uint8_t reg, uint8_t val)
{
    uint8_t status = 0;
    rpiCe0Ce1Fix();

    digitalWrite(_slaveSelectPin, LOW);
    status = _spi.transfer(reg | RH_SPI_WRITE_MASK);
    _spi.transfer(val);
    digitalWrite(_slaveSelectPin, HIGH);

    return status;
}

uint8_t RHSPIDriver::spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len)
{
    uint8_t status = 0;
    rpiCe0Ce1Fix();

    digitalWrite(_slaveSelectPin, LOW);
    status = _spi.transfer(reg & ~RH_SPI_WRITE_MASK);

    while (len--)
    {
	    *dest++ = _spi.transfer(0);
    }

    digitalWrite(_slaveSelectPin, HIGH);

    return status;
}

uint8_t RHSPIDriver::spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len)
{
    uint8_t status = 0;
    rpiCe0Ce1Fix();

    digitalWrite(_slaveSelectPin, LOW);
    status = _spi.transfer(reg | RH_SPI_WRITE_MASK);

    while (len--)
    {
	    _spi.transfer(*src++);
    }

    digitalWrite(_slaveSelectPin, HIGH);

    return status;
}

void RHSPIDriver::setSlaveSelectPin(uint8_t slaveSelectPin)
{
    _slaveSelectPin = slaveSelectPin;
}

void RHSPIDriver::rpiCe0Ce1Fix()
{
    if (_slaveSelectPin != 7)
    {
        bcm2835_gpio_fsel(7, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(7, HIGH);
    }
    if (_slaveSelectPin != 8)
    {
        bcm2835_gpio_fsel(8, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(8,HIGH);
    }
}

