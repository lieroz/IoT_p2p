#include "rsa.h"

#include <cmath>

namespace rsa
{
bool isPrime(std::size_t prime)
{
    std::size_t j = static_cast<std::size_t>(sqrt(static_cast<long double>(prime)));

    for (std::size_t i = 2; i <= j; i++)
    {
        if ( prime % i == 0 )
        {
            return false;
        }
    }

    return true;
}

std::size_t greatestCommonDivisor(std::size_t e, std::size_t t)
{
    while (e > 0)
    {
        std::size_t myTemp;

        myTemp = e;
        e = t % e;
        t = myTemp;
    }

    return t;
}

std::size_t calculateE(std::size_t t)
{
    std::size_t e;

    for (std::size_t e = 2; e < t; e++)
    {
        if (greatestCommonDivisor(e, t) == 1)
        {
            return e;
        }
    }

    return -1;
}

std::size_t calculateD(std::size_t e, std::size_t t)
{
    std::size_t d;
    std::size_t k = 1;

    while (true)
    {
        k = k + t;

        if (k % e == 0)
        {
            d = (k / e);
            return d;
        }
    }

}


std::size_t encrypt(std::size_t i, std::size_t e, std::size_t n)
{
    std::size_t current = i - 97;
    std::size_t result = 1;

    for (std::size_t j = 0; j < e; j++)
    {
        result = result * current;
        result = result % n;
    }

    return result;
}

std::size_t decrypt(std::size_t i, std::size_t d, std::size_t n)
{
    std::size_t current = i;
    std::size_t result = 1;

    for (std::size_t j = 0; j < d; j++)
    {
        result = result * current;
        result = result % n;
    }

    return result + 97;
}

}
