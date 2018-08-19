#include "input.h"

#include <SDL_keyboard.h>
#include <SDL_events.h>

int keyLeft = SDL_SCANCODE_LEFT;
int keyRight = SDL_SCANCODE_RIGHT;
int keyUp = SDL_SCANCODE_UP;
int keyDown = SDL_SCANCODE_DOWN;
int keyJump = SDL_SCANCODE_Z;
int keyShoot = SDL_SCANCODE_X;
int keyMenu = SDL_SCANCODE_Q;
int keyMap = SDL_SCANCODE_W;

int keyRotLeft = SDL_SCANCODE_A;
int keyRotRight = SDL_SCANCODE_S;

Uint8 key_prev[SDL_NUM_SCANCODES] = { 0 };
Uint8 key[SDL_NUM_SCANCODES] = { 0 };
int key_array_size = 0;

void getKeys(SDL_Event *event)
{
	SDL_PollEvent(event);
	memcpy(&key_prev, key, key_array_size);
	memcpy(&key, SDL_GetKeyboardState(&key_array_size), SDL_NUM_SCANCODES);
}

bool isKeyDown(int keynum)
{
	return (key[keynum] == 1);
}

bool isKeyPressed(int keynum)
{
	return (key_prev[keynum] == 0 && key[keynum] == 1);
}
