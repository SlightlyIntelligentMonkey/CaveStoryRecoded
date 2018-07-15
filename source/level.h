#pragma once
#include "common.h"
#include "npc.h"

extern int levelWidth;
extern int levelHeight;

extern BYTE *levelMap;
extern BYTE *levelTileAttributes;

extern SDL_Texture *levelTileset;

extern BYTE backgroundScroll;
extern SDL_Texture *levelBackground;

BYTE getTileAttribute(int x, int y);

void loadLevel(int levelIndex);
void drawLevel(bool foreground);