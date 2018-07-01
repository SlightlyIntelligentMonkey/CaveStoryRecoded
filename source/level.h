#pragma once
#include "common.h"

extern int levelWidth;
extern int levelHeight;

extern BYTE *levelMap;
extern BYTE *levelTileAttributes;

extern SDL_Texture *levelTileset;

extern BYTE backgroundScroll;
extern SDL_Texture *levelBackground;

void createNpc(int x, int y, int direction, int xsp, int ysp, short flag, short event, short type, short flags);

void updateNPC();
void drawNPC();

BYTE getTileAttribute(int x, int y);

void loadLevel(int levelIndex);
void drawLevel(bool foreground);
