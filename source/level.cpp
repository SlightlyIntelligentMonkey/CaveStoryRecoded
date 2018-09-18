#include "level.h"

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

using std::string;
using std::to_string;

int currentLevel;

MAPNAME mapName;

//Loaded level stuff
int levelWidth;
int levelHeight;

uint8_t *levelMap = nullptr;
uint8_t *levelTileAttributes = nullptr;

uint8_t backgroundScroll;

//Effect things
int backgroundEffect = 0;
int currentEffect = 0;

//Get stage data
STAGE_TABLE *stageTable;

void loadStageTable()
{
	SDL_RWops *tblStream = SDL_RWFromFile("data/stage.tbl", "rb");

	if (tblStream == nullptr)
		doError();

	const auto stages = static_cast<size_t>(SDL_RWsize(tblStream) / 200);

	stageTable = static_cast<STAGE_TABLE*>(malloc(stages * 200));
	if (stageTable == nullptr)
		doCustomError("Could not allocate memory for stage table");

	for (size_t i = 0; i < stages; i++)
	{
		tblStream->read(tblStream, stageTable[i].tileset, 0x20, 1);
		tblStream->read(tblStream, stageTable[i].filename, 0x20, 1);
		stageTable[i].backgroundScroll = SDL_ReadLE32(tblStream);
		tblStream->read(tblStream, stageTable[i].background, 0x20, 1);
		tblStream->read(tblStream, stageTable[i].npc1, 0x20, 1);
		tblStream->read(tblStream, stageTable[i].npc2, 0x20, 1);
		tblStream->read(tblStream, &stageTable[i].boss, 1, 1);
		tblStream->read(tblStream, stageTable[i].name, 0x23, 1);

		if (!strcmp(stageTable[i].name, "u"))
			strcpy(stageTable[i].name, "Studio Pixel presents");
	}
}

uint8_t getTileAttribute(int xTile, int yTile)
{
	if (xTile >= 0 && xTile < levelWidth && yTile >= 0 && yTile < levelHeight)  // Make sure coordinates are valid
		return levelTileAttributes[levelMap[xTile + yTile * levelWidth]];

    // Coordinates are invalid
	return 0;
}

void deleteTile(int x, int y)
{
	levelMap[x + y * levelWidth] = 0;
}

void shiftTile(int x, int y)
{
	--levelMap[x + y * levelWidth];
}

bool changeTile(int x, int y, uint8_t tile)
{
	if (levelMap[y * levelWidth + x] == tile)
		return false;

	levelMap[x + y * levelWidth] = tile;

	for (int i = 0; i < 3; ++i)
		createNpc(NPC_Smoke, tilesToUnits(x), tilesToUnits(y));

	return true;
}

