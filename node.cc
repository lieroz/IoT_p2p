#include <iostream>
#include <csignal>
#include <chrono>

#include <bcm2835.h>

#include <rf95.h>
#include <tools.h>

#define RF_NODE_FROM 1
#define RF_NODE_TO 2
#define RF_FREQUENCY  868.00

bool forceExit = false;

void sig_handler(int sig)
{
    forceExit = true;
}

int main(int argc, const char *argv[])
{
    RH_RF95 rf95(RF_CS_PIN, RF_IRQ_PIN);
    signal(SIGINT, sig_handler);

    if (!bcm2835_init())
    {
        return 1;
    }

    if (!rf95.init()) {
        std::cerr << "RF95 module init failed, Please verify wiring/module" << std::endl;
    } else {
        rf95.setTxPower(14, false);
        rf95.setFrequency(RF_FREQUENCY);
        rf95.setThisAddress(RF_NODE_FROM);
        rf95.setHeaderFrom(RF_NODE_FROM);
        rf95.setHeaderTo(RF_NODE_TO);
        rf95.setPromiscuous(true);

        std::cout << "OK NodeID: " << RF_NODE_FROM << " @ " << RF_FREQUENCY << "MHz" << std::endl;
        std::cout << "Listening packet..." << std::endl;

        auto t0 = std::chrono::high_resolution_clock::now();

        while (!forceExit)
        {
            rf95.setModeRx();
            auto t1 = std::chrono::high_resolution_clock::now();

            if (std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() > 5000)
            {
                rf95.setModeTx();
                t0 = std::chrono::high_resolution_clock::now();

                uint8_t data[] = "Hi Raspi!";
                int len = sizeof(data);

                std::cout << "Sending " << len << " bytes to node #" << RF_NODE_TO << " => ";
                tools::printbuffer(data, len);
                std::cout << std::endl;

                rf95.send(data, len);
                rf95.waitPacketSent();
            }

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
                    std::cout << "Packet[" << len << "] #" << from << "=> #" << to << " " << rssi << "dB: ";
                    tools::printbuffer(buf, len);
                }
                else
                {
                    std::cerr << "receive failed" << std::endl;
                }
            }
        }
    }

    bcm2835_close();

    return 0;
}
