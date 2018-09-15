#pragma once

#include <fstream>
#include <string>

extern std::fstream logFile;

void initLogFile();

void logDebug(const std::string& str);
void logInfo(const std::string& str);
void logWarning(const std::string& str);
void logError(const std::string& str);
