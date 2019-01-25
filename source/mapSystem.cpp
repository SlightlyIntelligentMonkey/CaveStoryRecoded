#include "game.h"

#include <string>
#include <cstring>
#include <SDL_events.h>
#include <SDL_render.h>
#include "input.h"
#include "script.h"
#include "hud.h"
#include "sound.h"
#include "render.h"
#include "stage.h"
#include "player.h"
#include "caret.h"
#include "valueview.h"
#include "log.h"
#include "main.h"

//hey brackohnn i stoel your thing
const uint8_t tileTypes[0x100] =
{
	0, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	1, 3, 3, 2, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	2, 1, 1, 2, 2, 1, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	1, 3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	2, 1, 1, 2, 2, 1, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
};

//Open and update
int openMapSystem()
{
    logInfo("Started openMapSystem");

	//Init player marker
	RECT my_rect;
	int my_x;
	int my_y;
	int line = 0; //What line the map is drawing currently
	int my_wait = 0; //The player marker's blinking

	my_rect.left = 0;
	my_rect.top = 57;
	my_rect.right = 1;
	my_rect.bottom = 58;
	my_x = (currentPlayer.x / 0x200 + 8) / 16;
	my_y = (currentPlayer.y / 0x200 + 8) / 16;

	//Create map buffer
	createTextureBuffer(TEX_MAPSYSTEM, gMap.width, gMap.height);

	//Do opening animation
	for (int f = 0; f <= 8; ++f)
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

		// Draw screenshot
		drawTextureNoScale(gSprites[TEX_SCREENSHOT], nullptr, 0, 0);

		//Draw opening box and name
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		drawRect(gScreenWidth / 2 - f * gMap.width / 16, gScreenHeight / 2 - f * gMap.height / 16, f * gMap.width / 16 * 2, f * gMap.height / 16 * 2);
		drawMapName(true);

		//Present
		if (!drawWindow())
			return 0;
	}

	while (true)
	{
		//Handle events
		getKeys();

		//Exit if Z or X pressed
		if (isKeyPressed(gKeyJump) || isKeyPressed(gKeyShoot))
			break;

		if (isKeyDown(SDL_SCANCODE_ESCAPE))
		{
			const int escape = escapeMenu();

			if (!escape)
				return 0;
			if (escape == 2)
				return 2;
		}

		// Draw screenshot
		drawTextureNoScale(gSprites[TEX_SCREENSHOT], nullptr, 0, 0);

		//Draw box
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		drawRect(gScreenWidth / 2 - gMap.width / 2 - 1, gScreenHeight / 2 - gMap.height / 2 - 1, gMap.width + 2, gMap.height + 2);

		//Draw contents
		RECT rcLevel[4];
		rcLevel[0].left = 240;
		rcLevel[0].top = 24;
		rcLevel[0].right = 241;
		rcLevel[0].bottom = 25;
		rcLevel[1].left = 241;
		rcLevel[1].top = 24;
		rcLevel[1].right = 242;
		rcLevel[1].bottom = 25;
		rcLevel[2].left = 242;
		rcLevel[2].top = 24;
		rcLevel[2].right = 243;
		rcLevel[2].bottom = 25;
		rcLevel[3].left = 243;
		rcLevel[3].top = 24;
		rcLevel[3].right = 244;
		rcLevel[3].bottom = 25;

		const int off_x = gScreenWidth / 2 - gMap.width / 2;
		const int off_y = gScreenHeight / 2 - gMap.height / 2;

		if (SDL_SetRenderTarget(gRenderer, gSprites[TEX_MAPSYSTEM]) < 0)
			doError();

		for (int i = 0; i < 2; i++)
		{
			if (line >= gMap.height)
				break;
			for (int x = 0; x < gMap.width; x++)
				drawTextureNoScale(gSprites[TEX_TEXTBOX], &rcLevel[tileTypes[getTileAttribute(x, line)]], x, line);
			line++;
		}

		if (SDL_SetRenderTarget(gRenderer, nullptr) < 0)
			doError();

		RECT rcMiniMap = { 0, 0, gMap.width, gMap.height };
		drawTexture(gSprites[TEX_MAPSYSTEM], &rcMiniMap, off_x, off_y);

		//Draw player marker
		if ((++my_wait >> 3) & 1)
			drawTexture(gSprites[TEX_TEXTBOX], &my_rect, off_x + my_x, off_y + my_y);

		//Draw name
		drawMapName(true);

		//Present
		if (!drawWindow())
			return 0;
	}

	//Do closing animation
	for (int f = 8; f > 0; --f)
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

		// Draw screenshot
		drawTextureNoScale(gSprites[TEX_SCREENSHOT], nullptr, 0, 0);

		//Draw closing box and name
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		drawRect(gScreenWidth / 2 - f * gMap.width / 16, gScreenHeight / 2 - f * gMap.height / 16, f * gMap.width / 16 * 2, f * gMap.height / 16 * 2);
		drawMapName(true);

		//Present
		if (!drawWindow())
			return 0;
	}

	return 1;
}
