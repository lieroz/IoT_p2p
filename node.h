#pragma once

#include <tools.h>
#include <lora.h>

class Node
{
public:
    Node(std::size_t _timeout);

    void start();

private:
    void init(LoRa_ctl *modem, char *txbuf, char *rxbuf);

public:
    static constexpr std::size_t loraBufSize = 255;
    static constexpr std::size_t rsaBits = 1040;

private:
    tools::KeyPairHex keys;
    std::size_t timeout;
};

