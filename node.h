#pragma once

#include <tools.h>
#include <lora.h>

class Node
{
public:
    Node(std::size_t _timeout);

    void start(const std::string &mode);

private:
    void init(LoRa_ctl *modem, char *txbuf, char *rxbuf);

    void sendSignRequest(LoRa_ctl *modem);
    void sendCheckRequest(LoRa_ctl *modem);

public:
    enum Command
    {
        Sign,
        CheckStart,
        CheckEnd,
        Check,
    };

    static constexpr std::size_t loraBufSize = 255;
    static constexpr std::size_t rsaBits = 1040;
    static constexpr std::size_t dhBits = 16;

private:
    tools::KeyPairHex keys;
    std::size_t timeout;

    unsigned long long dhG;
    unsigned long long dhP;
    unsigned long long dhPrivateKey;
    unsigned long long dhKey;
};

