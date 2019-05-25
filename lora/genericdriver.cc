#include "genericdriver.h"

#include <iostream>
#include <chrono>

RHGenericDriver::RHGenericDriver()
    : _mode(RHModeInitialising),
    _thisAddress(RH_BROADCAST_ADDRESS),
    _txHeaderTo(RH_BROADCAST_ADDRESS),
    _txHeaderFrom(RH_BROADCAST_ADDRESS),
    _txHeaderId(0),
    _txHeaderFlags(0),
    _rxBad(0),
    _rxGood(0),
    _txGood(0),
    _cad_timeout(0)
{
}

bool RHGenericDriver::init()
{
    return true;
}

void RHGenericDriver::waitAvailable()
{
    while (!available())
    {
        // do dome useful work here, yield() analog;
    }
}

bool RHGenericDriver::waitAvailableTimeout(uint16_t timeout)
{
    auto t0 = std::chrono::high_resolution_clock::now();
    auto t1 = std::chrono::high_resolution_clock::now();

    while (std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() < timeout)
    {
        if (available())
        {
            return true;
        }

        // do dome useful work here, yield() analog;
        t1 = std::chrono::high_resolution_clock::now();
    }

    return false;
}

bool RHGenericDriver::waitPacketSent()
{
    while (_mode == RHModeTx)
    {
        // do dome useful work here, yield() analog;
    }

    return true;
}

bool RHGenericDriver::waitPacketSent(uint16_t timeout)
{
    auto t0 = std::chrono::high_resolution_clock::now();
    auto t1 = std::chrono::high_resolution_clock::now();

    while (std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() < timeout)
    {
        if (_mode != RHModeTx)
        {
            return true;
        }

        // do dome useful work here, yield() analog;
        t1 = std::chrono::high_resolution_clock::now();
    }

    return false;
}

bool RHGenericDriver::waitCAD()
{
    if (!_cad_timeout)
    {
	    return true;
    }

    auto t0 = std::chrono::high_resolution_clock::now();
    auto t1 = std::chrono::high_resolution_clock::now();

    while (isChannelActive())
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() > _cad_timeout)
        {
	        return false;
        }

        t1 = std::chrono::high_resolution_clock::now();
    }

    return true;
}

bool RHGenericDriver::isChannelActive()
{
    return false;
}

void RHGenericDriver::setPromiscuous(bool promiscuous)
{
    _promiscuous = promiscuous;
}

void RHGenericDriver::setThisAddress(uint8_t address)
{
    _thisAddress = address;
}

void RHGenericDriver::setHeaderTo(uint8_t to)
{
    _txHeaderTo = to;
}

void RHGenericDriver::setHeaderFrom(uint8_t from)
{
    _txHeaderFrom = from;
}

void RHGenericDriver::setHeaderId(uint8_t id)
{
    _txHeaderId = id;
}

void RHGenericDriver::setHeaderFlags(uint8_t set, uint8_t clear)
{
    _txHeaderFlags &= ~clear;
    _txHeaderFlags |= set;
}

uint8_t RHGenericDriver::headerTo()
{
    return _rxHeaderTo;
}

uint8_t RHGenericDriver::headerFrom()
{
    return _rxHeaderFrom;
}

uint8_t RHGenericDriver::headerId()
{
    return _rxHeaderId;
}

uint8_t RHGenericDriver::headerFlags()
{
    return _rxHeaderFlags;
}

int8_t RHGenericDriver::lastRssi()
{
    return _lastRssi;
}

RHGenericDriver::RHMode  RHGenericDriver::mode()
{
    return _mode;
}

void  RHGenericDriver::setMode(RHMode mode)
{
    _mode = mode;
}

bool  RHGenericDriver::sleep()
{
    return false;
}

void RHGenericDriver::printBuffer(const char* prompt,
        const uint8_t* buf, uint8_t len)
{
    std::cout << prompt << std::endl;

    for (uint8_t i = 0; i < len; i++)
    {
        if (i % 16 == 15)
        {
            std::cout <<  std::hex << buf[i] << std::dec << std::endl;
        }
        else
        {
            std::cout << std::hex << buf[i] << std::dec << ' ';
        }
    }

    std::cout << std::endl;
}

uint16_t RHGenericDriver::rxBad()
{
    return _rxBad;
}

uint16_t RHGenericDriver::rxGood()
{
    return _rxGood;
}

uint16_t RHGenericDriver::txGood()
{
    return _txGood;
}

void RHGenericDriver::setCADTimeout(unsigned long cad_timeout)
{
    _cad_timeout = cad_timeout;
}

