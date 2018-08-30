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

struct ITEM
{
	int code;
};

//Game variables
constexpr auto ITEMS = 32;
constexpr auto PERMITSTAGES = 8;

extern PERMIT_STAGE permitStage[PERMITSTAGES];
extern ITEM items[ITEMS];
extern int selectedItem;

//Functions
void viewBounds() noexcept;
void initGame();

int escapeMenu();

int openInventory();
int stageSelect(int *runEvent);
int mainGameLoop();

void delimitFramerate() noexcept;
