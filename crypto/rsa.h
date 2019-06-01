#pragma once

#include <cstdlib>

namespace rsa
{
std::size_t calculateE(std::size_t t);

std::size_t calculateD(std::size_t e, std::size_t t);

std::size_t encrypt(std::size_t i, std::size_t e, std::size_t n);

std::size_t decrypt(std::size_t i, std::size_t d, std::size_t n);

}

