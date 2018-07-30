#pragma once
#include "common.h"

int getSin(uint8_t deg);
int getCos(uint8_t deg);
uint8_t getAtan(int x, int y);

int random(uint32_t mi, uint32_t ma);
int sign(int x);
int clamp(int x, int mi, int ma);