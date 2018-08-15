#include "common.h"

SDL_Rect drawRectangle = { 0 };

int screenWidth = 0;
int screenHeight = 0;
int screenScale = 0;

int prevWidth = 0;
int prevHeight = 0;
int prevScale = 0;

int windowFlags = 0;

int charWidth = 6;
int charHeight = 12;

//Create window
int createWindow(int width, int height, int scale, bool fullscreen) {
	const int createWidth = width * scale;
	const int createHeight = height * scale;

	screenWidth = width;
	screenHeight = height;
	screenScale = scale;

	//Set window
	if (!window)
		window = SDL_CreateWindow("Cave Story Engine",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			createWidth, createHeight,
			NULL);
	else
		SDL_SetWindowSize(window, createWidth, createHeight);

	//Set renderer
	if (!renderer)
		renderer = SDL_CreateRenderer(window, -1, 0);

	//SDL_RenderSetLogicalSize(renderer, screenWidth, screenHeight);

	return 0;
}

void switchScreenMode()
{
	windowFlags ^= SDL_WINDOW_FULLSCREEN_DESKTOP;

	//Unlike prevWidth and Height, this is used for fixing the view when going between fullscreen and windowed mode
	const int lastWidth = screenWidth;
	const int lastHeight = screenHeight;

	if (windowFlags & SDL_WINDOW_FULLSCREEN_DESKTOP)
	{
		SDL_DisplayMode dm;

		if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
			doError();

		prevWidth = screenWidth;
		prevHeight = screenHeight;
		prevScale = screenScale;

		screenWidth = (dm.w * 240) / dm.h;
		screenHeight = 240;
		screenScale = dm.h / 240;
	}
	else
	{
		screenWidth = prevWidth;
		screenHeight = prevHeight;
		screenScale = prevScale;
	}

	SDL_RenderSetLogicalSize(renderer, screenWidth * screenScale, screenHeight * screenScale);

	//Ensure that the view is shifted properly
	viewport.x += (lastWidth - screenWidth) * 0x100;
	viewport.y += (lastHeight - screenHeight) * 0x100;

	SDL_SetWindowSize(window, screenWidth * screenScale, screenHeight * screenScale);
	SDL_SetWindowFullscreen(window, windowFlags);
	return;
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

void drawTexture(SDL_Texture *texture, const RECT *rect, int x, int y) {
	ImageRect = { rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top };

	DrawRect.x = x * screenScale;
	DrawRect.y = y * screenScale;
	DrawRect.w = ImageRect.w * screenScale;
	DrawRect.h = ImageRect.h * screenScale;

	if (SDL_RenderCopy(renderer, texture, &ImageRect, &DrawRect) != 0)
		doError();

	return;
}

void drawTextureSize(SDL_Texture *texture, const RECT *rect, int x, int y, int w, int h) {
	ImageRect = { rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top };

	DrawRect.x = x * screenScale;
	DrawRect.y = y * screenScale;
	DrawRect.w = w * screenScale;
	DrawRect.h = h * screenScale;

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

bool isMultibyte(uint8_t c)
{
	if (c > 0x80u && c <= 0x9Fu)
		return true;
	if (c <= 0xDFu || c > 0xEFu)
		return false;
	return true;
}

void drawString(int x, int y, const char *str)
{
	RECT rcChar;

	for (int i = 0; ; i++)
	{
		if (str[i])
		{
			int charValue;

			if (isMultibyte(str[i]))
				charValue = str[i] * 0x100 + str[i += 1];
			else
				charValue = str[i];

			rcChar.left = (((charValue - 0x20) % 32) * 12);
			rcChar.top = (((charValue - 0x20) >> 5) * 24);
			rcChar.right = rcChar.left + 12;
			rcChar.bottom = rcChar.top + 24;

			drawTextureSize(sprites[0x26], &rcChar, x + (i * 6), y, charWidth, charHeight);
		}
		else
		{
			break;
		}
	}
}

void drawRect(int x, int y, int w, int h)
{
	drawRectangle.x = x * screenScale;
	drawRectangle.y = y * screenScale;
	drawRectangle.w = w * screenScale;
	drawRectangle.h = h * screenScale;

	SDL_RenderFillRect(renderer, &drawRectangle);
}
