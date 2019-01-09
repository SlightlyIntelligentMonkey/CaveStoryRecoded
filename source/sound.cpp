#include "sound.h"

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <SDL_audio.h>
#include "pxt.h"
#include "org.h"
#include "input.h"
#include "filesystem.h"
#include "stdUtils.h"
#include "mathUtils.h"	// For M_PI definition
#include "main.h"

using std::string;

//Variable things
SOUND sounds[0x100];

SDL_AudioDeviceID soundDev;
SDL_AudioSpec soundSpec;
SDL_AudioSpec want;

//Audio callback and things
void mixSounds(int16_t *stream, int len)
{
	for (int i = 0; i < len; ++i)
	{
		//Put current stream sample into temp samples
		int32_t tempSampleL = stream[i * 2];
		int32_t tempSampleR = stream[i * 2 + 1];

		for (auto& sound : sounds)
		{
			if (sound.playing)
			{
				const size_t position = floor(sound.pos);

				// Perform sound interpolation
				const int sample1 = (sound.wave[position] - 0x80) << 7;
				const int sample2 = (sound.loops || sound.pos < sound.length - 1) ? (sound.wave[(position + 1) % sound.length] - 0x80) << 7 : 0;

				const int val = static_cast<int>(sample1 + (sample2 - sample1) * fmod(sound.pos, 1.0f));

				tempSampleL += (val * 2);
				tempSampleR += (val * 2);

				sound.pos += 22050.0L / sampleRate;

				if (sound.pos >= sound.length)
				{
					if (sound.loops)
						sound.pos = fmod(sound.pos, sound.length);
					else
						sound.playing = false;
				}

			}
		}

		//Put into main stream and clip buffer
		stream[2 * i] = clamp(tempSampleL, -0x7FFF, 0x7FFF);
		stream[2 * i + 1] = clamp(tempSampleR, -0x7FFF, 0x7FFF);
	}
}

void audio_callback(void * /*userdata*/, Uint8 *stream, int len) // TBD : Handle userdata parameter
{
	memset(stream, 0, len);

	mixSounds(reinterpret_cast<int16_t*>(stream), len / 4);
	mixOrg(reinterpret_cast<int16_t*>(stream), len / 4);
}

void initAudio()
{
	//Initialize Organya
	initOrganya();

	//Initialize PXT
	makeWaveTables();

	//Create sound device
	want.channels = 2;
	want.freq = sampleRate;
	want.format = AUDIO_S16;
	want.samples = 1024;
	want.callback = audio_callback;
	want.userdata = nullptr;

	soundDev = SDL_OpenAudioDevice(
		nullptr,
		0,
		&want,
		&soundSpec,
		0);

	if (soundDev == 0)
		doError();

	SDL_PauseAudioDevice(soundDev, 0);
}

void loadSounds()
{
	for (auto& sound : sounds)
	{
		sound.wave = nullptr;
		sound.length = 0;
		sound.playing = false;
		sound.length = 0.0L;
		sound.loops = false;
	}

	for (unsigned int n1 = 0; n1 < 0x10; n1++)
	{
		for (unsigned int n2 = 0; n2 < 0x10; n2++)
		{
			char path[0x100];
			sprintf(path, "data/Sound/%X%X.pxt", n1, n2);

			if (fileExists(path))
			{
				SOUND *sound = &sounds[(n1 << 4) + n2];

				if (loadSound(path, &sound->wave, &sound->length) <= 0)
				{
					char error[0x100];
					sprintf(error, "Failed to load PXT at %s", path);
					doCustomError(error);
				}
			}
		}
	}
}

void freeSounds()
{
	for (auto& sound : sounds)
		delete[] sound.wave;
}

void playSound(size_t sound_no, int soundMode)
{
	switch (soundMode)
	{
		case -1:
			// Play and loop (if sound is already playing, just set the loop flag)
			if (sounds[sound_no].playing == false)
			{
				sounds[sound_no].pos = 0;
				sounds[sound_no].playing = true;
			}
			sounds[sound_no].loops = true;
			break;
		case 1:
			// Play sound (if sound if already playing, restart it)
			sounds[sound_no].pos = 0;
			sounds[sound_no].playing = true;
			sounds[sound_no].loops = false;
			break;
		case 0:
			// Stop sound
			sounds[sound_no].playing = false;
			break;
	}
}
