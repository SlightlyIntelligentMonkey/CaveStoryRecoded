#include <string>
#include <fstream>
#include <json.hpp>
#include <SDL_render.h>
#include <SDL_keyboard.h>
#include <SDL_gamecontroller.h>
#include <SDL_messagebox.h>

#include "loadConfig.h"
#include "game.h"
#include "org.h"
#include "sound.h"
#include "filesystem.h"
#include "player.h"
#include "render.h"
#include "input.h"

using std::string;
using std::ifstream;
using nlohmann::json;

//Save and load config.dat (original cvae story) thing
string configName = "Config.dat";

CONFIG *loadConfigdat()
{
	FILE *fp = fopen("Config.dat", "rb");
	if (fp == nullptr)
		return nullptr;
	else
	{
		CONFIG *config = (CONFIG*)malloc(sizeof(CONFIG));
		fread(config, 1, sizeof(CONFIG), fp);
		return config;
	}
}

json loadJsonFromFile(const string& path)
{
	if (!fileExists(path))
		return json();
	ifstream file(path);
	json j;
	file >> j;
	return j;
}

const int defaultKeyLeft = SDL_SCANCODE_LEFT;
const int defaultKeyRight = SDL_SCANCODE_RIGHT;
const int defaultKeyUp = SDL_SCANCODE_UP;
const int defaultKeyDown = SDL_SCANCODE_DOWN;
const int defaultKeyJump = SDL_SCANCODE_Z;
const int defaultKeyShoot = SDL_SCANCODE_X;
const int defaultKeyMenu = SDL_SCANCODE_Q;
const int defaultKeyMap = SDL_SCANCODE_W;
const int defaultKeyRotLeft = SDL_SCANCODE_A;
const int defaultKeyRotRight = SDL_SCANCODE_S;

const int defaultPadLeft = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
const int defaultPadRight = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
const int defaultPadUp = SDL_CONTROLLER_BUTTON_DPAD_UP;
const int defaultPadDown = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
const int defaultPadJump = SDL_CONTROLLER_BUTTON_A;
const int defaultPadShoot = SDL_CONTROLLER_BUTTON_X;
const int defaultPadMenu = SDL_CONTROLLER_BUTTON_Y;
const int defaultPadMap = SDL_CONTROLLER_BUTTON_B;
const int defaultPadRotLeft = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
const int defaultPadRotRight = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;

void loadConfigFiles()
{
	auto jconfig = loadJsonFromFile("config.json");

	debugFlags = 0;
	auto jDbgFlgs = jconfig["debugFlags"];
	if (jDbgFlgs["showSlots"] == true)
		debugFlags |= showSlots;
	if (jDbgFlgs["showNPCId"] == true)
		debugFlags |= showNPCId;
	if (jDbgFlgs["showCARId"] == true)
		debugFlags |= showCARId;
	if (jDbgFlgs["notifyOnNotImplemented"] == true)
		debugFlags |= notifyOnNotImplemented;
	if (jDbgFlgs["showNPCHealth"] == true)
		debugFlags |= showNPCHealth;

	disableOrg = false;

	if (jconfig["disableOrg"] == true)
		disableOrg = true;

	if (jconfig["profileName"].is_string())
		profileName = jconfig["profileName"];

	if (jconfig["profileCode"].is_string())
		profileCode = jconfig["profileCode"];

	if (jconfig["disableDamage"] == true)
		disableDamage = true;

	if (jconfig["millisecondsPerFrame"].is_number())
		framewait = jconfig["millisecondsPerFrame"];

	if (jconfig["width"].is_number())
		screenWidth = jconfig["width"];
	if (jconfig["height"].is_number())
		screenHeight = jconfig["height"];
	if (jconfig["scale"].is_number())
		screenScale = jconfig["scale"];

	if (jconfig["displayFpsCounter"] == true)
		displayFpsCounter = true;

	if (jconfig["useGamepad"] == true)
		useGamepad = true;

	keyLeft = defaultKeyLeft;
	keyRight = defaultKeyRight;
	keyUp = defaultKeyUp;
	keyDown = defaultKeyDown;
	keyJump = defaultKeyJump;
	keyShoot = defaultKeyShoot;
	keyMenu = defaultKeyMenu;
	keyMap = defaultKeyMap;
	keyRotLeft = defaultKeyRotLeft;
	keyRotRight = defaultKeyRotRight;
}