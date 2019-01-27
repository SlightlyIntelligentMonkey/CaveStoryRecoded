#include "game.h"

#include <string>
#include <cstring>
#include <SDL_events.h>
#include <SDL_render.h>
#include "input.h"
#include "script.h"
#include "sound.h"
#include "render.h"
#include "stage.h"
#include "player.h"
#include "caret.h"
#include "valueview.h"
#include "main.h"

//Teleporter Menu
PERMIT_STAGE gPermitStage[PERMITSTAGES];
int gSelectedStage;
int gStageSelectTitleY;
int gStageSelectFlash;

void moveStageSelectCursor()
{
	size_t stageNo;

	for (stageNo = 0; stageNo < PERMITSTAGES && gPermitStage[stageNo].index != 0; ++stageNo);

	if (stageNo)
	{
		//Left and right move
		if (isKeyPressed(gKeyLeft))
			--gSelectedStage;
		if (isKeyPressed(gKeyRight))
			++gSelectedStage;

		//Wrap around
		if (gSelectedStage < 0)
			gSelectedStage = stageNo - 1;
		if (static_cast<size_t>(gSelectedStage) >= stageNo)
			gSelectedStage = 0;

		//Run event and play sound
		if (isKeyPressed(gKeyLeft) || isKeyPressed(gKeyRight))
			startTscEvent(gTsc, gPermitStage[gSelectedStage].index + 1000);
		if (isKeyPressed(gKeyLeft) || isKeyPressed(gKeyRight))
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

	if (gStageSelectTitleY > 46)
		--gStageSelectTitleY;

	drawTexture(gSprites[TEX_TEXTBOX], &rcTitle1, (gScreenWidth / 2) - 32, ((gScreenHeight - 240) / 2) + gStageSelectTitleY);
	for (stageNo = 0; stageNo < PERMITSTAGES && gPermitStage[stageNo].index != 0; stageNo++);

	++gStageSelectFlash;

	if (stageNo)
	{
		const int stageX = (-40 * static_cast<int>(stageNo) + gScreenWidth) / 2;

		//Draw everything now
		drawTexture(gSprites[TEX_TEXTBOX], &rcCur[(gStageSelectFlash >> 1) & 1], stageX + 40 * gSelectedStage, ((gScreenHeight - 240) / 2) + 64);

		for (size_t i = 0; i < PERMITSTAGES && gPermitStage[i].index; ++i)
		{
			rcStage.left = 32 * (gPermitStage[i].index % 8);
			rcStage.right = rcStage.left + 32;
			rcStage.top = 16 * (gPermitStage[i].index / 8);
			rcStage.bottom = rcStage.top + 16;

			drawTexture(gSprites[0xE], &rcStage, stageX + 40 * i, ((gScreenHeight - 240) / 2) + 64);
		}
	}
}

int stageSelect(int *runEvent)
{
	if (runEvent == nullptr)
		doCustomError("runEvent was nullptr in stageSelect");

	//Keep track of old one
	std::string oldScript(gTsc.path);

	//Init some stuff
	gSelectedStage = 0;
	gStageSelectTitleY = 54;

	captureScreen(TEX_SCREENSHOT);

	//Load stage select tsc
	loadTsc2("data/StageSelect.tsc");
	startTscEvent(gTsc, gPermitStage[gSelectedStage].index + 1000);

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
		drawTextureNoScale(gSprites[TEX_SCREENSHOT], nullptr, 0, 0);

		//Draw menu
		drawStageSelect();
		drawTsc();

		//Select
		if (isKeyPressed(gKeyJump))
		{
			stopTsc(gTsc);
			loadStageTsc(oldScript);
			*runEvent = gPermitStage[gSelectedStage].event;
			return 1;
		}

		//Cancel
		if (isKeyPressed(gKeyShoot))
		{
			stopTsc(gTsc);
			loadStageTsc(oldScript);
			*runEvent = 0;
			return 1;
		}

		//Present
		if (!drawWindow())
			return 0;
	}
}

