#pragma once

#include "RadioHead.h"

class RHGenericSPI
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

    RHGenericSPI(Frequency frequency = Frequency1MHz, BitOrder bitOrder = BitOrderMSBFirst, DataMode dataMode = DataMode0);

    virtual uint8_t transfer(uint8_t data) = 0;
    virtual void begin() = 0;
    virtual void end() = 0;

    virtual void setBitOrder(BitOrder bitOrder);
    virtual void setDataMode(DataMode dataMode);
    virtual void setFrequency(Frequency frequency);

protected:
    Frequency _frequency;
    BitOrder _bitOrder;
    DataMode _dataMode;
};

