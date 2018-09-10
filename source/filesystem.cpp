#include "filesystem.h"
#include "fade.h"
#include "weapons.h"
#include "player.h"
#include "game.h"
#include "input.h"
#include "flags.h"
#include "render.h"
#include "script.h"
#include "org.h"
#include <SDL.h>

#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <SDL_rwops.h>

using std::string;
using std::vector;
using std::ifstream;
using std::getline;
using std::FILE;
using std::fopen;
using std::fseek;
using std::ftell;
using std::malloc;
using std::fread;
using std::fclose;

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
bool fileExists(const string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

int loadFile(const string& name, uint8_t **data)
{
	if (data == nullptr)
		throw std::invalid_argument("data was nullptr in loadFile");

	//Open file
	FILE *file = fopen(name.c_str(), "rb");
	if (file == nullptr)
		throw std::runtime_error("Could not open " + name);

	//Get filesize
	if (fseek(file, 0, SEEK_END) == -1)
		throw std::runtime_error("Could not seek in " + name);
	const int filesize = ftell(file);
	if (filesize == -1L)
		throw std::runtime_error("Could not find size of " + name);

	if (fseek(file, 0, SEEK_SET) == -1)
		throw std::runtime_error("Could not seek in " + name);

	//Load data
	*data = new uint8_t[filesize];
	if (fread(*data, 1, filesize, file) < static_cast<size_t>(filesize))
	{
		if (fclose(file) == EOF)
			throw std::runtime_error("Could not close " + name + " after failing to read it");
		throw std::runtime_error("Could not read from " + name);
	}

	//Close file
	if (fclose(file) == EOF)
		throw std::runtime_error("Could not close " + name);

	return filesize;
}

void writeFile(const string& name, const void *data, size_t amount)
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
}

//Profile code
string profileName = "Profile.dat";
string profileCode = "Do041220";

