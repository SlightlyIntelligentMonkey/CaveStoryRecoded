#include "sound.h"

uint32_t audioFrequency;

void initSound()
{
	audioFrequency = 44100; //if this isn't 44100 it probably won't sound good

	smixerInit();
	smixerStart();
}

void endSound()
{
	smixerQuit();
}
