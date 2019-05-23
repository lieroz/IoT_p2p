#include "RH_RF95.h"

PROGMEM static const RH_RF95::ModemConfig MODEM_CONFIG_TABLE[] =
{
    //  1d,     1e,      26
    { 0x72,   0x74,    0x00}, // Bw125Cr45Sf128 (the chip default)
    { 0x92,   0x74,    0x00}, // Bw500Cr45Sf128
    { 0x48,   0x94,    0x00}, // Bw31_25Cr48Sf512
    { 0x78,   0xc4,    0x00}, // Bw125Cr48Sf4096
};

RH_RF95::RH_RF95(uint8_t slaveSelectPin, uint8_t interruptPin, RHGenericSPI& spi)
    :
    RHSPIDriver(slaveSelectPin, spi),
    _rxBufValid(0)
{
}

bool RH_RF95::init()
{
    /// The reported device version
    uint8_t deviceVersion;

    if (!RHSPIDriver::init())
    {
	    return false;
    }

    deviceVersion = spiRead(RH_RF95_REG_42_VERSION);
    if (deviceVersion == 0 || deviceVersion == 0xff)
    {
        return false;
    }

    spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_SLEEP | RH_RF95_LONG_RANGE_MODE);
    if (spiRead(RH_RF95_REG_01_OP_MODE) != (RH_RF95_MODE_SLEEP | RH_RF95_LONG_RANGE_MODE))
    {
	    return false;
    }

    spiWrite(RH_RF95_REG_0E_FIFO_TX_BASE_ADDR, 0);
    spiWrite(RH_RF95_REG_0F_FIFO_RX_BASE_ADDR, 0);

    setModeIdle();

    setModemConfig(Bw125Cr45Sf128); // Radio default
    setPreambleLength(8); // Default is 8
    setFrequency(434.0);
    setTxPower(13);

    return true;
}

void RH_RF95::validateRxBuf()
{
    if (_bufLen < 4)
    {
    	return;
    }

    _rxHeaderTo    = _buf[0];
    _rxHeaderFrom  = _buf[1];
    _rxHeaderId    = _buf[2];
    _rxHeaderFlags = _buf[3];

    if (_promiscuous || _rxHeaderTo == _thisAddress ||
        _rxHeaderTo == RH_BROADCAST_ADDRESS)
    {
        _rxGood++;
        _rxBufValid = true;
    }
}

bool RH_RF95::available()
{
    uint8_t irq_flags = spiRead(RH_RF95_REG_12_IRQ_FLAGS);
    if (_mode == RHModeRx && irq_flags & RH_RF95_RX_DONE)
    {
        uint8_t len = spiRead(RH_RF95_REG_13_RX_NB_BYTES);

        spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));
        spiBurstRead(RH_RF95_REG_00_FIFO, _buf, len);
        _bufLen = len;
        spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff);

        _lastRssi = spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE) - 137;

        validateRxBuf(); 
        if (_rxBufValid)
        {
            setModeIdle();
        }
    }
    else if (_mode == RHModeCad && irq_flags & RH_RF95_CAD_DONE)
    {
        _cad = irq_flags & RH_RF95_CAD_DETECTED;
        setModeIdle();
    }
    
    spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff);

    if (_mode == RHModeTx)
    {
	    return false;
    }

    setModeRx();
    return _rxBufValid;
}

void RH_RF95::clearRxBuf()
{
    ATOMIC_BLOCK_START;
    _rxBufValid = false;
    _bufLen = 0;
    ATOMIC_BLOCK_END;
}

bool RH_RF95::recv(uint8_t* buf, uint8_t* len)
{
    if (!available())
    {
	    return false;
    }

    if (buf && len)
    {
        ATOMIC_BLOCK_START;
        if (*len > _bufLen-RH_RF95_HEADER_LEN)
        {
            *len = _bufLen-RH_RF95_HEADER_LEN;
        }

        memcpy(buf, _buf+RH_RF95_HEADER_LEN, *len);
        ATOMIC_BLOCK_END;
    }

    clearRxBuf();
    return true;
}

bool RH_RF95::send(const uint8_t* data, uint8_t len)
{
    if (len > RH_RF95_MAX_MESSAGE_LEN)
    {
	    return false;
    }

    waitPacketSent();
    setModeIdle();

    if (!waitCAD()) 
    {
	    return false;
    }

    spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, 0);

    spiWrite(RH_RF95_REG_00_FIFO, _txHeaderTo);
    spiWrite(RH_RF95_REG_00_FIFO, _txHeaderFrom);
    spiWrite(RH_RF95_REG_00_FIFO, _txHeaderId);
    spiWrite(RH_RF95_REG_00_FIFO, _txHeaderFlags);

    spiBurstWrite(RH_RF95_REG_00_FIFO, data, len);
    spiWrite(RH_RF95_REG_22_PAYLOAD_LENGTH, len + RH_RF95_HEADER_LEN);

    setModeTx();
    return true;
}

bool RH_RF95::waitPacketSent()
{
    if (_mode != RHModeTx)
    {
        return false;
    }

    while (!(spiRead(RH_RF95_REG_12_IRQ_FLAGS) & RH_RF95_TX_DONE))
    {
        YIELD;
    }

    _txGood++;
    setModeIdle();

    return true;
}

