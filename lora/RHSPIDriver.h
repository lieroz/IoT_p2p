#ifndef RHSPIDriver_h
#define RHSPIDriver_h

#include "RHGenericDriver.h"
#include "RHHardwareSPI.h"

#define RH_SPI_WRITE_MASK 0x80

class RHSPIDriver : public RHGenericDriver
{
public:
    RHSPIDriver(uint8_t slaveSelectPin = SS);

    bool init();

    uint8_t spiRead(uint8_t reg);
    uint8_t spiWrite(uint8_t reg, uint8_t val);

    uint8_t spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len);
    uint8_t spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len);

    void setSlaveSelectPin(uint8_t slaveSelectPin);

private:
    void rpiCe0Ce1Fix();

protected:
    RHHardwareSPI _spi;
    uint8_t _slaveSelectPin;
};

#endif

