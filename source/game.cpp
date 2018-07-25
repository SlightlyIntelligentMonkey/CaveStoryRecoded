#include "common.h"
#include "level.h"
#include "hud.h"

void gameUpdate()
{
	if (isKeyPressed(SDL_SCANCODE_1)) {
		loadLevel(13);
		currentPlayer.setPos(10 << 13, 8 << 13);

		//sound::playOrg(8);
	}
	else if (isKeyPressed(SDL_SCANCODE_2)) {
		loadLevel(12);
		currentPlayer.setPos(37 << 13, 11 << 13);

		//sound::playOrg(8);
	}
	else if (isKeyPressed(SDL_SCANCODE_3)) {
		loadLevel(2);
		currentPlayer.setPos(5 << 13, 6 << 13);

		//sound::playOrg(1);
	}
	else if (isKeyPressed(SDL_SCANCODE_4)) {
		loadLevel(11);
		currentPlayer.setPos(13 << 13, 34 << 13);

		//sound::playOrg(9);
	}
	else if (isKeyPressed(SDL_SCANCODE_5)) {
		loadLevel(49);
		currentPlayer.setPos(7 << 13, 6 << 13);

		//sound::playOrg(27);
	}
	else if (isKeyPressed(SDL_SCANCODE_6)) {
		loadLevel(67);
		currentPlayer.setPos(7 << 13, 52 << 13);

		//sound::playOrg(29);
	}
	else if (isKeyPressed(SDL_SCANCODE_7)) {
		loadLevel(48);
		currentPlayer.setPos(155 << 13, 1 << 13);

		//sound::playOrg(38);
	}
	else if (isKeyPressed(SDL_SCANCODE_8)) {
		loadLevel(53);
		currentPlayer.setPos(4 << 13, 165 << 13);

		//sound::playOrg(38);
	}
	else if (isKeyPressed(SDL_SCANCODE_9)) {
		loadLevel(6);
		currentPlayer.setPos(4 << 13, 18 << 13);

		//sound::playOrg(38);
	}

	//Update stuff
	updateNPC();
	currentPlayer.update(true);
	updateCarets();

	updateTsc();

	//Move view
	viewX += ((viewGoalX - (screenWidth << 8)) - viewX) / viewSpeed;
	viewY += ((viewGoalY - (screenHeight << 8)) - viewY) / viewSpeed;

	//Keep view in level
	if ((levelWidth - 1) << 4 > screenWidth)
		viewX = clamp(viewX, 0, ((levelWidth - 1) << 13) - (screenWidth << 9));
	else
		viewX = ((levelWidth - 1) << 12) - (screenWidth << 8);

	if ((levelHeight - 1) << 4 > screenHeight)
		viewY = clamp(viewY, 0, ((levelHeight - 1) << 13) - (screenHeight << 9));
	else
		viewY = ((levelHeight - 1) << 12) - (screenHeight << 8);
}

void gameDraw()
{
	//Draw background (and background tiles)
	drawLevel(false);

	//Draw stuff
	drawNPC();
	currentPlayer.draw();

	//Draw foreground tiles
	drawLevel(true);

	//Draw carets
	drawCarets();

	//Draw hud
	drawHud(false);
}