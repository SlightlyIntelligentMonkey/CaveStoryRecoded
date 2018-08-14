#include "hud.h"

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

	int x = (screenWidth / 2) - 40;
	int y = (screenHeight / 2) - 16;

	if (!(currentPlayer.equip & equip_airTank) && currentPlayer.air_get)
	{
		//Amount of air left
		if (currentPlayer.air_get % 6 <= 3)
			drawNumber(currentPlayer.air / 10, x + 32, y, false);

		//Draw the "AIR" thing
		drawTexture(sprites[0x1A], &rcAir[(currentPlayer.air % 30 <= 10)], x, y);
	}
	return;
}

void drawHud(bool hide)
{
	drawPlayerHealth(hide);
	drawPlayerAir();
}