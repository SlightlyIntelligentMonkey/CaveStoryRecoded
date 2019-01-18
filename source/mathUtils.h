#pragma once
#include "common.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#ifndef M_PI	// cmath doesn't define M_PI if it's C99 comformant
constexpr auto M_PI = 3.14159265358979323846;
#endif

// These functions work with degrees from 0 to 256 so they aren't the pinnacle of accuracy (lol)
int getSin(uint8_t deg)  attrConst;
int getCos(uint8_t deg)  attrConst;
uint8_t getAtan(int x, int y) attrConst;

int random(int32_t mi, int32_t ma);
template <typename T> int8_t sign(T x)
{
	return (x > 0) - (x < 0);
}

 template <typename T> attrConst T clamp(T x, T mi, T ma)
{
    return std::max(std::min(ma, x), mi);
}

//returns result of normalized sinc
template <typename T> T sinc(T x)
{
	return (sin(M_PI*x) / (M_PI*x));
}

template <typename T> T getGCD(T a, T b)
{
    T c = a % b;
    while (c != 0)
    {
        a = b;
        b = c;
        c = a % b;
    }
    return b;
}
