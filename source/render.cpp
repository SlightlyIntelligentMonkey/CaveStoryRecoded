#include "common.h"

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
