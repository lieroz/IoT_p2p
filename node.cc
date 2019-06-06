#include "node.h"

#include <stdexcept>
#include <functional>
#include <thread>
#include <chrono>
#include <cstring>
#include <iostream>

#include <tools.h>
#include <lora.h>

namespace node
{

static const std::size_t loraBufSize = 255;
static const std::size_t rsaBits = 1040;
static const std::size_t dhBits = 16;

enum Command
{
    Sign,
    CheckStart,
    Check,
    CheckEnd,
};

struct SessionContext
{
    SessionContext()
        : keys(tools::rsaGenerateHexKeyPair(rsaBits))
    {
        std::memset(message, '\0', 4096);
    }

    tools::KeyPairHex keys;

    unsigned long long dhG;
    unsigned long long dhP;
    unsigned long long dhPrivateKey;
    unsigned long long dhPublicKey;
    unsigned long long dhKey;

    char message[4096];
};

SessionContext session;

void sendCheckResponse(LoRa_ctl *modem, const std::string &hash)
{
    session.dhPrivateKey = tools::generatePrimeNumber<dhBits>();
    session.dhKey = tools::modpow(session.dhPublicKey, session.dhPrivateKey, session.dhP);
    unsigned long long dhB = tools::modpow(session.dhG, session.dhPrivateKey, session.dhP);

    std::cout << "KEY: " << std::endl;
    std::cout << "B: " << std::endl;

    std::size_t offset = 0;
    std::string signedHash = tools::rsaSignString(session.keys.privateKey, hash);

    std::memcpy(session.message, signedHash.c_str(), signedHash.length());
    offset += signedHash.length();

    std::memcpy(&session.message[offset], &dhB, sizeof(dhB));
    offset += sizeof(dhB);

    Command cmd;
    std::string pubKey = session.keys.publicKey;
    std::memcpy(&session.message[offset], pubKey.c_str(), pubKey.length());
    offset += pubKey.length();

    for (std::size_t i = 0; i < offset; ++i)
    {
        std::size_t bufSize = loraBufSize - sizeof(cmd);

        if (i == 0)
        {
            cmd = CheckStart;
        }
        else if (offset - i + 1 < loraBufSize)
        {
            cmd = CheckEnd;
            bufSize = offset - i + 1;
        }
        else
        {
            cmd = Check;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        std::memset(modem->tx.data.buf, '\0', loraBufSize);
        std::memcpy(modem->tx.data.buf, &cmd, sizeof(cmd));
        std::memcpy(&modem->tx.data.buf[sizeof(cmd)], &session.message[i], bufSize);

        modem->tx.data.size = bufSize + sizeof(cmd);
        LoRa_send(modem);
        i += bufSize;

        if (cmd == CheckEnd) break;
    }
}

void parseSignCommandAndSendResponse(rxData *rx)
{
    LoRa_ctl *modem = (LoRa_ctl *)(rx->userPtr);
    std::size_t offset = sizeof(Command);
    char sha256[65];

    std::memcpy(sha256, &rx->buf[offset], 64);
    offset += 64;
    sha256[64] = '\0';

    std::memcpy(&session.dhG, &rx->buf[offset], sizeof(session.dhG));
    offset += sizeof(session.dhG);

    std::memcpy(&session.dhP, &rx->buf[offset], sizeof(session.dhP));
    offset += sizeof(session.dhP);

    std::memcpy(&session.dhPublicKey, &rx->buf[offset], sizeof(session.dhPublicKey));
    offset += sizeof(session.dhPublicKey);

    sendCheckResponse(modem, sha256);
}

void txCallback(txData *tx)
{
    std::cout << "TX done;" << std::endl;

    LoRa_ctl *modem = (LoRa_ctl *)(tx->userPtr);
    std::memset(modem->rx.data.buf, '\0', loraBufSize);
    LoRa_receive(modem);
}

void rxCallback(rxData *rx)
{
    LoRa_ctl *modem = (LoRa_ctl *)(rx->userPtr);

    std::cout << "RX done;\tCRC error: " << rx->CRC
              << "\tData size: " << rx->size
              << "\tRSSI: " << rx->RSSI
              << "\tSNR: " << rx->SNR << std::endl;

    Command cmd;
    std::memcpy(&cmd, rx->buf, sizeof(cmd));

    switch (cmd)
    {
    case Sign:
        LoRa_stop_receive(modem);
        parseSignCommandAndSendResponse(rx);
        break;
    case CheckStart:
        std::memcpy(session.message, rx->buf, rx->size - sizeof(cmd));
        break;
    case Check:
        std::memcpy(session.message, rx->buf, rx->size - sizeof(cmd));
        break;
    case CheckEnd:
        LoRa_stop_receive(modem);
        std::memcpy(session.message, rx->buf, rx->size - sizeof(cmd));
        //parseCheckCommandAndSleep(mesage);
        break;
    }
}

void sendSignRequest(LoRa_ctl *modem)
{
    std::memset(modem->tx.data.buf, '\0', loraBufSize);

    session.dhG = tools::generatePrimeNumber<dhBits>();
    session.dhP = tools::generatePrimeNumber<dhBits>();
    session.dhPrivateKey = tools::generatePrimeNumber<dhBits>();

    std::string data = "some random data to be signed";
    std::string hash = tools::sha256(data);
    Command cmd = Sign;
    std::size_t offset = 0;

    std::memcpy(modem->tx.data.buf, &cmd, sizeof(cmd));
    offset += sizeof(cmd);

    std::memcpy(&modem->tx.data.buf[offset], hash.c_str(), hash.length());
    offset += hash.length();

    std::memcpy(&modem->tx.data.buf[offset], &session.dhG, sizeof(session.dhG));
    offset += sizeof(session.dhG);

    std::memcpy(&modem->tx.data.buf[offset], &session.dhP, sizeof(session.dhP));
    offset += sizeof(session.dhP);

    unsigned long long dhA = tools::modpow(session.dhG, session.dhPrivateKey, session.dhP);
    std::memcpy(&modem->tx.data.buf[offset], &dhA, sizeof(dhA));
    modem->tx.data.size = offset + sizeof(dhA);

    LoRa_send(modem);
}

void init(LoRa_ctl *modem, char *txbuf, char *rxbuf)
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
    modem->eth.sf = SF7; //Spreading Factor 12
    modem->eth.ecr = CR8; //Error coding rate CR4/8
    modem->eth.CRC = 1; //Turn on CRC checking
    modem->eth.freq = 868100000; // 868.1MHz
    modem->eth.resetGpioN = 4; //GPIO4 on lora RESET pin
    modem->eth.dio0GpioN = 17; //GPIO17 on lora DIO0 pin to control Rxdone and Txdone interrupts
    modem->eth.outPower = OP20; //Output power
    modem->eth.powerOutPin = PA_BOOST; //Power Amplifire pin
    modem->eth.AGC = 1; //Auto Gain Control
    modem->eth.OCP = 240; // 45 to 240 mA. 0 to turn off protection
    modem->eth.implicitHeader = 0; //Explicit header mode
    modem->eth.syncWord = 0x12;
}

void start(const std::string &mode)
{
    LoRa_ctl modem;
    char txbuf[loraBufSize];
    char rxbuf[loraBufSize];

    init(&modem, txbuf, rxbuf);

    if (LoRa_begin(&modem) != 0)
    {
        throw std::runtime_error("Error configuring LoRa");
    }

    if (mode == "sender")
    {
        sendSignRequest(&modem);
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

} //node

