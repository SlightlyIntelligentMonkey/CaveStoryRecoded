#include "game.h"

#include <string>
#include <cstring>
#include <SDL_scancode.h>
#include <SDL_timer.h>
#include <SDL_render.h>
#include <SDL_events.h>
#include "weapons.h"
#include "level.h"
#include "hud.h"
#include "script.h"
#include "fade.h"
#include "boss.h"
#include "input.h"
#include "filesystem.h"
#include "caret.h"
#include "valueview.h"
#include "render.h"
#include "sound.h"
#include "player.h"
#include "bullet.h"
#include "mathUtils.h"
#include "flags.h"
#include "org.h"
#include "log.h"
#include "main.h"
#include "playerCollision.h"
#include "flash.h"

using std::string;
using std::to_string;
using std::memset;

int gameMode = 1;
int gameFlags = 0;

VIEW viewport;

BOSSLIFE bossLife;

//Init game function
void initGame()
{
	//Clear flags
	memset(tscFlags, 0, sizeof(tscFlags));
	memset(mapFlags, 0, sizeof(mapFlags));

	//Clear other stuff
	init2();
	initWeapons();
	memset(permitStage, 0, sizeof(permitStage));
	memset(items, 0, sizeof(items));

	//Set up fade
	initFade();
	fade.bMask = true;

	//Load stage
	currentPlayer.init();
	currentPlayer.setPos(tilesToUnits(10), tilesToUnits(8));
	loadLevel(13);
	startTscEvent(tsc, 200);
}

//Init other important things
void init2()
{
	memset(&bossLife, 0, sizeof(bossLife));
}