void loadLevel(int levelIndex)
{
    logInfo("Loading level " + to_string(levelIndex));
	currentLevel = levelIndex;

	//Clear old carets
	carets.clear();
	carets.shrink_to_fit();

	//Clear old bullets
	bullets.clear();
	bullets.shrink_to_fit();

	//Clear old valueviews
	valueviews.clear();
	valueviews.shrink_to_fit();

	//Set up map name
	mapName.flag = 0;
	mapName.wait = 0;
	strcpy(mapName.name, stageTable[levelIndex].name);

	//Load boss
	initBoss(stageTable[levelIndex].boss);

	//Load pxm
	const string pxmPath = string("data/Stage/") + stageTable[levelIndex].filename + ".pxm";

	uint8_t *pxm = nullptr;
	const int pxmSize = loadFile(pxmPath.c_str(), &pxm);

	levelWidth = readLEshort(pxm, 4);
	levelHeight = readLEshort(pxm, 6);

	delete[] levelMap;

	levelMap = new uint8_t[pxmSize - 8];
	memcpy(levelMap, pxm + 8, pxmSize - 8);

	//DONE WITH PXM
	delete[] pxm;

	//Load pxa
	const string pxaPath = string("data/Stage/") + stageTable[levelIndex].tileset + ".pxa";

	uint8_t *pxa = nullptr;
	const int pxaSize = loadFile(pxaPath.c_str(), &pxa);

	delete[] levelTileAttributes;

	levelTileAttributes = new uint8_t[pxaSize];
	memcpy(levelTileAttributes, pxa, pxaSize);

	//DONE WITH PXA
	delete[] pxa;

	//Load pxe
	const string pxePath = string("data/Stage/") + stageTable[levelIndex].filename + ".pxe";

	uint8_t *pxe = nullptr;
	loadFile(pxePath.c_str(), &pxe);

	//Clear old npcs
	npcs.clear();
	npcs.shrink_to_fit();

	//Load npcs
	const int npcAmount = readLElong(pxe, 4);

	for (int i = 0; i < npcAmount; i++)
	{
		const int offset = (i * 12) + 8;

		if (readLEshort(pxe, offset + 10) & npc_appearSet && !(getFlag(readLEshort(pxe, offset + 4))))
			continue;

		if (readLEshort(pxe, offset + 10) & npc_hideSet && getFlag(readLEshort(pxe, offset + 4)))
			continue;

		npc newNPC;
		newNPC.init(readLEshort(pxe, offset + 8), tilesToUnits(readLEshort(pxe, offset)), tilesToUnits(readLEshort(pxe, offset + 2)), 0, 0, 0, nullptr);

		newNPC.code_event = readLEshort(pxe, offset + 6);
		newNPC.code_flag = readLEshort(pxe, offset + 4);
		newNPC.bits |= readLEshort(pxe, offset + 10);

		if (readLEshort(pxe, offset + 10) & npc_altDir)
			newNPC.direct = dirRight;

		npcs.push_back(newNPC);
	}

	delete[] pxe;

	//Load tileset
	const string tileImagePath = string("data/Stage/Prt") + stageTable[levelIndex].tileset + ".png";

	loadImage(tileImagePath.c_str(), &sprites[0x02]);

	//Load background
	const string backgroundImagePath = string("data/") + stageTable[levelIndex].background + ".png";

	backgroundScroll = stageTable[levelIndex].backgroundScroll;
	loadImage(backgroundImagePath.c_str(), &sprites[0x1C]);

	//Load npc sheets
	//Load sheet 1
	const string npcSheet1Path = string("data/Npc/Npc") + stageTable[levelIndex].npc1 + ".png";

	loadImage(npcSheet1Path.c_str(), &sprites[0x15]);

	//Load sheet 2
	const string npcSheet2Path = string("data/Npc/Npc") + stageTable[levelIndex].npc2 + ".png";

	loadImage(npcSheet2Path.c_str(), &sprites[0x16]);

	//Load tsc script
	const string tscPath = string("data/Stage/") + stageTable[levelIndex].filename + ".tsc";

	loadStageTsc(tscPath.c_str());


	//Fix viewport
	viewport.x = currentPlayer.x - (screenWidth << 8);
	viewport.y = currentPlayer.y - (screenHeight << 8);
	viewport.lookX = &currentPlayer.tgt_x;
	viewport.lookY = &currentPlayer.tgt_y;
	viewport.quake = 0;
	viewport.quake2 = 0;

	viewBounds();
}

