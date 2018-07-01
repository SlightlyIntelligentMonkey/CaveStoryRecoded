#pragma once
#include "common.h"

uint16_t readLEshort(BYTE *data, unsigned int offset);
uint32_t readLElong(BYTE *data, unsigned int offset);

int loadFile(const char * name, BYTE ** data);