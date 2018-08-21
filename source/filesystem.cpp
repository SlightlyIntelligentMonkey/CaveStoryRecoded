#include "filesystem.h"
#include "fade.h"
#include "weapons.h"
#include "player.h"
#include "game.h"
#include "flags.h"
#include "script.h"

#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <SDL_RWops.h>

using std::FILE;
using std::fopen;
using std::fseek;
using std::ftell;
using std::malloc;
using std::fread;
using std::fclose;

//Read function stuff
uint16_t readLEshort(const uint8_t * data, size_t offset)
{
	return ((data[offset + 1] << 8) + data[offset]);
}

uint32_t readLElong(const uint8_t * data, size_t offset)
{
	return ((data[offset + 3] << 24) + (data[offset + 2] << 16) + (data[offset + 1] << 8) + data[offset]);
}

//Write stuff
void writeLEshort(uint8_t *data, uint16_t input, size_t offset)
{
	data[offset] = static_cast<uint8_t>(input);
	data[offset + 1] = static_cast<uint8_t>(input >> 8);
}

void writeLElong(uint8_t *data, uint32_t input, size_t offset)
{
	data[offset] = static_cast<uint8_t>(input);
	data[offset + 1] = static_cast<uint8_t>(input >> 8);
	data[offset + 2] = static_cast<uint8_t>(input >> 16);
	data[offset + 3] = static_cast<uint8_t>(input >> 24);
}

//Loading and writing functions
bool fileExists(const char *name)
{
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}

int loadFile(const char *name, uint8_t **data)
{
	//Open file
	FILE *file = fopen(name, "rb");
	if (file == nullptr)
		return -1;

	//Get filesize
	fseek(file, 0, SEEK_END);
	const size_t filesize = ftell(file);
	fseek(file, 0, 0);

	//Load data
	*data = static_cast<uint8_t *>(malloc(filesize));
	if (fread(*data, 1, filesize, file) == 0)
	{
		fclose(file);
		return -1;
	}

	//Close file
	fclose(file);

	return filesize;
}

int writeFile(const char *name, void *data, size_t amount)
{
	FILE *file;
	if ((file = fopen(name, "wb")) == nullptr)
		return -1;

	if (fwrite(data, 1, amount, file) == 0)
	{
		fclose(file);
		return -1;
	}

	fclose(file);
	return 0;
}

//Profile code
const char *profileName = "Profile.dat";
const char *profileCode = "Do041220";

void loadProfile()
{
	if (fileExists(profileName))
	{
		SDL_RWops *profile = SDL_RWFromFile(profileName, "rb");

		if (profile == nullptr)
			return;

		const uint64_t code = SDL_ReadLE64(profile); //Code
		if (memcmp(&code, "Do041220", sizeof(code)) != 0)
			doCustomError("Invalid profile (first 8 bytes aren't \"Do041120\"");

		const int level = SDL_ReadLE32(profile); //level
		SDL_ReadLE32(profile); //song

		currentPlayer.init();

		currentPlayer.x = SDL_ReadLE32(profile); //Player X
		currentPlayer.y = SDL_ReadLE32(profile); //Player Y
		currentPlayer.direct = SDL_ReadLE32(profile); //Player Direction

		currentPlayer.max_life = SDL_ReadLE16(profile); //max health
		currentPlayer.star = SDL_ReadLE16(profile); //whimsical star
		currentPlayer.life = SDL_ReadLE16(profile); //health

		SDL_ReadLE16(profile); // a?

		selectedWeapon = SDL_ReadLE32(profile); //current weapon
		SDL_ReadLE32(profile); //current item

		currentPlayer.equip = SDL_ReadLE32(profile); //equipped items
		currentPlayer.unit = SDL_ReadLE32(profile); //physics

		SDL_ReadLE32(profile); //counter

		for (size_t i = 0; i < 8; i++)
		{
			weapons[i].code = SDL_ReadLE32(profile);
			weapons[i].level = SDL_ReadLE32(profile);
			weapons[i].exp = SDL_ReadLE32(profile);
			weapons[i].max_num = SDL_ReadLE32(profile);
			weapons[i].num = SDL_ReadLE32(profile);
		}

		SDL_RWseek(profile, 0x158, 0);

		for (size_t i = 0; i < 8; i++)
		{
			permitStage[i].index = SDL_ReadLE32(profile);
			permitStage[i].event = SDL_ReadLE32(profile);
		}

		for (size_t i = 0; i < 0x80; i++)
			SDL_RWread(profile, &mapFlags[i], 1, 1);

		SDL_ReadLE32(profile); //FLAG

		for (size_t i = 0; i < 1000; i++)
			SDL_RWread(profile, &tscFlags[i], 1, 1);

		//Now load level
		loadLevel(level);
		startTscEvent(0);
		initFade();

		//Close RW
		SDL_RWclose(profile);
	}
	else
	{
		initGame();
	}
}

void saveProfile()
{
	uint8_t profile[0x604] = { 0 };

	if (profile == nullptr)
		doCustomError("Could not allocate memory for profile");

	//Set data
	memcpy(profile, profileCode, 0x08);

	writeLElong(profile, currentLevel, 0x08); //Level

	writeLElong(profile, currentPlayer.x, 0x10); //Player X
	writeLElong(profile, currentPlayer.y, 0x14); //Player Y
	writeLElong(profile, currentPlayer.direct, 0x18); //Player direction

	writeLEshort(profile, currentPlayer.max_life, 0x1C); //Player max health
	writeLEshort(profile, currentPlayer.star, 0x1E); //Whimsical star
	writeLEshort(profile, currentPlayer.life, 0x20); //Player health

	writeLElong(profile, selectedWeapon, 0x24); //Selected weapon

	writeLElong(profile, currentPlayer.equip, 0x2C); //Equipped items
	writeLElong(profile, currentPlayer.unit, 0x30); //Current physics

	for (size_t i = 0; i < 8; i++)
	{
		writeLElong(profile, weapons[i].code, 0x38 + i * 0x14);
		writeLElong(profile, weapons[i].level, 0x3C + i * 0x14);
		writeLElong(profile, weapons[i].exp, 0x40 + i * 0x14);
		writeLElong(profile, weapons[i].max_num, 0x44 + i * 0x14);
		writeLElong(profile, weapons[i].num, 0x48 + i * 0x14);
	}

	for (size_t i = 0; i < 8; i++)
	{
		writeLElong(profile, permitStage[i].index, 0x158 + i * 8);
		writeLElong(profile, permitStage[i].event, 0x15C + i * 8);
	}

	memcpy(profile + 0x198, mapFlags, 0x80); // Consider cleaning up those magic numbers
	memcpy(profile + 0x218, "FLAG", 4);
	memcpy(profile + 0x21C, tscFlags, 1000);

	//Save to file
	writeFile(profileName, profile, 0x604);
}
