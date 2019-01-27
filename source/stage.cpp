#include "stage.h"
#include <string>
#include <cstring>
#include <SDL_render.h>
#include "render.h"
#include "mathUtils.h"
#include "game.h"
#include "player.h"
#include "script.h"
#include "flags.h"
#include "filesystem.h"
#include "boss.h"
#include "bullet.h"
#include "caret.h"
#include "log.h"
#include "main.h"
#include "valueview.h"

int gCurrentLevel;

MAPNAME gMapName;

//Loaded level stuff
MAP gMap;
BACKGROUND gBackground;

//Effect things
int gCurrentEffect = 0;

//water level (used in core area)
int gWaterY = 0;

//Get stage data
STAGE_TABLE *gStageTable;


void loadStageTable()
{
	SDL_RWops *tblStream = SDL_RWFromFile("data/stage.tbl", "rb");

	if (tblStream == nullptr)
		doError();

	const auto stages = static_cast<size_t>(tblStream->size(tblStream)) / sizeof(STAGE_TABLE);

	gStageTable = new STAGE_TABLE[stages];

	if (gStageTable == nullptr)
		doCustomError("Could not allocate memory for stage table");

	for (size_t i = 0; i < stages; i++)
	{
		tblStream->read(tblStream, gStageTable[i].tileset, 0x20, 1);
		tblStream->read(tblStream, gStageTable[i].filename, 0x20, 1);
		gStageTable[i].backgroundScroll = SDL_ReadLE32(tblStream);
		tblStream->read(tblStream, gStageTable[i].background, 0x20, 1);
		tblStream->read(tblStream, gStageTable[i].npc1, 0x20, 1);
		tblStream->read(tblStream, gStageTable[i].npc2, 0x20, 1);
		tblStream->read(tblStream, &gStageTable[i].boss, 1, 1);
		tblStream->read(tblStream, gStageTable[i].name, 0x23, 1);

		if (!strcmp(gStageTable[i].name, "u"))
			strcpy(gStageTable[i].name, "Studio Pixel presents");
	}
}

uint8_t getTileAttribute(size_t xTile, size_t yTile)
{
	if (xTile < static_cast<size_t>(gMap.width) && yTile < static_cast<size_t>(gMap.height))  // Make sure coordinates are valid
		return gMap.attribute[gMap.tile[xTile + yTile * gMap.width]];

    // Coordinates are invalid
	return 0;
}

void deleteTile(size_t x, size_t y)
{
	gMap.tile[x + y * gMap.width] = 0;
}

void shiftTile(size_t x, size_t y)
{
	--gMap.tile[x + y * gMap.width];
}

bool changeTile(size_t x, size_t y, uint8_t tile)
{
	if (gMap.tile[y * gMap.width + x] == tile)
		return false;

	gMap.tile[x + y * gMap.width] = tile;

	for (int i = 0; i < 3; ++i)
		createNpc(NPC_Smoke, tilesToUnits(x), tilesToUnits(y));

	return true;
}

void loadPxm(const std::string &name)
{
	//frees old map memory
	delete[] gMap.tile;

	auto tmp = loadFile("data/Stage/" + name + ".pxm");
	auto pxmSize = tmp.first;
	auto pxm = tmp.second;

	gMap.width = readLEshort(pxm.data(), 4);
	gMap.height = readLEshort(pxm.data(), 6);

	gMap.tile = new uint8_t[pxmSize - 8];
	memcpy(gMap.tile, pxm.data() + 8, pxmSize - 8);
}

void loadPxa(const std::string &name)
{
	//free old tile attribute memory
	delete[] gMap.attribute;

	auto tmp = loadFile("data/Stage/" + name + ".pxa");
	auto pxaSize = tmp.first;
	auto pxa = tmp.second;

	gMap.attribute = new uint8_t[pxaSize];
	memcpy(gMap.attribute, pxa.data(), pxaSize);
}

