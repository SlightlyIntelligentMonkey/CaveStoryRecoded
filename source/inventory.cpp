#include "game.h"

#include <string>
#include <cstring>
#include <SDL_events.h>
#include <SDL_render.h>
#include "weapons.h"
#include "input.h"
#include "script.h"
#include "sound.h"
#include "render.h"
#include "stage.h"
#include "player.h"
#include "caret.h"
#include "valueview.h"
#include "log.h"

//Inventory
ITEM gItems[ITEMS];

int gInventoryTitleY;
bool gInventoryActive;
int gSelectedItem;

static void doInventoryActive(bool &bChange, int itemNo)
{
	if (isKeyPressed(gKeyLeft))
	{
		if (gSelectedItem % 6)
			--gSelectedItem;
		else
			gSelectedItem += 5;

		bChange = true;
	}

	if (isKeyPressed(gKeyRight))
	{
		if (itemNo - 1 == gSelectedItem)
			gSelectedItem = 6 * (gSelectedItem / 6);
		else if (gSelectedItem % 6 == 5)
			gSelectedItem -= 5;
		else
			++gSelectedItem;

		bChange = true;
	}

	if (isKeyPressed(gKeyUp))
	{
		if (gSelectedItem + 5 > 10)
			gSelectedItem -= 6;
		else
			gInventoryActive = false;

		bChange = true;
	}

	if (isKeyPressed(gKeyDown))
	{
		if (gSelectedItem / 6 == (itemNo - 1) / 6)
			gInventoryActive = false;
		else
			gSelectedItem += 6;

		bChange = true;
	}

	if (gSelectedItem >= itemNo)
		gSelectedItem = itemNo - 1;

	if (gInventoryActive && isKeyPressed(gKeyJump))
		startTscEvent(tsc, gItems[gSelectedItem].code + 6000);
}

static void doInventoryNotActive(bool &bChange, int itemNo, int weaponNo)
{
	if (isKeyPressed(gKeyLeft))
	{
		--gSelectedWeapon;
		bChange = true;
	}
	if (isKeyPressed(gKeyRight))
	{
		++gSelectedWeapon;
		bChange = true;
	}
	if (isKeyPressed(gKeyUp) || isKeyPressed(gKeyDown))
	{
		if (itemNo)
			gInventoryActive = true;
		bChange = true;
	}

	if (gSelectedWeapon < 0)
		gSelectedWeapon = weaponNo - 1;

	if (gSelectedWeapon > weaponNo - 1)
		gSelectedWeapon = 0;
}

void moveInventoryCursor()
{
	int weaponNo = 0;
	int itemNo = 0;
	while (weaponNo < static_cast<int>(WEAPONS) && gWeapons[weaponNo].code != 0)
		++weaponNo;
	while (weaponNo < static_cast<int>(ITEMS) && gItems[itemNo].code != 0)
		++itemNo;

	if (weaponNo || itemNo)
	{
		bool bChange = false;
		if (gInventoryActive)
			doInventoryActive(bChange, itemNo);
		else
			doInventoryNotActive(bChange, itemNo, weaponNo);

		if (bChange)
		{
			if (gInventoryActive)
			{
				playSound(1);

				if (itemNo)
					startTscEvent(tsc, gItems[gSelectedItem].code + 5000);
				else
					startTscEvent(tsc, 5000);
			}
			else
			{
				playSound(4);

				if (weaponNo)
					startTscEvent(tsc, gWeapons[gSelectedWeapon].code + 1000);
				else
					startTscEvent(tsc, 1000);
			}
		}
	}
}

