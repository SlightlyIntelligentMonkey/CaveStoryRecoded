#include "sound.h"
#include "org.h"
#include "input.h"
#include "filesystem.h"
#include "stdUtils.h"
#include "mathUtils.h"	// For M_PI definition

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <SDL.h>

// M_PI is not Standard C++
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

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
	//Sine wave
	for (int i = 0; i < 256; ++i)
		waveModelTable[0][i] = (int8_t)(sin((long double)i * (M_PI * 2.0) / 256.0) * (long double)0x40);

	//Triangle
	int triangle = 0;
	for (int j = 0; j < 64; ++j) //Upwards
		waveModelTable[1][j] = (triangle++ << 6) / 0x40;
	triangle = 0;
	for (int j = 64; j < 192; ++j) //Downwards
		waveModelTable[1][j] = 0x40 - (triangle++ << 6) / 0x40;
	triangle = 0;
	for (int j = 192; j < 256; ++j) //Back upwards
		waveModelTable[1][j] = (triangle++ << 6) / 0x40 - 0x40;

	//Saw Up
	for (int j = 0; j < 256; ++j)
		waveModelTable[2][j] = j / 2 - 0x40;

	//Saw Down
	for (int j = 0; j < 256; ++j)
		waveModelTable[3][j] = 0x40 - j / 2;

	//Square
	for (int j = 0; j < 128; ++j)
		waveModelTable[4][j] = 0x40;
	for (int j = 128; j < 256; ++j)
		waveModelTable[4][j] = -0x40;

	//Noise
	srand(0);
	for (int j = 0; j < 256; ++j)
		waveModelTable[5][j] = (int8_t)rand() / 2;
}

//Audio callback and things
void mixSounds(int16_t *stream, int len)
{
	int32_t tempSampleL;
	int32_t tempSampleR;

	for (int i = 0; i < len; i++)
	{
		//Put current stream sample into temp samples
		tempSampleL = (int32_t)stream[i * 2];
		tempSampleR = (int32_t)stream[i * 2 + 1];

		for (auto& sound : sounds)
		{
			const auto waveSamples = (size_t)(22050.0 / (double)sampleRate * 4096.0);

			if (sound.playing == true)
			{
				sound.pos += waveSamples;

				if ((sound.pos >> 12) >= sound.length)
				{
					sound.playing = false;
				}
				else
				{
					const size_t s_offset_1 = sound.pos >> 12;

					const int sample1 = (sound.wave[s_offset_1] - 0x80) << 7;
					int sample2 = 0;

					if ((sound.pos >> 12) < sound.length - 1)
						sample2 = (sound.wave[s_offset_1 + 1] - 0x80) << 7;

					const auto val = (int)(sample1 + (sample2 - sample1) * ((double)(sound.pos & 0xFFF) / 4096.0));

					tempSampleL += val * 2;
					tempSampleR += val * 2;
				}
			}
		}

		//Clip buffer
		tempSampleL = clamp(tempSampleL, -0x7FFF, 0x7FFF);
		tempSampleR = clamp(tempSampleR, -0x7FFF, 0x7FFF);

		//Put into main stream
		stream[2 * i] = tempSampleL;
		stream[2 * i + 1] = tempSampleR;
	}
}

