#include "sound.h"
#include <SDL.h>

struct SOUND_EFFECT
{
	int *buf;
	Uint32 length;
	Uint32 pos;
};
SOUND_EFFECT sounds[0x100];

SDL_AudioStream *audioStream;
SDL_AudioDeviceID soundDev = 0;
SDL_AudioSpec soundSpec;
SDL_AudioSpec want;

//use int pointers for easy use
void mixAudioSFX(int *dst, Uint32 len, SOUND_EFFECT *sound, Uint8 lVolume, Uint8 rVolume) // TBD : Handle lVolume and rVolume parameters
{
	unsigned int currentPos = 0;
	constexpr int temp = 0;
	//using 32 float point on native system 
	while (sound->pos + currentPos < sound->length && (currentPos << 2) < len)
	{
		dst[currentPos] += sound->buf[sound->pos + currentPos];
		currentPos++;
	}
	sound->pos += currentPos;
	return;
}

void audio_callback(void *userdata, Uint8 *stream, int len) // TBD : Handle userdata parameter
{
	memset(stream, 0, len);

	for (int sfx = 0; sfx < _countof(sounds); sfx++)
	{
		if (sounds[sfx].pos < sounds[sfx].length)
		{
			mixAudioSFX((int*)stream, len, &sounds[sfx], 100, 100);
		}
	}

	return;
}

void ini_audio()
{
	want.channels = 2;
	want.freq = 44100;
	want.format = AUDIO_S32SYS;
	want.samples = 1024;
	want.callback = audio_callback;
	want.userdata = 0;

	soundDev = SDL_OpenAudioDevice(
		NULL,
		0,
		&want,
		&soundSpec,
		NULL);

	if (soundDev == 0) { doError(); }
	memset(sounds, 0, sizeof(sounds));

	return;
}

//since sdl doesn't actually get the loaded wav in the specified format,
//we have to do it ourselves
void loadSound(const char *path, SDL_AudioSpec *spec, int **buf, Uint32 *length)
{
	constexpr int view = 0;
	BYTE *pBuf = nullptr;
	SDL_AudioSpec *lSpec;

	lSpec = SDL_LoadWAV(path, spec, &pBuf, length);
	if (pBuf == NULL)
		doError();

	int *fakeBuf = (int*)malloc(*length * 4);
	if (fakeBuf == nullptr)
		doCustomError("Could not allocate memory for fakeBuf");

	//number of data points to interolate
	int dist = 2;

	int *fakeFakeBuf = (int *)malloc(*length * 4 * 2);
	if (fakeFakeBuf == nullptr)
		doCustomError("Could not allocate memory for fakeFakeBuf");

	int *realBuf = (int*)calloc(dist, *length * 4 * 2);
	if (realBuf == nullptr)
		doCustomError("Could not allocate memory for realBuf");

	//converting to 32 signed int format from unsigned 8 bit
	for (size_t b = 0; b < *length; b++)
	{
		fakeBuf[b] = (0x7FFFFFFF / 0xFF) * (pBuf[b] - ((0xFF / 2) + 1));
	}

	//channels++
	for (size_t i = 0; i < *length; i++)
	{
		fakeFakeBuf[i << 1] = fakeBuf[i];
		fakeFakeBuf[(i << 1) + 1] = fakeBuf[i];
	}

	//interpolation
	for (size_t i = 0; i < *length << 1; i++)
	{
		realBuf[i << 1] = fakeFakeBuf[i];
		realBuf[(i << 1) + 1] = fakeFakeBuf[i];
	}

	free(fakeBuf);
	free(fakeFakeBuf);
	SDL_FreeWAV(pBuf);

	*length *= (2 + dist);
	*buf = realBuf;
	return;
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
				sounds[s].buf = NULL;
			}
		}
	}

	SDL_PauseAudioDevice(soundDev, 0);
	return;
}

void freeSounds()
{
	for (int s = 0; s < _countof(sounds); s++)
	{
		if (sounds[s].buf != NULL)
		{
			free(sounds[s].buf);
		}
	}
	return;
}

void playSound(int sound_no)
{
	if (sound_no > _countof(sounds)) { return; }
	if (sounds[sound_no].buf == NULL) { return; }
	sounds[sound_no].pos = 0;

	return;
}
