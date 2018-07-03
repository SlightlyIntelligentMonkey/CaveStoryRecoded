//MOST SOUND CODE IS BASED OFF OF THE LINUX PORT OF CAVE STORY BY SIMON PARZER

#include "sound.h"

uint32_t audioFrequency;

void initSound()
{
	audioFrequency = 44100; //if this isn't 44100 it probably won't sound good

	smixerInit();
	//initWaveTables();
	smixerFreeChunks();
	initORG();
	smixerStart();
}

void endSound()
{
	//EndOrganya();
	smixerFreeChunks();
	smixerQuit();
}