//Keep view inside the level
void viewBounds()
{
	if ((levelWidth - 1) << 4 > screenWidth)
		viewport.x = clamp(viewport.x, 0, tilesToUnits(levelWidth - 1) - (screenWidth << 9));
	else
		viewport.x = ((levelWidth - 1) << 12) - (screenWidth << 8);

	if ((levelHeight - 1) << 4 > screenHeight)
		viewport.y = clamp(viewport.y, 0, tilesToUnits(levelHeight - 1) - (screenHeight << 9));
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

int debugFlags = 0;

void debugCMDGetInput(string &cmd)
{
	if (isKeyPressed(SDL_SCANCODE_COMMA)) { cmd += "<"; }
	if (isKeyPressed(SDL_SCANCODE_SEMICOLON)) { cmd += ":"; }
	if (isKeyPressed(SDL_SCANCODE_A)) { cmd += "A"; }
	if (isKeyPressed(SDL_SCANCODE_B)) { cmd += "B"; }
	if (isKeyPressed(SDL_SCANCODE_C)) { cmd += "C"; }
	if (isKeyPressed(SDL_SCANCODE_D)) { cmd += "D"; }
	if (isKeyPressed(SDL_SCANCODE_E)) { cmd += "E"; }
	if (isKeyPressed(SDL_SCANCODE_F)) { cmd += "F"; }
	if (isKeyPressed(SDL_SCANCODE_G)) { cmd += "G"; }
	if (isKeyPressed(SDL_SCANCODE_H)) { cmd += "H"; }
	if (isKeyPressed(SDL_SCANCODE_I)) { cmd += "I"; }
	if (isKeyPressed(SDL_SCANCODE_J)) { cmd += "J"; }
	if (isKeyPressed(SDL_SCANCODE_K)) { cmd += "K"; }
	if (isKeyPressed(SDL_SCANCODE_L)) { cmd += "L"; }
	if (isKeyPressed(SDL_SCANCODE_M)) { cmd += "M"; }
	if (isKeyPressed(SDL_SCANCODE_N)) { cmd += "N"; }
	if (isKeyPressed(SDL_SCANCODE_O)) { cmd += "O"; }
	if (isKeyPressed(SDL_SCANCODE_P)) { cmd += "P"; }
	if (isKeyPressed(SDL_SCANCODE_Q)) { cmd += "Q"; }
	if (isKeyPressed(SDL_SCANCODE_R)) { cmd += "R"; }
	if (isKeyPressed(SDL_SCANCODE_S)) { cmd += "S"; }
	if (isKeyPressed(SDL_SCANCODE_T)) { cmd += "T"; }
	if (isKeyPressed(SDL_SCANCODE_U)) { cmd += "U"; }
	if (isKeyPressed(SDL_SCANCODE_V)) { cmd += "V"; }
	if (isKeyPressed(SDL_SCANCODE_W)) { cmd += "W"; }
	if (isKeyPressed(SDL_SCANCODE_X)) { cmd += "X"; }
	if (isKeyPressed(SDL_SCANCODE_Y)) { cmd += "Y"; }
	if (isKeyPressed(SDL_SCANCODE_Z)) { cmd += "Z"; }
	if (isKeyPressed(SDL_SCANCODE_SPACE)) { cmd += " "; }

	if (isKeyPressed(SDL_SCANCODE_0) || isKeyPressed(SDL_SCANCODE_KP_0)) { cmd += "0"; }
	if (isKeyPressed(SDL_SCANCODE_1) || isKeyPressed(SDL_SCANCODE_KP_1)) { cmd += "1"; }
	if (isKeyPressed(SDL_SCANCODE_2) || isKeyPressed(SDL_SCANCODE_KP_2)) { cmd += "2"; }
	if (isKeyPressed(SDL_SCANCODE_3) || isKeyPressed(SDL_SCANCODE_KP_3)) { cmd += "3"; }
	if (isKeyPressed(SDL_SCANCODE_4) || isKeyPressed(SDL_SCANCODE_KP_4)) { cmd += "4"; }
	if (isKeyPressed(SDL_SCANCODE_5) || isKeyPressed(SDL_SCANCODE_KP_5)) { cmd += "5"; }
	if (isKeyPressed(SDL_SCANCODE_6) || isKeyPressed(SDL_SCANCODE_KP_6)) { cmd += "6"; }
	if (isKeyPressed(SDL_SCANCODE_7) || isKeyPressed(SDL_SCANCODE_KP_7)) { cmd += "7"; }
	if (isKeyPressed(SDL_SCANCODE_8) || isKeyPressed(SDL_SCANCODE_KP_8)) { cmd += "8"; }
	if (isKeyPressed(SDL_SCANCODE_9) || isKeyPressed(SDL_SCANCODE_KP_9)) { cmd += "9"; }
	if (isKeyPressed(SDL_SCANCODE_MINUS) || isKeyPressed(SDL_SCANCODE_KP_MINUS)) { cmd += "-"; }
	if (isKeyPressed(SDL_SCANCODE_EQUALS) || isKeyPressed(SDL_SCANCODE_KP_PLUS)) { cmd += "+"; }

	if (isKeyPressed(SDL_SCANCODE_BACKSPACE) && cmd.size() != 0) { cmd.erase(cmd.cend()-1); }
}

void debugFunction()
{
	static string cmd;
	static int debugMode = 0;
	static TSC dtsc;

	if (isKeyDown(SDL_SCANCODE_RSHIFT) && isKeyDown(SDL_SCANCODE_BACKSPACE))
	{
		//if (isKeyPressed(SDL_SCANCODE_M))
		//	debugMode = 1;
		if (isKeyPressed(SDL_SCANCODE_K))
			debugFlags ^= showSlots;
		if (isKeyPressed(SDL_SCANCODE_C))
			debugFlags ^= showCARId;
		if (isKeyPressed(SDL_SCANCODE_B))
			debugFlags ^= showBULId;
		if (isKeyPressed(SDL_SCANCODE_N))
			debugFlags ^= showNPCId;
		if (isKeyPressed(SDL_SCANCODE_H))
			debugFlags ^= showHitRects;
		if (isKeyPressed(SDL_SCANCODE_R))
			debugFlags ^= showHurtRects;
		if (isKeyPressed(SDL_SCANCODE_EQUALS))
			framerate--;
		if (isKeyPressed(SDL_SCANCODE_MINUS))
			framerate++;
	}

	switch (debugMode)
	{
	case(0):
		if (isKeyDown(SDL_SCANCODE_RSHIFT) && isKeyDown(SDL_SCANCODE_BACKSPACE) &&
			(isKeyPressed(SDL_SCANCODE_RETURN) || isKeyPressed(SDL_SCANCODE_KP_ENTER)))
		{
			debugMode = 1;
			cmd.clear();
			cmd.shrink_to_fit();
			cmd += "<";
		}
		break;
	case(1):
		debugCMDGetInput(cmd);
		drawString(0, 0, cmd.c_str());
		if (isKeyPressed(SDL_SCANCODE_RETURN) || isKeyPressed(SDL_SCANCODE_KP_ENTER))
		{
			cmd += "<END";
			dtsc.data = (uint8_t*)cmd.c_str();
			dtsc.mode = 1;
			dtsc.wait = 5;
			dtsc.p_read = 0;
			debugMode = 2;
		}
		break;
	case(2):
		if (updateTsc(dtsc) == 32)
			debugMode = 0;
		break;
	}

	if (debugFlags & showSlots)
	{
		string debugStr1 = "There are " + to_string(npcs.size()) + " npc slots.";
		string debugStr2 = "There are " + to_string(bullets.size()) + " bullet slots.";
		string debugStr3 = "There are " + to_string(carets.size()) + " caret slots.";
		string debugStr4 = "There are " + to_string(valueviews.size()) + " valueview slots";
		string debugStr5 = "Currently loaded boss is " + to_string(bossObj[0].code_char);
		drawString(8, screenHeight - 12, debugStr1.c_str());
		drawString(8, screenHeight - 24, debugStr2.c_str());
		drawString(8, screenHeight - 36, debugStr3.c_str());
		drawString(8, screenHeight - 48, debugStr4.c_str());
		drawString(8, screenHeight - 60, debugStr5.c_str());
	}
}

//Escape menu
RECT rcEscape = { 0, 128, 208, 144 };

int escapeMenu()
{
    logInfo("Started escapeMenu");
	do
	{
		//Handle events
		getKeys();

		if (isKeyPressed(SDL_SCANCODE_ESCAPE))
			return 0;
		if (isKeyPressed(SDL_SCANCODE_F1))
			return 1;
		if (isKeyPressed(SDL_SCANCODE_F2))
			return 2;

		if ((isKeyDown(SDL_SCANCODE_LALT) && isKeyPressed(SDL_SCANCODE_RETURN)) ||
		        (isKeyPressed(SDL_SCANCODE_LALT) && isKeyDown(SDL_SCANCODE_RETURN)))
			switchScreenMode();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		drawTexture(sprites[0x1A], &rcEscape, (screenWidth >> 1) - 104, (screenHeight >> 1) - 8);
	} while (drawWindow());

	return 0;
}

//Main States
int gameUpdatePlay()
{
    logInfo("Started gameUpdatePlay");
	init2();

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
				return 1;
		}

		if ((isKeyDown(SDL_SCANCODE_LALT) && isKeyPressed(SDL_SCANCODE_RETURN)) || (isKeyPressed(SDL_SCANCODE_LALT) && isKeyDown(SDL_SCANCODE_RETURN)))
			switchScreenMode();

		//Update stuff
		if (gameFlags & 1)
		{
			if (gameFlags & 2)
				currentPlayer.update(true);
			else
				currentPlayer.update(false);
			updateNPC();
			updateBoss();
			playerHitMap();
			playerHitNpcs();
			playerHitBosses();
			if (gameFlags & 2)
				actWeapon();
			updateBullets();
			updateCarets();
			updateValueView();
			if (gameFlags & 2)
				currentPlayer.animate(true);
			else
				currentPlayer.animate(false);

			handleView();
		}

		updateFade();

		// -- DRAW -- //
		SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
		SDL_RenderClear(renderer);

		drawLevel(false);
		drawBoss();
		drawNPC();
		drawBullets();
		currentPlayer.draw();
		drawLevel(true);
		drawCarets();
		drawValueView();
		drawFade();

		//Open inventory and map system
		if (!(gameFlags & 4))
		{
			if (isKeyPressed(keyMenu))
			{
				//captureScreen(TEX_SCREENSHOT); //Redundant
				const int inventoryRet = openInventory();
				if (!inventoryRet)
					return 0;
				if (inventoryRet == 2)
					return 1;
				currentPlayer.cond &= ~player_interact;
			}
			else if (currentPlayer.equip & equip_mapSystem && isKeyPressed(keyMap))
			{
				captureScreen(TEX_SCREENSHOT);
				const int mapRet = openMapSystem();
				if (!mapRet)
					return 0;
				if (mapRet == 2)
					return 1;
			}
		}

		//Rotate weapons
		if (gameFlags & 2)
		{
			if (isKeyPressed(keyRotRight))
				rotateWeaponRight();
			else if (isKeyPressed(keyRotLeft))
				rotateWeaponLeft();
		}

		//Do TSC stuff
		const int tscResult = updateTsc();

		if (!tscResult)
			return 0;
		if (tscResult == 2)
			return 1;

		drawMapName(false);
		drawHud(!(gameFlags & 2));
		actFlash();
		drawTsc();
		debugFunction();
		if (!drawWindow())
			return 0;
	}

	return 0;
}

