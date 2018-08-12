#include "game.h"
#include "level.h"
#include "hud.h"
#include "script.h"
#include "fade.h"

int gameMode = 0;
int prevGameMode = 0;

VIEW viewport;

void viewBounds()
{
	//Keep view in level
	if ((levelWidth - 1) << 4 > screenWidth)
		viewport.x = clamp(viewport.x, 0, ((levelWidth - 1) << 13) - (screenWidth << 9));
	else
		viewport.x = ((levelWidth - 1) << 12) - (screenWidth << 8);

	if ((levelHeight - 1) << 4 > screenHeight)
		viewport.y = clamp(viewport.y, 0, ((levelHeight - 1) << 13) - (screenHeight << 9));
	else
		viewport.y = ((levelHeight - 1) << 12) - (screenHeight << 8);
}

void handleView()
{
	//Move view
	viewport.x += ((*viewport.lookX - (screenWidth << 8)) - viewport.x) / viewport.speed;
	viewport.y += ((*viewport.lookY - (screenHeight << 8)) - viewport.y) / viewport.speed;

	viewBounds();

	//Quake
	if (viewport.quake2)
	{
		viewport.x += (random(-5, 5) << 9);
		viewport.y += (random(-3, 3) << 9);
		--viewport.quake2;
	}
	else if (viewport.quake)
	{
		viewport.x += (random(-1, 1) << 9);
		viewport.y += (random(-1, 1) << 9);
		--viewport.quake;
	}
}

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

void initGame()
{
	gameFlags = 3;
	memset(tscFlags, 0, sizeof(tscFlags));
	currentPlayer.init();
	currentPlayer.setPos(10 << 13, 8 << 13);
	loadLevel(13);
	runScriptEvent(200);
	tscDisplayFlags = 0;
	fadedOut = true;
	fadeCounter = 0;
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

		if (events.type == SDL_QUIT || exitGame)
			return QUIT;

		if (isKeyDown(SDL_SCANCODE_ESCAPE))
		{
			prevGameMode = PLAY;
			return ESCAPE;
		}

		if (isKeyDown(SDL_SCANCODE_LALT) && isKeyPressed(SDL_SCANCODE_RETURN) || isKeyPressed(SDL_SCANCODE_LALT) && isKeyDown(SDL_SCANCODE_RETURN))
			switchScreenMode();

		debugLevels();

		//Update stuff
		if (swPlay & 1 && gameFlags & 1)
		{
			if (gameFlags & 2)
				currentPlayer.update(true);
			else
				currentPlayer.update(false);

			updateNPC();

			updateCarets();
			handleView();
		}

		// -- DRAW -- //
		SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
		SDL_RenderClear(renderer);

		drawLevel(false);
		drawNPC();
		currentPlayer.draw();
		drawLevel(true);
		drawCarets();

		if (gameFlags & 2)
			drawHud(false);

		drawFade();

		if (swPlay & 1)
		{
			tscResult = updateTsc();

			if (!tscResult)
				return QUIT;
			if (tscResult == 2)
				return MENU;
		}

		drawTSC();

		SDL_RenderPresent(renderer);
	}

	return QUIT;
}

int gameUpdateMenu()
{
	int select = 0;
	int anime = 0;

	int version[4] = { 1, 0, 0, 6 };

	RECT rcVersion = { 152, 80, 208, 88 };
	RECT rcPeriod = { 152, 88, 208, 96 };

	RECT rcTitle = { 0, 0, 144, 32 };
	RECT rcNew = { 144, 0, 176, 16 };
	RECT rcLoad = { 144, 16, 176, 32 };
	RECT rcPixel = { 0, 0, 160, 16 };

	uint32_t frame = 0;
	BYTE frameOrder[] = { 0, 1, 0, 2 };

	if (fileExists("Profile.dat"))
		select = 1;
	else
		select = 0;

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

		if (events.type == SDL_QUIT || exitGame)
			return QUIT;

		if (isKeyDown(SDL_SCANCODE_ESCAPE))
		{
			prevGameMode = MENU;
			return ESCAPE;
		}

		if (isKeyDown(SDL_SCANCODE_LALT) && isKeyPressed(SDL_SCANCODE_RETURN) || isKeyPressed(SDL_SCANCODE_LALT) && isKeyDown(SDL_SCANCODE_RETURN))
			switchScreenMode();

		if (isKeyPressed(keyJump))
		{
			playSound(18);

			if (select == 0)
			{
				initGame();
				break;
			}
			else
			{
				loadProfile();
				break;
			}
		}

		if (isKeyPressed(keyUp) || isKeyPressed(keyDown))
		{
			playSound(1);
			select ^= 1;
		}

		if (++anime >= 40)
			anime = 0;

		// -- DRAW -- //
		SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
		SDL_RenderClear(renderer);

		//Draw version
		drawTexture(sprites[0x1A], &rcVersion, (screenWidth >> 1) - 60, 216);
		drawTexture(sprites[0x1A], &rcPeriod, (screenWidth >> 1) - 4, 216);
		drawNumber(version[0], (screenWidth >> 1) - 20, 216, false);
		drawNumber(version[1], (screenWidth >> 1) - 4, 216, false);
		drawNumber(version[2], (screenWidth >> 1) + 12, 216, false);
		drawNumber(version[3], (screenWidth >> 1) + 28, 216, false);

		//Draw title, new, load, and pixel 12.2004 thing
		drawTexture(sprites[0x00], &rcTitle, (screenWidth >> 1) - 72, 40);
		drawTexture(sprites[0x00], &rcNew, (screenWidth >> 1) - 24, 128);
		drawTexture(sprites[0x00], &rcLoad, (screenWidth >> 1) - 24, 148);
		drawTexture(sprites[0x01], &rcPixel, (screenWidth >> 1) - 80, 192);

		//Draw the character cursor
		RECT rcChar = { 0 + (frameOrder[((anime / 10) % 4)] << 4), 16, 16 + (frameOrder[((anime / 10) % 4)] << 4), 32 };
		drawTexture(sprites[0x10], &rcChar, (screenWidth >> 1) - 44, 127 + (20 * select));

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
	uint32_t frame = 0;
	fadedOut = true;

	viewport.x = 0;
	viewport.y = 0;
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

		if (isKeyDown(SDL_SCANCODE_LALT) && isKeyPressed(SDL_SCANCODE_RETURN) ||
			isKeyPressed(SDL_SCANCODE_LALT) && isKeyDown(SDL_SCANCODE_RETURN))
		{
			switchScreenMode();
		}

		if (isKeyPressed(keyJump) | isKeyDown(keyShoot)) { break; }

		updateNPC();
		updateCarets();

		handleView();

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

		drawFade();
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

		if (isKeyDown(SDL_SCANCODE_LALT) && isKeyPressed(SDL_SCANCODE_RETURN) ||
			isKeyPressed(SDL_SCANCODE_LALT) && isKeyDown(SDL_SCANCODE_RETURN))
		{
			switchScreenMode();
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		drawTexture(sprites[0x1A], &rcEscape,
			(screenWidth >> 1) - 104,
			(screenHeight >> 1) - 8);

		SDL_RenderPresent(renderer);
	}
	return MENU;
}

int mainGameLoop() {
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
