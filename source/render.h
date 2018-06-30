#pragma once
#include "common.h"

int getTextureDimensions(SDL_Texture *texture);
void loadBMP(const char *file, SDL_Texture **tex);

void drawTexture(SDL_Texture *texture, int x, int y, bool fixed);