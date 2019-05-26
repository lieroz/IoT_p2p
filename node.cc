#include <iostream>
#include <csignal>
#include <chrono>
#include <random>
#include <cstring>

#include <bcm2835.h>

#include <rf95.h>
#include <tools.h>

#define RF_FREQUENCY  868.00

RH_RF95 rf95(RF_CS_PIN, RF_IRQ_PIN);

void recv();

void syn()
{
    std::cout << "SYN" << std::endl;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 254);

    const uint8_t sessionId = dist(rng);
    const uint8_t nodeFrom = dist(rng);

    rf95.setHeaderFrom(nodeFrom);
    rf95.setHeaderId(sessionId);

    rf95.setModeTx();

    uint8_t data[] = "SYN";
    int len = sizeof(data);

    rf95.send(data, len);
    rf95.waitPacketSent();

    recv();
}

void ack(const uint8_t nodeFrom, const uint8_t nodeTo, const uint8_t sessionId)
{
    std::cout << "ACK" << std::endl;

    rf95.setHeaderFrom(nodeFrom);
    rf95.setHeaderTo(nodeTo);
    rf95.setHeaderId(sessionId);

    rf95.setModeTx();

    uint8_t data[] = "ACK";
    int len = sizeof(data);

    rf95.send(data, len);
    rf95.waitPacketSent();

    recv();
}

void synAck(const uint8_t nodeTo, const uint8_t sessionId)
{
    std::cout << "SYNACK" << std::endl;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 254);

    const uint8_t nodeFrom = dist(rng);

    rf95.setHeaderFrom(nodeFrom);
    rf95.setHeaderTo(nodeTo);
    rf95.setHeaderId(sessionId);

    rf95.setModeTx();

    uint8_t data[] = "SYNACK";
    int len = sizeof(data);

    rf95.send(data, len);
    rf95.waitPacketSent();

    recv();
}

void recv()
{
    rf95.setModeRx();
    auto t0 = std::chrono::high_resolution_clock::now();
    auto t1 = std::chrono::high_resolution_clock::now();

    while (std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() < 5000)
    {
        if (rf95.available())
        {
            uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
            int len = sizeof(buf);
            int from = rf95.headerFrom();
            int to = rf95.headerTo();
            int id = rf95.headerId();
            int flags = rf95.headerFlags();;
            int rssi = rf95.lastRssi();

            if (rf95.recv(buf, (uint8_t *) &len))
            {
                if (!std::memcmp(buf, "SYN", 3))
                {
                    synAck(from, id);
                }
                else if (!std::memcmp(buf, "ACK", 3))
                {
                    rf95.setHeaderFrom(to);
                    rf95.setHeaderTo(from);
                    rf95.setHeaderId(id);

                    rf95.setModeTx();

                    uint8_t data[] = "MSG";
                    int len = sizeof(data);

                    rf95.send(data, len);
                    rf95.waitPacketSent();
                }
                else if (!std::memcmp(buf, "SYNACK", 6))
                {
                    ack(to, from, id);
                }
            }
            else
            {
                std::cerr << "receive failed" << std::endl;
            }
        }

        t1 = std::chrono::high_resolution_clock::now();
    }
}

bool forceExit = false;

void sig_handler(int sig)
{
    forceExit = true;
}

int main(int argc, const char *argv[])
{
    signal(SIGINT, sig_handler);

    if (!bcm2835_init())
    {
        return 1;
    }

    if (!rf95.init())
    {
        std::cerr << "RF95 module init failed, Please verify wiring/module" << std::endl;
    }
    else
    {
        rf95.setTxPower(14, false);
        rf95.setFrequency(RF_FREQUENCY);
        rf95.setPromiscuous(true);

        std::cout << "Listening packet..." << std::endl;

        auto t0 = std::chrono::high_resolution_clock::now();

        while (!forceExit)
        {
            syn();
        }
    }

    bcm2835_close();

    return 0;
}