void loadPxe(const std::string &name)
{
	//Clear old npcs
	gNPC.clear();
	gNPC.shrink_to_fit();

	auto pxe = loadFile("data/Stage/" + name + ".pxe").second;

	//Load npcs
	const int npcAmount = readLElong(pxe.data(), 4);

	for (int i = 0; i < npcAmount; i++)
	{
		const int offset = (i * 12) + 8;

		if (readLEshort(pxe.data(), offset + 10) & npc_appearSet && !(getFlag(readLEshort(pxe.data(), offset + 4))))
			continue;

		if (readLEshort(pxe.data(), offset + 10) & npc_hideSet && getFlag(readLEshort(pxe.data(), offset + 4)))
			continue;

		npc newNPC;
		newNPC.init(readLEshort(pxe.data(), offset + 8), tilesToUnits(readLEshort(pxe.data(), offset)), tilesToUnits(readLEshort(pxe.data(), offset + 2)), 0, 0, 0, nullptr);

		newNPC.code_event = readLEshort(pxe.data(), offset + 6);
		newNPC.code_flag = readLEshort(pxe.data(), offset + 4);
		newNPC.bits |= readLEshort(pxe.data(), offset + 10);

		if (readLEshort(pxe.data(), offset + 10) & npc_altDir)
			newNPC.direct = dirRight;

		gNPC.push_back(newNPC);
	}
}

void iniBackground(const std::string &name, int mode)
{
	gBackground.mode = mode;
	gBackground.tileWidth = gMap.width;
	gBackground.tileHeight = gMap.height;

	gBackground.flag = 1;
	loadImage(name, &gSprites[TEX_BACKGROUND]);	// Load background texture
	gWaterY = tilesToUnits(240);
}

void loadLevel(size_t levelIndex, int w, int x, int y)
{
	logInfo("Loading level " + std::to_string(levelIndex));

	currentPlayer.setPos(tilesToUnits(x), tilesToUnits(y));

	// -- clears/frees stuff from last map -- //
	carets.clear();
	carets.shrink_to_fit();

	bullets.clear();
	bullets.shrink_to_fit();

	valueviews.clear();
	valueviews.shrink_to_fit();

	//load tileset
	loadImage(std::string("Stage/Prt") + gStageTable[levelIndex].tileset, &gSprites[TEX_TILESET]);

	//load pxm, pxa, pxe, and tsc
	loadPxa(gStageTable[levelIndex].tileset);
	loadPxm(gStageTable[levelIndex].filename);
	loadPxe(gStageTable[levelIndex].filename);
	loadStageTsc(gStageTable[levelIndex].filename);

	//Load background
	iniBackground(gStageTable[levelIndex].background, gStageTable[levelIndex].backgroundScroll);

	// -- loads map images -- //
	//load sheet 1
	loadImage(std::string("Npc/Npc") + gStageTable[levelIndex].npc1, &gSprites[TEX_NPC_1]);
	//load sheet 2
	loadImage(std::string("Npc/Npc") + gStageTable[levelIndex].npc2, &gSprites[TEX_NPC_2]);

	// -- loads some map stuff -- //
	//Set up map name
	gMapName.flag = 0;
	gMapName.wait = 0;
	strcpy(gMapName.name, gStageTable[levelIndex].name);

	startTscEvent(tsc, w);

	SetFrameMyChar();
	initBoss(gStageTable[levelIndex].boss);

	gCurrentLevel = levelIndex;

	// -- fix gViewport -- //
//	gViewport.x = currentPlayer.x - (gScreenWidth << 8);
//	gViewport.y = currentPlayer.y - (gScreenHeight << 8);
//	gViewport.lookX = &currentPlayer.tgt_x;
//	gViewport.lookY = &currentPlayer.tgt_y;
	gViewport.quake = 0;
	gViewport.quake2 = 0;

	viewBounds();
}

void drawForeground(void)
{
	RECT rect;

	if (gBackground.mode == 3)
	{
		const int xOff = gViewport.x % pixelsToUnits(32);

		rect.left = 0;
		rect.right = 32;

		rect.top = 0;
		rect.bottom = 16;
		//draws top of water
		for (int x = gViewport.x - xOff; x < gViewport.x + pixelsToUnits(gScreenWidth); x += pixelsToUnits(32))
			drawTexture(gSprites[TEX_BACKGROUND], &rect,
				unitsToPixels(x) - unitsToPixels(gViewport.x), unitsToPixels(gWaterY) - unitsToPixels(gViewport.y));
		//draws bottom of water
		rect.top = 16;
		rect.bottom = 32;
		for (int y = gWaterY; y < gViewport.y + pixelsToUnits(gScreenHeight); y += pixelsToUnits(16))
		{
			for (int x = gViewport.x - xOff; x < gViewport.x + pixelsToUnits(gScreenWidth); x += pixelsToUnits(32))
				drawTexture(gSprites[TEX_BACKGROUND], &rect, 
					unitsToPixels(x) - unitsToPixels(gViewport.x), unitsToPixels(y) - unitsToPixels(gViewport.y));
		}
	}
}

