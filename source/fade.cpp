#include <cstring>
#include <algorithm>
#include <SDL_render.h>

#include "render.h"
#include "fade.h"
#include "game.h"

using std::memset;
using std::max;

FADE_STATE fade;

const int fadeFrames = 16;

void initFade() 
{
	memset(&fade, 0, sizeof(FADE_STATE));
}

void updateFade() 
{
	if (fade.mode)
	{
		fade.bMask = false;

		if (++fade.count > (max(screenWidth, screenHeight) / 16 + fadeFrames))
		{
			fade.bMask = (fade.mode == 2);
			fade.mode = 0;
		}
	}
}

void drawFade()
{
	RECT rect = { 0, 0, 0, 16 };

	int xt;
	int yt;
	int frame = 0;

	const int w = screenWidth / 16 + 1;
	const int h = screenHeight / 16 + 1;
	const int frames = (max(screenWidth, screenHeight) / 16 + fadeFrames);

	if (fade.bMask)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
		SDL_RenderClear(renderer);
	}
	else if (fade.mode == 1)
	{
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				switch (fade.dir)
				{
				case 0:
					xt = (frames - fade.count);
					frame = xt - x;
					break;

				case 1:
					yt = (frames - fade.count);
					frame = yt - y;
					break;

				case 2:
					xt = w - (frames - fade.count);
					frame = x - xt;
					break;

				case 3:
					yt = h - (frames - fade.count);
					frame = y - yt;
					break;

				case 4:
					frame = fadeFrames - (fade.count - abs(x - (screenWidth >> 5)) - abs(y - (screenHeight >> 5)));
					break;
				}

				if (frame > 0)
				{
					rect.left = std::min(frame, fadeFrames - 1) * 16;
					rect.right = rect.left + 16;

					const int fixX = (w * 16) - screenWidth;
					const int fixY = (h * 16) - screenHeight;
					drawTexture(sprites[TEX_FADE], &rect, x * 16 - fixX, y * 16 - fixY);
				}
			}
		}
	}
	else if (fade.mode == 2)
	{
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				switch (fade.dir)
				{
				case 0:
					xt = w - fade.count;
					frame = x - xt;
					break;

				case 1:
					yt = h - fade.count;
					frame = y - yt;
					break;

				case 2:
					xt = fade.count;
					frame = xt - x;
					break;

				case 3:
					yt = fade.count;
					frame = yt - y;
					break;

				case 4:
					frame = fadeFrames - ((frames - fade.count) - abs(x - (screenWidth >> 5)) - abs(y - (screenHeight >> 5)));
					break;
				}

				if (frame > 0)
				{
					rect.left = std::min(frame, fadeFrames - 1) * 16;
					rect.right = rect.left + 16;

					const int fixX = (w * 16) - screenWidth;
					const int fixY = (h * 16) - screenHeight;
					drawTexture(sprites[TEX_FADE], &rect, x * 16 - fixX, y * 16 - fixY);
				}
			}
		}
	}
}

struct FLASH
{
	int x;
	int y;
	flashModes mode;
	int timer;
	int vW;
	int vH;
	int hW;
	int hH;
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
};
FLASH flash = 
{ 
	0,
	0,
	none,
	0,
	0,
	0,
	0,
	0,
	0xFF,
	0xFF,
	0xFE,
	0xFF
};

void setFlash(int x, int y, flashModes mode, int length)
{
	flash.x = x;
	flash.y = y;
	flash.mode = mode;
	flash.timer = length;
	flash.vW = 0;
	flash.vH = 0;
	flash.hW = 0;
	flash.hH = 0;
	return;
}

void setFlashColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	flash.r = r;
	flash.g = g;
	flash.b = b;
	flash.a = a;
	return;
}

void flashExplosion()
{	
	if (1)
	{
		flash.hH -= flash.hH / 8;
		if (flash.hH <= 0)
			flash.mode = none;
	}
	else
	{

	}

	if (!1)
		drawRect((flash.x - viewport.x) - (flash.vW / 2), (flash.y - viewport.y) - (flash.vH / 2), flash.vW, flash.vH);
	drawRect((flash.x - viewport.x) - (flash.hW / 2), (flash.y - viewport.y) - (flash.hH / 2), flash.hW, flash.hH);

	return;
}

void flashNormal()
{
	--flash.timer;
	if (flash.timer / 2 & 1)
	{
		SDL_SetRenderDrawColor(renderer, flash.r, flash.g, flash.b, flash.a);
		SDL_RenderClear(renderer);
	}
	if (flash.timer < 0)
		flash.mode = none;
	return;
}

void actFlash()
{
	switch (flash.mode)
	{
	case(1):
		flashNormal();
		break;
	case(2):
		flashExplosion();
		break;
	default:
		break;
	}

	return;
}