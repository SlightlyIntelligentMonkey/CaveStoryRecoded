#include "level.h"

int levelWidth;
int levelHeight;

BYTE *levelMap;
BYTE *levelTileAttributes;

//SDL_Texture *levelTileset;

BYTE backgroundScroll;
//SDL_Texture *levelBackground;

std::vector<npc> npcs(0);

SDL_Rect blackBack;

//NPC Functions
void updateNPC()
{
	//Remove stuff
	for (unsigned int i = npcs.size() - 1; i > 0; i--) {
		if (!(npcs[i].cond & npccond_alive))
			npcs.erase(npcs.begin() + i);
	}

	//Update
	for (unsigned int i = 0; i < npcs.size(); i++) {
		npcs[i].update();
	}
}

void drawNPC()
{
	//Draw
	for (unsigned int i = 0; i < npcs.size(); i++) {
		npcs[i].draw();
	}
}

//Get stage data
BYTE *stageData = nullptr;
char stblPath[256] = "data/stage.tbl";

int stblSize = loadFile(stblPath, &stageData);

BYTE getTileAttribute(int x, int y) {
	return levelTileAttributes[levelMap[(x + y * levelWidth)]];
}

void createNpc(int x, int y, int direction, int xsp, int ysp, short flag, short event, short type, short flags)
{
	npc newNPC;
	newNPC.init(x, y, flag, event, type, flags);

	newNPC.direction = direction;
	newNPC.xsp = xsp;
	newNPC.ysp = ysp;

	npcs.push_back(newNPC);
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
	backgroundScroll = stageData[tbloffset + 0x40];

	char* backgroundFilename = (char*)&stageData[tbloffset + 0x44];

	//Npc sprite sheets
	char* npcSheet1 = (char*)&stageData[tbloffset + 0x64];
	char* npcSheet2 = (char*)&stageData[tbloffset + 0x84];

	//Load pxm
	char pxmPath[256];
	strcpy(pxmPath,"data/Stage/");
	strcat(pxmPath, levelFilename);
	strcat(pxmPath, ".pxm");

	BYTE *pxm = nullptr;
	int pxmSize = loadFile(pxmPath, &pxm);
	if (pxmSize < 0 ) { doCustomError("Couldn't read pxm."); doCustomError(pxmPath); }
	
	short *nums = (short*)pxm;

	levelWidth = (int)nums[2];
	levelHeight = (int)nums[3];

	levelMap = new BYTE[pxmSize-8];

	for (int i = 8; i < pxmSize; i++) {
		levelMap[i - 8] = pxm[i];
	}

	//DONE WITH PXM
	free(pxm);

	//Load pxa
	char pxaPath[256];
	strcpy(pxaPath, "data/Stage/");
	strcat(pxaPath, tilesetFilename);
	strcat(pxaPath, ".pxa");

	BYTE *pxa = nullptr;
	int pxaSize = loadFile(pxaPath, &pxa);
	if (pxaSize < 0) { doCustomError("Couldn't read pxa."); doCustomError(pxaPath); }

	levelTileAttributes = new BYTE[pxaSize];
	
	for (int i = 0; i < pxaSize; i++) {
		levelTileAttributes[i] = pxa[i];
	}

	//DONE WITH PXA
	free(pxa);

	//Load pxe
	char pxePath[256];
	strcpy(pxePath, "data/Stage/");
	strcat(pxePath, levelFilename);
	strcat(pxePath, ".pxe");

	BYTE *pxe = nullptr;
	int pxeSize = loadFile(pxePath, &pxe);
	if (pxeSize < 0) { doCustomError("Couldn't read pxe."); doCustomError(pxePath); }

	//Clear old npcs
	npcs.clear();
	npcs.shrink_to_fit();

	//Load npcs
	int entityCount = (int)pxe[4];

	for (int i = 0; i < entityCount; i++) {
		int offset = (i * 12) + 8;

		short x = readLEshort(pxe, offset);
		short y = readLEshort(pxe, offset + 2);
		short flag = readLEshort(pxe, offset + 4);
		short event = readLEshort(pxe, offset + 6);
		short type = readLEshort(pxe, offset + 8);
		short flags = readLEshort(pxe, offset + 10);

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
			newNPC.init(x << 13, y << 13, flag, event, type, flags);
			npcs.push_back(newNPC);
		}
	}

	//Load tileset
	char tileImagePath[256];
	strcpy(tileImagePath, "data/Stage/Prt");
	strcat(tileImagePath, tilesetFilename);
	strcat(tileImagePath, ".bmp");

	loadBMP(tileImagePath, &sprites[0x02]);

	//Load background
	char bgImagePath[256];
	strcpy(bgImagePath, "data/");
	strcat(bgImagePath, backgroundFilename);
	strcat(bgImagePath, ".bmp");

	loadBMP(bgImagePath, &sprites[0x1C]);

	//Load npc sheets
	//Load sheet 1
	char npcSheet1Path[256];
	strcpy(npcSheet1Path, "data/Npc/Npc");
	strcat(npcSheet1Path, npcSheet1);
	strcat(npcSheet1Path, ".bmp");

	loadBMP(npcSheet1Path, &sprites[0x15]);

	//Load sheet 2
	char npcSheet2Path[256];
	strcpy(npcSheet2Path, "data/Npc/Npc");
	strcat(npcSheet2Path, npcSheet2);
	strcat(npcSheet2Path, ".bmp");

	loadBMP(npcSheet2Path, &sprites[0x16]);

	//Load tsc script
	char tscPath[256];
	strcpy(tscPath, "data/Stage/");
	strcat(tscPath, levelFilename);
	strcat(tscPath, ".tsc");

	loadTsc(tscPath);

	//END
	return;
}

