#include "common.h"
#include "level.h"
#include "hud.h"

void gameUpdate()
{
	if (isKeyPressed(SDL_SCANCODE_1)) {
		loadLevel(13);
		currentPlayer.init(10, 8, 2);

		//playOrg(8);
	}
	else if (isKeyPressed(SDL_SCANCODE_2)) {
		loadLevel(12);
		currentPlayer.init(37, 11, 0);

		//playOrg(8);
	}
	else if (isKeyPressed(SDL_SCANCODE_3)) {
		loadLevel(2);
		currentPlayer.init(5, 6, 2);

		//playOrg(1);
	}
	else if (isKeyPressed(SDL_SCANCODE_4)) {
		loadLevel(11);
		currentPlayer.init(13, 34, 2);

		//playOrg(9);
	}
	else if (isKeyPressed(SDL_SCANCODE_5)) {
		loadLevel(49);
		currentPlayer.init(7, 6, 2);

		//playOrg(27);
	}
	else if (isKeyPressed(SDL_SCANCODE_6)) {
		loadLevel(67);
		currentPlayer.init(7, 52, 2);

		//playOrg(29);
	}
	else if (isKeyPressed(SDL_SCANCODE_7)) {
		loadLevel(48);
		currentPlayer.init(155, 1, 0);

		//playOrg(38);
	}
	else if (isKeyPressed(SDL_SCANCODE_8)) {
		loadLevel(53);
		currentPlayer.init(4, 165, 2);

		//playOrg(38);
	}
	else if (isKeyPressed(SDL_SCANCODE_9)) {
		loadLevel(6);
		currentPlayer.init(4, 18, 2);

		//playOrg(38);
	}

	//Update stuff
	updateNPC();
	currentPlayer.update();
	updateCarets();

	updateTsc();

	//Move view
	viewX += (viewGoalX - viewX) / viewSpeed;
	viewY += (viewGoalY - viewY) / viewSpeed;

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