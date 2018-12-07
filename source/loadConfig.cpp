#include <string>
#include <limits>
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

/*constexpr int defaultPadLeft = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
constexpr int defaultPadRight = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
constexpr int defaultPadUp = SDL_CONTROLLER_BUTTON_DPAD_UP;
constexpr int defaultPadDown = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
constexpr int defaultPadJump = SDL_CONTROLLER_BUTTON_A;
constexpr int defaultPadShoot = SDL_CONTROLLER_BUTTON_X;
constexpr int defaultPadMenu = SDL_CONTROLLER_BUTTON_Y;
constexpr int defaultPadMap = SDL_CONTROLLER_BUTTON_B;
constexpr int defaultPadRotLeft = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
constexpr int defaultPadRotRight = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;*/

template<typename T> void safeGet(const json& j, const string& name, T& varTbc)
{
	if (typeid(T) == typeid(string))
	{
		auto tmp = j[name];
		if (tmp.is_string())
			varTbc = tmp;
	}
	else if (typeid(T) == typeid(bool))
    {
        if (j[name] == true)
            varTbc = true;
    }
    else if (std::numeric_limits<T>::is_integer)
    {
        auto tmp = j[name];
        if (tmp.is_number())
            varTbc = tmp;
    }
    else
    {
        // I dunno how to make this fail here at compile-time
        throw std::runtime_error("Bad type for safeGet");
    }
}

void loadConfigFiles()
{
	auto jConfig = loadJsonFromFile("config.json");

	debugFlags = 0;
	auto jDbgFlgs = jConfig["debugFlags"];
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

    safeGet(jConfig, "profileName", profileName);
    safeGet(jConfig, "profileCode", profileCode);
    safeGet(jConfig, "disableDamage", disableDamage);
    safeGet(jConfig, "disableOrg", disableOrg);
    safeGet(jConfig, "millisecondsPerFrame", framewait);

    auto jScreen = jConfig["screen"];
    safeGet(jScreen, "width", screenWidth);
    safeGet(jScreen, "height", screenHeight);
    safeGet(jScreen, "scale", screenScale);

    safeGet(jConfig, "displayFpsCounter", displayFpsCounter);
    safeGet(jConfig, "useGamepad", useGamepad);

	auto jKeys = jConfig["keys"];

	safeGet(jKeys, "keyLeft", keyLeft);
	safeGet(jKeys, "keyRight", keyRight);
    safeGet(jKeys, "keyUp", keyUp);
    safeGet(jKeys, "keyDown", keyDown);
    safeGet(jKeys, "keyJump", keyJump);
    safeGet(jKeys, "keyShoot", keyShoot);
    safeGet(jKeys, "keyMenu", keyMenu);
    safeGet(jKeys, "keyMap", keyMap);
    safeGet(jKeys, "keyRotLeft", keyRotLeft);
    safeGet(jKeys, "keyRotRight", keyRotRight);
}
