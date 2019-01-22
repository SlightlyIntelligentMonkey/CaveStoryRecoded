#pragma once

#include "npc.h"

struct SDL_Texture;

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

struct MAPNAME
{
	int flag;
	int wait;
	char name[32];
};

struct BACKGROUND
{
	int flag;
	int tileWidth;
	int tileHeight;
	int numX;
	int numY;
	int mode;
	int effect;
	char unknownFiller[12];
};

struct MAP
{
	uint8_t *tile;
	uint8_t *attribute;
	int width;
	int height;
};

extern MAPNAME mapName;

extern STAGE_TABLE *stageTable;

extern MAP map;
extern BACKGROUND background;

extern int currentLevel;

extern int gWaterY;

extern SDL_Texture *levelTileset;
extern SDL_Texture *levelBackground;

uint8_t getTileAttribute(size_t x, size_t y) attrPure;

void deleteTile(size_t x, size_t y);
void shiftTile(size_t x, size_t y);
bool changeTile(size_t x, size_t y, uint8_t tile);

void loadStageTable();

void loadLevel(size_t levelIndex, int w, int x, int y);
void drawLevel(bool foreground);

template<typename T> constexpr inline int32_t tilesToUnits(T x)
{
    return x * 0x2000;  // x * 8192 or x << 13
}

template<typename T> constexpr inline int32_t unitsToTiles(T x)
{
	return x / 0x2000;  // x / 8192 or x >> 13
}

template<typename T> constexpr inline int32_t tilesToPixels(T x)
{
	return x * 0x10;    // x * 16 or x << 4
}

template<typename T> constexpr inline int32_t pixelsToTiles(T x)
{
	return x / 0x10;    // x / 16 or x >> 4
}

template <typename T> constexpr inline int32_t pixelsToUnits(T x)
{
	return x * 0x200;   // x * 512 or x << 9
}

template <typename T> constexpr inline T unitsToPixels(T x)
{
	return x / 0x200;   // x / 512 or x >> 9
}

