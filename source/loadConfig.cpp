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

namespace config
{
	
namespace old
{
	
//Save and load config.dat (config file for the original Cave Story)
std::string gName = "Config.dat";

bool gUse = false;

std::unique_ptr<CONFIG> load(const std::string& name)
{
	if (!config::old::gUse)
		return std::unique_ptr<CONFIG>();

	std::unique_ptr<CONFIG> config;

	FILE *fp = fopen(name.c_str(), "rb");

	if (fp != nullptr)
	{
		CONFIG_RAW config_raw;
		const size_t elements_read = fread(&config_raw, sizeof(CONFIG), 1, fp);
		fclose(fp);

		if (elements_read == 1 && !strcmp(config_raw.proof, "DOUKUTSU20041206"))
		{
			config = std::make_unique<CONFIG>();
			memcpy(config->proof, config_raw.proof, 32);
			memcpy(config->font_name, config_raw.font_name + 32, 64);
			config->move_button_mode = readLElong(config_raw.move_button_mode, 0);
			config->attack_button_mode = readLElong(config_raw.attack_button_mode, 0);
			config->ok_button_mode = readLElong(config_raw.ok_button_mode, 0);
			config->display_mode = readLElong(config_raw.display_mode, 0);
			config->bJoystick = readLElong(config_raw.bJoystick, 0);
			for (unsigned int i = 0; i < 8; ++i)
				config->joystick_button[i] = readLElong(config_raw.joystick_button[i], 0);
		}
	}

	return config;
}

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
	if (j.find(name) == j.end())
		return;

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
void safeGet(const nlohmann::json& j, const std::string& name, std::string& varTbc)
{
	if (j.find(name) == j.end())
		return;
    if (j[name].is_string())
        varTbc = j[name];
}

void safeGet(const nlohmann::json& j, const std::string& name, bool& varTbc)
{
	if (j.find(name) == j.end())
		return;
    if (j[name].is_boolean())
        varTbc = j[name];
}

void safeGet(const nlohmann::json& j, const std::string& name, int& varTbc)
{
	if (j.find(name) == j.end())
		return;
    if (j[name].is_number())
        varTbc = j[name];
}

#endif

void load()
{
	auto jConfig = loadJsonFromFile("config.json");

	auto jOldConfig = jConfig["oldConfig"];
    safeGet(jOldConfig, "name", old::gName);
	safeGet(jOldConfig, "use", old::gUse);
	if (old::gUse)
		return;

	auto jGame = jConfig["game"];

	debugFlags = 0;
	auto jDebugFlags = jGame["debugFlags"];
	if (jDebugFlags["showSlots"] == true)
		debugFlags |= showSlots;
	if (jDebugFlags["showNPCId"] == true)
		debugFlags |= showNPCId;
	if (jDebugFlags["showCARId"] == true)
		debugFlags |= showCARId;
	if (jDebugFlags["notifyOnNotImplemented"] == true)
		debugFlags |= notifyOnNotImplemented;
	if (jDebugFlags["showNPCHealth"] == true)
		debugFlags |= showNPCHealth;

	auto jPlayer = jGame["player"];
    safeGet(jPlayer, "disableDamage", disableDamage);

	auto jProfile = jGame["profile"];
    safeGet(jProfile, "name", profileName);
    safeGet(jProfile, "code", profileCode);

	auto jSound = jConfig["sound"];
	auto jOrg = jSound["org"];
    safeGet(jOrg, "disable", disableOrg);

    auto jScreen = jConfig["screen"];
    safeGet(jScreen, "width", screenWidth);
    safeGet(jScreen, "height", screenHeight);
    safeGet(jScreen, "scale", screenScale);
	
	auto jFps = jScreen["fps"];
	safeGet(jFps, "millisecondsPerFrame", framewait);
	safeGet(jFps, "displayCounter", displayFpsCounter);

	auto jInput = jConfig["input"];
    safeGet(jInput, "useGamepad", useGamepad);

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

}