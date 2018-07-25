#pragma once
#include "common.h"

extern int screenWidth;
extern int screenHeight;

extern int screenScale;

int createWindow(int width, int height, int scale, bool fullscreen);

void loadImage(const char *file, SDL_Texture **tex);

void drawTexture(SDL_Texture *texture, RECT *rect, int x, int y);

void drawNumber(int value, int x, int y, bool bZero);

void drawRect(int x, int y, int w, int h);