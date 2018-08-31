#include "game.h"
#include "input.h"
#include "script.h"
#include "sound.h"
#include "render.h"
#include "level.h"
#include "player.h"
#include "caret.h"
#include "valueview.h"

#include <string>
#include <cstring>
#include <SDL_events.h>
#include <SDL_render.h>

using std::string;
using std::strcpy;

//Teleporter Menu
PERMIT_STAGE permitStage[PERMITSTAGES];
int selectedStage;
int stageSelectTitleY;
int stageSelectFlash;

void moveStageSelectCursor()
{
	size_t stageNo;

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
		if (static_cast<size_t>(selectedStage) >= stageNo)
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

	size_t stageNo;

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
		const int stageX = (-40 * static_cast<int>(stageNo) + screenWidth) / 2;

		//Draw everything now
		drawTexture(sprites[TEX_TEXTBOX], &rcCur[(stageSelectFlash >> 1) & 1], stageX + 40 * selectedStage, 64);

		for (size_t i = 0; i < PERMITSTAGES && permitStage[i].index; ++i)
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
	if (runEvent == nullptr)
		doCustomError("runEvent was nullptr in stageSelect");

	//Keep track of old one
	string oldScript(tsc.path);

	//Init some stuff
	selectedStage = 0;
	stageSelectTitleY = 54;

	captureScreen(TEX_SCREENSHOT);

	//Load stage select tsc
	loadTsc2("data/StageSelect.tsc");
	startTscEvent(permitStage[selectedStage].index + 1000);

	while (true)
	{
		delimitFramerate();

		//Handle events
		getKeys(&events);
		if (events.type == SDL_QUIT || exitGame) { return 0; }

		if (isKeyDown(SDL_SCANCODE_ESCAPE))
		{
			const int escape = escapeMenu();

			if (!escape)
				return 0;
			if (escape == 2)
				return 2;
		}

		//Update menu
		moveStageSelectCursor();

		const int tscResult = updateTsc();
		if (!tscResult)
			return 0;
		if (tscResult == 2)
			return 2;

		// Draw screenshot
		if (SDL_RenderCopy(renderer, sprites[TEX_SCREENSHOT], nullptr, nullptr) != 0)
			doError();

		//Draw menu
		drawStageSelect();
		drawTsc();

		//Select
		if (isKeyPressed(keyJump))
		{
			stopTsc();
			loadStageTsc(oldScript.c_str());
			*runEvent = permitStage[selectedStage].event;
			return 1;
		}

		//Cancel
		if (isKeyPressed(keyShoot))
		{
			stopTsc();
			loadStageTsc(oldScript.c_str());
			*runEvent = 0;
			return 1;
		}

		//Present
		drawWindow();
	}
}

