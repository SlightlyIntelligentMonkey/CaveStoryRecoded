#pragma once
#include "common.h"

extern uint8_t tscFlags[1000]; //game flags
extern uint8_t skipFlags[64]; //skip flags
extern uint8_t mapFlags[128]; //Map flags

void initFlags() noexcept;

void setFlag(int a) noexcept;
void clearFlag(int a) noexcept;
bool getFlag(int a) noexcept attrPure;

void setSkipFlag(int a) noexcept;
void clearSkipFlag(int a) noexcept;
bool getSkipFlag(int a) noexcept attrPure;

void setMapFlag(int a) noexcept;
bool getMapFlag(int a) noexcept attrPure;
