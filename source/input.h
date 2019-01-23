#pragma once

#include "common.h"

extern bool gUseGamepad;

extern int gKeyLeft;
extern int gKeyRight;
extern int gKeyUp;
extern int gKeyDown;
extern int gKeyJump;
extern int gKeyShoot;
extern int gKeyMenu;
extern int gKeyMap;
extern int gKeyRotateLeft;
extern int gKeyRotateRight;

void initGamepad();

void getKeys();

attrPure bool isKeyDown(int keynum);
attrPure bool isKeyPressed(int keynum);
bool handleEvents();
