#pragma once
#include "common.h"

void loadBMP(const char *file, SDL_Texture **tex);

void drawTexture(SDL_Texture *texture, int x, int y, bool fixed);
void drawRect(int x, int y, int w, int h);