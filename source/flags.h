#pragma once
#include "common.h"

extern BYTE tscFlags[1000]; //game flags
extern BYTE skipFlags[64]; //skip flags
extern BYTE mapFlags[128]; //Map flags

void initFlags();

void setFlag(int a);
void clearFlag(int a);
bool getFlag(int a);

void setSkipFlag(int a);
void clearSkipFlag(int a);
bool getSkipFlag(int a);

void setMapFlag(int a);
bool getMapFlag(int a);
