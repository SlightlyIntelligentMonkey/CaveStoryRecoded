#pragma once
#include "common.h"

extern int keyLeft;
extern int keyRight;
extern int keyUp;
extern int keyDown;
extern int keyJump;
extern int keyShoot;
extern int keyMenu;
extern int keyMap;

extern int keyRotLeft;
extern int keyRotRight;

void getKeys(SDL_Event *event);

bool isKeyDown(int keynum)  attrPure;
bool isKeyPressed(int keynum)  attrPure;
