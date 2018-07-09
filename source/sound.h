#pragma once
#include "common.h"

extern uint32_t audioFrequency;

extern SDL_AudioSpec soundDevice;

//Sound functions
void initSound();
void endSound();