void drawInventory()
{
	//Draw main box
	RECT rcBoxTop = { 0, 0, 244, 8 };
	RECT rcBoxBody = { 0, 8, 244, 16 };
	RECT rcBoxBottom = { 0, 16, 244, 24 };

	drawTexture(gSprites[TEX_TEXTBOX], &rcBoxTop, gScreenWidth / 2 - 122, ((gScreenHeight - 240) / 2) + 8);

	int stripY;
	for (stripY = 1; stripY < 18; ++stripY)
		drawTexture(gSprites[TEX_TEXTBOX], &rcBoxBody, gScreenWidth / 2 - 122, ((gScreenHeight - 240) / 2) + 8 * (stripY + 1));

	drawTexture(gSprites[TEX_TEXTBOX], &rcBoxBottom, gScreenWidth / 2 - 122, ((gScreenHeight - 240) / 2) + 8 * (stripY + 1));

	//Draw title text
	RECT rcTitle1 = { 80, 48, 144, 56 };
	RECT rcTitle2 = { 80, 56, 144, 64 };

	if (gInventoryTitleY > 16)
		--gInventoryTitleY;

	drawTexture(gSprites[TEX_TEXTBOX], &rcTitle1, gScreenWidth / 2 - 112, ((gScreenHeight - 240) / 2) + gInventoryTitleY);
	drawTexture(gSprites[TEX_TEXTBOX], &rcTitle2, gScreenWidth / 2 - 112, ((gScreenHeight - 240) / 2) + gInventoryTitleY + 52);

	//Draw weapon selection box
	RECT rcCur1[2];
	rcCur1[0] = { 0, 88, 40, 128 };
	rcCur1[1] = { 40, 88, 80, 128 };

	static int inventoryFlash;
	++inventoryFlash;
	if (gInventoryActive)
		drawTexture(gSprites[TEX_TEXTBOX], &rcCur1[1], 40 * gSelectedWeapon + gScreenWidth / 2 - 112, ((gScreenHeight - 240) / 2) + 24);
	else
		drawTexture(gSprites[TEX_TEXTBOX], &rcCur1[(inventoryFlash >> 1) & 1], 40 * gSelectedWeapon + gScreenWidth / 2 - 112, ((gScreenHeight - 240) / 2) + 24);

	//Draw weapons
	RECT rcLv = { 80, 80, 96, 88 };
	RECT rcNone = { 80, 48, 96, 56 };
	RECT rcPer = { 72, 48, 80, 56 };
	RECT rcArms;

	for (size_t i = 0; i < WEAPONS && gWeapons[i].code; ++i)
	{
		rcArms.left = 16 * (gWeapons[i].code % 16);
		rcArms.right = rcArms.left + 16;
		rcArms.top = 16 * (gWeapons[i].code / 16);
		rcArms.bottom = rcArms.top + 16;

		drawTexture(gSprites[TEX_ARMSIMAGE], &rcArms, 40 * i + gScreenWidth / 2 - 112, ((gScreenHeight - 240) / 2) + 24);
		drawTexture(gSprites[TEX_TEXTBOX], &rcPer, 40 * i + gScreenWidth / 2 - 112, ((gScreenHeight - 240) / 2) + 56);
		drawTexture(gSprites[TEX_TEXTBOX], &rcLv, 40 * i + gScreenWidth / 2 - 112, ((gScreenHeight - 240) / 2) + 40);

		drawNumber(gWeapons[i].level, 40 * i + gScreenWidth / 2 - 112, ((gScreenHeight - 240) / 2) + 40, false);

		if (gWeapons[i].max_num)
		{
			drawNumber(gWeapons[i].num, 40 * i + gScreenWidth / 2 - 112, ((gScreenHeight - 240) / 2) + 48, false);
			drawNumber(gWeapons[i].max_num, 40 * i + gScreenWidth / 2 - 112, ((gScreenHeight - 240) / 2) + 56, false);
		}
		else
		{
			drawTexture(gSprites[TEX_TEXTBOX], &rcNone, 40 * i + gScreenWidth / 2 - 96, ((gScreenHeight - 240) / 2) + 48);
			drawTexture(gSprites[TEX_TEXTBOX], &rcNone, 40 * i + gScreenWidth / 2 - 96, ((gScreenHeight - 240) / 2) + 56);
		}
	}

	//Draw item selection box
	RECT rcCur2[2];

	rcCur2[0] = { 80, 88, 112, 104 };
	rcCur2[1] = { 80, 104, 112, 120 };

	if (gInventoryActive)
		drawTexture(
			gSprites[TEX_TEXTBOX],
			&rcCur2[(inventoryFlash >> 1) & 1],
			32 * (gSelectedItem % 6) + gScreenWidth / 2 - 112,
			16 * (gSelectedItem / 6) + ((gScreenHeight - 240) / 2) + 76);
	else
		drawTexture(
			gSprites[TEX_TEXTBOX],
			&rcCur2[1],
			32 * (gSelectedItem % 6) + gScreenWidth / 2 - 112,
			16 * (gSelectedItem / 6) + ((gScreenHeight - 240) / 2) + 76);

	//Draw items
	RECT rcItem;

	for (size_t i = 0; i < ITEMS && gItems[i].code; ++i)
	{
		rcItem.left = 32 * (gItems[i].code % 8);
		rcItem.right = rcItem.left + 32;
		rcItem.top = 16 * (gItems[i].code / 8);
		rcItem.bottom = rcItem.top + 16;

		drawTexture(gSprites[TEX_ITEMIMAGE], &rcItem, 32 * (i % 6) + gScreenWidth / 2 - 112, 16 * (i / 6) + ((gScreenHeight - 240) / 2) + 76);
	}
}

static int inventoryLoop(std::string &oldScript)
{
	while (true)
	{
		//Handle events
		getKeys();

		if (isKeyDown(SDL_SCANCODE_ESCAPE))
		{
			const int escape = escapeMenu();

			if (!escape)
				return 0;
			if (escape == 2)
				return 2;
		}

		//Update menu
		if (gGameFlags & 2)
			moveInventoryCursor();

		const int tscResult = updateTsc();
		if (!tscResult)
			return 0;
		if (tscResult == 2)
			return 2;

		// Draw screenshot
		drawTextureNoScale(gSprites[TEX_SCREENSHOT], nullptr, 0, 0);

		//Draw menu
		drawInventory();
		drawTsc();

		//Check to close
		if (!gInventoryActive && (isKeyPressed(gKeyMenu) || isKeyPressed(gKeyJump) || isKeyPressed(gKeyShoot)))
		{
			stopTsc(tsc);
			loadStageTsc(oldScript);
			gWeaponShiftX = 32;
			return 1;
		}

		if (gGameFlags & 2 && (isKeyPressed(gKeyMenu) || isKeyPressed(gKeyShoot)))
		{
			stopTsc(tsc);
			loadStageTsc(oldScript);
			gWeaponShiftX = 32;
			return 1;
		}

		//Present
		if (!drawWindow())
			return 0;
	}
}

int openInventory()
{
    logInfo("Started openInventory");

	//Keep track of old one
	std::string oldScript(tsc.path);

	//Set up some variables
	gInventoryTitleY = 24;
	gInventoryActive = false;
	gSelectedItem = 0;

	captureScreen(TEX_SCREENSHOT);

	//Load stage select tsc
	loadTsc2("data/ArmsItem.tsc");

	//Start appropriate event
	size_t weaponNo;
	for (weaponNo = 0; weaponNo < WEAPONS && gWeapons[weaponNo].code != 0; ++weaponNo);

	if (weaponNo)
		startTscEvent(tsc, gWeapons[gSelectedWeapon].code + 1000);
	else
		startTscEvent(tsc, gItems[gSelectedItem].code + 5000);

	return inventoryLoop(oldScript);
}