static void updateBackgroundEffect(int w)
{
	if (gGameFlags & 1)
	{
		if (gBackground.mode == 5)
		{
			gBackground.effect += 0xC00;
		}

		else if (gBackground.mode >= 5 && gBackground.mode <= 7)
		{
			++gBackground.effect;
			gBackground.effect %= (w * 2);
		}
	}
}

void drawBackground(void)
{
	int w, h;
	SDL_QueryTexture(gSprites[TEX_BACKGROUND], nullptr, nullptr, &w, &h);

	RECT rect = { 0, 0, w, h };

	updateBackgroundEffect(w);

	switch (gBackground.mode)
	{
	case 0:
		for (int x = 0; x < gScreenWidth; x += w)
		{
			for (int y = 0; y < gScreenHeight; y += h)
				drawTexture(gSprites[TEX_BACKGROUND], &rect, x, y);
		}

		break;

	case 1:
		for (int x = -(gViewport.x / 0x400 % w); x < gScreenWidth; x += w)
		{
			for (int y = -(gViewport.y / 0x400 % h); y < gScreenHeight; y += h)
				drawTexture(gSprites[TEX_BACKGROUND], &rect, x, y);
		}

		break;

	case 2:
		for (int x = -(gViewport.x / 0x200 % w); x < gScreenWidth; x += w)
		{
			for (int y = -(gViewport.y / 0x200 % h); y < gScreenHeight; y += h)
				drawTexture(gSprites[TEX_BACKGROUND], &rect, x, y);
		}

		break;

	case 5:
		for (int x = -(gBackground.effect / 0x200 % w); x < gScreenWidth; x += w)
		{
			for (int y = 0; y < gScreenHeight; y += h)
				drawTexture(gSprites[TEX_BACKGROUND], &rect, x, y);
		}

		break;

	case 6:
	case 7:
		//Draw sky
		rect = { 0, 0, w / 2, 88 };

		int skyOff = (((w / 2) - gScreenWidth) / 2);

		//Draw middle
		drawTexture(gSprites[0x1C], &rect, -skyOff, 0);

		//Repeat stars or whatever
		rect = { w / 2, 0, w, 88 };

		for (int i = 0; i < gScreenWidth - (skyOff / 2 + rect.left); i += rect.left)
		{
			drawTexture(gSprites[TEX_BACKGROUND], &rect, -skyOff + (rect.left + i), 0);
			drawTexture(gSprites[TEX_BACKGROUND], &rect, -skyOff - (rect.left + i), 0);
		}

		//Cloud layers
		rect.left = 0;
		rect.right = w;

		rect.top = 88;
		rect.bottom = 123;
		for (int i = 0; i <= (gScreenWidth / w) + 1; ++i)
			drawTexture(gSprites[TEX_BACKGROUND], &rect, (w * i) - (gBackground.effect / 2) % w, rect.top);

		rect.top = 123;
		rect.bottom = 146;
		for (int i = 0; i <= (gScreenWidth / w) + 1; ++i)
			drawTexture(gSprites[TEX_BACKGROUND], &rect, (w * i) - gBackground.effect % w, rect.top);

		rect.top = 146;
		rect.bottom = 176;
		for (int i = 0; i <= (gScreenWidth / w) + 1; ++i)
			drawTexture(gSprites[TEX_BACKGROUND], &rect, (w * i) - (gBackground.effect * 2) % w, rect.top);

		rect.top = 176;
		rect.bottom = 240;
		for (int i = 0; i <= (gScreenWidth / w) + 1; ++i)
			drawTexture(gSprites[TEX_BACKGROUND], &rect, (w * i) - (gBackground.effect * 4) % w, rect.top);

		break;
	}
}

