#pragma once
#include "common.h"

uint16_t readLEshort(const uint8_t *data, size_t offset);
uint32_t readLElong(const uint8_t *data, size_t offset);

void writeLEshort(uint8_t *data, uint16_t input, size_t offset);
void writeLElong(uint8_t *data, uint32_t input, size_t offset);

bool fileExists(const char *name);

int loadFile(const char *name, uint8_t **data);
int writeFile(const char *name, void *data, size_t amount);

void loadProfile();
void saveProfile();