int gameUpdateMenu()
{
    logInfo("Started gameUpdateMenu");

	int select = 0;
	int anime = 0;

	const int version[4] = { 1, 0, 0, 6 };

	RECT rcVersion = { 152, 80, 208, 88 };
	RECT rcPeriod = { 152, 88, 208, 96 };

	RECT rcTitle = { 0, 0, 144, 32 };
	RECT rcNew = { 144, 0, 176, 16 };
	RECT rcLoad = { 144, 16, 176, 32 };
	RECT rcPixel = { 0, 0, 160, 16 };

	uint32_t frame = 0;
	const uint8_t frameOrder[] = { 0, 1, 0, 2 };

	changeOrg(mus_CaveStory);

	if (fileExists("Profile.dat"))
		select = 1;
	else
		select = 0;

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
				return 1;
		}

		if ((isKeyDown(SDL_SCANCODE_LALT) && isKeyPressed(SDL_SCANCODE_RETURN)) || (isKeyPressed(SDL_SCANCODE_LALT) && isKeyDown(SDL_SCANCODE_RETURN)))
			switchScreenMode();

		if (isKeyPressed(keyJump))
		{
			playSound(SFX_YNConfirm);
			break;
		}

		if (isKeyPressed(keyUp) || isKeyPressed(keyDown))
		{
			playSound(SFX_YNChangeChoice);
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
		RECT rcChar = { 0 + (frameOrder[(anime / 10) % 4] << 4), 16, 16 + (frameOrder[(anime / 10) % 4] << 4), 32 };
		drawTexture(sprites[0x10], &rcChar, (screenWidth >> 1) - 44, 127 + (20 * select));

		if (!drawWindow())
			return 0;
	}

	changeOrg(0);

	frame = SDL_GetTicks();

	while (SDL_GetTicks() < frame + 1000)
	{
		getKeys();
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		if (!drawWindow())
			return 0;
	}

	if (select == 0)
		initGame();
	else
		loadProfile();

	return PLAY;
}

