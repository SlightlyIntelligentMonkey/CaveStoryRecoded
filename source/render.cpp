#include "common.h"

SDL_Rect drawRectangle = {0};

int screenWidth = 0;
int screenHeight = 0;

int screenScale = 0;

//Create window
int createWindow(int width, int height, int scale, bool fullscreen) {
	int createWidth = width * scale;
	int createHeight = height * scale;

	screenWidth = width;
	screenHeight = height;
	screenScale = scale;

	//Set window
	if (!window)
		window = SDL_CreateWindow("Cave Story Remake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, createWidth, createHeight, 0);
	else
		SDL_SetWindowSize(window, createWidth, createHeight);

	//Set renderer
	if (!renderer)
		renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_RenderSetLogicalSize(renderer, screenWidth, screenHeight); //This is done to make sure the view is scaled up to the window (hardware sided)

	return 0;
}

//Texture and drawing stuff
void loadImage(const char *file, SDL_Texture **tex) {
	//Destroy previously existing texture and load new one
	if (*tex != NULL) { SDL_DestroyTexture(*tex); }
	*tex = IMG_LoadTexture(renderer, file);

	//Crash if anything failed
	if (*tex == NULL)
		doError();

	if (SDL_SetTextureBlendMode(*tex, SDL_BLENDMODE_BLEND) != 0)
		doError();
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
	RECT drawRect;

	number = std::abs(number);

	if (style == "hudrl")
	{
		int drawX = x + 0x3000;
		bool first = true; //Render if 0

		while (number > 0 || first)
		{
			first = false;

			int thisNumber = number % 10;

			drawRect = {thisNumber * 8, 56 + disappear, (thisNumber + 1) * 8, 64};
			drawTextureFromRect(sprites[0x1A], &drawRect, drawX, y - disappear, true);

			drawX -= 0x1000;

			number /= 10;
		}
	}
}

void drawRect(int x, int y, int w, int h)
{
	drawRectangle.x = x >> 9;
	drawRectangle.y = y >> 9;
	drawRectangle.w = w >> 9;
	drawRectangle.h = h >> 9;

	SDL_RenderFillRect(renderer, &drawRectangle);
}