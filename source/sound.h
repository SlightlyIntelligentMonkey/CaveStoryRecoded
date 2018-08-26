#pragma once
#include "common.h"

void ini_audio();
void loadSounds();
void freeSounds();

void playSound(int sound_no);
void playSound(int sound_no, int mode);

//these two are here so that way one only needs to include sound for org or sound effects
void changeOrg(const int num);
void resumeOrg();

//debug function
char *debugSound();

extern const unsigned int sampleRate;
extern Uint32 currentOrg;