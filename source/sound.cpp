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
int8_t waveModelTable[6][256];

void makeWaveTables()
{
	int v0;
	signed int i;
	signed int j;
	signed int k;
	signed int l;
	signed int m;
	signed int n;
	signed int o;
	int v2;
	int v1;
	int b3;

	for (i = 0; i < 256; ++i)
	{
		waveModelTable[0][i] = (unsigned __int64)(sin((long double)i * 6.283184 / 256.0) * (long double)0x40);
		v0 = waveModelTable[0][i];
	}

	v2 = 0;
	for (j = 0; j < 64; ++j)
		waveModelTable[1][j] = (v2++ << 6) / 0x40;

	v1 = 0;
	while (j < 192)
		waveModelTable[1][j++] = 0x40 - (v1++ << 6) / 0x40;

	b3 = 0;
	while (j < 256)
		waveModelTable[1][j++] = (b3++ << 6) / 0x40 - 0x40;

	for (k = 0; k < 256; ++k)
		waveModelTable[2][k] = k / 2 - 0x40;

	for (l = 0; l < 256; ++l)
		waveModelTable[3][l] = 0x40 - l / 2;

	for (m = 0; m < 128; ++m)
		waveModelTable[4][m] = 0x40;

	for (n = 128; n < 256; n++)
		waveModelTable[4][n] = -0x40;

	srand(0);
	for (o = 0; o < 256; ++o)
		waveModelTable[5][o] = (char)rand() / 2;
}

//Audio callback and things
void mixSounds(Uint8 *stream, int len)
{
	for (int i = 0; i < len; i++)
	{
		for (int sound = 0; sound < 0x100; sound++)
		{
			const auto waveSamples = (size_t)(22050.0 / (double)sampleRate * 4096.0);

			if (sounds[sound].playing == true)
			{
				sounds[sound].pos += waveSamples;

				if ((sounds[sound].pos >> 12) >= sounds[sound].length)
				{
					sounds[sound].playing = false;
				}
				else
				{
					const size_t s_offset_1 = sounds[sound].pos >> 12;

					const int sample1 = sounds[sound].wave[s_offset_1] - 0x80;
					int sample2 = 0; //orgDrums[wave].wave[(s_offset_1 + 1) % 0x100] - 0x80; //(unsigned __int8)(((unsigned int)((s_offset_1 + 1) >> 31) >> 24) + s_offset_1 + 1) - ((unsigned int)((s_offset_1 + 1) >> 31) >> 24)];

					if ((sounds[sound].pos >> 12) < sounds[sound].length - 1)
						sample2 = sounds[sound].wave[s_offset_1 + 1] - 0x80;

					const auto val = (int)(sample1 + (sample2 - sample1) * ((double)(sounds[sound].pos & 0xFFF) / 4096.0));

					stream[2 * i] += (uint8_t)((long double)val);
					stream[2 * i + 1] += (uint8_t)((long double)val);
				}
			}
		}
	}
}

