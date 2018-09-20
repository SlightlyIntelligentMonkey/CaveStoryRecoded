#include "flags.h"

#include <cstring>
#include <string>
#include "log.h"

using std::memset;
using std::to_string;

uint8_t tscFlags[1000]; //game flags
uint8_t skipFlags[0x40]; //skip flags
uint8_t mapFlags[0x80]; //map flags

//Normal flags
void initFlags()
{
	memset(tscFlags, 0, sizeof(tscFlags));
	memset(skipFlags, 0, sizeof(skipFlags));
	memset(mapFlags, 0, sizeof(mapFlags));
}

void setFlag(size_t a)
{
    if (a > _countof(tscFlags) * 8)
        logWarning("Tried to set flag " + to_string(a) + " (Out of bounds)");
    else
        tscFlags[a / 8] |= 1 << (a % 8);
}

void clearFlag(size_t a)
{
    if (a > _countof(tscFlags) * 8)
        logWarning("Tried to clear flag " + to_string(a) + " (Out of bounds)");
    else
        tscFlags[a / 8] &= ~(1 << (a % 8));
}

bool getFlag(size_t a)
{
    if (a > _countof(tscFlags) * 8)
    {
        logWarning("Tried to get flag " + to_string(a) + " (Out of bounds)");
        return false;
    }
    else
        return ((tscFlags[a / 8] >> (a % 8)) & 1) != 0;
}

//Skip flags
void setSkipFlag(size_t a)
{
    if (a > _countof(skipFlags) * 8)
        logWarning("Tried to set skipFlag " + to_string(a) + " (Out of bounds)");
    else
        skipFlags[a / 8] |= 1 << (a % 8);
}

void clearSkipFlag(size_t a)
{
    if (a > _countof(skipFlags) * 8)
        logWarning("Tried to clear skipFlag " + to_string(a) + " (Out of bounds)");
    else
        skipFlags[a / 8] &= ~(1 << (a % 8));
}

bool getSkipFlag(size_t a)
{
    if (a > _countof(skipFlags) * 8)
    {
        logWarning("Tried to get skipFlag " + to_string(a) + " (Out of bounds)");
        return false;
    }
    else
        return ((skipFlags[a / 8] >> (a % 8)) & 1) != 0;
}

//Map flags
void setMapFlag(size_t a)
{
    if (a > _countof(mapFlags) * 8)
        logWarning("Tried to set mapFlag " + to_string(a) + " (Out of bounds)");
    else
        mapFlags[a / 8] |= 1 << (a % 8);
}

bool getMapFlag(size_t a)
{
    if (a > _countof(mapFlags) * 8)
    {
        logWarning("Tried to get mapFlag " + to_string(a) + " (Out of bounds)");
        return false;
    }
    else
        return ((mapFlags[a / 8] >> (a % 8)) & 1) != 0;
}
