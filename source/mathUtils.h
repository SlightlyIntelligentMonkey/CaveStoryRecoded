#pragma once
#include "common.h"

#ifndef M_PI	// cmath doesn't define M_PI if it's C99 comformant
constexpr auto M_PI = 3.14159265358979323846;
#endif

int getSin(uint8_t deg)  attrConst;
int getCos(uint8_t deg)  attrConst;
uint8_t getAtan(int x, int y) attrConst;

int random(int32_t mi, int32_t ma);
int sign(int x)  attrConst;
int clamp(int x, int mi, int ma)  attrConst;

double sinc(double x) attrConst;

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
