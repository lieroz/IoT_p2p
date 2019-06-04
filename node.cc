#include "node.h"

#include <stdexcept>
#include <functional>
#include <thread>
#include <cstring>
#include <iostream>

Node::Node(std::size_t _timeout)
    : keys(tools::RsaGenerateHexKeyPair(rsaBits)),
    timeout(_timeout)
{
    init();
}

void Node::init()
{
    modem.spiCS = 0; //Raspberry SPI CE pin number
    modem.tx.callback = (UserTxDoneCallback)&Node::txCallback;
    modem.tx.data.buf = txbuf;
    modem.rx.callback = (UserRxDoneCallback)&Node::rxCallback;
    modem.rx.data.buf = rxbuf;
    modem.rx.data.userPtr = static_cast<void *>(&modem); //To handle with chip from rx callback
    modem.tx.data.userPtr = static_cast<void *>(&modem); //To handle with chip from tx callback
    modem.eth.preambleLen = 6;
    modem.eth.bw = BW62_5; //Bandwidth 62.5KHz
    modem.eth.sf = SF12; //Spreading Factor 12
    modem.eth.ecr = CR8; //Error coding rate CR4/8
    modem.eth.CRC = 1; //Turn on CRC checking
    modem.eth.freq = 434800000; // 434.8MHz
    modem.eth.resetGpioN = 4; //GPIO4 on lora RESET pin
    modem.eth.dio0GpioN = 17; //GPIO17 on lora DIO0 pin to control Rxdone and Txdone interrupts
    modem.eth.outPower = OP20; //Output power
    modem.eth.powerOutPin = PA_BOOST; //Power Amplifire pin
    modem.eth.AGC = 1; //Auto Gain Control
    modem.eth.OCP = 240; // 45 to 240 mA. 0 to turn off protection
    modem.eth.implicitHeader = 0; //Explicit header mode
    modem.eth.syncWord = 0x12;
}

void Node::start()
{
    if (LoRa_begin(&modem) != 0)
    {
        throw std::runtime_error("Error configuring LoRa");
    }

    timePoint = std::chrono::high_resolution_clock::now();
    auto now = timePoint + std::chrono::milliseconds(timeout);

    do
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - timePoint).count() > timeout)
        {
            std::memset(txbuf, '\0', 255);
            std::memcpy(txbuf, "ping", 4);
            modem.tx.data.size = 4;

            LoRa_send(&modem);
            timePoint = std::chrono::high_resolution_clock::now();
        }

        now = timePoint + std::chrono::milliseconds(timeout);
        std::this_thread::yield();
    } while (LoRa_get_op_mode(&modem) != SLEEP_MODE);

    LoRa_end(&modem);
}

void Node::txCallback(txData *tx)
{
    std::cout << "tx done;\tsent string: " << txbuf <<std::endl;

    std::memset(rxbuf, '\0', 255);

    LoRa_receive(&modem);
}

void Node::rxCallback(rxData *rx)
{
    timePoint = std::chrono::high_resolution_clock::now();

    std::cout << "RX done;\treceived string: " << rxbuf << std::endl;

    std::memset(txbuf, '\0', 255);
    std::memcpy(txbuf, "pong", 4);
    modem.tx.data.size = 4;

    LoRa_send(&modem);
}

