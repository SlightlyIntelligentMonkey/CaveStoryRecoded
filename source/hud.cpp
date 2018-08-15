#include "hud.h"
#include "level.h"

void drawMapName(bool bMini)
{
	const int len = strlen(mapName.name);

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

void drawPlayerHealth(bool hide)
{
	RECT rcBr = { 0, 32, 232, 40 };
	RECT rcLife = { 0, 24, 232, 32 };
	RECT rcCase = { 0, 40, 232, 48 };

	if (!(hide || ((currentPlayer.shock >> 1) & 1)))
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
		drawTexture(sprites[0x1A], &rcAir[(currentPlayer.air % 30 <= 10)], x, y);
	}
}

void drawHud(bool hide)
{
	drawPlayerHealth(hide);
	drawPlayerAir();
}
