#include <SDL_render.h>
#include <math.h>

#include "flash.h"
#include "render.h"
#include "game.h"
#include "stage.h"


struct FLASH
{
	int x;
	int y;
	flashModes mode;
	int timer;
	int vW;
	int hH;
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
};
FLASH gFlash =
{
	0,
	0,
	flashModes::none,
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
	gFlash.x = x;
	gFlash.y = y;
	gFlash.mode = mode;
	gFlash.timer = length;
	gFlash.vW = 0;
	gFlash.hH = 0;
	return;
}

void setFlashColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	gFlash.r = r;
	gFlash.g = g;
	gFlash.b = b;
	gFlash.a = a;
	return;
}

void flashExplosion()
{
	static bool explosionEnd = false;
	static int explInc = 0;
	static float h = 0;

	if (explosionEnd == true)
	{
		h -= h / 8;
		gFlash.hH = static_cast<int>(h);
		if (gFlash.hH <= 0)
		{
			gFlash.mode = flashModes::none;
			explInc = 0;
			h = 0;
			explosionEnd = false;
			return;
		}
	}
	else
	{
		gFlash.timer += 512;
		explInc += gFlash.timer;

		gFlash.vW = explInc / 512;
		gFlash.hH = explInc / 512;

		if (explInc > 655360)
		{
			explosionEnd = true;
			gFlash.timer = 0;
			explInc = 122880;
			h = gScreenHeight;
		}
	}

	SDL_SetRenderDrawColor(gRenderer, gFlash.r, gFlash.g, gFlash.b, gFlash.a);
	if (explosionEnd != true)
		drawRect(unitsToPixels(gFlash.x - pixelsToUnits(gFlash.vW/2) - gViewport.x), 0, gFlash.vW, gScreenHeight);
	drawRect(0, unitsToPixels(gFlash.y - pixelsToUnits(gFlash.hH/2) - gViewport.y), gScreenWidth, gFlash.hH);
}

void flashNormal()
{
	--gFlash.timer;
	if (gFlash.timer / 2 & 1)
	{
		SDL_SetRenderDrawColor(gRenderer, gFlash.r, gFlash.g, gFlash.b, gFlash.a);
		SDL_RenderFillRect(gRenderer, nullptr);
	}
	if (gFlash.timer < 0)
		gFlash.mode = flashModes::none;
	return;
}

void actFlash()
{
	switch (gFlash.mode)
	{
	case flashModes::normal:
		flashNormal();
		break;
	case flashModes::explosion:
		flashExplosion();
		break;

	case flashModes::none:
	default:
		break;
	}

	return;
}
