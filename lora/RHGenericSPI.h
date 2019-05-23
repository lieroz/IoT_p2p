#ifndef RHGenericSPI_h
#define RHGenericSPI_h

#include "RadioHead.h"

class RHGenericSPI 
{
public:
    typedef enum
    {
	DataMode0 = 0, ///< SPI Mode 0: CPOL = 0, CPHA = 0
	DataMode1,     ///< SPI Mode 1: CPOL = 0, CPHA = 1
	DataMode2,     ///< SPI Mode 2: CPOL = 1, CPHA = 0
	DataMode3,     ///< SPI Mode 3: CPOL = 1, CPHA = 1
    } DataMode;

    typedef enum
    {
	Frequency1MHz = 0,  ///< SPI bus frequency close to 1MHz
	Frequency2MHz,      ///< SPI bus frequency close to 2MHz
	Frequency4MHz,      ///< SPI bus frequency close to 4MHz
	Frequency8MHz,      ///< SPI bus frequency close to 8MHz
	Frequency16MHz      ///< SPI bus frequency close to 16MHz
    } Frequency;

    typedef enum
    {
	BitOrderMSBFirst = 0,  ///< SPI MSB first
	BitOrderLSBFirst,      ///< SPI LSB first
    } BitOrder;

    RHGenericSPI(Frequency frequency = Frequency1MHz, BitOrder bitOrder = BitOrderMSBFirst, DataMode dataMode = DataMode0);

    virtual uint8_t transfer(uint8_t data) = 0;

    virtual void attachInterrupt() {};

    virtual void detachInterrupt() {};

    virtual void begin() = 0;

    virtual void end() = 0;

    virtual void setBitOrder(BitOrder bitOrder);

    virtual void setDataMode(DataMode dataMode);

    virtual void setFrequency(Frequency frequency);

protected:
    Frequency    _frequency; // Bus frequency, one of RHGenericSPI::Frequency
    BitOrder     _bitOrder;  
    DataMode     _dataMode;  
};
#endif
