#include "hud.h"
#include "level.h"
#include "weapons.h"
#include "render.h"
#include "game.h"
#include "player.h"
#include "caret.h"
#include "bullet.h"
#include "valueview.h"

#include <string>
#include <cstring>
#include <SDL_render.h>

using std::strlen;
using std::string;
using std::to_string;

void drawMapName(bool bMini)
{
	const auto len = strlen(mapName.name);

	const int x = screenWidth / 2 - len * 3;
	const int y = 80;

	if (bMini)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		drawRect(0, 7, screenWidth, 17);
		drawString(x, 10, mapName.name);
	}
	else if (mapName.flag)
	{
		SDL_SetTextureColorMod(sprites[0x26], 17, 0, 34);
		drawString(x, y + 1, mapName.name);
		SDL_SetTextureColorMod(sprites[0x26], 255, 255, 255);
		drawString(x, y, mapName.name);

		if (++mapName.wait > 160)
			mapName.flag = 0;
	}
}

//Boss health bar
void drawBossHealth()
{
	RECT rcBr;
	RECT rcLife;
	RECT rcBox2;
	RECT rcBox1;
	RECT rcText;

	rcText.left = 0;
	rcText.top = 48;
	rcText.right = 32;
	rcText.bottom = 56;
	rcBox1.left = 0;
	rcBox1.top = 0;
	rcBox1.right = 244;
	rcBox1.bottom = 8;
	rcBox2.left = 0;
	rcBox2.top = 16;
	rcBox2.right = 244;
	rcBox2.bottom = 24;
	rcLife.left = 0;
	rcLife.top = 24;
	rcLife.right = 0;
	rcLife.bottom = 32;
	rcBr.left = 0;
	rcBr.top = 32;
	rcBr.right = 232;
	rcBr.bottom = 40;

	if (bossLife.flag)
	{
		if (*bossLife.pLife > 0)
		{
			rcLife.right = 198 * *bossLife.pLife / bossLife.max;

			if (bossLife.br <= *bossLife.pLife)
			{
				bossLife.count = 0;
			}
			else if (++bossLife.count > 30)
			{
				--bossLife.br;
			}

			rcBr.right = 198 * bossLife.br / bossLife.max;

			drawTexture(sprites[TEX_TEXTBOX], &rcBox1, screenWidth / 2 - 128, 220);
			drawTexture(sprites[TEX_TEXTBOX], &rcBox2, screenWidth / 2 - 128, 228);
			drawTexture(sprites[TEX_TEXTBOX], &rcBr, screenWidth / 2 - 88, 224);
			drawTexture(sprites[TEX_TEXTBOX], &rcLife, screenWidth / 2 - 88, 224);
			drawTexture(sprites[TEX_TEXTBOX], &rcText, screenWidth / 2 - 120, 224);
		}
		else
		{
			bossLife.flag = 0;
		}
	}
}

//Weapon stats
int addFlash = 0;

void drawWeaponStats()
{
	//Define rects
	RECT rcExpFlash;
	RECT rcExpMax;
	RECT rcExpVal;
	RECT rcExpBox;
	RECT rcNone = { 80, 48, 96, 56 };
	RECT rcLv = { 80, 80, 96, 88 };
	RECT rcPer = { 72, 48, 80, 56 };

	//Shift weapons back into correct position
	if (weaponShiftX > 16)
		weaponShiftX -= 2;
	if (weaponShiftX < 16)
		weaponShiftX += 2;

	//Draw amount of ammo
	if (weapons[selectedWeapon].max_num)
	{
		drawNumber(weapons[selectedWeapon].num, weaponShiftX + 32, 16, false);
		drawNumber(weapons[selectedWeapon].max_num, weaponShiftX + 32, 24, false);
	}
	else
	{
		drawTexture(sprites[TEX_TEXTBOX], &rcNone, weaponShiftX + 48, 16);
		drawTexture(sprites[TEX_TEXTBOX], &rcNone, weaponShiftX + 48, 24);
	}

	//Draw experience
	if (!((currentPlayer.shock >> 1) & 1))
	{
		//Draw level
		drawTexture(sprites[TEX_TEXTBOX], &rcPer, weaponShiftX + 32, 24);
		drawTexture(sprites[TEX_TEXTBOX], &rcLv, weaponShiftX, 32);
		drawNumber(weapons[selectedWeapon].level, weaponShiftX - 8, 32, false);

		//Set framerects
		rcExpBox = { 0, 72, 40, 80 };
		rcExpVal = { 0, 80, 0, 88 };
		rcExpMax = { 40, 72, 80, 80 };
		rcExpFlash = { 40, 80, 80, 88 };

		//Set up some variables
		const int lv = weapons[selectedWeapon].level - 1;
		const int arms_code = weapons[selectedWeapon].code;
		const int exp_now = weapons[selectedWeapon].exp;
		const int exp_next = weaponLevels[arms_code].exp[lv];

		//Draw the bar
		drawTexture(sprites[TEX_TEXTBOX], &rcExpBox, weaponShiftX + 24, 32);

		if (lv != 2 || weapons[selectedWeapon].exp != weaponLevels[arms_code].exp[2]) //If not at max level
		{
			//Scale bar
			if (exp_next)
				rcExpVal.right += 40 * exp_now / exp_next;
			else
				rcExpVal.right = 0;

			drawTexture(sprites[TEX_TEXTBOX], &rcExpVal, weaponShiftX + 24, 32);
		}
		else
		{
			drawTexture(sprites[TEX_TEXTBOX], &rcExpMax, weaponShiftX + 24, 32);
		}

		//Draw the flashing
		if (currentPlayer.exp_wait && (addFlash++ >> 1) & 1)
			drawTexture(sprites[TEX_TEXTBOX], &rcExpFlash, weaponShiftX + 24, 32);
	}
}

