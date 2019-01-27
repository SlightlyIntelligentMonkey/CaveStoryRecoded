#include "hud.h"
#include "stage.h"
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

void drawMapName(bool bMini)
{
	const auto len = strlen(gMapName.name);

	const int x = gScreenWidth / 2 - len * 3;
	const int y = ((gScreenHeight - 240) / 2) + 80;

	if (bMini)
	{
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		drawRect(0, 7, gScreenWidth, 17);
		drawString(x, 10, gMapName.name);
	}
	else if (gMapName.flag)
	{
		SDL_SetTextureColorMod(gSprites[0x26], 17, 0, 34);
		drawString(x, y + 1, gMapName.name);
		SDL_SetTextureColorMod(gSprites[0x26], 255, 255, 255);
		drawString(x, y, gMapName.name);

		if (++gMapName.wait > 160)
			gMapName.flag = 0;
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

	if (gBossLife.flag)
	{
		if (*gBossLife.pLife > 0)
		{
			rcLife.right = 198 * *gBossLife.pLife / gBossLife.max;

			if (gBossLife.br <= *gBossLife.pLife)
			{
				gBossLife.count = 0;
			}
			else if (++gBossLife.count > 30)
			{
				--gBossLife.br;
			}

			rcBr.right = 198 * gBossLife.br / gBossLife.max;

			drawTexture(gSprites[TEX_TEXTBOX], &rcBox1, gScreenWidth / 2 - 128, gScreenHeight - 20);
			drawTexture(gSprites[TEX_TEXTBOX], &rcBox2, gScreenWidth / 2 - 128, gScreenHeight - 12);
			drawTexture(gSprites[TEX_TEXTBOX], &rcBr, gScreenWidth / 2 - 88, gScreenHeight - 16);
			drawTexture(gSprites[TEX_TEXTBOX], &rcLife, gScreenWidth / 2 - 88, gScreenHeight - 16);
			drawTexture(gSprites[TEX_TEXTBOX], &rcText, gScreenWidth / 2 - 120, gScreenHeight - 16);
		}
		else
		{
			gBossLife.flag = 0;
		}
	}
}

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
	if (gWeaponShiftX > 16)
		gWeaponShiftX -= 2;
	if (gWeaponShiftX < 16)
		gWeaponShiftX += 2;

	//Draw amount of ammo
	if (gWeapons[gSelectedWeapon].max_num)
	{
		drawNumber(gWeapons[gSelectedWeapon].num, gWeaponShiftX + 32, 16, false);
		drawNumber(gWeapons[gSelectedWeapon].max_num, gWeaponShiftX + 32, 24, false);
	}
	else
	{
		drawTexture(gSprites[TEX_TEXTBOX], &rcNone, gWeaponShiftX + 48, 16);
		drawTexture(gSprites[TEX_TEXTBOX], &rcNone, gWeaponShiftX + 48, 24);
	}

	//Draw experience
	if (!((gCurrentPlayer.shock >> 1) & 1))
	{
		//Draw level
		drawTexture(gSprites[TEX_TEXTBOX], &rcPer, gWeaponShiftX + 32, 24);
		drawTexture(gSprites[TEX_TEXTBOX], &rcLv, gWeaponShiftX, 32);
		drawNumber(gWeapons[gSelectedWeapon].level, gWeaponShiftX - 8, 32, false);

		//Set framerects
		rcExpBox = { 0, 72, 40, 80 };
		rcExpVal = { 0, 80, 0, 88 };
		rcExpMax = { 40, 72, 80, 80 };
		rcExpFlash = { 40, 80, 80, 88 };

		//Set up some variables
		const int lv = gWeapons[gSelectedWeapon].level - 1;
		const int arms_code = gWeapons[gSelectedWeapon].code;
		const int exp_now = gWeapons[gSelectedWeapon].exp;
		const int exp_next = gWeaponLevels[arms_code].exp[lv];

		//Draw the bar
		drawTexture(gSprites[TEX_TEXTBOX], &rcExpBox, gWeaponShiftX + 24, 32);

		if (lv != 2 || gWeapons[gSelectedWeapon].exp != gWeaponLevels[arms_code].exp[2]) //If not at max level
		{
			//Scale bar
			if (exp_next)
				rcExpVal.right += 40 * exp_now / exp_next;
			else
				rcExpVal.right = 0;

			drawTexture(gSprites[TEX_TEXTBOX], &rcExpVal, gWeaponShiftX + 24, 32);
		}
		else
		{
			drawTexture(gSprites[TEX_TEXTBOX], &rcExpMax, gWeaponShiftX + 24, 32);
		}
		
		static int flashAlternator = 0;
		//Draw the flashing
		if (gCurrentPlayer.exp_wait && (flashAlternator++ >> 1) & 1)
			drawTexture(gSprites[TEX_TEXTBOX], &rcExpFlash, gWeaponShiftX + 24, 32);
	}
}

