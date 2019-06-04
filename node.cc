#include "node.h"

#include <stdexcept>
#include <functional>
#include <thread>
#include <cstring>
#include <iostream>

namespace
{
void txCallback(txData *tx)
{
    std::cout << "tx done;\tsent string: " << tx->buf << std::endl;

    LoRa_ctl *modem = (LoRa_ctl *)(tx->userPtr);
    std::memset(modem->rx.data.buf, '\0', Node::loraBufSize);
    LoRa_receive(modem);
}

void rxCallback(rxData *rx)
{
    LoRa_ctl *modem = (LoRa_ctl *)(rx->userPtr);
    LoRa_stop_receive(modem);

    std::cout << "rx done;\tCRC error: " << rx->CRC
              << "\tData size: " << rx->size
              << "\tReceived string: \"" << rx->buf
              << "\";\tRSSI: " << rx->RSSI
              << ";\tSNR: " << rx->SNR << std::endl;

    std::memset(modem->tx.data.buf, '\0', Node::loraBufSize);
    std::memcpy(modem->tx.data.buf, "pong", 4);
    modem->tx.data.size = 4;

    LoRa_send(modem);
}

}

Node::Node(std::size_t _timeout)
    : keys(tools::RsaGenerateHexKeyPair(rsaBits)),
    timeout(_timeout)
{
}

void Node::init(LoRa_ctl *modem, char *txbuf, char *rxbuf)
{
    modem->spiCS = 0; //Raspberry SPI CE pin number
    modem->tx.callback = txCallback;
    modem->tx.data.buf = txbuf;
    modem->rx.callback = rxCallback;
    modem->rx.data.buf = rxbuf;
    modem->rx.data.userPtr = static_cast<void *>(modem);
    modem->tx.data.userPtr = static_cast<void *>(modem);
    modem->eth.preambleLen = 6;
    modem->eth.bw = BW62_5; //Bandwidth 62.5KHz
    modem->eth.sf = SF12; //Spreading Factor 12
    modem->eth.ecr = CR8; //Error coding rate CR4/8
    modem->eth.CRC = 1; //Turn on CRC checking
    modem->eth.freq = 434800000; // 434.8MHz
    modem->eth.resetGpioN = 4; //GPIO4 on lora RESET pin
    modem->eth.dio0GpioN = 17; //GPIO17 on lora DIO0 pin to control Rxdone and Txdone interrupts
    modem->eth.outPower = OP20; //Output power
    modem->eth.powerOutPin = PA_BOOST; //Power Amplifire pin
    modem->eth.AGC = 1; //Auto Gain Control
    modem->eth.OCP = 240; // 45 to 240 mA. 0 to turn off protection
    modem->eth.implicitHeader = 0; //Explicit header mode
    modem->eth.syncWord = 0x12;
}

void Node::start(const std::string &mode)
{
    LoRa_ctl modem;
    char txbuf[loraBufSize];
    char rxbuf[loraBufSize];

    std::memset(txbuf, '\0', loraBufSize);
    std::memset(rxbuf, '\0', loraBufSize);

    init(&modem, txbuf, rxbuf);

    if (LoRa_begin(&modem) != 0)
    {
        throw std::runtime_error("Error configuring LoRa");
    }

    if (mode == "sender")
    {
        std::memset(modem.tx.data.buf, '\0', loraBufSize);
        std::memcpy(modem.tx.data.buf, "ping", 4);
        modem.tx.data.size = 4;

        LoRa_send(&modem);
    }
    else if (mode == "receiver")
    {
        LoRa_receive(&modem);
    }
    else
    {
        throw std::runtime_error("Unsupported node mode");
    }

    do
    {
        std::this_thread::yield();
    } while (LoRa_get_op_mode(&modem) != SLEEP_MODE);

    LoRa_end(&modem);
}

