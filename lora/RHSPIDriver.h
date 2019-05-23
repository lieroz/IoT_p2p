#ifndef RHSPIDriver_h
#define RHSPIDriver_h

#include "RHGenericDriver.h"
#include "RHHardwareSPI.h"

#define RH_SPI_WRITE_MASK 0x80

#define RPI_CE0_CE1_FIX { \
          if (_slaveSelectPin!=7) {   \
            bcm2835_gpio_fsel(7,BCM2835_GPIO_FSEL_OUTP); \
            bcm2835_gpio_write(7,HIGH); \
          }                           \
          if (_slaveSelectPin!=8) {   \
            bcm2835_gpio_fsel(8,BCM2835_GPIO_FSEL_OUTP); \
            bcm2835_gpio_write(8,HIGH); \
          }                           \
        }

class RHGenericSPI;

class RHSPIDriver : public RHGenericDriver
{
public:
    RHSPIDriver(uint8_t slaveSelectPin = SS, RHGenericSPI& spi = hardware_spi);

    bool init();

    uint8_t        spiRead(uint8_t reg);

    uint8_t           spiWrite(uint8_t reg, uint8_t val);

    uint8_t           spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len);

    uint8_t           spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len);

    void setSlaveSelectPin(uint8_t slaveSelectPin);

protected:
    RHGenericSPI&       _spi;
    uint8_t             _slaveSelectPin;
};

#endif
