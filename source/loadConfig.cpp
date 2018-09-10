#include "loadConfig.h"

#include "game.h"
#include "org.h"
#include "filesystem.h"
#include "player.h"
#include "render.h"

#include <string>
#include <fstream>
#include <json.hpp>

using std::string;
using std::ifstream;
using nlohmann::json;

const string baseJsonFolder = "Config/";

json loadJsonFromFile(const string& path)
{
	if (!fileExists(path))
		return json();
	ifstream file(path);
	json j;
	file >> j;
	return j;
}

void loadGameJson()
{
	auto jGame = loadJsonFromFile(baseJsonFolder + "game.json");

	debugFlags = 0;
	auto jDbgFlgs = jGame["debugFlags"];
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
}

void loadOrgJson()
{
	auto jOrg = loadJsonFromFile(baseJsonFolder + "org.json");

	disableOrg = false;

	if (jOrg["disableOrg"] == true)
		disableOrg = true;
}

void loadFilesystemJson()
{
	auto jFilesystem = loadJsonFromFile(baseJsonFolder + "filesystem.json");

	if (jFilesystem["profileName"].is_string())
		profileName = jFilesystem["profileName"];

	if (jFilesystem["profileCode"].is_string())
		profileCode = jFilesystem["profileCode"];
}

void loadPlayerJson()
{
    auto jPlayer = loadJsonFromFile(baseJsonFolder + "player.json");

    if (jPlayer["disableDamage"] == true)
        disableDamage = true;
}

void loadConfigFiles()
{
	loadGameJson();
	loadOrgJson();
	loadFilesystemJson();
	loadPlayerJson();
}
