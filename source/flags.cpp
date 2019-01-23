#include "flags.h"

#include <cstring>
#include <string>
#include "log.h"

using std::memset;
using std::to_string;

uint8_t gTscFlags[bitsToBytes(8000)]; //game flags
uint8_t gSkipFlags[bitsToBytes(512)]; //skip flags
uint8_t gMapFlags[bitsToBytes(1024)]; //map flags

//Normal flags
void initFlags()
{
	memset(gTscFlags, 0, sizeof(gTscFlags));
	memset(gSkipFlags, 0, sizeof(gSkipFlags));
	memset(gMapFlags, 0, sizeof(gMapFlags));
}

inline void setBit(uint8_t *buf, size_t x)
{
    buf[bitsToBytes(x)] |= 1 << (x % CHAR_BIT);
}

inline void clearBit(uint8_t *buf, size_t x)
{
    buf[bitsToBytes(x)] &= ~(1 << (x % CHAR_BIT));
}

inline bool getBit(uint8_t *buf, size_t x)
{
    return ((1 << (x % CHAR_BIT)) & buf[bitsToBytes(x)]) != 0;
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
    if (a > bytesToBits(_countof(gTscFlags)))
        logWarning("Tried to set flag " + to_string(a) + " (Out of bounds)");
    else
        setBit(gTscFlags, a);
}

void clearFlag(size_t a)
{
    if (a > bytesToBits(_countof(gTscFlags)))
        logWarning("Tried to clear flag " + to_string(a) + " (Out of bounds)");
    else
        clearBit(gTscFlags, a);
}

bool getFlag(size_t a)
{
    if (a > bytesToBits(_countof(gTscFlags)))
    {
        logWarning("Tried to get flag " + to_string(a) + " (Out of bounds)");
        return false;
    }
    else
        return getBit(gTscFlags, a);
}

//Skip flags
void setSkipFlag(size_t a)
{
    if (a > bytesToBits(_countof(gSkipFlags)))
        logWarning("Tried to set skipFlag " + to_string(a) + " (Out of bounds)");
    else
        setBit(gSkipFlags, a);
}

void clearSkipFlag(size_t a)
{
    if (a > bytesToBits(_countof(gSkipFlags)))
        logWarning("Tried to clear skipFlag " + to_string(a) + " (Out of bounds)");
    else
        clearBit(gSkipFlags, a);
}

bool getSkipFlag(size_t a)
{
    if (a > bytesToBits(_countof(gSkipFlags)))
    {
        logWarning("Tried to get skipFlag " + to_string(a) + " (Out of bounds)");
        return false;
    }
    else
        return getBit(gSkipFlags, a);
}

//Map flags
void setMapFlag(size_t a)
{
    if (a > bytesToBits(_countof(gMapFlags)))
        logWarning("Tried to set mapFlag " + to_string(a) + " (Out of bounds)");
    else
        setBit(gMapFlags, a);
}

bool getMapFlag(size_t a)
{
    if (a > bytesToBits(_countof(gMapFlags)))
    {
        logWarning("Tried to get mapFlag " + to_string(a) + " (Out of bounds)");
        return false;
    }
    else
        return getBit(gMapFlags, a);
}
