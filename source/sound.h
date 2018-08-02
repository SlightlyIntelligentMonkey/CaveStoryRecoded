#pragma once
#include "common.h"

//struct of audio to be played
struct AUDIO
{
	BYTE *buf;
	Uint32 len;

	BYTE volume;

	AUDIO *next;
};

void ini_audio();
void loadSounds();
void freeSounds();
void playSound(int sound_no);
