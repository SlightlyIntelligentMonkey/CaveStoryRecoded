#include "log.h"

#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <SDL_version.h>
#include "filesystem.h"

static std::fstream logFile;

const std::string logFileName = "CaveStoryRecoded.log";

void initLogFile()
{
    if (fileExists(logFileName))
    {
        logFile.open(logFileName, std::fstream::app);
        logFile << "\n\n";
    }
    else
        logFile.open(logFileName, std::fstream::out);

    const auto now = time(nullptr);
    logFile << "Cave Story Recoded - Date of compilation : " << __DATE__ << ". Time of compilation : " << __TIME__ << '\n';
    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    logFile << "Compiled against SDL2 version " << static_cast<int>(compiled.major) 
    << '.' << static_cast<int>(compiled.minor) 
    << '.' << static_cast<int>(compiled.patch)
    << " and linked against version " << static_cast<int>(linked.major) 
    << '.' << static_cast<int>(linked.minor) 
    << '.' << static_cast<int>(linked.patch) << '\n';
    logFile << "Exact SDL2 revision number : " << SDL_GetRevisionNumber() << '\n';
    logFile << "New session started at : " << std::put_time(localtime(&now), "%c") << std::endl;
}

void logDebug(const std::string& str)
{
    const auto now = time(nullptr);
    logFile << '<' << std::put_time(localtime(&now), "%c") << '>' << " : Debug : " << str << std::endl;
}

void logInfo(const std::string& str)
{
    const auto now = time(nullptr);
    logFile << '<' << std::put_time(localtime(&now), "%c") << '>' << " : Information : " << str << std::endl;
}

void logWarning(const std::string& str)
{
    const auto now = time(nullptr);
    logFile << '<' << std::put_time(localtime(&now), "%c") << '>' << " : Warning : " << str << std::endl;
}

void logError(const std::string& str)
{
    const auto now = time(nullptr);
    logFile << '<' << std::put_time(localtime(&now), "%c") << '>' << " : Error : " << str << std::endl;
}
