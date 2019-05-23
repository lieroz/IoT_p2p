#include "RHGenericSPI.h"

RHGenericSPI::RHGenericSPI(Frequency frequency, BitOrder bitOrder, DataMode dataMode)
    :
    _frequency(frequency),
    _bitOrder(bitOrder),
    _dataMode(dataMode)
{
}

void RHGenericSPI::setBitOrder(BitOrder bitOrder)
{
    _bitOrder = bitOrder;
}

void RHGenericSPI::setDataMode(DataMode dataMode)
{
    _dataMode = dataMode; 
}

void RHGenericSPI::setFrequency(Frequency frequency)
{
    _frequency = frequency;
}

