#pragma once
#include "common.h"

enum gameStates
{
	QUIT = -1,
	INTRO = 0,
	MENU = 1,
	PLAY = 2,
	ESCAPE = 3
};

int mainGameLoop();
