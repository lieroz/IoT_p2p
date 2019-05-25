#pragma once

#include <cstdint>

class RHHardwareSPI
{
public:
    RHHardwareSPI();

    void begin();
    uint8_t transfer(uint8_t data);
    void end();

    void setClockDivider(uint16_t rate);
    void setBitOrder(uint8_t order);
    void setDataMode(uint8_t mode);

private:
    uint8_t bitOrder;
    uint8_t dataMode;
    uint16_t divider;
};

