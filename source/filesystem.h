#pragma once
#include "common.h"
#include <vector>
#include <string>

struct CONFIG {
	int version;

	//Window things
	int framerate;
	int screenWidth;
	int screenHeight;
	int screenScale;

	//Gamepad bools
	bool useGamepad;
	bool enableThumbstick;

	bool fullscreen;

	//Keybinds
	int keyLeft;
	int keyRight;
	int keyUp;
	int keyDown;
	int keyJump;
	int keyShoot;
	int keyMenu;
	int keyMap;

	int keyRotLeft;
	int keyRotRight;

	//Gamepad keybinds
	int padLeft;
	int padRight;
	int padUp;
	int padDown;
	int padJump;
	int padShoot;
	int padMenu;
	int padMap;

	int padRotLeft;
	int padRotRight;
};

uint16_t readLEshort(const uint8_t *data, size_t offset) attrPure;
uint32_t readLElong(const uint8_t *data, size_t offset) attrPure;

void writeLEshort(uint8_t *data, uint16_t input, size_t offset);
void writeLElong(uint8_t *data, uint32_t input, size_t offset);

/// Returns whether the given filename points to a valid file
bool fileExists(const std::string& name);

int loadFile(const std::string& name, uint8_t **data);
void writeFile(const std::string& name, const void *data, size_t amount);

void loadProfile();
void saveProfile();

extern CONFIG *currentConfig;

void setFromConfig(CONFIG *config);

void loadConfig();
void saveConfig();

std::vector<std::string> getLinesFromFile(const std::string& fileName);

extern std::string profileName;
extern std::string profileCode;
