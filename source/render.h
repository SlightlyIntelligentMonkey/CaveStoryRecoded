#pragma once
#include "common.h"

void loadBMP(const char *file, SDL_Texture **tex);

void drawTexture(SDL_Texture *texture, int x, int y, bool fixed);
void drawTextureFromRect(SDL_Texture *texture, RECT *rect, int x, int y, bool fixed);

void drawNumber(int number, int x, int y, const char *style, int disappear);

void drawRect(int x, int y, int w, int h);