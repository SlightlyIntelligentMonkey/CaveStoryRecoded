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

//Sound struct
struct SOUND {
	struct SOUND *next;

	uint8_t *wave; // Dynamic size
	size_t length;
	bool playing;
	long double pos;
	bool loops;
	unsigned long freq;
	float volume;
	float volume_l;
	float volume_r;
};

//Variable things
static SOUND *gSoundListHead = nullptr;

static SDL_AudioDeviceID gSoundDev;
static SDL_AudioSpec gSoundSpec;
static SDL_AudioSpec gWant;

SOUND* SoundObject_Create(size_t size, unsigned long freq)
{
	SOUND *sound = new SOUND;
	sound->wave = new uint8_t[size];
	sound->length = size;
	sound->playing = false;
	sound->pos = 0.0L;
	sound->loops = false;
	sound->freq = freq;
	sound->volume = 1.0f;
	sound->volume_l = 1.0f;
	sound->volume_r = 1.0f;
	SDL_LockAudioDevice(gSoundDev);
	sound->next = gSoundListHead;
	gSoundListHead = sound;
	SDL_UnlockAudioDevice(gSoundDev);
	return sound;
}

void SoundObject_Destroy(SOUND *sound)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(gSoundDev);

		for (SOUND **list_sound = &gSoundListHead; *list_sound != nullptr; list_sound = &(*list_sound)->next)
		{
			if (*list_sound == sound)
			{
				*list_sound = (*list_sound)->next;
				break;
			}
		}

		SDL_UnlockAudioDevice(gSoundDev);

		delete[] sound->wave;
		delete sound;

	}
}

void SoundObject_Lock(SOUND *sound, uint8_t **buffer, size_t *size)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(gSoundDev);

		if (buffer != nullptr)
			*buffer = sound->wave;

		if (size != nullptr)
			*size = sound->length;
	}
}

void SoundObject_Unlock(SOUND *sound)
{
	if (sound != nullptr)
		SDL_UnlockAudioDevice(gSoundDev);
}

void SoundObject_SetPosition(SOUND *sound, size_t pos)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(gSoundDev);
		sound->pos = pos;
		SDL_UnlockAudioDevice(gSoundDev);
	}
}

void SoundObject_SetFrequency(SOUND *sound, unsigned long freq)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(gSoundDev);
		sound->freq = freq;
		SDL_UnlockAudioDevice(gSoundDev);
	}
}

static float MillibelToVolume(long volume)
{
	// Volume is in hundredths of decibels, from 0 to -10000
	volume = clamp(volume, static_cast<decltype(volume)>(-10000), static_cast<decltype(volume)>(0));
	return pow(10.0f, volume / 2000.0f);
}

void SoundObject_SetVolume(SOUND *sound, long volume)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(gSoundDev);
		sound->volume = MillibelToVolume(volume);
		SDL_UnlockAudioDevice(gSoundDev);
	}
}

void SoundObject_SetPan(SOUND *sound, long pan)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(gSoundDev);
		sound->volume_l = MillibelToVolume(-pan);
		sound->volume_r = MillibelToVolume(pan);
		SDL_UnlockAudioDevice(gSoundDev);
	}
}

void SoundObject_Play(SOUND *sound, bool loops)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(gSoundDev);
		sound->playing = true;
		sound->loops = loops;
		SDL_UnlockAudioDevice(gSoundDev);
	}
}

void SoundObject_Stop(SOUND *sound)
{
	if (sound != nullptr)
	{
		SDL_LockAudioDevice(gSoundDev);
		sound->playing = false;
		SDL_UnlockAudioDevice(gSoundDev);
	}
}

// Audio callback and things
void mixSounds(float (*stream)[2], int len)
{
	for (SOUND *sound = gSoundListHead; sound != nullptr; sound = sound->next)
	{
		if (sound->playing)
		{
			for (int i = 0; i < len; ++i)
			{
				if (sound->pos >= sound->length)
				{
					if (sound->loops)
					{
						sound->pos = fmod(sound->pos, sound->length);
					}
					else
					{
						SoundObject_Stop(sound);
						break;
					}
				}

				const size_t position = static_cast<size_t>(sound->pos);

				// Perform sound interpolation
				const float sample1 = (sound->wave[position] - 0x80) / 128.0f;
				const float sample2  = (!sound->loops && position + 1 >= sound->length) 
					? 0.0f : (sound->wave[(position + 1) % sound->length] - 0x80) / 128.0f;

				const float interpolated_sample 
					= sample1 + (sample2 - sample1) * static_cast<float>(fmod(sound->pos, 1.0f));

				stream[i][0] += interpolated_sample * sound->volume * sound->volume_l;
				stream[i][1] += interpolated_sample * sound->volume * sound->volume_r;

				sound->pos += static_cast<double>(sound->freq) / sampleRate;
			}
		}
	}
}

void audio_callback(void *, Uint8 *stream_bytes, int len)
{
	float (*stream)[2] = reinterpret_cast<float(*)[2]>(stream_bytes);
	const int frames = len / (sizeof(float) * 2);

	for (int i = 0; i < frames; ++i)
	{
		stream[i][0] = 0.0f;
		stream[i][1] = 0.0f;
	}

	updateOrg(frames);
	mixSounds(stream, frames);
}

void initAudio()
{
	// Initialize Organya
	initOrganya();

	// Initialize PXT
	makeWaveTables();

	// Create sound device
	gWant.channels = 2;
	gWant.freq = sampleRate;
	gWant.format = AUDIO_F32;
	gWant.samples = 1024;
	gWant.callback = audio_callback;
	gWant.userdata = nullptr;

	gSoundDev = SDL_OpenAudioDevice(
		nullptr,
		0,
		&gWant,
		&gSoundSpec,
		0);

	if (gSoundDev == 0)
		doError();

	SDL_PauseAudioDevice(gSoundDev, 0);
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
	for (auto& sound : gSounds)
		SoundObject_Destroy(sound);
}

void playSound(size_t sound_no, int soundMode)
{
	switch (soundMode)
	{
		case -1:
			// Play and loop (if sound is already playing, just set the loop flag)
			SoundObject_Play(gSounds[sound_no], true);
			break;
		case 1:
			// Play sound (if sound if already playing, restart it)
			SoundObject_Stop(gSounds[sound_no]);
			SoundObject_SetPosition(gSounds[sound_no], 0);
			SoundObject_Play(gSounds[sound_no], false);
			break;
		case 0:
			// Stop sound
			SoundObject_Stop(gSounds[sound_no]);
			break;
	}
}
