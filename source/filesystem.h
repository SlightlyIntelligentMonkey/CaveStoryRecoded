#pragma once
#include "common.h"

unsigned short readLEshort(BYTE *data, unsigned int offset);
unsigned long readLElong(BYTE *data, unsigned int offset);

int loadFile(char *name, BYTE **data);