#include "log.h"

#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <SDL_version.h>
#include "filesystem.h"

static std::fstream gLogFile;

const std::string logFileName = "CaveStoryRecoded.log";

void initLogFile()
{
    if (fileExists(logFileName))
    {
        gLogFile.open(logFileName, std::fstream::app);
        gLogFile << "\n\n";
    }
    else
        gLogFile.open(logFileName, std::fstream::out);

    const auto now = time(nullptr);
    gLogFile << "Cave Story Recoded - Date of compilation : " << __DATE__ << ". Time of compilation : " << __TIME__ << '\n';
    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    gLogFile << "Compiled against SDL2 version " << static_cast<int>(compiled.major) 
    << '.' << static_cast<int>(compiled.minor) 
    << '.' << static_cast<int>(compiled.patch)
    << " and linked against version " << static_cast<int>(linked.major) 
    << '.' << static_cast<int>(linked.minor) 
    << '.' << static_cast<int>(linked.patch) << '\n';
    gLogFile << "Exact SDL2 revision number : " << SDL_GetRevisionNumber() << '\n';
    gLogFile << "New session started at : " << std::put_time(localtime(&now), "%c") << std::endl;
}

void logDebug(const std::string& str)
{
    const auto now = time(nullptr);
    gLogFile << '<' << std::put_time(localtime(&now), "%c") << '>' << " : Debug : " << str << std::endl;
}

void logInfo(const std::string& str)
{
    const auto now = time(nullptr);
    gLogFile << '<' << std::put_time(localtime(&now), "%c") << '>' << " : Information : " << str << std::endl;
}

void logWarning(const std::string& str)
{
    const auto now = time(nullptr);
    gLogFile << '<' << std::put_time(localtime(&now), "%c") << '>' << " : Warning : " << str << std::endl;
}

void logError(const std::string& str)
{
    const auto now = time(nullptr);
    gLogFile << '<' << std::put_time(localtime(&now), "%c") << '>' << " : Error : " << str << std::endl;
}