//Draw weapons
void drawHudWeapons()
{
	RECT rect = { 0, 0, 0, 16 };

	size_t weaponNo;
	for (weaponNo = 0; weaponNo < WEAPONS && gWeapons[weaponNo].code != 0; ++weaponNo);

	if (weaponNo)
	{
		for (size_t a = 0; a < weaponNo; ++a)
		{
			int x = 16 * (a - gSelectedWeapon) + gWeaponShiftX;

			if (x >= 8)
			{
				if (x >= 24)
					x += 48;
			}
			else
			{
				x += 16 * (weaponNo + 3);
			}

			if (8 * (2 * (static_cast<ssize_t>(weaponNo) + 3) + 1) <= x)
				x += 16 * (-3 - weaponNo);

			if (x < 72 && x >= 24)
				x -= 48;

			//Set rect and draw
			rect.left = 16 * gWeapons[a].code;
			rect.right = rect.left + 16;
			drawTexture(gSprites[TEX_ARMSIMAGE], &rect, x, 16);
		}
	}
}

//Health
void drawPlayerHealth()
{
	RECT rcBr = { 0, 32, 232, 40 };
	RECT rcLife = { 0, 24, 232, 32 };
	RECT rcCase = { 0, 40, 232, 48 };

	if (!((gCurrentPlayer.shock >> 1) & 1))
	{
		if (gCurrentPlayer.lifeBr <= gCurrentPlayer.life)
		{
			gCurrentPlayer.lifeBr_count = 0;
			gCurrentPlayer.lifeBr = gCurrentPlayer.life;
		}
		else if (gCurrentPlayer.lifeBr_count++ > 30)
		{
			gCurrentPlayer.lifeBr--;
		}

		//Set rects
		rcCase.right = 64;
		rcLife.right = 40 * gCurrentPlayer.life / gCurrentPlayer.max_life - 1;
		rcBr.right = 40 * gCurrentPlayer.lifeBr / gCurrentPlayer.max_life - 1;

		//Draw health bar
		drawTexture(gSprites[0x1A], &rcCase, 16, 40);
		drawTexture(gSprites[0x1A], &rcBr, 40, 40);
		drawTexture(gSprites[0x1A], &rcLife, 40, 40);

		drawNumber(gCurrentPlayer.lifeBr, 8, 40, false);
	}
}

//Air left
void drawPlayerAir()
{
	RECT rcAir[2];
	rcAir[0] = { 112, 72, 144, 80 };
	rcAir[1] = { 112, 80, 144, 88 };

	const int x = (gScreenWidth / 2) - 40;
	const int y = (gScreenHeight / 2) - 16;

	if (!(gCurrentPlayer.equip & equip_airTank) && gCurrentPlayer.air_get)
	{
		//Amount of air left
		if (gCurrentPlayer.air_get % 6 <= 3)
			drawNumber(gCurrentPlayer.air / 10, x + 32, y, false);

		//Draw the "AIR" thing
		drawTexture(gSprites[0x1A], &rcAir[gCurrentPlayer.air % 30 <= 10], x, y);
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
