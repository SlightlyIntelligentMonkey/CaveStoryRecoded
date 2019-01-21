#include <string>
#include <limits>
#include <fstream>
#include <SDL_render.h>
#include <SDL_keyboard.h>
#include <SDL_gamecontroller.h>
#include <SDL_messagebox.h>
#include "JsonModernCpp/json.hpp"

#include "loadConfig.h"
#include "game.h"
#include "org.h"
#include "sound.h"
#include "filesystem.h"
#include "player.h"
#include "render.h"
#include "input.h"
#include "log.h"
#include "main.h"
struct CONFIG_RAW
{
	char proof[32];
	char font_name[64];
	uint8_t move_button_mode[4];
	uint8_t attack_button_mode[4];
	uint8_t ok_button_mode[4];
	uint8_t display_mode[4];
	uint8_t bJoystick[4];
	uint8_t joystick_button[8][4];
};

//Save and load config.dat (config file for the original Cave Story)
std::string configName = "Config.dat";

CONFIG *loadConfigDat()
{
	CONFIG *config = nullptr;

	FILE *fp = fopen(configName.c_str(), "rb");

	if (fp != nullptr)
	{
		CONFIG_RAW *config_raw = new CONFIG_RAW;
		const size_t elements_read = fread(config_raw, sizeof(CONFIG), 1, fp);
		fclose(fp);

		if (elements_read == 1 && !strcmp(config_raw->proof, "DOUKUTSU20041206"))
		{
			config = new CONFIG;
			memcpy(config->proof, config_raw->proof, 32);
			memcpy(config->font_name, config_raw->font_name + 32, 64);
			config->move_button_mode = readLElong(config_raw->move_button_mode, 0);
			config->attack_button_mode = readLElong(config_raw->attack_button_mode, 0);
			config->ok_button_mode = readLElong(config_raw->ok_button_mode, 0);
			config->display_mode = readLElong(config_raw->display_mode, 0);
			config->bJoystick = readLElong(config_raw->bJoystick, 0);
			for (unsigned int i = 0; i < 8; ++i)
				config->joystick_button[i] = readLElong(config_raw->joystick_button[i], 0);
		}

		delete config_raw;
	}

	return config;
}

nlohmann::json loadJsonFromFile(const std::string& path)
{
	if (!fileExists(path))
		return nlohmann::json();
	std::ifstream file(path);
	nlohmann::json j;
	logInfo("Opening " + path);
	try
	{
        file >> j;
	}
	catch (const nlohmann::detail::parse_error& e)
    {
        doCustomError("Exception while loading \"" + path + "\" at byte " + std::to_string(e.byte) + ". Exception details : " + e.what());
	}
	catch (const std::exception& e)
	{
	    doCustomError("Exception while loading \"" + path + "\". Exception details : " + e.what());
	}
	return j;
}

#if __cplusplus >= 201703L
template<typename T> void safeGet(const nlohmann::json& j, const std::string& name, T& varTbc)
{
	if constexpr(std::is_same_v<T, std::string>)
	{
		if (j[name].is_string())
			varTbc = j[name];
	}
	else if constexpr(std::is_same_v<T, bool>)
    {
        if (j[name].is_boolean())
            varTbc = j[name];
    }
    else if constexpr(std::numeric_limits<T>::is_integer)
    {
        if (j[name].is_number())
            varTbc = j[name];
    }
    else
        static_assert(std::is_same_v<T, void>, "This type is not supported");   // Best I can think of right now. There are probably better ways, but this works I suppose
}
#else
// C++11-friendly replacement
void safeGet(const json& j, const string& name, string& varTbc)
{
    if (j[name].is_string())
        varTbc = j[name];
}

void safeGet(const json& j, const string& name, bool& varTbc)
{
    if (j[name].is_boolean())
        varTbc = j[name];
}

void safeGet(const json& j, const string& name, int& varTbc)
{
    if (j[name].is_number())
        varTbc = j[name];
}
#endif

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

    safeGet(jConfig, "configName", configName);
    safeGet(jConfig, "profileName", profileName);
    safeGet(jConfig, "profileCode", profileCode);
    safeGet(jConfig, "disableDamage", disableDamage);
    safeGet(jConfig, "disableOrg", disableOrg);
    safeGet(jConfig, "millisecondsPerFrame", framewait);
    safeGet(jConfig, "displayFpsCounter", displayFpsCounter);
    safeGet(jConfig, "useGamepad", useGamepad);

    auto jScreen = jConfig["screen"];
    safeGet(jScreen, "width", screenWidth);
    safeGet(jScreen, "height", screenHeight);
    safeGet(jScreen, "scale", screenScale);


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
