#pragma once
#include "common.h"

extern int screenWidth;
extern int screenHeight;

extern int screenScale;

enum TextureNums
{
	TEX_TITLE = 0,
	TEX_PIXEL = 1,
	TEX_TILESET = 2,
	TEX_FADE = 6,
	TEX_ITEMIMAGE = 8,
	TEX_SCREENSHOT = 10,
	TEX_ARMS = 11,
	TEX_ARMSIMAGE = 12,
	TEX_MAPNAMEBUF = 13,
	TEX_LOADING = 15,
	TEX_MYCHAR = 16,
	TEX_BULLET = 17,
	TEX_CARET = 19,
	TEX_NPC_SYM = 20,
	TEX_NPC_1 = 21,
	TEX_NPC_2 = 22,
	TEX_NPC_REGU = 23,
	TEX_TEXTBOX = 26,
	TEX_FACE = 27,
	TEX_BACKGROUND = 28,
};

//Functions for handling the window
int createWindow(int width, int height, int scale, bool fullscreen) noexcept;
void switchScreenMode();
void drawWindow();
void captureScreen(enum TextureNums texture_id);

//Load texture function
void loadImage(const char *file, SDL_Texture **tex);

//Drawing functions
void setCliprect(const RECT *rect) noexcept;
void drawTexture(SDL_Texture *texture, const RECT *rect, int x, int y);
void drawTextureSize(SDL_Texture *texture, const RECT *rect, int x, int y, int w, int h);
void drawNumber(int value, int x, int y, bool bZero);
void drawString(int x, int y, const char *str, const uint8_t *flag = nullptr);
void drawRect(int x, int y, int w, int h) noexcept;

template <typename T>
inline T pixelToCoord(T x) noexcept
{
	return x << 9;
}

template <typename T>
inline T coordToPixel(T x) noexcept
{
	return x >> 9;
}
