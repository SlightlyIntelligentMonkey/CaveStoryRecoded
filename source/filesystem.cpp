#include "filesystem.h"

#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <SDL_rwops.h>
#include "main.h"
#include "render.h"
#include "flags.h"
#include "game.h"
#include "weapons.h"
#include "player.h"
#include "stage.h"
#include "fade.h"
#include "script.h"
#include "org.h"
#include "log.h"

uint16_t readLEshort(const uint8_t * data, size_t offset)
{
	if (data == nullptr)
		throw std::invalid_argument("data was nullptr in readLEshort");

	return ((data[offset + 1] << 8) + data[offset]);
}

uint32_t readLElong(const uint8_t * data, size_t offset)
{
	if (data == nullptr)
		throw std::invalid_argument("data was nullptr in readLEshort");

	return ((data[offset + 3] << 24) + (data[offset + 2] << 16) + (data[offset + 1] << 8) + data[offset]);
}

//Write stuff
void writeLEshort(uint8_t *data, uint16_t input, size_t offset)
{
	if (data == nullptr)
		throw std::invalid_argument("data was nullptr in writeLEshort");

	data[offset] = static_cast<uint8_t>(input);
	data[offset + 1] = static_cast<uint8_t>(input >> 8);
}

void writeLElong(uint8_t *data, uint32_t input, size_t offset)
{
	if (data == nullptr)
		throw std::invalid_argument("data was nullptr in writeLElong");

	data[offset] = static_cast<uint8_t>(input);
	data[offset + 1] = static_cast<uint8_t>(input >> 8);
	data[offset + 2] = static_cast<uint8_t>(input >> 16);
	data[offset + 3] = static_cast<uint8_t>(input >> 24);
}

//Loading and writing functions
bool fileExists(const std::string& name)
{
	struct stat buffer;
	return stat(name.c_str(), &buffer) == 0;
}

std::pair<size_t, std::vector<uint8_t>> loadFile(const std::string& name)
{
	//Open file
	FILE *file = fopen(name.c_str(), "rb");
	if (file == nullptr)
		throw std::runtime_error("Could not open " + name);

	//Get filesize
	if (fseek(file, 0, SEEK_END) == -1)
	{
		if (fclose(file) == EOF)
			throw std::runtime_error("Could not close " + name + " after failure of seek");
		throw std::runtime_error("Could not seek in " + name);
	}

	const auto filesize = ftell(file);
	if (filesize == -1L)
	{
		if (fclose(file) == EOF)
			throw std::runtime_error("Could not close " + name + " after failure to find size");
		throw std::runtime_error("Could not find size of " + name);
	}

	if (fseek(file, 0, SEEK_SET) == -1)
	{
		if (fclose(file) == EOF)
			throw std::runtime_error("Could not close " + name + " after failure of seek");
		throw std::runtime_error("Could not seek in " + name);
	}

	//Load data
	std::vector<uint8_t> data(filesize);
	if (fread(data.data(), 1, filesize, file) < static_cast<size_t>(filesize))
	{
		if (fclose(file) == EOF)
			throw std::runtime_error("Could not close " + name + " after failing to read it");
		throw std::runtime_error("Could not read from " + name);
	}

	//Close file
	if (fclose(file) == EOF)
		throw std::runtime_error("Could not close " + name);

    logDebug("Loaded from " + name);
	return std::pair(filesize, data);
}

void writeFile(const std::string& name, const void *data, size_t amount)
{
	FILE *file;
	if ((file = fopen(name.c_str(), "wb")) == nullptr)
		throw std::runtime_error("Could not open " + name);

	if (fwrite(data, 1, amount, file) == 0)
	{
		if (fclose(file) == EOF)
			throw std::runtime_error("Could not close " + name);
		throw std::runtime_error("Could not write to " + name);
	}

	if (fclose(file) == EOF)
		throw std::runtime_error("Could not close " + name);

    logDebug("Wrote to " + name);
}

//Profile code
std::string gProfileName = "Profile.dat";
std::string gProfileCode = "Do041220";

