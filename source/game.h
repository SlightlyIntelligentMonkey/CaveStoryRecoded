#pragma once
#include "common.h"

enum gameStates
{
	INTRO = 1,
	MENU = 2,
	PLAY = 3,
};

struct PERMIT_STAGE
{
	int index;
	int event;
};

//Game variables
extern PERMIT_STAGE permitStage[8];

//Functions
void viewBounds();
void initGame();
int stageSelect(int *runEvent);
int mainGameLoop();
