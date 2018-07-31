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
		window = SDL_CreateWindow("Cave Story Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, createWidth, createHeight, 0);
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

void drawTexture(SDL_Texture *texture, RECT *rect, int x, int y) {
	ImageRect = { rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top };

	DrawRect.x = x;
	DrawRect.y = y;

	DrawRect.w = ImageRect.w;
	DrawRect.h = ImageRect.h;

	if (SDL_RenderCopy(renderer, texture, &ImageRect, &DrawRect) != 0)
		doError();

	return;
}

void drawNumber(int value, int x, int y, bool bZero)
{
	RECT numbRect;

	int offset = 0;
	int pos = 1000; //replacing an array a day keeps the doctor away

	if (value > 9999)
		value = 9999;

	int count = 0;
	while (offset < 4)
	{
		int drawValue = 0;

		while (pos <= value)
		{
			value -= pos;

			++drawValue;
			++count;
		}

		if (bZero && offset == 2 || count != 0 || offset == 3) //I don't really understand this
		{
			numbRect = { drawValue << 3, 56, (drawValue + 1) << 3, 64 };
			drawTexture(sprites[26], &numbRect, x + (offset << 3), y);
		}

		offset++;
		pos /= 10;
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
