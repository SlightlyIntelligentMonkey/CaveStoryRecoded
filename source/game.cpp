#include "game.h"
#include "common.h"
#include "level.h"
#include "hud.h"
#include "script.h"

int gameMode = 0;
int prevGameMode = 0;

enum gameStates
{
	QUIT = -1,
	INTRO = 0,
	MENU = 1,
	PLAY = 2,
	ESCAPE = 3
};

void debugLevels()
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
	return;
}

int gameUpdatePlay()
{
	int swPlay = 1;
	int tscResult = 0;
	while (true)
	{
		//Framerate limiter
		Uint32 timeNow = SDL_GetTicks();
		Uint32 timeNext = framerateTicks + framerate;

		if (timeNow >= timeNext) {
			framerateTicks = SDL_GetTicks();
		}
		else
		{
			SDL_Delay(timeNext - timeNow);
			continue;
		}

		//Handle events
		getKeys(&events);
		if (events.type == SDL_QUIT || exitGame) { return QUIT; }

		if (isKeyDown(SDL_SCANCODE_ESCAPE)) { prevGameMode = PLAY; return ESCAPE; }

		debugLevels();

		//Update stuff
		if (gameFlags & 1)
		{
			updateNPC();
			if (gameFlags & 2) { currentPlayer.update(true); }
			else { currentPlayer.update(false); }
			updateCarets();
		}

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

		// -- DRAW -- //
		SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
		SDL_RenderClear(renderer);

		//Draw background (and background tiles)
		drawLevel(false);

		//Draw stuff
		drawNPC();
		currentPlayer.draw();

		//Draw foreground tiles
		drawLevel(true);

		//Draw carets
		drawCarets();

		drawHud(false);

		if (swPlay & 1)
		{
			tscResult = updateTsc();
			if (!tscResult) { return QUIT; }
			if (tscResult == 2) { return MENU; }
		}
		drawTSC();

		SDL_RenderPresent(renderer);
	}

	return QUIT;
}

int select;
int anime;
RECT rcTitle = { 0, 0, 144, 32 };
RECT rcNew = { 144, 0, 176, 16 };
RECT rcLoad = { 144, 16, 176, 32 };
int gameUpdateMenu()
{
	int frame = 0;
	BYTE frameOrder[] = { 0, 1, 0, 2 };
	while (true)
	{
		//Framerate limiter
		Uint32 timeNow = SDL_GetTicks();
		Uint32 timeNext = framerateTicks + framerate;

		if (timeNow >= timeNext) {
			framerateTicks = SDL_GetTicks();
		}
		else
		{
			SDL_Delay(timeNext - timeNow);
			continue;
		}

		//Handle events
		getKeys(&events);
		if (events.type == SDL_QUIT || exitGame) { return QUIT; }

		if (isKeyDown(SDL_SCANCODE_ESCAPE)) { prevGameMode = MENU; return ESCAPE; }

		if (isKeyPressed(keyJump))
		{
			//playSound(18);
			if (select == 0)
			{
				gameFlags = 3;
				memset(tscFlags, 0, sizeof(tscFlags));
				loadLevel(13);
				currentPlayer.init();
				currentPlayer.setPos(10 << 13, 8 << 13);
				runScriptEvent(200);

				//sound::playOrg(8);
				break;
			}
			else
			{
				tscMode = END;
				gameFlags = 3;
				loadProfile();
				break;
			}
		}
		if (isKeyPressed(keyUp) || isKeyPressed(keyDown))
		{
			//playSound(1);
			select ^= 1;
		}

		// -- DRAW -- //
		SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
		SDL_RenderClear(renderer);

		drawTexture(sprites[0x00], &rcTitle, (screenWidth >> 1) - 72, 40);
		drawTexture(sprites[0x00], &rcNew, (screenWidth >> 1) - 16, 128);
		drawTexture(sprites[0x00], &rcLoad, (screenWidth >> 1) - 16, 148);
		RECT rcChar = { 0 + (frameOrder[((anime / 10) % 4)] << 4), 16,
			16 + (frameOrder[((anime / 10) % 4)] << 4), 32 };
		drawTexture(sprites[0x10], &rcChar, (screenWidth >> 1) - 32, 127 + (20 * select));
		anime++;

		SDL_RenderPresent(renderer);
	}

	frame = SDL_GetTicks();
	while (SDL_GetTicks() < frame + 1000)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}
	return PLAY;
}

int gameUpdateIntro()
{
	int frame = 0;
	viewX = 0;
	viewY = 0;
	loadLevel(72);
	runScriptEvent(100);
	while (frame < 500)
	{
		//Framerate limiter
		Uint32 timeNow = SDL_GetTicks();
		Uint32 timeNext = framerateTicks + framerate;

		if (timeNow >= timeNext) {
			framerateTicks = SDL_GetTicks();
		}
		else
		{
			SDL_Delay(timeNext - timeNow);
			continue;
		}
		frame++;

		//Handle events
		getKeys(&events);
		if (events.type == SDL_QUIT || exitGame) { return QUIT; }

		if (isKeyDown(SDL_SCANCODE_ESCAPE)) { prevGameMode = INTRO; return ESCAPE; }

		if (isKeyPressed(keyJump) | isKeyDown(keyShoot)) { break; }

		updateNPC();
		updateCarets();

		// -- DRAW -- //
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		//Draw background (and background tiles)
		drawLevel(false);

		//Draw stuff
		drawNPC();

		//Draw foreground tiles
		drawLevel(true);

		//Draw carets
		drawCarets();

		updateTsc();
		drawTSC();

		SDL_RenderPresent(renderer);
	}

	//wait 500 ms
	frame = SDL_GetTicks();
	while (SDL_GetTicks() < frame + 500)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}
	return MENU;
}

RECT rcEscape = { 0, 128, 208, 144 };
int gameUpdateEscape()
{
	while (true)
	{
		//Framerate limiter
		Uint32 timeNow = SDL_GetTicks();
		Uint32 timeNext = framerateTicks + framerate;

		if (timeNow >= timeNext) {
			framerateTicks = SDL_GetTicks();
		}
		else
		{
			SDL_Delay(timeNext - timeNow);
			continue;
		}

		//Handle events
		getKeys(&events);
		if (events.type == SDL_QUIT || exitGame) { return QUIT; }

		if (isKeyPressed(SDL_SCANCODE_ESCAPE)) { return QUIT; }
		if (isKeyPressed(SDL_SCANCODE_F1)) { return prevGameMode; }
		if (isKeyPressed(SDL_SCANCODE_F2)) { return INTRO; }

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		drawTexture(sprites[0x1A], &rcEscape,
			(screenWidth >> 1) - 104,
			(screenHeight >> 1) - 8);

		SDL_RenderPresent(renderer);
	}
	return MENU;
}

int game() {
	while (gameMode > -1) {
		//////UPDATE//////
		switch (gameMode)
		{
		case(INTRO):
			gameMode = gameUpdateIntro();
			break;
		case(MENU):
			gameMode = gameUpdateMenu();
			break;
		case(PLAY):
			gameMode = gameUpdatePlay();
			break;
		case(ESCAPE):
			gameMode = gameUpdateEscape();
			break;
		}
	}

	return QUIT;
}