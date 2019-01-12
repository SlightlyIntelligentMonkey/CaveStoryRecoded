#include "sound.h"

#include <list>
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
using std::list;

//Sound struct
struct SOUND {
	uint8_t *wave; //Dynamic size
	size_t length;
	bool playing;
	long double pos;
	bool loops;
	unsigned long freq;
	long double volume;
	long double volume_l;
	long double volume_r;
};

//Variable things
list<SOUND> sound_objects;

SDL_AudioDeviceID soundDev;
SDL_AudioSpec soundSpec;
SDL_AudioSpec want;

SOUND* SoundObject_Create(size_t size, unsigned long freq)
{
	SOUND sound;
	sound.wave = new uint8_t[size];
	sound.length = size;
	sound.playing = false;
	sound.pos = 0.0L;
	sound.loops = false;
	sound.freq = freq;
	sound.volume = 1.0L;
	sound.volume_l = 1.0L;
	sound.volume_r = 1.0L;
	SDL_LockAudioDevice(soundDev);
	sound_objects.push_front(sound);
	SDL_UnlockAudioDevice(soundDev);
	return &sound_objects.front();
}

void SoundObject_Destroy(SOUND *sound)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(soundDev);

		delete[] sound->wave;

		for (auto it = sound_objects.begin(); it != sound_objects.end(); ++it)
		{
			if (&*it == sound)
			{
				sound_objects.erase(it);
				break;
			}
		}

		SDL_UnlockAudioDevice(soundDev);
	}
}

void SoundObject_Lock(SOUND *sound, uint8_t **buffer, size_t *size)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(soundDev);

		if (buffer != nullptr)
			*buffer = sound->wave;

		if (size != nullptr)
			*size = sound->length;
	}
}

void SoundObject_Unlock(SOUND *sound)
{
	if (sound != nullptr)
		SDL_UnlockAudioDevice(soundDev);
}

void SoundObject_SetPosition(SOUND *sound, size_t pos)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(soundDev);
		sound->pos = pos;
		SDL_UnlockAudioDevice(soundDev);
	}
}

void SoundObject_SetFrequency(SOUND *sound, unsigned long freq)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(soundDev);
		sound->freq = freq;
		SDL_UnlockAudioDevice(soundDev);
	}
}

static long double MillibelToVolume(long volume)
{
	// Volume is in hundredths of decibels, from 0 to -10000
	volume = clamp(volume, (long)-10000, (long)0);
	return pow(10, volume / 2000.0);
}

void SoundObject_SetVolume(SOUND *sound, long volume)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(soundDev);
		sound->volume = MillibelToVolume(volume);
		SDL_UnlockAudioDevice(soundDev);
	}
}

void SoundObject_SetPan(SOUND *sound, long pan)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(soundDev);
		sound->volume_l = MillibelToVolume(-pan);
		sound->volume_r = MillibelToVolume(pan);
		SDL_UnlockAudioDevice(soundDev);
	}
}

void SoundObject_Play(SOUND *sound, bool loops)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(soundDev);
		sound->playing = true;
		sound->loops = loops;
		SDL_UnlockAudioDevice(soundDev);
	}
}

void SoundObject_Stop(SOUND *sound)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(soundDev);
		sound->playing = false;
		SDL_UnlockAudioDevice(soundDev);
	}
}

//Audio callback and things
void mixSounds(int16_t *stream, int len)
{
	for (int i = 0; i < len; ++i)
	{
		//Put current stream sample into temp samples
		int32_t tempSampleL = stream[i * 2];
		int32_t tempSampleR = stream[i * 2 + 1];

		for (auto& sound : sound_objects)
		{
			if (sound.playing)
			{
				if (sound.pos >= sound.length)
				{
					if (sound.loops)
					{
						sound.pos = fmod(sound.pos, sound.length);
					}
					else
					{
						SoundObject_Stop(&sound);
						continue;
					}
				}

				const size_t position = floor(sound.pos);

				// Perform sound interpolation
				const int sample1 = (sound.wave[position] - 0x80) << 8;
				const int sample2 = (!sound.loops && position + 1 >= sound.length) ? 0 : (sound.wave[(position + 1) % sound.length] - 0x80) << 8;

				const int val = static_cast<int>(sample1 + (sample2 - sample1) * fmod(sound.pos, 1.0f));

				tempSampleL += (val * sound.volume * sound.volume_l);
				tempSampleR += (val * sound.volume * sound.volume_r);

				sound.pos += (long double)sound.freq / sampleRate;
			}
		}

		//Put into main stream and clip buffer
		stream[2 * i] = clamp(tempSampleL, (int32_t)-0x7FFF, (int32_t)0x7FFF);
		stream[2 * i + 1] = clamp(tempSampleR, (int32_t)-0x7FFF, (int32_t)0x7FFF);
	}
}

void audio_callback(void * /*userdata*/, Uint8 *stream, int len) // TBD : Handle userdata parameter
{
	memset(stream, 0, len);

	updateOrg(len / 4);
	mixSounds(reinterpret_cast<int16_t*>(stream), len / 4);
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
	for (unsigned int n1 = 0; n1 < 0x10; n1++)
	{
		for (unsigned int n2 = 0; n2 < 0x10; n2++)
		{
			char path[0x100];
			sprintf(path, "data/Sound/%X%X.pxt", n1, n2);

			if (fileExists(path))
			{
				if (loadSound(path, (n1 << 4) + n2) <= 0)
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
		SoundObject_Destroy(sound);
}

void playSound(size_t sound_no, int soundMode)
{
	switch (soundMode)
	{
		case -1:
			// Play and loop (if sound is already playing, just set the loop flag)
			SoundObject_Play(sounds[sound_no], true);
			break;
		case 1:
			// Play sound (if sound if already playing, restart it)
			SoundObject_Stop(sounds[sound_no]);
			SoundObject_SetPosition(sounds[sound_no], 0);
			SoundObject_Play(sounds[sound_no], false);
			break;
		case 0:
			// Stop sound
			SoundObject_Stop(sounds[sound_no]);
			break;
	}
}
