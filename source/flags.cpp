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

inline void setBit(uint8_t *buf, size_t x)
{
    buf[x >> 3] |= 1 << (x & 7);
}

inline void clearBit(uint8_t *buf, size_t x)
{
    buf[x >> 3] &= ~(1 << (x & 7));
}

inline bool getBit(uint8_t *buf, size_t x)
{
    return (buf[x >> 3] >> (x & 7)) & 1;
}

inline void flipBit(uint8_t *buf, size_t x)
{
    if (getBit(buf, x))
        clearBit(buf, x);
    else
        setBit(buf, x);
}

void setFlag(size_t a)
{
    if (a > _countof(tscFlags) * 8)
        logWarning("Tried to set flag " + to_string(a) + " (Out of bounds)");
    else
        setBit(tscFlags, a);
}

void clearFlag(size_t a)
{
    if (a > _countof(tscFlags) * 8)
        logWarning("Tried to clear flag " + to_string(a) + " (Out of bounds)");
    else
        clearBit(tscFlags, a);
}

bool getFlag(size_t a)
{
    if (a > _countof(tscFlags) * 8)
    {
        logWarning("Tried to get flag " + to_string(a) + " (Out of bounds)");
        return false;
    }
    else
        return getBit(tscFlags, a);
}

//Skip flags
void setSkipFlag(size_t a)
{
    if (a > _countof(skipFlags) * 8)
        logWarning("Tried to set skipFlag " + to_string(a) + " (Out of bounds)");
    else
        setBit(skipFlags, a);
}

void clearSkipFlag(size_t a)
{
    if (a > _countof(skipFlags) * 8)
        logWarning("Tried to clear skipFlag " + to_string(a) + " (Out of bounds)");
    else
        clearBit(skipFlags, a);
}

bool getSkipFlag(size_t a)
{
    if (a > _countof(skipFlags) * 8)
    {
        logWarning("Tried to get skipFlag " + to_string(a) + " (Out of bounds)");
        return false;
    }
    else
        return getBit(skipFlags, a);
}

//Map flags
void setMapFlag(size_t a)
{
    if (a > _countof(mapFlags) * 8)
        logWarning("Tried to set mapFlag " + to_string(a) + " (Out of bounds)");
    else
        setBit(mapFlags, a);
}

bool getMapFlag(size_t a)
{
    if (a > _countof(mapFlags) * 8)
    {
        logWarning("Tried to get mapFlag " + to_string(a) + " (Out of bounds)");
        return false;
    }
    else
        return getBit(mapFlags, a);
}
