#include <SDL_render.h>

#include "flash.h"
#include "render.h"
#include "game.h"


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

void setFlashColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
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
		SDL_RenderFillRect(renderer, nullptr);
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