void drawLevel(bool foreground) {
	if (foreground == false) { //Draw background
		int xMove = 0;
		int yMove = 0;

		switch (backgroundScroll)
		{
			case(0): case(1): case(2):
				int w, h;
				SDL_QueryTexture(sprites[0x1C], NULL, NULL, &w, &h);
				
				if (backgroundScroll == 1) { xMove = (viewX >> 1) % (w << 9); yMove = (viewY >> 1) % (h << 9); }
				if (backgroundScroll == 2) { xMove = viewX % (w << 9); yMove = viewY % (h << 9); }

				ImageRect.x = 0;
				ImageRect.y = 0;
				ImageRect.w = w;
				ImageRect.h = h;

				for (int x = 0; x <= (screenWidth / w + 1); x++) {
					for (int y = 0; y <= (screenHeight / h + 1); y++) {
						drawTexture(sprites[0x1C], (x * w << 9) - xMove, (y * h << 9) - yMove, true);
					}
				}
				break;

			case(4):
				SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);

				blackBack.x = 0;
				blackBack.y = 0;
				blackBack.w = screenWidth;
				blackBack.h = screenHeight;

				SDL_RenderFillRect(renderer, &blackBack);
				break;
		}
	}

	//Render tiles
	int xFrom = (viewX + 0x1000) >> 13;
	int xTo = (((viewX + 0x1000) + (screenWidth << 9)) >> 13) + 1; //add 1 because edge wouldn't appear

	int yFrom = (viewY + 0x1000) >> 13;
	int yTo = (((viewY + 0x1000) + (screenHeight << 9)) >> 13) + 1; //add 1 because edge wouldn't appear

	for (int x = xFrom; x < xTo; x++) {
		for (int y = yFrom; y < yTo; y++) {
			int i = x + y * levelWidth;

			if (levelMap[i] != NULL) {
				int tile = levelMap[i];
				int attribute = levelTileAttributes[tile];

				if ((attribute < 0x20 && foreground == false) || (attribute >= 0x40 && foreground == true))
				{
					int drawX = i % levelWidth;
					int drawY = i / levelWidth;

					switch (attribute)
					{
						case(0x43):
							ImageRect.x = 256;
							ImageRect.y = 48;
							ImageRect.w = 16;
							ImageRect.h = 16;

							drawTexture(sprites[0x14], (drawX << 13) - 0x1000, (drawY << 13) - 0x1000, false);

							continue;

						default:
							ImageRect.x = (tile % 16) * 16;
							ImageRect.y = (tile / 16) * 16;
							ImageRect.w = 16;
							ImageRect.h = 16;

							drawTexture(sprites[0x02], (drawX << 13) - 0x1000, (drawY << 13) - 0x1000, false);

							continue;
					}
				}
			}
		}
	}
}