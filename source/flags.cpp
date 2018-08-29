//FLAG FUNCTIONS
#include "common.h"
#include "flags.h"

#include <cstring>

using std::memset;

uint8_t tscFlags[1000]; //game flags
uint8_t skipFlags[0x40]; //skip flags
uint8_t mapFlags[0x80]; //map flags

//Normal flags
void initFlags() noexcept
{
	memset(tscFlags, 0, sizeof(tscFlags));
	memset(skipFlags, 0, sizeof(skipFlags));
	memset(mapFlags, 0, sizeof(mapFlags));
}

void setFlag(int a) noexcept
{
	tscFlags[a / 8] |= 1 << (a % 8);
}

void clearFlag(int a) noexcept
{
	tscFlags[a / 8] &= ~(1 << (a % 8));
}

bool getFlag(int a) noexcept
{
	return ((tscFlags[a / 8] >> (a % 8)) & 1) != 0;
}

//Skip flags
void setSkipFlag(int a) noexcept
{
	skipFlags[a / 8] |= 1 << (a % 8);
}

void clearSkipFlag(int a) noexcept
{
	skipFlags[a / 8] &= ~(1 << (a % 8));
}

bool getSkipFlag(int a) noexcept
{
	return ((skipFlags[a / 8] >> (a % 8)) & 1) != 0;
}

//Map flags
void setMapFlag(int a) noexcept
{
	mapFlags[a / 8] |= 1 << (a % 8);
}

bool getMapFlag(int a) noexcept
{
	return ((mapFlags[a / 8] >> (a % 8)) & 1) != 0;
}
