#include "common.h"

SDL_Rect drawRectangle = {0};

void loadBMP(const char *file, SDL_Texture **tex) {
	//Destroy previously existing texture
	if (*tex != NULL) {
		SDL_DestroyTexture(*tex);
	}

	SDL_Surface *tempSurface = SDL_LoadBMP(file);

	if (tempSurface == NULL) { doError(); }

	SDL_SetColorKey(tempSurface, SDL_TRUE, 0);
	*tex = SDL_CreateTextureFromSurface(renderer, tempSurface);

	if (tex == NULL) { doError(); }

	SDL_FreeSurface(tempSurface);

	return;
}

void drawTexture(SDL_Texture *texture, int x, int y, bool fixed) {
	DrawRect.x = x >> 9;
	DrawRect.y = y >> 9;
	
	if (fixed == false) {
		DrawRect.x -= (viewX >> 9);
		DrawRect.y -= (viewY >> 9);
	}

	DrawRect.w = ImageRect.w;
	DrawRect.h = ImageRect.h;
	
	if (SDL_RenderCopy(renderer, texture, &ImageRect, &DrawRect) != 0)
		doError();

	return;
}

void drawTextureFromRect(SDL_Texture *texture, RECT *rect, int x, int y, bool fixed) {
	ImageRect = { rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top };

	DrawRect.x = x >> 9;
	DrawRect.y = y >> 9;

	if (fixed == false) {
		DrawRect.x -= (viewX >> 9);
		DrawRect.y -= (viewY >> 9);
	}

	DrawRect.w = ImageRect.w;
	DrawRect.h = ImageRect.h;

	if (SDL_RenderCopy(renderer, texture, &ImageRect, &DrawRect) != 0)
		doError();

	return;
}

void drawNumber(int number, int x, int y, const char *style, int disappear)
{
	if (style == "hudrl")
	{
		int drawX = x + 0x3000;
		bool first = true; //Render if 0

		while (number > 0 || first)
		{
			first = false;

			int thisNumber = number % 10;

			RECT drawRect = {thisNumber * 8, 56 + disappear, (thisNumber + 1) * 8, 64};
			drawTextureFromRect(sprites[0x1A], &drawRect, drawX, y - disappear, true);

			drawX -= 0x1000;

			number /= 10;
		}
	}
}

void drawRect(int x, int y, int w, int h)
{
	drawRectangle.x = x;
	drawRectangle.y = y;
	drawRectangle.w = w;
	drawRectangle.h = h;

	SDL_RenderFillRect(renderer, &drawRectangle);
}