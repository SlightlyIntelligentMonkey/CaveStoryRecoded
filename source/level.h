#pragma once
#include "common.h"
#include "npc.h"

struct STAGE_TABLE
{
	char tileset[0x20];
	char filename[0x20];
	uint32_t backgroundScroll;
	char background[0x20];
	char npc1[0x20];
	char npc2[0x20];
	uint8_t boss;
	char name[0x23];
};

extern STAGE_TABLE *stageTable;

extern int levelWidth;
extern int levelHeight;

extern int currentLevel;

extern BYTE *levelMap;
extern BYTE *levelTileAttributes;

extern SDL_Texture *levelTileset;

extern BYTE backgroundScroll;
extern SDL_Texture *levelBackground;

BYTE getTileAttribute(int x, int y);

void deleteTile(int x, int y);
void shiftTile(int x, int y);
bool changeTile(int x, int y, int tile);

void loadStageTable();

void loadLevel(int levelIndex);
void drawLevel(bool foreground);