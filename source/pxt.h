#pragma once

#include <string>
#include <cstdint>

void makeWaveTables();
int loadSound(const std::string& path, uint8_t **buf, size_t *length);
