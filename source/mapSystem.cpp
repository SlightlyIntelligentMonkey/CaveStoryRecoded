#include "game.h"
#include "input.h"
#include "script.h"
#include "hud.h"
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
	createTextureBuffer(TEX_MAPSYSTEM, levelWidth, levelHeight);

	//Do opening animation
	for (int f = 0; f <= 8; ++f)
	{
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

		// Draw screenshot
		if (SDL_RenderCopy(renderer, sprites[TEX_SCREENSHOT], nullptr, nullptr) != 0)
			doError();

		//Draw opening box and name
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		drawRect(screenWidth / 2 - f * levelWidth / 16, screenHeight / 2 - f * levelHeight / 16, f * levelWidth / 16 * 2, f * levelHeight / 16 * 2);
		drawMapName(true);

		//Present
		drawWindow();
	}

	while (true)
	{
		//Handle events
		getKeys(&events);
		if (events.type == SDL_QUIT || exitGame) { return 0; }

		//Exit if Z or X pressed
		if (isKeyPressed(keyJump) || isKeyPressed(keyShoot))
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
		if (SDL_RenderCopy(renderer, sprites[TEX_SCREENSHOT], nullptr, nullptr) != 0)
			doError();

		//Draw box
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		drawRect(screenWidth / 2 - levelWidth / 2 - 1, screenHeight / 2 - levelHeight / 2 - 1, levelWidth + 2, levelHeight + 2);

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

		const int off_x = screenWidth / 2 - levelWidth / 2;
		const int off_y = screenHeight / 2 - levelHeight / 2;

		if (SDL_SetRenderTarget(renderer, sprites[TEX_MAPSYSTEM]) < 0)
			doError();

		for (int i = 0; i < 2; i++)
		{
			if (line >= levelHeight)
			{
				break;
			}
			else
			{
				for (int x = 0; x < levelWidth; x++)
					drawTextureNoScale(sprites[TEX_TEXTBOX], &rcLevel[tileTypes[getTileAttribute(x, line)]], x, line);
				line++;
			}
		}

		if (SDL_SetRenderTarget(renderer, nullptr) < 0)
			doError();

		RECT rcMiniMap = { 0, 0, levelWidth, levelHeight };
		drawTexture(sprites[TEX_MAPSYSTEM], &rcMiniMap, off_x, off_y);

		//Draw player marker
		if ((++my_wait >> 3) & 1)
			drawTexture(sprites[TEX_TEXTBOX], &my_rect, off_x + my_x, off_y + my_y);

		//Draw name
		drawMapName(true);

		//Present
		drawWindow();
	}

	//Do closing animation
	for (int f = 8; f > 0; --f)
	{
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

		// Draw screenshot
		if (SDL_RenderCopy(renderer, sprites[TEX_SCREENSHOT], nullptr, nullptr) != 0)
			doError();

		//Draw closing box and name
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		drawRect(screenWidth / 2 - f * levelWidth / 16, screenHeight / 2 - f * levelHeight / 16, f * levelWidth / 16 * 2, f * levelHeight / 16 * 2);
		drawMapName(true);

		//Present
		drawWindow();
	}

	return 1;
}
