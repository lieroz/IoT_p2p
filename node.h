#pragma once

#include <chrono>

#include <lora.h>
#include <tools.h>

class Node
{
    static constexpr std::size_t loraBufSize = 255;
    static constexpr std::size_t rsaBits = 1040;

public:
    Node(std::size_t _timeout);

    void start();

private:
    void init();

    void txCallback(txData *tx);
    void rxCallback(rxData *rx);

private:
    LoRa_ctl modem;
    char txbuf[loraBufSize];
    char rxbuf[loraBufSize];

    tools::KeyPairHex keys;
    std::chrono::time_point<std::chrono::high_resolution_clock> timePoint;
    std::size_t timeout;
};