void audio_callback(void *userdata, Uint8 *stream, int len) // TBD : Handle userdata parameter
{
	memset(stream, 0, len);

	mixSounds(stream, len / 2);
	mixOrg(stream, len / 2);
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

int makePixelWaveData(std::vector<long double> pxtData, uint8_t *data)
{
	int v2; // eax
	long double v3; // fst7
	double d1; // [esp+18h] [ebp-158h]
	double mainOffset; // [esp+28h] [ebp-148h]
	double currentEnvelope; // [esp+38h] [ebp-138h]
	double currentEnvelopea; // [esp+38h] [ebp-138h]
	double currentEnvelopeb; // [esp+38h] [ebp-138h]
	double currentEnvelopec; // [esp+38h] [ebp-138h]
	double d3; // [esp+40h] [ebp-130h]
	double d2; // [esp+48h] [ebp-128h]
	double pitchOffset; // [esp+50h] [ebp-120h]
	char envelopeTable[256]; // [esp+58h] [ebp-118h]
	int a; // [esp+164h] [ebp-Ch]
	double volumeOffset; // [esp+168h] [ebp-8h]

	memset(envelopeTable, 0, 0x100u);
	currentEnvelope = (long double)pxtData[14];

	size_t i = 0;

	currentEnvelope = (long double)pxtData[14];
	while (i < pxtData[15])
	{
		envelopeTable[i] = (unsigned __int64)currentEnvelope;
		currentEnvelope = ((long double)pxtData[16] - (long double)pxtData[14])
			/ (long double)pxtData[15]
			+ currentEnvelope;
		++i;
	}
	currentEnvelopea = (long double)pxtData[16];
	while (i < pxtData[17])
	{
		envelopeTable[i] = (unsigned __int64)currentEnvelopea;
		currentEnvelopea = ((long double)pxtData[18] - (long double)pxtData[16])
			/ (long double)(pxtData[17] - pxtData[15])
			+ currentEnvelopea;
		++i;
	}
	currentEnvelopeb = (long double)pxtData[18];
	while (i < pxtData[19])
	{
		envelopeTable[i] = (unsigned __int64)currentEnvelopeb;
		currentEnvelopeb = ((long double)pxtData[20] - (long double)pxtData[18])
			/ (long double)(pxtData[19] - pxtData[17])
			+ currentEnvelopeb;
		++i;
	}
	currentEnvelopec = (long double)pxtData[20];
	while (i < 256)
	{
		envelopeTable[i] = (unsigned __int64)currentEnvelopec;
		currentEnvelopec = currentEnvelopec
			- (long double)pxtData[20] / (long double)(256 - pxtData[19]);
		++i;
	}

	pitchOffset = (long double)pxtData[9];
	mainOffset = (long double)pxtData[5];
	volumeOffset = (long double)pxtData[13];

	if (pxtData[3] == 0.0)
		d1 = 0.0;
	else
		d1 = 256.0 / ((long double)pxtData[1] / pxtData[3]);
	if (pxtData[7] == 0.0)
		d2 = 0.0;
	else
		d2 = 256.0 / ((long double)pxtData[1] / pxtData[7]);
	if (pxtData[11] == 0.0)
		d3 = 0.0;
	else
		d3 = 256.0 / ((long double)pxtData[1] / pxtData[11]);

	for (i = 0; i < pxtData[1]; ++i)
	{
		a = (signed int)(unsigned __int64)mainOffset % 256;
		v2 = (signed int)(unsigned __int64)pitchOffset % 256;

		data[i] = envelopeTable[(unsigned __int64)((long double)(i << 8) / (long double)pxtData[1])]
			* (pxtData[4]
				* waveModelTable[(size_t)pxtData[2]][a]
				/ 64
				* (pxtData[12]
					* waveModelTable[(size_t)pxtData[10]][(signed int)(unsigned __int64)volumeOffset % 256]
					/ 64
					+ 64)
				/ 64)
			/ 64
			+ -128;

		if (waveModelTable[(size_t)pxtData[6]][v2] >= 0)
			v3 = (d1 + d1)
			* (long double)waveModelTable[(size_t)pxtData[6]][(signed int)(unsigned __int64)pitchOffset % 256]
			* (long double)pxtData[8]
			/ 64.0
			/ 64.0
			+ d1
			+ mainOffset;
		else
			v3 = d1
			- d1
			* 0.5
			* (long double)-waveModelTable[(size_t)pxtData[6]][v2]
			* (long double)pxtData[8]
			/ 64.0
			/ 64.0
			+ mainOffset;
		mainOffset = v3;
		pitchOffset = pitchOffset + d2;
		volumeOffset = volumeOffset + d3;
	}

	return 1;
}

int loadSound(const char *path, size_t id)
{
	std::vector<std::string> lines = getLinesFromFile(path);

	if (lines.size() == 92)
	{
		size_t size = 0;

		std::vector<long double> lineNumbers[4];
		for (int i = 0; i < 4; ++i)
		{
			lineNumbers[i] = getNumbersFromString(lines[88 + i]);

			if (lineNumbers[i].size() != 21)
			{
				char error[0x100];
				sprintf(error, "PXT: %s\nChannel: %s\nInvalid amount of properties: %s", path, std::to_string(i).c_str(), std::to_string(lineNumbers[i].size()).c_str());
				doCustomError(error);
			}
		}
		
		//Get size
		for (int i = 0; i < 4; i++)
		{
			if (lineNumbers[i][1] > size)
				size = lineNumbers[i][1];
		}

		uint8_t *dest = (uint8_t *)malloc(size);
		uint8_t *pBlock = (uint8_t *)malloc(size);

		if (dest && pBlock)
		{
			memset(dest, 0x80, size);
			memset(pBlock, 0x80, size);
			
			for (int i = 0; i < 4; ++i)
			{
				if (!makePixelWaveData(lineNumbers[i], dest))
				{
					if (dest)
						free(dest);
					if (pBlock)
						free(pBlock);
					return -1;
				}
				
				for (int j = 0; j < lineNumbers[i][1]; ++j)
				{
					if (dest[j] + pBlock[j] - 0x100 >= -0x7F)
					{
						if (dest[j] + pBlock[j] - 0x100 <= 0x7F)
							pBlock[j] += dest[j] + -0x80;
						else
							pBlock[j] = -1;
					}
					else
					{
						pBlock[j] = 0;
					}
				}
			}

			sounds[id].wave = (uint8_t*)malloc(size);
			if (!sounds[id].wave)
				return -1;
			sounds[id].length = size;
			memcpy(sounds[id].wave, pBlock, size);
		}
		else
		{
			if (dest)
				free(dest);
			if (pBlock)
				free(pBlock);
			return -1;
		}
	}
	else
	{
		char error[0x100];
		sprintf(error, "Can't parse .pxt which isn't exactly 92 lines long.\n(given file is %s lines long)", std::to_string(lines.size()).c_str());
		doCustomError(error);
	}

	return 1;
}

const char *hexNibble[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

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
				if (loadSound(path, (n1 << 4) + n2) <= 0)
				{
					char error[0x100];
					sprintf(error, "Failed to load PXT %s", path);
					doCustomError(error);
				}
			}
		}
	}
}

void freeSounds() noexcept
{

}

void playSound(size_t sound_no, int soundMode) noexcept
{
	sounds[sound_no].pos = 0;
	sounds[sound_no].playing = true;
}
