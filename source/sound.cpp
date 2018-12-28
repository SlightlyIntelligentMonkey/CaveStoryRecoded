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
	for (int i = 0; i < len; i++)
	{
		//Put current stream sample into temp samples
		auto tempSampleL = (int32_t)stream[i * 2];
		auto tempSampleR = (int32_t)stream[i * 2 + 1];

		for (auto& sound : sounds)
		{
			const long double waveSamples = 22050.0 / (double)sampleRate;

			if (sound.playing)
			{
				sound.pos += waveSamples;

				if (sound.pos >= sound.length)
					sound.playing = false;
				else
				{
					const size_t s_offset_1 = (size_t)floor(sound.pos);

					const int sample1 = (sound.wave[s_offset_1] - 0x80) << 7;
					int sample2 = 0;

					if (sound.pos < sound.length - 1)
						sample2 = (sound.wave[s_offset_1 + 1] - 0x80) << 7;

					const auto val = static_cast<int>(sample1 + (sample2 - sample1) * fmod(sound.pos, 1.0f));

					tempSampleL += (val * 2);
					tempSampleR += (val * 2);
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

constexpr const char *hexNibble[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
void loadSounds()
{
	memset(sounds, 0, sizeof(sounds));
	for (size_t n1 = 0; n1 < 16; n1++)
	{
		for (size_t n2 = 0; n2 < 16; n2++)
		{
			char path[0x100];
			sprintf(path, "data/Sound/%s%s.pxt", hexNibble[n1], hexNibble[n2]);

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

void playSound(size_t sound_no, int /*soundMode*/)
{
	sounds[sound_no].pos = 0;
	sounds[sound_no].playing = true;
}
