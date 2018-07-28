#include "level.h"

int currentLevel;

//Loaded level stuff
int levelWidth;
int levelHeight;

BYTE *levelMap = nullptr;
BYTE *levelTileAttributes = nullptr;

BYTE backgroundScroll;

//Effect things
int backgroundEffect = 0;
int currentEffect = 0;

//Get stage data
STAGE_TABLE *stageTable;

void loadStageTable()
{
	SDL_RWops *tblStream = SDL_RWFromFile("data/stage.tbl", "rb");

	int stages = SDL_RWsize(tblStream) / 200;

	stageTable = (STAGE_TABLE *)malloc(stages * 200);

	if (tblStream)
	{
		for (int i = 0; i < stages; i++)
		{
			tblStream->read(tblStream, stageTable[i].tileset, 0x20, 1);
			tblStream->read(tblStream, stageTable[i].filename, 0x20, 1);
			stageTable[i].backgroundScroll = SDL_ReadLE32(tblStream);
			tblStream->read(tblStream, stageTable[i].background, 0x20, 1);
			tblStream->read(tblStream, stageTable[i].npc1, 0x20, 1);
			tblStream->read(tblStream, stageTable[i].npc2, 0x20, 1);
			tblStream->read(tblStream, &stageTable[i].boss, 1, 1);
			tblStream->read(tblStream, stageTable[i].name, 0x23, 1);
		}
	}
	else
		doError();
}

BYTE getTileAttribute(int x, int y) {
	if (x >= 0 && x < levelWidth && y >= 0 && y < levelHeight)
		return levelTileAttributes[levelMap[(x + y * levelWidth)]];

	return 0;
}

void loadLevel(int levelIndex) {
	currentLevel = levelIndex;

	backgroundScroll = stageTable[levelIndex].backgroundScroll;

	//Load pxm
	char pxmPath[256];
	snprintf(pxmPath, 256, "data/Stage/%s.pxm", stageTable[levelIndex].filename);

	BYTE *pxm = nullptr;
	int pxmSize = loadFile(pxmPath, &pxm);

	if (pxmSize < 0)
	{
		char errorMsg[256];
		snprintf(errorMsg, 256, "Couldn't read %s", pxmPath);

		doCustomError(errorMsg);
	}
	
	levelWidth = readLEshort(pxm, 4);
	levelHeight = readLEshort(pxm, 6);

	if (levelMap)
		delete levelMap;

	levelMap = new BYTE[pxmSize - 8];
	memcpy(levelMap, pxm + 8, pxmSize - 8);

	//DONE WITH PXM
	free(pxm);

	//Load pxa
	char pxaPath[256];
	snprintf(pxaPath, 256, "data/Stage/%s.pxa", stageTable[levelIndex].tileset);

	BYTE *pxa = nullptr;
	int pxaSize = loadFile(pxaPath, &pxa);

	if (pxaSize < 0)
	{
		char errorMsg[256];
		snprintf(errorMsg, 256, "Couldn't read %s", pxaPath);

		doCustomError(errorMsg);
	}

	if (levelTileAttributes)
		delete levelTileAttributes;

	levelTileAttributes = new BYTE[pxaSize];
	memcpy(levelTileAttributes, pxa, pxaSize);

	//DONE WITH PXA
	free(pxa);

	//Clear old carets
	carets.clear();
	carets.shrink_to_fit();

	//Load pxe
	char pxePath[256];
	snprintf(pxePath, 256, "data/Stage/%s.pxe", stageTable[levelIndex].filename);

	BYTE *pxe = nullptr;
	if (loadFile(pxePath, &pxe) < 0)
	{
		char errorMsg[256];
		snprintf(errorMsg, 256, "Couldn't read %s", pxePath);

		doCustomError(errorMsg);
	}

	//Clear old npcs
	npcs.clear();
	npcs.shrink_to_fit();

	//Load npcs
	int npcAmount = readLElong(pxe, 4);

	for (int i = 0; i < npcAmount; i++) {
		int offset = (i * 12) + 8;

		if (readLEshort(pxe, offset + 10) & npc_appearset && getFlag(readLEshort(pxe, offset + 4)) == false)
			continue;

		if (readLEshort(pxe, offset + 10) & npc_hideset && getFlag(readLEshort(pxe, offset + 4)) == true)
			continue;

		npc newNPC;
		newNPC.init(readLEshort(pxe, offset + 8), readLEshort(pxe, offset) << 13, readLEshort(pxe, offset + 2) << 13, 0, 0, 0, nullptr);

		newNPC.code_event = readLEshort(pxe, offset + 6);
		newNPC.code_flag = readLEshort(pxe, offset + 4);
		newNPC.bits |= readLEshort(pxe, offset + 10);

		if (readLEshort(pxe, offset + 10) & npc_altdir)
			newNPC.direct = 2;

		npcs.push_back(newNPC);
	}

	free(pxe);

	//Load tileset
	char tileImagePath[256];
	snprintf(tileImagePath, 256, "data/Stage/Prt%s.png", stageTable[levelIndex].tileset);

	loadImage(tileImagePath, &sprites[0x02]);

	//Load background
	char bgImagePath[256];
	snprintf(bgImagePath, 256, "data/%s.png", stageTable[levelIndex].background);

	loadImage(bgImagePath, &sprites[0x1C]);

	//Load npc sheets
	//Load sheet 1
	char npcSheet1Path[256];
	snprintf(npcSheet1Path, 256, "data/Npc/Npc%s.png", stageTable[levelIndex].npc1);

	loadImage(npcSheet1Path, &sprites[0x15]);

	//Load sheet 2
	char npcSheet2Path[256];
	snprintf(npcSheet2Path, 256, "data/Npc/Npc%s.png", stageTable[levelIndex].npc2);

	loadImage(npcSheet2Path, &sprites[0x16]);

	//Load tsc script
	char tscPath[256];
	snprintf(tscPath, 256, "data/Stage/%s.tsc", stageTable[levelIndex].filename);

	loadTsc(tscPath);

	//END
	return;
}

