#pragma once
#include "common.h"

#ifndef M_PI	// cmath doesn't define M_PI if it's C99 comformant
#define M_PI 3.14159265358979323846
#endif

int getSin(uint8_t deg);
int getCos(uint8_t deg);
uint8_t getAtan(int x, int y);

int random(int32_t mi, int32_t ma);
int sign(int x);
int clamp(int x, int mi, int ma);

double sinc(double x);
int getGCD(int a, int b);
