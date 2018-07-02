//ALL SOUND CODE IS BASED OFF OF THE LINUX PORT OF CAVE STORY BY SIMON PARZER

#include "sound.h"

void initSound()
{
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