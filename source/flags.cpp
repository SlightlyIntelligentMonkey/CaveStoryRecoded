//FLAG FUNCTIONS
#include "common.h"

BYTE tscFlags[1000]; //game flags
BYTE skipFlags[64];

void initFlags()
{
	memset(tscFlags, 0, sizeof(tscFlags));
}

void setFlag(int a)
{
	tscFlags[a / 8] |= 1 << a % 8;
	return;
}

void clearFlag(int a)
{
	tscFlags[a / 8] &= ~(1 << a % 8);
	return;
}

bool getFlag(int a)
{
	return ((tscFlags[a / 8] >> a % 8) & 1) != 0;
}