#include "sound.h"
#include "org.h"
#include "input.h"
#include "filesystem.h"

#include <stdlib.h>
#include <memory>
#include <stdio.h>
#include <SDL.h>

struct SOUND_EFFECT
{
	int *buf;
	Uint32 length;
	Uint32 pos;
	uint8_t mode;
};
SOUND_EFFECT sounds[160];

SDL_AudioDeviceID soundDev = 0;
SDL_AudioSpec soundSpec;
SDL_AudioSpec want;

//use int pointers for easy use
void mixAudioSFX(int *dst, Uint32 len, SOUND_EFFECT *sound)
{
	unsigned int currentPos = 0;
	//using 32 signed int on native system 
	while (sound->pos + currentPos < sound->length && (currentPos << 3) < len)
	{
		dst[(currentPos << 1)] += sound->buf[sound->pos + currentPos];
		dst[(currentPos << 1) + 1] += sound->buf[sound->pos + currentPos];
		currentPos++;
	}
	sound->pos += currentPos;
}

Uint32 frequency = 22050;
void mixAudioOrg(int *dst, Uint32 len)
{
	unsigned int currentPos = 0;

	if (org == nullptr || org->samplesPerStep == 0)
	{
		return;
	}

	if (isKeyDown(SDL_SCANCODE_UP))
	{
		frequency = static_cast<uint32_t>(static_cast<double>(frequency) * 1.01);
	}
	if (isKeyDown(SDL_SCANCODE_DOWN))
	{
		frequency = static_cast<uint32_t>(static_cast<double>(frequency) * 0.99);
	}

//playWave(66, dst, len, frequency);
//return;

	while ((currentPos << 2) < len)
	{
		if (org->stepBufPos >= org->stepBufLen)
		{
			org->stepBufPos = 0;
			org->playData();
		}
		while (org->stepBufPos < org->stepBufLen && (currentPos << 2) < len)
		{
			if (org->samplesPerStep == 0)
			{
				return;
			}
			dst[currentPos] += org->stepBuf[org->stepBufPos];
			currentPos++;
			org->stepBufPos++;
		}
	}
}

void audio_callback(void *userdata, Uint8 *stream, int len) // TBD : Handle userdata parameter
{
	memset(stream, 0, len);

	for (unsigned int sfx = 0; sfx < _countof(sounds); sfx++)
	{
		if (sounds[sfx].pos < sounds[sfx].length)
		{
			mixAudioSFX((int*)stream, len, &sounds[sfx]);
		}
	}

	mixAudioOrg((int*)stream, len);
}

void initAudio()
{
	want.channels = 2;
	want.freq = sampleRate;
	want.format = AUDIO_S32SYS;
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
	{
		doError();
	}
	memset(sounds, 0, sizeof(sounds));

	iniOrg();
}

//since sdl doesn't actually get the loaded wav in the specified format,
//we have to do it ourselves
void loadSound(const char *path, SDL_AudioSpec *spec, int **buf, Uint32 *length)
{
	uint8_t *pBuf = nullptr;

	SDL_LoadWAV(path, spec, &pBuf, length);

	if (pBuf == nullptr)
		doError();

	auto fakeBuf = static_cast<int *>(malloc(*length * 4));
	if (fakeBuf == nullptr)
		doCustomError("Could not allocate memory for fakeBuf");

	//number of data points to interolate (22050 is the sample rate of .pxt wavs)
	int dist = sampleRate / 22050;

	auto realBuf = static_cast<int *>(calloc(dist, *length * 4));
	if (realBuf == nullptr)
		doCustomError("Could not allocate memory for realBuf");

	//converting to 32 signed int format from unsigned 8 bit
	for (size_t b = 0; b < *length; b++)
	{
		fakeBuf[b] = (0x7FFFFFFF / 0xFF) * (pBuf[b] - ((0xFF / 2) + 1));
	}

	//interpolation
	for (size_t i = 0; i < *length; i++)
	{
		realBuf[i << 1] = fakeBuf[i];
		realBuf[(i << 1) + 1] = fakeBuf[i];
	}

	free(fakeBuf);
	SDL_FreeWAV(pBuf);

	*length *= dist;
	*buf = realBuf;
}


const char* hexStr[16] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };

void loadSounds()
{
	char path[64];

	for (int i = 0; i < 10; i++)
	{
		for (int v = 0; v < 16; v++)
		{
			int s = i * 16 + v;
			sprintf(path, "data/Sound/%s%s.wav", hexStr[i], hexStr[v]);

			if (fileExists(path))
			{
				loadSound(path, &soundSpec, &sounds[s].buf, &sounds[s].length);
				sounds[s].pos = sounds[s].length;
			}
			else
			{
				sounds[s].buf = nullptr;
			}
		}
	}

	//have to organize drums like this because pixel dumb
	drumTbl[0].buf = sounds[0x96 + 0].buf;
	drumTbl[0].len = sounds[0x96 + 0].length;

	//drumTbl[1].buf = sounds[0x96 + 0].buf;
	//drumTbl[1].len = sounds[0x96 + 0].length;

	drumTbl[2].buf = sounds[0x96 + 1].buf;
	drumTbl[2].len = sounds[0x96 + 1].length;

	//drumTbl[3].buf = sounds[0x96 + 0].buf;
	//drumTbl[3].len = sounds[0x96 + 0].length;

	drumTbl[4].buf = sounds[0x96 + 4].buf;
	drumTbl[4].len = sounds[0x96 + 4].length;

	drumTbl[5].buf = sounds[0x96 + 2].buf;
	drumTbl[5].len = sounds[0x96 + 2].length;

	drumTbl[6].buf = sounds[0x96 + 3].buf;
	drumTbl[6].len = sounds[0x96 + 3].length;

	//drumTbl[7].buf = sounds[0x96 + 3].buf;
	//drumTbl[7].len = sounds[0x96 + 3].length;

	drumTbl[8].buf = sounds[0x96 + 5].buf;
	drumTbl[8].len = sounds[0x96 + 5].length;

	SDL_PauseAudioDevice(soundDev, 0);
}

void freeSounds()
{
	for (int s = 0; s < _countof(sounds); s++)
	{
		free(sounds[s].buf);
	}
}

void playSound(int sound_no)
{
	if (sound_no > _countof(sounds) - 1)
	{
		return;
	}
	if (sounds[sound_no].buf == nullptr)
	{
		return;
	}
	sounds[sound_no].pos = 0;
	sounds[sound_no].mode = 1;
}

void playSound(int sound_no, int mode)
{
	if (sound_no > _countof(sounds) - 1)
	{
		return;
	}
	if (sounds[sound_no].buf == nullptr)
	{
		return;
	}
	sounds[sound_no].pos = 0;
	sounds[sound_no].mode = mode;
}
