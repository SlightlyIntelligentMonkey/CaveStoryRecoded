#pragma once

#include <string>
#include "common.h"

struct SDL_Texture;
struct SDL_Window;
struct SDL_Rect;
struct SDL_Renderer;

extern SDL_Texture* sprites[40];

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern SDL_Rect rcDraw;
extern SDL_Rect rcImage;

extern int screenWidth;
extern int screenHeight;

extern int screenScale;

extern int framewait;

extern uint32_t windowFlags;

extern bool displayFpsCounter;

enum TextureNums
{
	TEX_TITLE = 0,
	TEX_PIXEL = 1,
	TEX_TILESET = 2,
	TEX_FADE = 6,
	TEX_ITEMIMAGE = 8,
	TEX_MAPSYSTEM = 9,
	TEX_SCREENSHOT = 10,
	TEX_ARMS = 11,
	TEX_ARMSIMAGE = 12,
	TEX_STAGEIMAGE = 14,
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
int createWindow(int width, int height, int scale);
void switchScreenMode();
bool drawWindow();
void captureScreen(enum TextureNums texture_id);

//Load texture function
void loadImage(const std::string& file, SDL_Texture **tex);

//Drawing functions
void setCliprect(const RECT *rect);
void drawTexture(SDL_Texture *texture, const RECT *rect, int x, int y);
void drawTextureNoScale(SDL_Texture *texture, const RECT *rect, int x, int y);
void drawTextureSize(SDL_Texture *texture, const RECT *rect, int x, int y, int w, int h);
void drawNumber(int value, int x, int y, bool bZero);
void drawString(int x, int y, const std::string& str, const uint8_t *flag = nullptr);
void drawRect(int x, int y, int w, int h);

void createTextureBuffer(enum TextureNums texture_id, int width, int height);

// 1 pixel is 0x200 units

template <typename T> constexpr inline int32_t pixelsToUnits(T x)
{
	return x * 0x200;
}

template <typename T> constexpr inline T unitsToPixels(T x)
{
	return x / 0x200;
}

template <typename T> constexpr inline int32_t secondsToFrames(T x)
{
    return x * 50;
}

template <typename T> constexpr inline T framesToSeconds(T x)
{
    return x / 50;
}
