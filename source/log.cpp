#include "log.h"

#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include "filesystem.h"

using std::fstream;
using std::endl;
using std::string;
using std::put_time;

static fstream logFile;

const string logFileName = "CaveStoryRecoded.log";

void initLogFile()
{
    if (fileExists(logFileName))
    {
        logFile.open(logFileName, fstream::app);
        logFile << "\n\n";
    }
    else
        logFile.open(logFileName, fstream::out);

    const auto now = time(nullptr);
    logFile << "New session started at : " << put_time(localtime(&now), "%c") << endl;
}

void logDebug(const string& str)
{
    const auto now = time(nullptr);
    logFile << '<' << put_time(localtime(&now), "%c") << '>' << " : Debug : " << str << endl;
}

void logInfo(const string& str)
{
    const auto now = time(nullptr);
    logFile << '<' << put_time(localtime(&now), "%c") << '>' << " : Information : " << str << endl;
}

void logWarning(const string& str)
{
    const auto now = time(nullptr);
    logFile << '<' << put_time(localtime(&now), "%c") << '>' << " : Warning : " << str << endl;
}

void logError(const string& str)
{
    const auto now = time(nullptr);
    logFile << '<' << put_time(localtime(&now), "%c") << '>' << " : Error : " << str << endl;
}