bool RH_RF95::printRegisters()
{
    uint8_t registers[] = {
        0x01, 0x06, 0x07, 0x08, 0x09,
        0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
        0x0f, 0x10, 0x11, 0x12, 0x13,
        0x014, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1a, 0x1b, 0x1c, 0x1d,
        0x1e, 0x1f, 0x20, 0x21, 0x22,
        0x23, 0x24, 0x25, 0x26, 0x27
    };

    for (uint8_t i = 0; i < sizeof(registers); i++)
    {
        Serial.print(registers[i], HEX);
        Serial.print(": ");
        Serial.println(spiRead(registers[i]), HEX);
    }

    return true;
}

uint8_t RH_RF95::maxMessageLength()
{
    return RH_RF95_MAX_MESSAGE_LEN;
}

bool RH_RF95::setFrequency(float centre)
{
    uint32_t frf = (centre * 1000000.0) / RH_RF95_FSTEP;
    spiWrite(RH_RF95_REG_06_FRF_MSB, (frf >> 16) & 0xff);
    spiWrite(RH_RF95_REG_07_FRF_MID, (frf >> 8) & 0xff);
    spiWrite(RH_RF95_REG_08_FRF_LSB, frf & 0xff);

    return true;
}

void RH_RF95::setModeIdle()
{
    if (_mode != RHModeIdle)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_STDBY);
        _mode = RHModeIdle;
    }
}

bool RH_RF95::sleep()
{
    if (_mode != RHModeSleep)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_SLEEP);
        _mode = RHModeSleep;
    }
    return true;
}

void RH_RF95::setModeRx()
{
    if (_mode != RHModeRx)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_RXCONTINUOUS);
        spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x00); // Interrupt on RxDone
        _mode = RHModeRx;
    }
}

void RH_RF95::setModeTx()
{
    if (_mode != RHModeTx)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_TX);
        spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x40); // Interrupt on TxDone
        _mode = RHModeTx;
    }
}

void RH_RF95::setTxPower(int8_t power, bool useRFO)
{
    if (useRFO)
    {
        power = power > 14 ? 14 : power < -1 ? -1 : power;
        spiWrite(RH_RF95_REG_09_PA_CONFIG, RH_RF95_MAX_POWER | (power + 1));
    }
    else
    {
        if (power > 20)
        {
            power = 20;
            spiWrite(RH_RF95_REG_4D_PA_DAC, RH_RF95_PA_DAC_ENABLE);
        }
        else
        {
            power = power < 5 ? 5 : power;
            spiWrite(RH_RF95_REG_4D_PA_DAC, RH_RF95_PA_DAC_DISABLE);
        }

        spiWrite(RH_RF95_REG_09_PA_CONFIG, RH_RF95_PA_SELECT | (power-5));
    }
}

void RH_RF95::setModemRegisters(const ModemConfig* config)
{
    spiWrite(RH_RF95_REG_1D_MODEM_CONFIG1, config->reg_1d);
    spiWrite(RH_RF95_REG_1E_MODEM_CONFIG2, config->reg_1e);
    spiWrite(RH_RF95_REG_26_MODEM_CONFIG3, config->reg_26);
}

bool RH_RF95::setModemConfig(ModemConfigChoice index)
{
    if (index > (signed int)(sizeof(MODEM_CONFIG_TABLE) / sizeof(ModemConfig)))
    {
        return false;
    }

    ModemConfig cfg;
    memcpy_P(&cfg, &MODEM_CONFIG_TABLE[index], sizeof(RH_RF95::ModemConfig));
    setModemRegisters(&cfg);

    return true;
}

bool RH_RF95::getModemConfig(ModemConfigChoice index, ModemConfig* config)
{
    if (index > (signed int)(sizeof(MODEM_CONFIG_TABLE) / sizeof(ModemConfig)))
    {
        return false;
    }

    memcpy_P(config, &MODEM_CONFIG_TABLE[index], sizeof(RH_RF95::ModemConfig));

    return true;
}



void RH_RF95::setPreambleLength(uint16_t bytes)
{
    spiWrite(RH_RF95_REG_20_PREAMBLE_MSB, bytes >> 8);
    spiWrite(RH_RF95_REG_21_PREAMBLE_LSB, bytes & 0xff);
}

bool RH_RF95::isChannelActive()
{
    if (_mode != RHModeCad)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_CAD);
        spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x80); // Interrupt on CadDone
        _mode = RHModeCad;
    }

    while (_mode == RHModeCad)
    {
        YIELD;
    }

    return _cad;
}

void RH_RF95::enableTCXO()
{
    while ((spiRead(RH_RF95_REG_4B_TCXO) & RH_RF95_TCXO_TCXO_INPUT_ON) != RH_RF95_TCXO_TCXO_INPUT_ON)
    {
        sleep();
        spiWrite(RH_RF95_REG_4B_TCXO, (spiRead(RH_RF95_REG_4B_TCXO) | RH_RF95_TCXO_TCXO_INPUT_ON));
    } 
}