void drawLevel(bool foreground)
{
	if (!foreground)   //Draw background
	{
		RECT rect;

		int skyOff;

		int w, h;
		SDL_QueryTexture(sprites[0x1C], nullptr, nullptr, &w, &h);

		rect = { 0, 0, w, h };

		//Update background effect
		if (gameFlags & 1)
		{
			if (backgroundScroll == 5)
			{
				backgroundEffect += 0xC00;
			}

			else if (backgroundScroll >= 5 && backgroundScroll <= 7)
			{
				++backgroundEffect;
				backgroundEffect %= (w * 2);
			}
		}

		switch (backgroundScroll)
		{
		case 0:
			for (int x = 0; x < screenWidth; x += w)
			{
				for (int y = 0; y < screenHeight; y += h)
					drawTexture(sprites[0x1C], &rect, x, y);
			}

			break;

		case 1:
			for (int x = -(viewport.x / 0x400 % w); x < screenWidth; x += w)
			{
				for (int y = -(viewport.y / 0x400 % h); y < screenHeight; y += h)
					drawTexture(sprites[0x1C], &rect, x, y);
			}

			break;

		case 2:
			for (int x = -(viewport.x / 0x200 % w); x < screenWidth; x += w)
			{
				for (int y = -(viewport.y / 0x200 % h); y < screenHeight; y += h)
					drawTexture(sprites[0x1C], &rect, x, y);
			}

			break;

			case 5:
				for (int x = -(backgroundEffect / 0x200 % w); x < screenWidth; x += w)
				{
					for (int y = 0; y < screenHeight; y += h)
						drawTexture(sprites[0x1C], &rect, x, y);
				}

				break;

			case 6:
      case 7:
				//Draw sky
				rect = { 0, 0, w / 2, 88 };

			skyOff = (((w / 2) - screenWidth) / 2);

			//Draw middle
			drawTexture(sprites[0x1C], &rect, -skyOff, 0);

			//Repeat stars or whatever
			rect = { w / 2, 0, w, 88 };

			for (int i = 0; i < screenWidth - (skyOff / 2 + rect.left); i += rect.left)
			{
				drawTexture(sprites[0x1C], &rect, -skyOff + (rect.left + i), 0);
				drawTexture(sprites[0x1C], &rect, -skyOff - (rect.left + i), 0);
			}

			//Cloud layers
			rect.left = 0;
			rect.right = w;

			rect.top = 88;
			rect.bottom = 123;
			for (int i = 0; i <= (screenWidth / w) + 1; ++i)
				drawTexture(sprites[0x1C], &rect, (w * i) - (backgroundEffect / 2) % w, rect.top);

			rect.top = 123;
			rect.bottom = 146;
			for (int i = 0; i <= (screenWidth / w) + 1; ++i)
				drawTexture(sprites[0x1C], &rect, (w * i) - backgroundEffect % w, rect.top);

			rect.top = 146;
			rect.bottom = 176;
			for (int i = 0; i <= (screenWidth / w) + 1; ++i)
				drawTexture(sprites[0x1C], &rect, (w * i) - (backgroundEffect * 2) % w, rect.top);

			rect.top = 176;
			rect.bottom = 240;
			for (int i = 0; i <= (screenWidth / w) + 1; ++i)
				drawTexture(sprites[0x1C], &rect, (w * i) - (backgroundEffect * 4) % w, rect.top);

			break;

		default:
			break;
		}
	}

	//Animate currents
	if (gameFlags & 1)
	{
		if (foreground)
			currentEffect += 2;
	}

	//Render tiles
	RECT tileRect;

	const int xFrom = clamp(unitsToTiles(viewport.x + 0x1000), 0, levelWidth);
	const int xTo = clamp((unitsToTiles((viewport.x + 0x1000) + (screenWidth << 9))) + 1, 0, levelWidth); //add 1 because edge wouldn't appear

	const int yFrom = clamp(unitsToTiles(viewport.y + 0x1000), 0, levelHeight);
	const int yTo = clamp((unitsToTiles((viewport.y + 0x1000) + (screenWidth << 9))) + 1, 0, levelHeight); //add 1 because edge wouldn't appear

	for (int x = xFrom; x < xTo; x++)
	{
		for (int y = yFrom; y < yTo; y++)
		{
			const int i = x + y * levelWidth;

			const int tile = levelMap[i];
			if (tile)
			{
				const int attribute = levelTileAttributes[tile];

				if ((attribute < 0x20 && !foreground) || (attribute >= 0x40 && foreground))
				{
					const int drawX = i % levelWidth;
					const int drawY = i / levelWidth;

					if (attribute < 0x80)
					{
						tileRect = { (tile % 16) * 16, (tile / 16) * 16, (tile % 16) * 16 + 16, (tile / 16) * 16 + 16 };

						drawTexture(sprites[0x02], &tileRect, (drawX * 16) - viewport.x / 0x200 - 8, (drawY * 16) - viewport.y / 0x200 - 8);

						if (attribute == 0x43) //Star block
						{
							tileRect = { 256, 48, 272, 64 };

							drawTexture(sprites[0x14], &tileRect, (drawX * 16) - viewport.x / 0x200 - 8, (drawY * 16) - viewport.y / 0x200 - 8);
						}
					}
					else
					{
						switch (attribute)
						{
						case 0x80:
						case 0xA0:
							tileRect.left = (currentEffect & 0xF) + 224;
							tileRect.right = (currentEffect & 0xF) + 240;
							tileRect.top = 48;
							tileRect.bottom = 64;
							break;

						case 0x81:
						case 0xA1:
							tileRect.left = 224;
							tileRect.right = 240;
							tileRect.top = (currentEffect & 0xF) + 48;
							tileRect.bottom = (currentEffect & 0xF) + 64;
							break;

						case 0x82:
						case 0xA2:
							tileRect.left = 240 - (currentEffect & 0xF);
							tileRect.right = 256 - (currentEffect & 0xF);
							tileRect.top = 48;
							tileRect.bottom = 64;
							break;

						case 0x83:
						case 0xA3:
							tileRect.left = 224;
							tileRect.right = 240;
							tileRect.top = 64 - (currentEffect & 0xF);
							tileRect.bottom = 80 - (currentEffect & 0xF);
							break;

						default:
							return;
						}

						drawTexture(sprites[0x13], &tileRect, (drawX * 16) - viewport.x / 0x200 - 8, (drawY * 16) - viewport.y / 0x200 - 8);
					}
				}
			}
		}
	}

	//Render black bars in foreground
	if (foreground)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);

		//Left and right
		const int leftBorder = -(viewport.x / 0x200);
		const int rightBorder = ((viewport.x / 0x200) + screenWidth) - ((levelWidth - 1) << 4);

		drawRect(0, 0, leftBorder, screenHeight);
		drawRect(screenWidth - rightBorder, 0, rightBorder, screenHeight);

		//Top and bottom
		const int topBorder = -(viewport.y / 0x200);
		const int bottomBorder = ((viewport.y / 0x200) + screenHeight) - ((levelHeight - 1) << 4);

		drawRect(0, 0, screenWidth, topBorder);
		drawRect(0, screenHeight - bottomBorder, screenWidth, bottomBorder);
	}
}
