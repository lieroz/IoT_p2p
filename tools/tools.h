#pragma once

#include <string>
#include <cmath>
#include <type_traits>
#include <bitset>
#include <random>
#include <stdexcept>

namespace tools
{

struct KeyPairHex
{
  std::string publicKey;
  std::string privateKey;
};

KeyPairHex RsaGenerateHexKeyPair(unsigned int aKeySize);

std::string RsaSignString(const std::string &aPrivateKeyStrHex,
        const std::string &aMessage);

bool RsaVerifyString(const std::string &aPublicKeyStrHex,
        const std::string &aMessage,
        const std::string &aSignatureStrHex);

std::string Sha256(const std::string &input);

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

} //tools

