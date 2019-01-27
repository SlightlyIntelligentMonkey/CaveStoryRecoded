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

extern MAPNAME gMapName;

extern STAGE_TABLE *gStageTable;

extern MAP gMap;
extern BACKGROUND gBackground;

extern int gCurrentLevel;

extern int gWaterY;

extern SDL_Texture *gLevelTileset;
extern SDL_Texture *gLevelBackground;

uint8_t getTileAttribute(size_t x, size_t y) attrPure;

void deleteTile(size_t x, size_t y);
void shiftTile(size_t x, size_t y);
bool changeTile(size_t x, size_t y, uint8_t tile);

void loadStageTable();

void loadLevel(size_t levelIndex, int w, int x, int y);
void drawLevel(bool foreground);

template<typename T> constexpr inline int tilesToUnits(T x)
{
    return static_cast<int>(x * static_cast<T>(0x2000));  // x * 8192 or x << 13
}

template<typename T> constexpr inline T unitsToTiles(T x)
{
	return static_cast<T>(x / static_cast<T>(0x2000));  // x / 8192 or x >> 13
}

template<typename T> constexpr inline int tilesToPixels(T x)
{
	return static_cast<int>(x * static_cast<T>(0x10));    // x * 16 or x << 4
}

template<typename T> constexpr inline T pixelsToTiles(T x)
{
	return static_cast<T>(x / static_cast<T>(0x10));    // x / 16 or x >> 4
}

template <typename T> constexpr inline int pixelsToUnits(T x)
{
	return static_cast<int>(x * static_cast<T>(0x200));   // x * 512 or x << 9
}

template <typename T> constexpr inline T unitsToPixels(T x)
{
	return static_cast<T>(x / static_cast<T>(0x200));   // x / 512 or x >> 9
}