void drawLevel(bool foreground)
{
	if (!foreground) { //Draw background
		RECT rect;

		int skyOff;

		int w, h;
		SDL_QueryTexture(sprites[0x1C], NULL, NULL, &w, &h);

		rect = { 0, 0, w, h };

		//Update background effect
		if (backgroundScroll == 5)
		{
			backgroundEffect += 0xC00;
		}

		else if (backgroundScroll >= 5 && backgroundScroll <= 7)
		{
			++backgroundEffect;
			backgroundEffect %= (w * 2);
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
				for (int x = -(viewX / 0x400 % w); x < screenWidth; x += w)
				{
					for (int y = -(viewY / 0x400 % h); y < screenHeight; y += h)
						drawTexture(sprites[0x1C], &rect, x, y);
				}

				break;

			case 2:
				for (int x = -(viewX / 0x200 % w); x < screenWidth; x += w)
				{
					for (int y = -(viewY / 0x200 % h); y < screenHeight; y += h)
						drawTexture(sprites[0x1C], &rect, x, y);
				}

				break;

			case 6: case 7:
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
	if (foreground)
		currentEffect += 2;

	//Render tiles
	RECT tileRect;

	int xFrom = clamp((viewX + 0x1000) >> 13, 0, levelWidth);
	int xTo = clamp((((viewX + 0x1000) + (screenWidth << 9)) >> 13) + 1, 0, levelWidth); //add 1 because edge wouldn't appear

	int yFrom = clamp((viewY + 0x1000) >> 13, 0, levelHeight);
	int yTo = clamp((((viewY + 0x1000) + (screenHeight << 9)) >> 13) + 1, 0, levelHeight); //add 1 because edge wouldn't appear

	for (int x = xFrom; x < xTo; x++) {
		for (int y = yFrom; y < yTo; y++) {
			int i = x + y * levelWidth;

			int tile = levelMap[i];
			if (tile) {
				int attribute = levelTileAttributes[tile];

				if ((attribute < 0x20 && foreground == false) || (attribute >= 0x40 && foreground == true))
				{
					int drawX = i % levelWidth;
					int drawY = i / levelWidth;

					if (attribute < 0x80)
					{
						tileRect = { (tile % 16) * 16, (tile / 16) * 16, (tile % 16) * 16 + 16, (tile / 16) * 16 + 16 };

						drawTexture(sprites[0x02], &tileRect, (drawX * 16) - viewX / 0x200 - 8, (drawY * 16) - viewY / 0x200 - 8);

						if (attribute == 0x43) //Star block
						{
							tileRect = { 256, 48, 272, 64 };

							drawTexture(sprites[0x14], &tileRect, (drawX * 16) - viewX / 0x200 - 8, (drawY * 16) - viewY / 0x200 - 8);
						}
					}
					else
					{
						switch (attribute)
						{
						case 0x80: case 0xA0:
							tileRect.left = (currentEffect & 0xF) + 224;
							tileRect.right = (currentEffect & 0xF) + 240;
							tileRect.top = 48;
							tileRect.bottom = 64;
							break;

						case 0x81: case 0xA1:
							tileRect.left = 224;
							tileRect.right = 240;
							tileRect.top = (currentEffect & 0xF) + 48;
							tileRect.bottom = (currentEffect & 0xF) + 64;
							break;

						case 0x82: case 0xA2:
							tileRect.left = 240 - (currentEffect & 0xF);
							tileRect.right = 256 - (currentEffect & 0xF);
							tileRect.top = 48;
							tileRect.bottom = 64;
							break;

						case 0x83: case 0xA3:
							tileRect.left = 224;
							tileRect.right = 240;
							tileRect.top = 64 - (currentEffect & 0xF);
							tileRect.bottom = 80 - (currentEffect & 0xF);
							break;

						default:
							return;
						}

						drawTexture(sprites[0x13], &tileRect, (drawX * 16) - viewX / 0x200 - 8, (drawY * 16) - viewY / 0x200 - 8);
					}
				}
			}
		}
	}

	//Render black bars in foreground
	if (foreground)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);

		if (viewX < 0)
		{
			drawRect(0, 0, viewX / -0x200, screenHeight);
			drawRect(screenWidth + viewX / 0x200, 0, viewX / -0x200, screenHeight);
		}

		if (viewY < 0)
		{
			drawRect(0, 0, screenWidth, viewY / -0x200);
			drawRect(0, screenHeight + viewY / 0x200, screenWidth, viewY / -0x200);
		}
	}
}