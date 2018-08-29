#pragma once
#include "common.h"

#ifndef M_PI	// cmath doesn't define M_PI if it's C99 comformant
#define M_PI 3.14159265358979323846
#endif

int getSin(uint8_t deg) noexcept attrConst;
int getCos(uint8_t deg) noexcept attrConst;
uint8_t getAtan(int x, int y) attrConst;

int random(int32_t mi, int32_t ma);
int sign(int x) noexcept attrConst;
int clamp(int x, int mi, int ma) noexcept attrConst;

double sinc(double x) noexcept attrConst;
int getGCD(int a, int b) noexcept attrConst;
