#ifndef RHHardwareSPI_h
#define RHHardwareSPI_h

#include "RHGenericSPI.h"

class RHHardwareSPI : public RHGenericSPI
{
public:
    RHHardwareSPI(Frequency frequency = Frequency1MHz,
            BitOrder bitOrder = BitOrderMSBFirst,
            DataMode dataMode = DataMode0);

    uint8_t transfer(uint8_t data);
    void begin();
    void end();

private:
    uint8_t setAndGetDataMode();
    uint8_t setAndGetBitOrder();
    uint32_t setAndGetDivider();
};

#endif
