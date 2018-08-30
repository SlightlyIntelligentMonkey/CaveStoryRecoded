#include "sound.h"
#include "org.h"
#include "input.h"
#include "filesystem.h"

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <cstdlib>
#include <cstdio>
#include <SDL.h>

//Variable things
SDL_AudioDeviceID soundDev;
SDL_AudioSpec soundSpec;
SDL_AudioSpec want;

void __cdecl audio_callback(void *userdata, Uint8 *stream, int len) // TBD : Handle userdata parameter
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
std::vector<long double> getNumbersFromString(char *string)
{
	std::vector<long double> numbers;

	long double currentValue = 0;
	bool parsingNumber = false;
	bool numberDecimal = false;

	size_t i = 0;
	while (string[i])
	{
		if (string[i] == '.' || (string[i] >= 0x30 && string[i] <= 0x39))
		{
			if (!parsingNumber)
			{
				parsingNumber = true;
				numberDecimal = false;
				currentValue = 0;
				numberDecimal = (string[i] == '.');
			}

			if (string[i] != '.')
			{
				//Get position
				size_t v = i;
				while (string[v])
				{
					if (string[v] == '.' || (string[v] >= 0x30 && string[v] <= 0x39))
					{
						if (string[v] == '.')
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
					currentValue += (long double)(string[i] - 0x30) * (pow(10, (v - i) - 1));
				else
					currentValue += (long double)(string[i] - 0x30) * (1.0 / pow(10, 2 - ((v - i) - 1)));
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

void loadSound(char *path)
{
	std::vector<std::string> lines = getLinesFromFile(path);

	if (lines.size() == 92)
	{

	}
	else
	{
		char error[0x100];
		sprintf(error, "Can't parse .pxt which isn't exactly 92 lines long.\n(given file is %s lines long)", std::to_string(lines.size()).c_str());
		doCustomError(error);
	}
}

const char *hexNibble[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
void loadSounds()
{
	for (int n1 = 0; n1 < 16; n1++)
	{
		for (int n2 = 0; n2 < 16; n2++)
		{
			char path[0x100];
			sprintf(path, "data/Sound/%s%s.pxt", hexNibble[n1], hexNibble[n2]);

			if (fileExists(path))
			{
				loadSound(path);
			}
		}
	}
}

void freeSounds() noexcept
{

}

void playSound(size_t sound_no, int soundMode) noexcept
{
	
}