void audio_callback(void *userdata, Uint8 *stream, int len) // TBD : Handle userdata parameter
{
	memset(stream, 0, len);

	mixSounds((int16_t*)stream, len / 4);
	mixOrg((int16_t*)stream, len / 4);
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
	long double currentEnvelope;
	long double currentEnvelopea;
	long double currentEnvelopeb;
	long double currentEnvelopec;
	char envelopeTable[256];
	
	//Get some envelope stuff
	memset(envelopeTable, 0, 0x100u);
	currentEnvelope = pxtData[14];

	size_t i = 0;

	//Point A
	currentEnvelope = pxtData[14];
	while (i < pxtData[15])
	{
		envelopeTable[i] = (unsigned __int64)currentEnvelope;
		currentEnvelope = (pxtData[16] - pxtData[14])
			/ pxtData[15]
			+ currentEnvelope;
		++i;
	}

	//Point B
	currentEnvelopea = pxtData[16];
	while (i < pxtData[17])
	{
		envelopeTable[i] = (unsigned __int64)currentEnvelopea;
		currentEnvelopea = (pxtData[18] - pxtData[16])
			/ (pxtData[17] - pxtData[15])
			+ currentEnvelopea;
		++i;
	}

	//Point C
	currentEnvelopeb = pxtData[18];
	while (i < pxtData[19])
	{
		envelopeTable[i] = (unsigned __int64)currentEnvelopeb;
		currentEnvelopeb = (pxtData[20] - pxtData[18])
			/ (pxtData[19] - pxtData[17])
			+ currentEnvelopeb;
		++i;
	}

	//End
	currentEnvelopec = pxtData[20];
	while (i < 256)
	{
		envelopeTable[i] = (unsigned __int64)currentEnvelopec;
		currentEnvelopec = currentEnvelopec
			- pxtData[20] / (256 - pxtData[19]);
		++i;
	}

	long double pitchOffset = pxtData[9];
	long double mainOffset = pxtData[5];
	long double volumeOffset = pxtData[13];
	
	//Main
	long double mainFreq;
	if (pxtData[3] == 0.0)
		mainFreq = 0.0;
	else
		mainFreq = 256.0 / (pxtData[1] / pxtData[3]);

	//Pitch
	long double pitchFreq;
	if (pxtData[7] == 0.0)
		pitchFreq = 0.0;
	else
		pitchFreq = 256.0 / (pxtData[1] / pxtData[7]);

	//Volume
	long double volumeFreq;
	if (pxtData[11] == 0.0)
		volumeFreq = 0.0;
	else
		volumeFreq = 256.0 / (pxtData[1] / pxtData[11]);

	for (i = 0; i < pxtData[1]; ++i)
	{
		int a = (int)(uint64_t)mainOffset % 256;
		int v2 = (int)(uint64_t)pitchOffset % 256;

		//Input data
		data[i] = envelopeTable[(unsigned __int64)((long double)(i << 8) / pxtData[1])]
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

		long double newMainOffset;
		if (waveModelTable[(size_t)pxtData[6]][v2] >= 0)
			newMainOffset = (mainFreq * 2)
			* (long double)waveModelTable[(size_t)pxtData[6]][(signed int)(unsigned __int64)pitchOffset % 256]
			* pxtData[8]
			/ 64.0
			/ 64.0
			+ mainFreq
			+ mainOffset;
		else
			newMainOffset = mainFreq
			- mainFreq
			* 0.5
			* (long double)-waveModelTable[(size_t)pxtData[6]][v2]
			* pxtData[8]
			/ 64.0
			/ 64.0
			+ mainOffset;

		mainOffset = newMainOffset;
		pitchOffset = pitchOffset + pitchFreq;
		volumeOffset = volumeOffset + volumeFreq;
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

		//Allocate buffers
		uint8_t *dest = (uint8_t *)malloc(size);
		uint8_t *pBlock = (uint8_t *)malloc(size);

		if (dest && pBlock)
		{
			//Set buffers to default value of 0x80
			memset(dest, 0x80, size);
			memset(pBlock, 0x80, size);
			
			for (int i = 0; i < 4; ++i)
			{
				//Get wave data
				if (!makePixelWaveData(lineNumbers[i], dest))
				{
					free(dest);
					free(pBlock);
					return -1;
				}
				
				//Put data into buffer
				for (int j = 0; j < lineNumbers[i][1]; ++j)
				{
					if (dest[j] + pBlock[j] - 0x100 >= -0x7F)
					{
						if (dest[j] + pBlock[j] - 0x100 <= 0x7F)
							pBlock[j] += dest[j] + -0x80;
						else
							pBlock[j] = (uint8_t)-1;
					}
					else
					{
						pBlock[j] = 0;
					}
				}
			}

			//Put data from buffers into main sound buffer
			sounds[id].wave = (uint8_t*)malloc(size);
			if (!sounds[id].wave)
			{
				free(dest);
				free(pBlock);
				return -1;
			}
			sounds[id].length = size;
			memcpy(sounds[id].wave, pBlock, size);

			//Free the two buffers
			free(dest);
			free(pBlock);

			return 1;
		}
		else
		{
			free(dest);
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
		free(sound.wave);
}

void playSound(size_t sound_no, int soundMode) 
{
	sounds[sound_no].pos = 0;
	sounds[sound_no].playing = true;
}