int gameUpdateIntro()
{
    logInfo("Started gameUpdateIntro");

	init2();

	uint32_t frame = 0;
	loadLevel(72);
	startTscEvent(tsc, 100);
	changeOrg(0);

	//Set up fade
	initFade();
	fade.bMask = true;

	while (frame < 500)
	{
		frame++;

		//Handle events
		getKeys();

		if (isKeyDown(SDL_SCANCODE_ESCAPE))
		{
			const int escape = escapeMenu();

			if (!escape)
				return 0;
			if (escape == 2)
				return 1;
		}

		if ((isKeyDown(SDL_SCANCODE_LALT) && isKeyPressed(SDL_SCANCODE_RETURN)) || (isKeyPressed(SDL_SCANCODE_LALT) && isKeyDown(SDL_SCANCODE_RETURN)))
			switchScreenMode();

		if (isKeyPressed(keyJump) || isKeyDown(keyShoot))
		{
			break;
		}

		updateNPC();
		updateBoss();
		updateCarets();

		handleView();

		updateFade();

		// -- DRAW -- //
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		drawLevel(false);
		drawNPC();
		drawLevel(true);
		drawCarets();
		drawValueView();

		drawFade();

		//Do TSC stuff
		updateTsc();
		drawTsc();

		drawMapName(false);

		if (!drawWindow())
			return 0;
	}

	//wait 500 ms
	frame = SDL_GetTicks();
	while (SDL_GetTicks() < frame + 500)
	{
		getKeys();
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		if (!drawWindow())
			return 0;
	}

	return MENU;
}

int mainGameLoop()
{
	while (gameMode != 0)
	{
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
		default:
			doCustomError("Invalid gameMode");
			break;
		}
	}

	saveConfig();
	return 0;
}

