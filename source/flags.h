#pragma once
#include "common.h"

extern uint8_t tscFlags[1000]; //game flags
extern uint8_t skipFlags[64]; //skip flags
extern uint8_t mapFlags[128]; //Map flags

void initFlags();

void setFlag(int a);
void clearFlag(int a);
bool getFlag(int a)  attrPure;

void setSkipFlag(int a);
void clearSkipFlag(int a);
bool getSkipFlag(int a)  attrPure;

void setMapFlag(int a);
bool getMapFlag(int a)  attrPure;