void drawLevel(bool foreground)
{
	if (!foreground)   //Draw background
	{
		drawBackground();
	}

	//Animate currents
	if (gGameFlags & 1 && foreground)
		gCurrentEffect += 2;

	//Render tiles
	RECT tileRect;

	const int xFrom = clamp(unitsToTiles(gViewport.x + 0x1000), 0, gMap.width);
	const int xTo = clamp((unitsToTiles((gViewport.x + 0x1000) + (gScreenWidth << 9))) + 1, 0, gMap.width); //add 1 because edge wouldn't appear

	const int yFrom = clamp(unitsToTiles(gViewport.y + 0x1000), 0, gMap.height);
	const int yTo = clamp((unitsToTiles((gViewport.y + 0x1000) + (gScreenWidth << 9))) + 1, 0, gMap.height); //add 1 because edge wouldn't appear

	for (int x = xFrom; x < xTo; x++)
	{
		for (int y = yFrom; y < yTo; y++)
		{
			const int i = x + y * gMap.width;

			const int tile = gMap.tile[i];
			if (tile)
			{
				const int attribute = gMap.attribute[tile];

				if ((attribute < 0x20 && !foreground) || (attribute >= 0x40 && foreground))
				{
					const int drawX = i % gMap.width;
					const int drawY = i / gMap.width;

					if (attribute < 0x80)
					{
						tileRect = { (tile % 16) * 16, (tile / 16) * 16, (tile % 16) * 16 + 16, (tile / 16) * 16 + 16 };

						drawTexture(gSprites[TEX_TILESET], &tileRect, 
							tilesToPixels(drawX) - unitsToPixels(gViewport.x) - 8, 
							tilesToPixels(drawY) - unitsToPixels(gViewport.y) - 8);

						if (attribute == 0x43) //Star block
						{
							tileRect = { 256, 48, 272, 64 };

							drawTexture(gSprites[TEX_NPC_SYM], &tileRect, 
								tilesToPixels(drawX) - unitsToPixels(gViewport.x) - 8, 
								tilesToPixels(drawY) - unitsToPixels(gViewport.y) - 8);
						}
					}
					else
					{
						switch (attribute)
						{
						case 0x80:
						case 0xA0:
							tileRect.left = (gCurrentEffect & 0xF) + 224;
							tileRect.right = (gCurrentEffect & 0xF) + 240;
							tileRect.top = 48;
							tileRect.bottom = 64;
							break;

						case 0x81:
						case 0xA1:
							tileRect.left = 224;
							tileRect.right = 240;
							tileRect.top = (gCurrentEffect & 0xF) + 48;
							tileRect.bottom = (gCurrentEffect & 0xF) + 64;
							break;

						case 0x82:
						case 0xA2:
							tileRect.left = 240 - (gCurrentEffect & 0xF);
							tileRect.right = 256 - (gCurrentEffect & 0xF);
							tileRect.top = 48;
							tileRect.bottom = 64;
							break;

						case 0x83:
						case 0xA3:
							tileRect.left = 224;
							tileRect.right = 240;
							tileRect.top = 64 - (gCurrentEffect & 0xF);
							tileRect.bottom = 80 - (gCurrentEffect & 0xF);
							break;

						default:
							return;
						}

						drawTexture(gSprites[TEX_CARET], &tileRect, 
							tilesToPixels(drawX) - unitsToPixels(gViewport.x) - 8, 
							tilesToPixels(drawY) - unitsToPixels(gViewport.y) - 8);
					}
				}
			}
		}
	}

	if (foreground)
	{
		//draw foreground
		drawForeground();

		//Render black bars in foreground
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 32, 255);

		//Left and right
		const int leftBorder = -(gViewport.x / 0x200);
		const int rightBorder = ((gViewport.x / 0x200) + gScreenWidth) - ((gMap.width - 1) << 4);

		drawRect(0, 0, leftBorder, gScreenHeight);
		drawRect(gScreenWidth - rightBorder, 0, rightBorder, gScreenHeight);

		//Top and bottom
		const int topBorder = -(gViewport.y / 0x200);
		const int bottomBorder = ((gViewport.y / 0x200) + gScreenHeight) - ((gMap.height - 1) << 4);

		drawRect(0, 0, gScreenWidth, topBorder);
		drawRect(0, gScreenHeight - bottomBorder, gScreenWidth, bottomBorder);
	}
}

