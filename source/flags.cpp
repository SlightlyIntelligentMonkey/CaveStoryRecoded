//FLAG FUNCTIONS
#include "common.h"

BYTE tscFlags[1000]; //game flags
BYTE skipFlags[0x40]; //skip flags
BYTE mapFlags[0x80]; //map flags

//Normal flags
void initFlags()
{
	memset(tscFlags, 0, sizeof(tscFlags));
	memset(skipFlags, 0, sizeof(skipFlags));
	memset(mapFlags, 0, sizeof(mapFlags));
}

void setFlag(int a)
{
	tscFlags[a / 8] |= 1 << a % 8;
}

void clearFlag(int a)
{
	tscFlags[a / 8] &= ~(1 << a % 8);
}

bool getFlag(int a)
{
	return ((tscFlags[a / 8] >> a % 8) & 1) != 0;
}

//Skip flags
void setSkipFlag(int a)
{
	skipFlags[a / 8] |= 1 << a % 8;
}

void clearSkipFlag(int a)
{
	skipFlags[a / 8] &= ~(1 << a % 8);
}

bool getSkipFlag(int a)
{
	return ((skipFlags[a / 8] >> a % 8) & 1) != 0;
}

//Map flags
void setMapFlag(int a)
{
	mapFlags[a / 8] |= 1 << a % 8;
}

bool getMapFlag(int a)
{
	return ((mapFlags[a / 8] >> a % 8) & 1) != 0;
}
