#pragma once

#include "RadioHead.h"

class RHHardwareSPI
{
public:
    typedef enum
    {
        DataMode0 = 0,
        DataMode1,
        DataMode2,
        DataMode3,
    } DataMode;

    typedef enum
    {
        Frequency1MHz = 0,
        Frequency2MHz,
        Frequency4MHz,
        Frequency8MHz,
        Frequency16MHz,
    } Frequency;

    typedef enum
    {
        BitOrderMSBFirst = 0,
        BitOrderLSBFirst,
    } BitOrder;

    RHHardwareSPI(Frequency frequency = Frequency1MHz,
            BitOrder bitOrder = BitOrderMSBFirst,
            DataMode dataMode = DataMode0);

    uint8_t transfer(uint8_t data);
    void begin();
    void end();

    void setBitOrder(BitOrder bitOrder);
    void setDataMode(DataMode dataMode);
    void setFrequency(Frequency frequency);

private:
    uint8_t setAndGetDataMode();
    uint8_t setAndGetBitOrder();
    uint32_t setAndGetDivider();

private:
    Frequency _frequency;
    BitOrder _bitOrder;
    DataMode _dataMode;
};

