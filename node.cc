#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cmath>
#include <type_traits>
#include <bitset>
#include <random>

#include <lora.h>

template <typename T>
T modpow(T base, T exp, T modulus)
{
    static_assert(std::is_integral<T>::value, "Only integer types can be used");

    base %= modulus;
    T result = 1;

    while (exp > 0)
    {
        if (exp & 1)
        {
            result = (result * base) % modulus;
        }

        base = (base * base) % modulus;
        exp >>= 1;
    }

    return result;
}

template <typename T>
bool isPrime(T n, int k)
{
    static_assert(std::is_integral<T>::value, "Only integer types can be used");

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, n - 1);

    if (n == 2 || n == 3)
    {
        return true;
    }

    if (n <= 1 || n % 2 == 0)
    {
        return false;
    }

    T s = 0;
    T r = n - 1;

    while (r & 1 == 0)
    {
        ++s;
        r /= 2;
    }

    for (int i = 0; i < k; ++i)
    {
        T a = dist(rng);
        T x = modpow<T>(a, r, n);

        if (x != 1 && x != n - 1)
        {
            T j = 1;

            while (j < s && x != n - 1)
            {
                x = modpow<T>(x, 2, n);

                if (x == 1)
                {
                    return false;
                }

                j += 1;
            }

            if (x != n - 1)
            {
                return false;
            }
        }
    }

    return true;
}

template <std::size_t size>
typename std::bitset<size> randomBitset(double p = 0.5)
{
    std::bitset<size> bits;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::bernoulli_distribution dist(p);

    for (int i = 0; i < size; ++i)
    {
        bits[i] = dist(rng);
    }

    return bits;
}

template <std::size_t size>
unsigned long long generatePrimeCandidate()
{
    std::bitset<size> p = randomBitset<size>();
    p |= (static_cast<unsigned long long>(1) << size - 1) | 1;
    return p.to_ullong();
}

template <std::size_t size>
unsigned long long generatePrimeNumber()
{
    unsigned long long p = 4;

    while (!isPrime(p, 8))
    {
        p = generatePrimeCandidate<size>();
    }

    return p;
}

bool connectionEstablished = false;

void tx_f(txData *tx)
{
    LoRa_ctl *modem = (LoRa_ctl *)(tx->userPtr);

    printf("tx done;\t");
    printf("sent string: \"%s\"\n", tx->buf); //Data we've sent

    std::memset(modem->rx.data.buf, '\0', 255);
    LoRa_receive(modem);
}

unsigned long long g = 0;
unsigned long long p = 0;

unsigned long long a = 0;
unsigned long long b = 0;

unsigned long long A = 0;
unsigned long long B = 0;

unsigned long long key = 0;

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
    std::memset(modem->tx.data.buf, '\0', 255);

    if (!connectionEstablished)
    {
        if (std::strncmp(rx->buf, "syn", 3) == 0)
        {
            char _g[8];
            char _p[8];
            char _A[8];

            std::memcpy(&rx->buf[3], _g, 8);
            std::memcpy(&rx->buf[11], _p, 8);
            std::memcpy(&rx->buf[19], _A, 8);

            g = *(unsigned long long *)_g;
            p = *(unsigned long long *)_p;
            A = *(unsigned long long *)_A;

            std::cout << g << " " << p << " " << A << std::endl;

            b = generatePrimeNumber<16>();
            B = modpow(g, b, p);

            std::size_t size = sizeof(unsigned long long);

            std::memcpy(modem->tx.data.buf, (char *)&g, size);
            std::memcpy(modem->tx.data.buf, (char *)&p, size);
            std::memcpy(modem->tx.data.buf, (char *)&B, size);

            data = "synack";
            len = std::strlen(data) + 3 * size;
        }
        else if (std::strncmp(rx->buf, "synack", 6) == 0)
        {
            char _g[8];
            char _p[8];
            char _B[8];

            std::memcpy(&rx->buf[6], _g, 8);
            std::memcpy(&rx->buf[14], _p, 8);
            std::memcpy(&rx->buf[22], _B, 8);

            g = *(unsigned long long *)_g;
            p = *(unsigned long long *)_p;
            B = *(unsigned long long *)_B;

            std::cout << g << " " << p << " " << B << std::endl;

            data = "ack";
            len = std::strlen(data) + 3;
            connectionEstablished = true;
        }
        else if (std::strncmp(rx->buf, "ack", 3) == 0)
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
    modem->rx.data.userPtr = static_cast<void *>(modem); //To handle with chip from rx callback
    modem->tx.data.userPtr = static_cast<void *>(modem); //To handle with chip from tx callback
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
        g = generatePrimeNumber<16>();
        p = generatePrimeNumber<16>();
        a = generatePrimeNumber<16>();
        A = modpow(g, a, p);

        std::cout << g << " " << p << " " << A << std::endl;

        std::size_t size = sizeof(unsigned long long);

        std::memcpy(modem.tx.data.buf, (char *)&g, size);
        std::memcpy(modem.tx.data.buf, (char *)&p, size);
        std::memcpy(modem.tx.data.buf, (char *)&A, size);

        const char *data = "syn";
        std::size_t len = std::strlen(data);
        memcpy(modem.tx.data.buf, data, len);
        modem.tx.data.size = len + 3 * size;
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
