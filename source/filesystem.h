#pragma once
#include "common.h"

uint16_t readLEshort(const BYTE *data, size_t offset);
uint32_t readLElong(const BYTE *data, size_t offset);

bool fileExists(const char *name);

int loadFile(const char *name, BYTE **data);
int writeFile(const char *name, void *data, size_t amount);

void loadProfile();
void saveProfile();
