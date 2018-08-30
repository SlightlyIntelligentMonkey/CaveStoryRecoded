#include "sound.h"
#include "org.h"
#include "input.h"
#include "filesystem.h"
#include "stdUtils.h"

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <SDL.h>

using std::string;

//Variable things
SOUND sounds[0x100];

SDL_AudioDeviceID soundDev;
SDL_AudioSpec soundSpec;
SDL_AudioSpec want;

//Pxt waves
uint8_t waveModelTable[6][256];

void makeWaveTables()
{
	int v0;
	signed int i;
	signed int j;
	signed int k;
	signed int l;
	signed int m;
	signed int n;
	int v2;
	int v1;
	int b3;

	for (i = 0; i < 256; ++i)
	{
		waveModelTable[0][i] = (unsigned __int64)(sin((long double)i * 6.283184 / 256.0) * 64.0);
		v0 = waveModelTable[0][i];
	}

	v2 = 0;
	for (j = 0; j < 64; ++j)
		waveModelTable[1][j] = (v2++ << 6) / 64;

	v1 = 0;
	while (j < 192)
		waveModelTable[1][j++] = 64 - (v1++ << 6) / 64;

	b3 = 0;
	while (j < 256)
		waveModelTable[1][j++] = (b3++ << 6) / 64 - 64;

	for (k = 0; k < 256; ++k)
		waveModelTable[2][k] = k / 2 - 64;

	for (l = 0; l < 256; ++l)
		waveModelTable[3][l] = 64 - l / 2;

	for (m = 0; m < 128; ++m)
		waveModelTable[4][m] = 64;

	while (m < 256)
		waveModelTable[4][m++] = -64;

	srand(0);
	for (n = 0; n < 256; ++n)
		waveModelTable[5][n] = (char)rand() / 2;
}

//Audio callback and things
void audio_callback(void *userdata, Uint8 *stream, int len) // TBD : Handle userdata parameter
{
	memset(stream, 0, len);
	mixOrg(stream, len / 2);
}

void initAudio()
{
	initOrganya();

	//Create sound device
	want.channels = 2;
	want.freq = sampleRate;
	want.format = AUDIO_S8;
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

//Loading and parsing pxt
std::vector<long double> getNumbersFromString(const string& str)
{
	std::vector<long double> numbers;

	long double currentValue = 0;
	bool parsingNumber = false;
	bool numberDecimal = false;

	size_t i = 0;
	while (str[i])
	{
		if (str[i] == '.' || isdigit(str[i]))
		{
			if (!parsingNumber)
			{
				parsingNumber = true;
				currentValue = 0;
				numberDecimal = (str[i] == '.');
			}

			if (str[i] != '.')
			{
				//Get position
				size_t v = i;
				while (str[v])
				{
					if (str[v] == '.' || (isdigit(str[v])))
					{
						if (str[v] == '.')
							break;
					}
					else
					{
						break;
					}
					v++;
				}

				//Add value to number
				if (!numberDecimal)
					currentValue += (long double)(str[i] - 0x30) * (pow(10, (v - i) - 1));
				else
					currentValue += (long double)(str[i] - 0x30) * (1.0 / pow(10, 2 - ((v - i) - 1)));
			}
			else
			{
				numberDecimal = true;
			}
		}
		else
		{
			if (parsingNumber)
			{
				parsingNumber = false;
				numbers.push_back(currentValue);
			}
		}

		i++;
	}

	if (parsingNumber)
	{
		parsingNumber = false;
		numbers.push_back(currentValue);
	}

	return numbers;
}

void loadSound(const char *path)
{
	std::vector<std::string> lines = getLinesFromFile(path);

	if (lines.size() == 92)
	{

	}
	else
	{
		char error[0x100];
		sprintf(error, "Can't parse .pxt which isn't exactly 92 lines long.\n(given file is %s lines long)", std::to_string(lines.size()).c_str());
		// doCustomError(error);
	}
}

void loadSounds()
{
	memset(sounds, 0, sizeof(sounds));
	for (size_t s = 0; s < 0x80; s++)
	{
		string path = "data/Sound/" + hexToString(s) + ".wav";

		if (fileExists(path.c_str()))
			loadSound(path.c_str());
	}
}

void freeSounds() noexcept
{

}

void playSound(size_t sound_no, int soundMode) noexcept
{
	
}
