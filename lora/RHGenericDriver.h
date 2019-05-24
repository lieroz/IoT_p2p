#ifndef RHGenericDriver_h
#define RHGenericDriver_h

#include "RadioHead.h"

#define RH_FLAGS_RESERVED                 0xf0
#define RH_FLAGS_APPLICATION_SPECIFIC     0x0f
#define RH_FLAGS_NONE                     0

#define RH_CAD_DEFAULT_TIMEOUT            10000

class RHGenericDriver
{
public:
    typedef enum
    {
        RHModeInitialising = 0,
        RHModeSleep,
        RHModeIdle,
        RHModeTx,
        RHModeRx,
        RHModeCad
    } RHMode;

    RHGenericDriver();

    virtual bool init();

    virtual bool available() = 0;
    virtual bool recv(uint8_t* buf, uint8_t* len) = 0;
    virtual bool send(const uint8_t* data, uint8_t len) = 0;

    virtual uint8_t maxMessageLength() = 0;

    virtual void waitAvailable();
    virtual bool waitPacketSent();
    virtual bool waitPacketSent(uint16_t timeout);
    virtual bool waitAvailableTimeout(uint16_t timeout);
    virtual bool waitCAD();

    void setCADTimeout(unsigned long cad_timeout);
    virtual bool isChannelActive();

    virtual void setThisAddress(uint8_t thisAddress);
    virtual void setHeaderTo(uint8_t to);
    virtual void setHeaderFrom(uint8_t from);
    virtual void setHeaderId(uint8_t id);
    virtual void setHeaderFlags(uint8_t set,
            uint8_t clear = RH_FLAGS_APPLICATION_SPECIFIC);
    virtual void setPromiscuous(bool promiscuous);

    virtual uint8_t headerTo();
    virtual uint8_t headerFrom();
    virtual uint8_t headerId();
    virtual uint8_t headerFlags();
    int8_t lastRssi();
    RHMode mode();

    void setMode(RHMode mode);
    virtual bool sleep();
    static void printBuffer(const char* prompt,
            const uint8_t* buf, uint8_t len);

    uint16_t rxBad();
    uint16_t rxGood();
    uint16_t txGood();

protected:
    volatile RHMode _mode;
    uint8_t _thisAddress;
    bool _promiscuous;

    volatile uint8_t _rxHeaderTo;
    volatile uint8_t _rxHeaderFrom;
    volatile uint8_t _rxHeaderId;
    volatile uint8_t _rxHeaderFlags;

    uint8_t _txHeaderTo;
    uint8_t _txHeaderFrom;
    uint8_t _txHeaderId;
    uint8_t _txHeaderFlags;

    volatile int8_t _lastRssi;
    volatile uint16_t _rxBad;
    volatile uint16_t _rxGood;
    volatile uint16_t _txGood;

    volatile bool _cad;
    unsigned int _cad_timeout;
};

#endif

