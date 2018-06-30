#include "input.h"

Uint8 key_prev[SDL_NUM_SCANCODES] = { 0 };
Uint8 key[SDL_NUM_SCANCODES] = { 0 };
int key_array_size = 0;

void getKeys(SDL_Event *event)
{
	SDL_PollEvent(event);
	memcpy(&key_prev, key, key_array_size);
	memcpy(&key, SDL_GetKeyboardState(&key_array_size), SDL_NUM_SCANCODES);
	return;
}

bool keyDown(int keynum)
{
	if (key[keynum] == 1)
	{
		return true;
	}
	return false;
}

bool keyPressed(int keynum)
{
	if (key_prev[keynum] == 0 && key[keynum] == 1)
	{
		return true;
	}
	return false;
}