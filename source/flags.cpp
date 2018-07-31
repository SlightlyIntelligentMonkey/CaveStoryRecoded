//FLAG FUNCTIONS
#include "common.h"

BYTE tscFlags[1000]; //game flags
BYTE skipFlags[64];

//Normal flags
void initFlags()
{
	memset(tscFlags, 0, sizeof(tscFlags));
	memset(skipFlags, 0, sizeof(skipFlags));
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
