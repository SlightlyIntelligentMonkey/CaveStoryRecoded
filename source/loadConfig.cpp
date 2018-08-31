#include "loadConfig.h"

#include <string>
#include <fstream>
#include <json.hpp>

using std::string;
using std::ifstream;
using json = nlohmann::json;

const string baseJsonFolder = "data/Config/";

json loadJsonFromFile(const string& path)
{
	ifstream file(path);
	json j;
	file >> j;
	return j;
}

void loadGameJson()
{
	auto jsonTree = loadJsonFromFile(baseJsonFolder + "game.json");
}

void loadConfigFiles()
{
	loadGameJson();
}
