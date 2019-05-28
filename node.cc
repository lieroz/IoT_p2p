#include <iostream>
#include <cstring>
#include <unistd.h>

#include <lora.h>

bool connectionEstablished = false;

void tx_f(txData *tx)
{
    LoRa_ctl *modem = (LoRa_ctl *)(tx->userPtr);

    printf("tx done;\t");
    printf("sent string: \"%s\"\n", tx->buf); //Data we've sent

    std::memset(modem->rx.data.buf, '\0', 255);
    LoRa_receive(modem);
}

void rx_f(rxData *rx)
{
    LoRa_ctl *modem = (LoRa_ctl *)(rx->userPtr);
    LoRa_stop_receive(modem); //manually stoping RxCont mode

    printf("rx done;\t");
    printf("CRC error: %d;\t", rx->CRC);
    printf("Data size: %d;\t", rx->size);
    printf("received string: \"%s\";\t", rx->buf); //Data we've received
    printf("RSSI: %d;\t", rx->RSSI);
    printf("SNR: %f\n\n", rx->SNR);

    const char* data;
    std::size_t len = 0;

    if (!connectionEstablished)
    {
        if (std::strcmp(rx->buf, "syn") == 0)
        {
            data = "synack";
            len = std::strlen(data);
            connectionEstablished = true;
        }
        else if (std::strcmp(rx->buf, "synack") == 0)
        {
            data = "ack";
            len = std::strlen(data);
        }
        else if (std::strcmp(rx->buf, "ack") == 0)
        {
            data = "ack";
            len = std::strlen(data);
            connectionEstablished = true;
        }
        else
        {
            std::cerr << "Unknown handshake step" << std::endl;
            LoRa_sleep(modem);
            return;
        }
    }
    else
    {
        data = "making tunnel great again";
        len = std::strlen(data);
    }

    std::memset(modem->tx.data.buf, '\0', 255);
    std::memcpy(modem->tx.data.buf, data, len);
    modem->tx.data.size = len;

    LoRa_send(modem);

    printf("Time on air data - Tsym: %f;\t", modem->tx.data.Tsym);
    printf("Tpkt: %f;\t", modem->tx.data.Tpkt);
    printf("payloadSymbNb: %u\n", modem->tx.data.payloadSymbNb);
}

void init(LoRa_ctl *modem, char *txbuf, char *rxbuf)
{
    //See for typedefs, enumerations and there values in LoRa.h header file
    modem->spiCS = 0; //Raspberry SPI CE pin number
    modem->tx.callback = tx_f;
    modem->tx.data.buf = txbuf;
    modem->rx.callback = rx_f;
    modem->rx.data.buf = rxbuf;
    modem->rx.data.userPtr = (void *)(modem); //To handle with chip from rx callback
    modem->tx.data.userPtr = (void *)(modem); //To handle with chip from tx callback
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
    //For detail information about SF, Error Coding Rate, Explicit header, Bandwidth, AGC, Over current protection and other features refer to sx127x datasheet https://www.semtech.com/uploads/documents/DS_SX1276-7-8-9_W_APP_V5.pdf
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Plz specify sender or receiver" << std::endl;
        return 1;
    }

    char txbuf[255];
    char rxbuf[255];
    LoRa_ctl modem;

    std::memset(txbuf, '\0', 255);
    std::memset(rxbuf, '\0', 255);

    init(&modem, txbuf, rxbuf);

    if (LoRa_begin(&modem) != 0)
    {
        std::cerr << "Error configuring lora" << std::endl;
        return 1;
    }

    if (std::strcmp(argv[1], "receiver") == 0)
    {
        LoRa_receive(&modem);
    }
    else if (std::strcmp(argv[1], "sender") == 0)
    {
        const char *data = "syn";
        std::size_t len = std::strlen(data);
        memcpy(modem.tx.data.buf, data, len);
        modem.tx.data.size = len;
        LoRa_send(&modem);
    }
    else
    {
        std::cerr << "Unknown node start option" << std::endl;
        return 1;
    }

    while (LoRa_get_op_mode(&modem) != SLEEP_MODE)
    {
        sleep(1);
    }

    printf("end\n");
    LoRa_end(&modem);

    return 0;
}
