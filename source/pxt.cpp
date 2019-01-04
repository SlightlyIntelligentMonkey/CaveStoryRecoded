#include "pxt.h"

#include <string>
#include <vector>
#include <cmath>
#include <cstring>
#include "main.h"
#include "mathUtils.h"
#include "filesystem.h"

using std::vector;
using std::string;
using std::to_string;

//For pxt
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

//Pxt loading
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
						break;
					v++;
				}

				//Add value to number
				if (!numberDecimal)
					currentValue += (long double)(str[i] - 0x30) * (pow(10, (v - i) - 1));
				else
					currentValue += (long double)(str[i] - 0x30) * (1.0 / pow(10, 2 - ((v - i) - 1)));
			}
			else
				numberDecimal = true;
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

int makePixelWaveData(const std::vector<long double>& pxtData, uint8_t *data)
{
	long double currentEnvelope;
	long double currentEnvelopea;
	long double currentEnvelopeb;
	long double currentEnvelopec;
	char envelopeTable[256];

	//Get some envelope stuff
	memset(envelopeTable, 0, 0x100u);

	size_t i = 0;

	//Point A
	currentEnvelope = pxtData[14];
	while (i < pxtData[15])
	{
		envelopeTable[i] = (char)currentEnvelope;
		currentEnvelope = (pxtData[16] - pxtData[14])
			/ pxtData[15]
			+ currentEnvelope;
		++i;
	}

	//Point B
	currentEnvelopea = pxtData[16];
	while (i < pxtData[17])
	{
		envelopeTable[i] = (char)currentEnvelopea;
		currentEnvelopea = (pxtData[18] - pxtData[16])
			/ (pxtData[17] - pxtData[15])
			+ currentEnvelopea;
		++i;
	}

	//Point C
	currentEnvelopeb = pxtData[18];
	while (i < pxtData[19])
	{
		envelopeTable[i] = (char)currentEnvelopeb;
		currentEnvelopeb = (pxtData[20] - pxtData[18])
			/ (pxtData[19] - pxtData[17])
			+ currentEnvelopeb;
		++i;
	}

	//End
	currentEnvelopec = pxtData[20];
	while (i < 256)
	{
		envelopeTable[i] = (char)currentEnvelopec;
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
		const int a = (int)(uint64_t)mainOffset % 256;
		const int v2 = (int)(uint64_t)pitchOffset % 256;

		//Input data
		data[i] = envelopeTable[(uint64_t)((long double)(i << 8) / pxtData[1])]
			* (pxtData[4]
				* waveModelTable[(size_t)pxtData[2]][a]
				/ 64
				* (pxtData[12]
					* waveModelTable[(size_t)pxtData[10]][(signed int)(uint64_t)volumeOffset % 256]
					/ 64
					+ 64)
				/ 64)
			/ 64
			+ 128;	// This was originally -128, but casting a negative double to an unsigned char results in undefined behaviour

		long double newMainOffset;
		if (waveModelTable[(size_t)pxtData[6]][v2] >= 0)
			newMainOffset = (mainFreq * 2)
			* (long double)waveModelTable[(size_t)pxtData[6]][(signed int)(uint64_t)pitchOffset % 256]
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

int loadSound(const string& path, uint8_t **buf, size_t *length)
{
	if (fileExists(path))
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
					string error("PXT: " + path + "\nChannel: " + to_string(i) + "\nInvalid amount of properties: " + to_string(lineNumbers[i].size()));
					doCustomError(error);
				}
			}

			//Get size
			for (int i = 0; i < 4; i++)
			{
				if (lineNumbers[i][1] > size)
					size = (size_t)lineNumbers[i][1];
			}

			//Allocate buffers
			uint8_t *dest = new uint8_t[size];
			uint8_t *pBlock = new uint8_t[size];

			//Set buffers to default value of 0x80
			memset(dest, 0x80, size);
			memset(pBlock, 0x80, size);

			for (int i = 0; i < 4; ++i)
			{
				//Get wave data
				if (!makePixelWaveData(lineNumbers[i], dest))
				{
					delete[] dest;
					delete[] pBlock;
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
						pBlock[j] = 0;
				}
			}

			//Put data from buffers into main sound buffer
			*buf = new uint8_t[size];

			if (!*buf)
			{
				delete[] dest;
				delete[] pBlock;
				return -2;
			}

			*length = size;
			memcpy(*buf, pBlock, size);

			//Free the two buffers
			delete[] dest;
			delete[] pBlock;

			return 1;
		}
		else
			return -4;

		return 1;
	}
	else
	{
		string error("Attempted to load .pxt which doesn't exist:\n" + path);
		doCustomError(error);
	}
}