//Draw weapons
void drawHudWeapons()
{
	RECT rect = { 0, 0, 0, 16 };

	size_t weaponNo;
	for (weaponNo = 0; weaponNo < WEAPONS && weapons[weaponNo].code != 0; ++weaponNo);

	if (weaponNo)
	{
		for (size_t a = 0; a < weaponNo; ++a)
		{
			int x = 16 * (a - selectedWeapon) + weaponShiftX;

			if (x >= 8)
			{
				if (x >= 24)
					x += 48;
			}
			else
			{
				x += 16 * (weaponNo + 3);
			}

			if (8 * (2 * (static_cast<int>(weaponNo) + 3) + 1) <= x)
				x += 16 * (-3 - weaponNo);

			if (x < 72 && x >= 24)
				x -= 48;

			//Set rect and draw
			rect.left = 16 * weapons[a].code;
			rect.right = rect.left + 16;
			drawTexture(sprites[TEX_ARMSIMAGE], &rect, x, 16);
		}
	}
}

//Health
void drawPlayerHealth()
{
	RECT rcBr = { 0, 32, 232, 40 };
	RECT rcLife = { 0, 24, 232, 32 };
	RECT rcCase = { 0, 40, 232, 48 };

	if (!((currentPlayer.shock >> 1) & 1))
	{
		if (currentPlayer.lifeBr <= currentPlayer.life)
		{
			currentPlayer.lifeBr_count = 0;
			currentPlayer.lifeBr = currentPlayer.life;
		}
		else if (currentPlayer.lifeBr_count++ > 30)
		{
			currentPlayer.lifeBr--;
		}

		//Set rects
		rcCase.right = 64;
		rcLife.right = 40 * currentPlayer.life / currentPlayer.max_life - 1;
		rcBr.right = 40 * currentPlayer.lifeBr / currentPlayer.max_life - 1;

		//Draw health bar
		drawTexture(sprites[0x1A], &rcCase, 16, 40);
		drawTexture(sprites[0x1A], &rcBr, 40, 40);
		drawTexture(sprites[0x1A], &rcLife, 40, 40);

		drawNumber(currentPlayer.lifeBr, 8, 40, false);
	}
}

//Air left
void drawPlayerAir()
{
	RECT rcAir[2];
	rcAir[0] = { 112, 72, 144, 80 };
	rcAir[1] = { 112, 80, 144, 88 };

	const int x = (screenWidth / 2) - 40;
	const int y = (screenHeight / 2) - 16;

	if (!(currentPlayer.equip & equip_airTank) && currentPlayer.air_get)
	{
		//Amount of air left
		if (currentPlayer.air_get % 6 <= 3)
			drawNumber(currentPlayer.air / 10, x + 32, y, false);

		//Draw the "AIR" thing
		drawTexture(sprites[0x1A], &rcAir[currentPlayer.air % 30 <= 10], x, y);
	}
}

void drawHud(bool hide)
{
	drawBossHealth();

	if (hide)
		return;

	drawWeaponStats();
	drawPlayerHealth();
	drawPlayerAir();
	drawHudWeapons();
}
