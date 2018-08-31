#include "loadConfig.h"

#include "game.h"
#include "org.h"

#include <string>
#include <fstream>
#include <json.hpp>

using std::string;
using std::ifstream;
using nlohmann::json;

const string baseJsonFolder = "data/Config/";

json loadJsonFromFile(const string& path)
{
	ifstream file(path);
	if (file.bad())
		return json();
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

	if (jGame.count("millisecondsPerFrame"))
		framerate = jGame["millisecondsPerFrame"];
}

void loadOrgJson()
{
	auto jOrg = loadJsonFromFile(baseJsonFolder + "org.json");

	disableOrg = false;

	if (jOrg["disableOrg"] == true)
		disableOrg = true;
}

void loadConfigFiles()
{
	loadGameJson();
	loadOrgJson();
}
