#include "level.h"

int levelWidth;
int levelHeight;

BYTE *levelMap = nullptr;
BYTE *levelTileAttributes = nullptr;

BYTE backgroundScroll;

//Effect things
int backgroundEffect = 0;
int currentEffect = 0;

//Get stage data
BYTE *stageData = nullptr;
char stblPath[256] = "data/stage.tbl";

int stblSize = loadFile(stblPath, &stageData);

BYTE getTileAttribute(int x, int y) {
	if (x >= 0 && x < levelWidth && y >= 0 && y < levelHeight)
		return levelTileAttributes[levelMap[(x + y * levelWidth)]];

	return 0;
}

void loadLevel(int levelIndex) {
	if (stblSize < 0)
		doCustomError("stage.tbl failed to load");

	//Get stage's stage.tbl offset
	int tbloffset = levelIndex * 0xE5;

	//Get tileset filename
	char* tilesetFilename = (char*)&stageData[tbloffset];

	//Get filename
	char* levelFilename = (char*)&stageData[tbloffset+0x20];

	//Get background stuff
	backgroundScroll = readLElong(stageData, tbloffset + 0x40);

	char* backgroundFilename = (char*)&stageData[tbloffset + 0x44];

	//Npc sprite sheets
	char* npcSheet1 = (char*)&stageData[tbloffset + 0x64];
	char* npcSheet2 = (char*)&stageData[tbloffset + 0x84];

	//Load pxm
	char pxmPath[256];
	snprintf(pxmPath, 256, "data/Stage/%s.pxm", levelFilename);

	BYTE *pxm = nullptr;
	int pxmSize = loadFile(pxmPath, &pxm);
	if (pxmSize < 0 ) { doCustomError("Couldn't read pxm."); doCustomError(pxmPath); }
	
	levelWidth = readLEshort(pxm, 4);
	levelHeight = readLEshort(pxm, 6);

	if (levelMap)
		delete levelMap;

	levelMap = new BYTE[pxmSize-8];

	for (int i = 8; i < pxmSize; i++) {
		levelMap[i - 8] = pxm[i];
	}

	//DONE WITH PXM
	free(pxm);

	//Load pxa
	char pxaPath[256];
	snprintf(pxaPath, 256, "data/Stage/%s.pxa", tilesetFilename);

	BYTE *pxa = nullptr;
	int pxaSize = loadFile(pxaPath, &pxa);
	if (pxaSize < 0) { doCustomError("Couldn't read pxa."); doCustomError(pxaPath); }

	if (levelTileAttributes)
		delete levelTileAttributes;
	levelTileAttributes = new BYTE[pxaSize];
	
	for (int i = 0; i < pxaSize; i++) {
		levelTileAttributes[i] = pxa[i];
	}

	//DONE WITH PXA
	free(pxa);

	//Load pxe
	char pxePath[256];
	snprintf(pxePath, 256, "data/Stage/%s.pxe", levelFilename);

	BYTE *pxe = nullptr;
	int pxeSize = loadFile(pxePath, &pxe);
	if (pxeSize < 0) { doCustomError("Couldn't read pxe."); doCustomError(pxePath); }

	//Clear old npcs
	npcs.clear();
	npcs.shrink_to_fit();

	//Clear old carets
	carets.clear();
	carets.shrink_to_fit();

	//Load npcs
	int entityCount = readLElong(pxe, 4);

	for (int i = 0; i < entityCount; i++) {
		int offset = (i * 12) + 8;

		uint16_t x = readLEshort(pxe, offset);
		uint16_t y = readLEshort(pxe, offset + 2);
		uint16_t flag = readLEshort(pxe, offset + 4);
		uint16_t event = readLEshort(pxe, offset + 6);
		uint16_t type = readLEshort(pxe, offset + 8);
		uint16_t flags = readLEshort(pxe, offset + 10);

		bool create = true;

		if (flags & npc_appearset) {
			if (getFlag(flag) == false) { create = false; }
		}

		if (flags & npc_hideset) {
			if (getFlag(flag) == true) { create = false; }
		}

		if (create == true)
		{
			npc newNPC;
			newNPC.init(type, x << 13, y << 13, 0, 0, 0, nullptr);

			newNPC.code_event = event;
			newNPC.bits |= flags;

			if (flags & npc_altdir)
				newNPC.direct = 2;

			npcs.push_back(newNPC);
		}
	}

	//Load tileset
	char tileImagePath[256];
	snprintf(tileImagePath, 256, "data/Stage/Prt%s.png", tilesetFilename);

	loadImage(tileImagePath, &sprites[0x02]);

	//Load background
	char bgImagePath[256];
	snprintf(bgImagePath, 256, "data/%s.png", backgroundFilename);

	loadImage(bgImagePath, &sprites[0x1C]);

	//Load npc sheets
	//Load sheet 1
	char npcSheet1Path[256];
	snprintf(npcSheet1Path, 256, "data/Npc/Npc%s.png", npcSheet1);

	loadImage(npcSheet1Path, &sprites[0x15]);

	//Load sheet 2
	char npcSheet2Path[256];
	snprintf(npcSheet2Path, 256, "data/Npc/Npc%s.png", npcSheet2);
	strcpy(npcSheet2Path, "data/Npc/Npc");
	strcat(npcSheet2Path, npcSheet2);
	strcat(npcSheet2Path, ".png");

	loadImage(npcSheet2Path, &sprites[0x16]);

	//Load tsc script
	char tscPath[256];
	snprintf(tscPath, 256, "data/Stage/%s.tsc", levelFilename);

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
						drawTextureFromRect(sprites[0x1C], &rect, x, y, true);
				}

				break;

			case 1:
				for (int x = -(viewX / 0x400 % w); x < screenWidth; x += w)
				{
					for (int y = -(viewY / 0x400 % h); y < screenHeight; y += h)
						drawTextureFromRect(sprites[0x1C], &rect, x << 9, y << 9, true);
				}

				break;

			case 2:
				for (int x = -(viewX / 0x200 % w); x < screenWidth; x += w)
				{
					for (int y = -(viewY / 0x200 % h); y < screenHeight; y += h)
						drawTextureFromRect(sprites[0x1C], &rect, x << 9, y << 9, true);
				}

				break;

			case 6: case 7:
				//Draw sky
				rect = { 0, 0, w / 2, 88 };

				skyOff = ((((w / 2) - screenWidth) / 2) * 0x200);
				
				//Draw middle
				drawTextureFromRect(sprites[0x1C], &rect, -skyOff, 0, true);

				//Repeat stars or whatever
				rect = { w / 2, 0, w, 88 };

				for (int i = 0; i < screenWidth - (skyOff / 2 + rect.left); i += rect.left)
				{
					drawTextureFromRect(sprites[0x1C], &rect, -skyOff + ((rect.left + i) * 0x200), 0, true);
					drawTextureFromRect(sprites[0x1C], &rect, -skyOff - ((rect.left + i) * 0x200), 0, true);
				}

				//Cloud layers
				rect.left = 0;
				rect.right = w;

				rect.top = 88;
				rect.bottom = 123;
				for (int i = 0; i <= (screenWidth / w) + 1; ++i)
					drawTextureFromRect(sprites[0x1C], &rect, ((w * i) - ((backgroundEffect / 2) % w)) << 9, rect.top << 9, true);

				rect.top = 123;
				rect.bottom = 146;
				for (int i = 0; i <= (screenWidth / w) + 1; ++i)
					drawTextureFromRect(sprites[0x1C], &rect, ((w * i) - (backgroundEffect % w)) << 9, rect.top << 9, true);

				rect.top = 146;
				rect.bottom = 176;
				for (int i = 0; i <= (screenWidth / w) + 1; ++i)
					drawTextureFromRect(sprites[0x1C], &rect, ((w * i) - ((backgroundEffect * 2) % w)) << 9, rect.top << 9, true);

				rect.top = 176;
				rect.bottom = 240;
				for (int i = 0; i <= (screenWidth / w) + 1; ++i)
					drawTextureFromRect(sprites[0x1C], &rect, ((w * i) - ((backgroundEffect * 4) % w)) << 9, rect.top << 9, true);

				break;

			default:
				break;
		}
	}

	//Animate currents
	RECT currentRect = { 0 };

	if (foreground)
		currentEffect += 2;

	//Render tiles
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

					if (attribute == 0x43)
					{
						ImageRect = { 256, 48, 16, 16 };

						drawTexture(sprites[0x14], (drawX << 13) - 0x1000, (drawY << 13) - 0x1000, false);
					}
					else
					{
						if (attribute < 0x80)
						{
							ImageRect = { (tile % 16) * 16, (tile / 16) * 16, 16, 16 };

							drawTexture(sprites[0x02], (drawX << 13) - 0x1000, (drawY << 13) - 0x1000, false);
						}
						else
						{
							bool draw = true;

							switch (attribute)
							{
							case 128: case 160:
								currentRect.left = (currentEffect & 0xF) + 224;
								currentRect.right = (currentEffect & 0xF) + 240;
								currentRect.top = 48;
								currentRect.bottom = 64;
								break;

							case 129: case 161:
								currentRect.left = 224;
								currentRect.right = 240;
								currentRect.top = (currentEffect & 0xF) + 48;
								currentRect.bottom = (currentEffect & 0xF) + 64;
								break;

							case 130: case 162:
								currentRect.left = 240 - (currentEffect & 0xF);
								currentRect.right = currentRect.left + 16;
								currentRect.top = 48;
								currentRect.bottom = 64;
								break;

							case 131: case 163:
								currentRect.left = 224;
								currentRect.right = 240;
								currentRect.top = 64 - (currentEffect & 0xF);
								currentRect.bottom = currentRect.top + 16;
								break;

							default:
								draw = false;
								break;
							}

							if (draw)
								drawTextureFromRect(sprites[0x13], &currentRect, (drawX << 13) - 0x1000, (drawY << 13) - 0x1000, false);
						}
					}
				}
			}
		}
	}

	//Render black bars in foreground
	if (foreground)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		if (viewX < 0) //Typically won't be out of bounds unless centred to stage
		{
			drawRect(0, 0, -viewX, screenHeight << 9);
			drawRect((screenWidth << 9) + viewX, 0, -viewX, screenHeight << 9);
		}

		if (viewY < 0)
		{
			drawRect(0, 0, screenWidth << 9, -viewY);
			drawRect(0, (screenHeight << 9) + viewY, screenWidth << 9, -viewY);
		}
	}
}