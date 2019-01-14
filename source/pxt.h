#pragma once

#include <string>
#include <cstdint>
#include "sound.h"

extern SOUND *sounds[0x100];

void makeWaveTables();
int loadSound(const std::string& path, int no);
