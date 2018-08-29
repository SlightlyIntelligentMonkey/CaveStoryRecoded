#include "game.h"
#include "weapons.h"
#include "level.h"
#include "hud.h"
#include "script.h"
#include "fade.h"
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

#include <cstring>
#include <SDL_scancode.h>
#include <SDL_timer.h>
#include <SDL_render.h>
#include <SDL_events.h>

using std::memset;

//Teleporter Menu
PERMIT_STAGE permitStage[PERMITSTAGES];
int selectedStage;
int stageSelectTitleY;
int stageSelectFlash;

void moveStageSelectCursor()
{
	int stageNo;

	for (stageNo = 0; stageNo < PERMITSTAGES && permitStage[stageNo].index != 0; ++stageNo);

	if (stageNo)
	{
		//Left and right move
		if (isKeyPressed(keyLeft))
			--selectedStage;
		if (isKeyPressed(keyRight))
			++selectedStage;

		//Wrap around
		if (selectedStage < 0)
			selectedStage = stageNo - 1;
		if (selectedStage >= stageNo)
			selectedStage = 0;

		//Run event and play sound
		if (isKeyPressed(keyLeft) || isKeyPressed(keyRight))
			startTscEvent(permitStage[selectedStage].index + 1000);
		if (isKeyPressed(keyLeft) || isKeyPressed(keyRight))
			playSound(SFX_YNChangeChoice);
	}
}

void drawStageSelect()
{
	RECT rcCur[2];
	RECT rcTitle1;
	RECT rcStage;

	int stageNo;

	rcCur[0] = { 80, 88, 112, 104 };
	rcCur[1] = { 80, 104, 112, 120 };
	rcTitle1 = { 80, 64, 144, 72 };

	if (stageSelectTitleY > 46)
		--stageSelectTitleY;

	drawTexture(sprites[TEX_TEXTBOX], &rcTitle1, (screenWidth / 2) - 32, stageSelectTitleY);
	for (stageNo = 0; stageNo < PERMITSTAGES && permitStage[stageNo].index != 0; stageNo++);

	++stageSelectFlash;

	if (stageNo)
	{
		const int stageX = (-40 * stageNo + screenWidth) / 2;

		//Draw everything now
		drawTexture(sprites[TEX_TEXTBOX], &rcCur[(stageSelectFlash >> 1) & 1], stageX + 40 * selectedStage, 64);

		for (int i = 0; i < PERMITSTAGES && permitStage[i].index; ++i)
		{
			rcStage.left = 32 * (permitStage[i].index % 8);
			rcStage.right = rcStage.left + 32;
			rcStage.top = 16 * (permitStage[i].index / 8);
			rcStage.bottom = rcStage.top + 16;

			drawTexture(sprites[0xE], &rcStage, stageX + 40 * i, 64);
		}
	}
}

int stageSelect(int *runEvent)
{
	//Keep track of old one
	char oldScript[260];
	strcpy(oldScript, tsc.path);

	//Init some stuff
	selectedStage = 0;
	stageSelectTitleY = 54;

	//Load stage select tsc
	loadTsc2("data/StageSelect.tsc");
	startTscEvent(permitStage[selectedStage].index + 1000);

	while (true)
	{
		//Framerate limiter
		const Uint32 timeNow = SDL_GetTicks();
		const Uint32 timeNext = framerateTicks + framerate;

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
		if (events.type == SDL_QUIT || exitGame) { return 0; }

		if (isKeyDown(SDL_SCANCODE_ESCAPE))
		{
			const int escape = escapeMenu();

			if (!escape)
				return 0;
			if (escape == 2)
				return 1;
		}

		//Update menu
		moveStageSelectCursor();

		const int tscResult = updateTsc();
		if (!tscResult)
			return 0;
		if (tscResult == 2)
			return 2;

		//Draw background
		SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
		SDL_RenderClear(renderer);

		drawLevel(false);
		drawNPC();
		currentPlayer.draw();
		drawLevel(true);
		drawCarets();
		drawValueView();

		//Draw menu
		drawStageSelect();
		drawTsc();

		//Select
		if (isKeyPressed(keyJump))
		{
			stopTsc();
			loadStageTsc(oldScript);
			*runEvent = permitStage[selectedStage].event;
			return 1;
		}

		//Cancel
		if (isKeyPressed(keyShoot))
		{
			stopTsc();
			loadStageTsc(oldScript);
			*runEvent = 0;
			return 1;
		}

		//Present
		SDL_RenderPresent(renderer);
	}
}
