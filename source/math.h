#pragma once
#include "common.h"

int getSin(BYTE deg);
int getCos(BYTE deg);
uint8_t getAtan(int x, int y);

int random(uint32_t mi, uint32_t ma);
int sign(int x);
int clamp(int x, int mi, int ma);