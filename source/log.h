#pragma once

#include <fstream>
#include <string>

extern std::fstream logFile;

void initLogFile();

void logInfo(const std::string& str);
void logWarning(const std::string& str);
void logError(const std::string& str);