void loadProfile()
{
	if (fileExists(gProfileName))
	{
		SDL_RWops *profile = SDL_RWFromFile(gProfileName.c_str(), "rb");

		if (profile == nullptr)
			doCustomError("Profile.dat exists, but couldn't be read from.");

		const uint64_t code = SDL_ReadLE64(profile); //Code
		if (memcmp(&code, gProfileCode.c_str(), sizeof(code)) != 0)
		{
			const std::string errorMsg(std::string("Invalid profile (first 8 bytes aren't \"") + gProfileCode + "\"");
			doCustomError(errorMsg);
		}

		const uint32_t level = SDL_ReadLE32(profile); //level
		changeOrg(SDL_ReadLE32(profile)); //song

		currentPlayer.init();

		const uint32_t player_x = SDL_ReadLE32(profile); //Player X
		const uint32_t player_y = SDL_ReadLE32(profile); //Player Y
		currentPlayer.direct = SDL_ReadLE32(profile); //Player Direction

		currentPlayer.max_life = SDL_ReadLE16(profile); //max health
		currentPlayer.star = SDL_ReadLE16(profile); //whimsical star
		currentPlayer.life = SDL_ReadLE16(profile); //health

		SDL_ReadLE16(profile); // a?

		gSelectedWeapon = SDL_ReadLE32(profile); //current weapon
		gSelectedItem = SDL_ReadLE32(profile); //current item

		currentPlayer.equip = SDL_ReadLE32(profile); //equipped items
		currentPlayer.unit = SDL_ReadLE32(profile); //physics

		SDL_ReadLE32(profile); //counter

		for (auto& i : gWeapons)
		{
			i.code = SDL_ReadLE32(profile);
			i.level = SDL_ReadLE32(profile);
			i.exp = SDL_ReadLE32(profile);
			i.max_num = SDL_ReadLE32(profile);
			i.num = SDL_ReadLE32(profile);
		}

		for (auto& i : gItems)
			i.code = SDL_ReadLE32(profile);

		for (auto& i : gPermitStage)
		{
			i.index = SDL_ReadLE32(profile);
			i.event = SDL_ReadLE32(profile);
		}

		for (auto& i : gMapFlags)
			SDL_RWread(profile, &i, 1, 1);

		SDL_ReadLE32(profile); //FLAG

		for (auto& i : gTscFlags)
			SDL_RWread(profile, &i, 1, 1);

		loadLevel(level, 0, 0, 1);
		currentPlayer.x = player_x;
		currentPlayer.y = player_y;
		initFade();
		SetFrameMyChar();
		SetFrameTargetMyChar(16);

		//Close RW
		SDL_RWclose(profile);

		logDebug("Loaded profile");
	}
	else
	{
		initGame();
		logInfo("Attempted to load profile, but it didn't exist.");
	}
}

void saveProfile()
{
	uint8_t profile[0x604] = { 0 };

	//Set data
	memcpy(profile, gProfileCode.c_str(), 0x08);

	writeLElong(profile, gCurrentLevel, 0x08); //Level
	writeLElong(profile, gCurrentOrg, 0xC); //song

	writeLElong(profile, currentPlayer.x, 0x10); //Player X
	writeLElong(profile, currentPlayer.y, 0x14); //Player Y
	writeLElong(profile, currentPlayer.direct, 0x18); //Player direction

	writeLEshort(profile, currentPlayer.max_life, 0x1C); //Player max health
	writeLEshort(profile, currentPlayer.star, 0x1E); //Whimsical star
	writeLEshort(profile, currentPlayer.life, 0x20); //Player health

	writeLElong(profile, gSelectedWeapon, 0x24); //Selected weapon
	writeLElong(profile, gSelectedItem, 0x28); //Selected item

	writeLElong(profile, currentPlayer.equip, 0x2C); //Equipped items
	writeLElong(profile, currentPlayer.unit, 0x30); //Current physics

	for (size_t i = 0; i < 8; i++)
	{
		writeLElong(profile, gWeapons[i].code, 0x38 + i * 0x14);
		writeLElong(profile, gWeapons[i].level, 0x3C + i * 0x14);
		writeLElong(profile, gWeapons[i].exp, 0x40 + i * 0x14);
		writeLElong(profile, gWeapons[i].max_num, 0x44 + i * 0x14);
		writeLElong(profile, gWeapons[i].num, 0x48 + i * 0x14);
	}

	for (size_t i = 0; i < 32; i++)
	{
		writeLElong(profile, gItems[i].code, 0xD8 + i * 0x4);
	}

	for (size_t i = 0; i < 8; i++)
	{
		writeLElong(profile, gPermitStage[i].index, 0x158 + i * 8);
		writeLElong(profile, gPermitStage[i].event, 0x15C + i * 8);
	}

	memcpy(profile + 0x198, gMapFlags, 0x80); // Consider cleaning up those magic numbers
	memcpy(profile + 0x218, "FLAG", 4);
	memcpy(profile + 0x21C, gTscFlags, 1000);

	//Save to file
	writeFile(gProfileName, profile, 0x604);
}

/// Return a vector of strings containing the contents of the file who's name is given as argument
std::vector<std::string> getLinesFromFile(const std::string& fileName)
{
	std::vector<std::string> lines;
	std::ifstream inFile(fileName);
	std::string line;
	while (std::getline(inFile, line))
		lines.push_back(line);
	return lines;
}
