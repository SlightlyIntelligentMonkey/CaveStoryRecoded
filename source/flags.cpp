//FLAG FUNCTIONS
#include "common.h"

BYTE gameFlags[1000]; //game flags

void initFlags()
{
	memset(gameFlags, 0, sizeof(gameFlags));
}

void setFlag(int a)
{
	gameFlags[a / 8] |= 1 << a % 8;
	return;
}

void clearFlag(int a)
{
	gameFlags[a / 8] &= ~(1 << a % 8);
	return;
}

bool getFlag(int a)
{
	return ((gameFlags[a / 8] >> a % 8) & 1) != 0;
}