#pragma once

#include "common.h"

extern uint8_t tscFlags[1000]; //game flags
extern uint8_t skipFlags[64]; //skip flags
extern uint8_t mapFlags[128]; //Map flags

void initFlags();

void setFlag(size_t a);
void clearFlag(size_t a);
attrPure bool getFlag(size_t a);

void setSkipFlag(size_t a);
void clearSkipFlag(size_t a);
attrPure bool getSkipFlag(size_t a);

void setMapFlag(size_t a);
attrPure bool getMapFlag(size_t a);