void loadProfile()
{
	if (fileExists(profileName))
	{
		SDL_RWops *profile = SDL_RWFromFile(profileName.c_str(), "rb");

		if (profile == nullptr)
			return;

		const uint64_t code = SDL_ReadLE64(profile); //Code
		if (memcmp(&code, profileCode.c_str(), sizeof(code)) != 0)
		{
			const string errorMsg(string("Invalid profile (first 8 bytes aren't \"") + profileCode + "\"");
			doCustomError(errorMsg.c_str());
		}

		const int level = SDL_ReadLE32(profile); //level
		changeOrg(SDL_ReadLE32(profile)); //song

		currentPlayer.init();

		currentPlayer.x = SDL_ReadLE32(profile); //Player X
		currentPlayer.y = SDL_ReadLE32(profile); //Player Y
		currentPlayer.direct = SDL_ReadLE32(profile); //Player Direction

		currentPlayer.max_life = SDL_ReadLE16(profile); //max health
		currentPlayer.star = SDL_ReadLE16(profile); //whimsical star
		currentPlayer.life = SDL_ReadLE16(profile); //health

		SDL_ReadLE16(profile); // a?

		selectedWeapon = SDL_ReadLE32(profile); //current weapon
		selectedItem = SDL_ReadLE32(profile); //current item

		currentPlayer.equip = SDL_ReadLE32(profile); //equipped items
		currentPlayer.unit = SDL_ReadLE32(profile); //physics

		SDL_ReadLE32(profile); //counter

		for (auto& i : weapons)
		{
			i.code = SDL_ReadLE32(profile);
			i.level = SDL_ReadLE32(profile);
			i.exp = SDL_ReadLE32(profile);
			i.max_num = SDL_ReadLE32(profile);
			i.num = SDL_ReadLE32(profile);
		}

		for (auto& i : items)
			i.code = SDL_ReadLE32(profile);

		for (auto& i : permitStage)
		{
			i.index = SDL_ReadLE32(profile);
			i.event = SDL_ReadLE32(profile);
		}

		for (auto& i : mapFlags)
			SDL_RWread(profile, &i, 1, 1);

		SDL_ReadLE32(profile); //FLAG

		for (auto& i : tscFlags)
			SDL_RWread(profile, &i, 1, 1);

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

	//Set data
	memcpy(profile, profileCode.c_str(), 0x08);

	writeLElong(profile, currentLevel, 0x08); //Level
	writeLElong(profile, currentOrg, 0xC); //song

	writeLElong(profile, currentPlayer.x, 0x10); //Player X
	writeLElong(profile, currentPlayer.y, 0x14); //Player Y
	writeLElong(profile, currentPlayer.direct, 0x18); //Player direction

	writeLEshort(profile, currentPlayer.max_life, 0x1C); //Player max health
	writeLEshort(profile, currentPlayer.star, 0x1E); //Whimsical star
	writeLEshort(profile, currentPlayer.life, 0x20); //Player health

	writeLElong(profile, selectedWeapon, 0x24); //Selected weapon
	writeLElong(profile, selectedItem, 0x28); //Selected item

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

	for (size_t i = 0; i < 32; i++)
	{
		writeLElong(profile, items[i].code, 0xD8 + i * 0x4);
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

//Save and load config.dat thing
string configName = "Config.dat";
CONFIG *currentConfig;
int configVersion = 2;

int defaultPadLeft = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
int defaultPadRight = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
int defaultPadUp = SDL_CONTROLLER_BUTTON_DPAD_UP;
int defaultPadDown = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
int defaultPadJump = SDL_CONTROLLER_BUTTON_A;
int defaultPadShoot = SDL_CONTROLLER_BUTTON_X;
int defaultPadMenu = SDL_CONTROLLER_BUTTON_Y;
int defaultPadMap = SDL_CONTROLLER_BUTTON_B;

int defaultPadRotLeft = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
int defaultPadRotRight = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;

CONFIG defaultConfigData = { configVersion, 20, 320, 240, 2, false, true, false, keyLeft, keyRight, keyUp, keyDown, keyJump, keyShoot, keyMenu, keyMap, keyRotLeft, keyRotRight, defaultPadLeft, defaultPadRight, defaultPadUp, defaultPadDown, defaultPadJump, defaultPadShoot, defaultPadMenu, defaultPadMap, defaultPadRotLeft, defaultPadRotRight };

void setFromConfig(CONFIG *config)
{
	framerate = config->framerate;
	createWindow(config->screenWidth, config->screenHeight, config->screenScale, true);

	if (config->fullscreen)
		switchScreenMode();

	if (config->useGamepad)
	{
		keyLeft = config->padLeft;
		keyRight = config->padRight;
		keyUp = config->padUp;
		keyDown = config->padDown;
		keyJump = config->padJump;
		keyShoot = config->padShoot;
		keyMenu = config->padMenu;
		keyMap = config->padMap;

		keyRotLeft = config->padRotLeft;
		keyRotRight = config->padRotRight;
	}
	else
	{
		keyLeft = config->keyLeft;
		keyRight = config->keyRight;
		keyUp = config->keyUp;
		keyDown = config->keyDown;
		keyJump = config->keyJump;
		keyShoot = config->keyShoot;
		keyMenu = config->keyMenu;
		keyMap = config->keyMap;

		keyRotLeft = config->keyRotLeft;
		keyRotRight = config->keyRotRight;
	}

	currentConfig = config;
}

void defaultConfig()
{
	setFromConfig(&defaultConfigData);
}

void loadConfig()
{
	if (!fileExists(configName.c_str()))
	{
		return defaultConfig();
	}
	else
	{
		CONFIG *config;
		int configSize = loadFile(configName.c_str(), (uint8_t**)&config);

		if (configSize < sizeof(config->version) || config->version != configVersion)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Invalid config.dat", "Version isn't valid", nullptr);
			return defaultConfig();
		}
		else if (configSize != sizeof(CONFIG))
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Invalid config.dat", "Not valid size.", nullptr);
			return defaultConfig();
		}

		setFromConfig(config);
	}
}

void saveConfig()
{
	uint8_t *writeData = (uint8_t*)malloc(sizeof(*currentConfig));
	memcpy(writeData, currentConfig, sizeof(*currentConfig));
	writeFile(configName.c_str(), writeData, sizeof(*currentConfig));
}

//thing
vector<string> getLinesFromFile(const string& fileName)
{
	vector<string> lines;
	ifstream inFile(fileName);
	string line;
	while (getline(inFile, line))
		lines.push_back(line);
	return lines;
}
