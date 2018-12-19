#include "flags.h"

#include <cstring>
#include <string>
#include "log.h"

using std::memset;
using std::to_string;

uint8_t tscFlags[bitsToBytes(8000)]; //game flags
uint8_t skipFlags[bitsToBytes(512)]; //skip flags
uint8_t mapFlags[bitsToBytes(1024)]; //map flags

//Normal flags
void initFlags()
{
	memset(tscFlags, 0, sizeof(tscFlags));
	memset(skipFlags, 0, sizeof(skipFlags));
	memset(mapFlags, 0, sizeof(mapFlags));
}

inline void setBit(uint8_t *buf, size_t x)
{
    buf[bitsToBytes(x)] |= 1 << x % CHAR_BIT;
}

inline void clearBit(uint8_t *buf, size_t x)
{
    buf[bitsToBytes(x)] &= ~(1 << x % CHAR_BIT);
}

inline bool getBit(uint8_t *buf, size_t x)
{
    return ((1 << x % CHAR_BIT) & buf[bitsToBytes(x)]) != 0;
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
    if (a > bytesToBits(_countof(tscFlags)))
        logWarning("Tried to set flag " + to_string(a) + " (Out of bounds)");
    else
        setBit(tscFlags, a);
}

void clearFlag(size_t a)
{
    if (a > bytesToBits(_countof(tscFlags)))
        logWarning("Tried to clear flag " + to_string(a) + " (Out of bounds)");
    else
        clearBit(tscFlags, a);
}

bool getFlag(size_t a)
{
    if (a > bytesToBits(_countof(tscFlags)))
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
    if (a > bytesToBits(_countof(skipFlags)))
        logWarning("Tried to set skipFlag " + to_string(a) + " (Out of bounds)");
    else
        setBit(skipFlags, a);
}

void clearSkipFlag(size_t a)
{
    if (a > bytesToBits(_countof(skipFlags)))
        logWarning("Tried to clear skipFlag " + to_string(a) + " (Out of bounds)");
    else
        clearBit(skipFlags, a);
}

bool getSkipFlag(size_t a)
{
    if (a > bytesToBits(_countof(skipFlags)))
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
    if (a > bytesToBits(_countof(mapFlags)))
        logWarning("Tried to set mapFlag " + to_string(a) + " (Out of bounds)");
    else
        setBit(mapFlags, a);
}

bool getMapFlag(size_t a)
{
    if (a > bytesToBits(_countof(mapFlags)))
    {
        logWarning("Tried to get mapFlag " + to_string(a) + " (Out of bounds)");
        return false;
    }
    else
        return getBit(mapFlags, a);
}
