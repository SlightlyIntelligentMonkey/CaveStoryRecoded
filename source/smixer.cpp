#include "smixer.h"

SDL_AudioSpec soundDevice;

//Mixer callback
void smixerCallback(void *userdata, Uint8 *stream, int len)
{

}

//Init, Start, and Quit
void smixerInit()
{
	SDL_zero(soundDevice);

	soundDevice.freq = audioFrequency;
	soundDevice.samples = 1024;
	soundDevice.format = AUDIO_S16;
	soundDevice.channels = 2;
	soundDevice.callback = smixerCallback;

	if (SDL_OpenAudio(&soundDevice, NULL) != 0)
		doCustomError("Couldn't open audio device");
}

void smixerStart()
{
	SDL_PauseAudio(0);
}

void smixerQuit()
{
	SDL_CloseAudio();
}
