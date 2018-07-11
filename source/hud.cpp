#include "hud.h"

void drawPlayerHealth(bool hide)
{
	RECT rcBr = { 0, 32, 232, 40 };
	RECT rcLife = { 0, 24, 232, 32 };
	RECT rcCase = { 0, 40, 232, 48 };

	if (!hide)
	{
		if (currentPlayer.lastHealth <= currentPlayer.health)
		{
			currentPlayer.healthDrainCount = 0;
			currentPlayer.lastHealth = currentPlayer.health;
		}
		else if (currentPlayer.healthDrainCount++ > 30)
		{
			currentPlayer.lastHealth--;
		}

		//Set rects
		rcCase.right = 64;
		rcLife.right = 40 * currentPlayer.health / currentPlayer.maxHealth - 1;
		rcBr.right = 40 * currentPlayer.lastHealth / currentPlayer.maxHealth - 1;

		//Draw health bar
		drawTextureFromRect(sprites[0x1A], &rcCase, 16 << 9, 40 << 9, true);
		drawTextureFromRect(sprites[0x1A], &rcBr, 40 << 9, 40 << 9, true);
		drawTextureFromRect(sprites[0x1A], &rcLife, 40 << 9, 40 << 9, true);

		drawNumber(currentPlayer.lastHealth, 8 << 9, 40 << 9, false);
	}
}

void drawPlayerAir()
{
	RECT rcAir[2];
	rcAir[0] = { 112, 72, 144, 80 };
	rcAir[1] = { 112, 80, 144, 88 };

	int x = (screenWidth << 8) - 0x5000;
	int y = (screenHeight << 8) - 0x2000;

	if (currentPlayer.airShow)
	{
		//Amount of air left divided
		if (currentPlayer.airShow % 6 <= 3)
			drawNumber(currentPlayer.air / 10, x + (32 << 9), y, false);

		//Draw the "AIR" thing
		drawTextureFromRect(sprites[0x1A], &rcAir[(currentPlayer.air % 30 <= 10)], x, y, true);
	}
	return;
}

void drawHud(bool hide)
{
	drawPlayerHealth(hide || ((currentPlayer.shock >> 1) & 1));
	drawPlayerAir();
}