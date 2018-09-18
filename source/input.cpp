#include "input.h"
#include "filesystem.h"

#include <cstring>
#include <SDL_keyboard.h>
#include <SDL_events.h>
#include "main.h"

using std::memcpy;

SDL_GameController* currentGamepad = NULL;

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

static uint8_t key_prev[SDL_NUM_SCANCODES] = { 0 };
static uint8_t key[SDL_NUM_SCANCODES] = { 0 };
static int key_array_size = 0;

static uint8_t controller_buttons_prev[SDL_CONTROLLER_BUTTON_MAX];
static uint8_t controller_buttons[SDL_CONTROLLER_BUTTON_MAX];

void initGamepad()
{
	if (fileExists("data/gamecontrollerdb.txt") && SDL_GameControllerAddMappingsFromFile("data/gamecontrollerdb.txt") < 0)
		doError();

	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			currentGamepad = SDL_GameControllerOpen(i);

			if (currentGamepad)
				break;
		}
	}
}

void getKeys()
{
	memcpy(key_prev, key, key_array_size);
	memcpy(key, SDL_GetKeyboardState(&key_array_size), SDL_NUM_SCANCODES);

	memcpy(controller_buttons_prev, controller_buttons, sizeof(controller_buttons));
}

bool isKeyDown(int keynum)
{
	if (currentConfig->useGamepad)
		return (controller_buttons[keynum] == SDL_PRESSED);
	else
		return (key[keynum] == 1);
}

bool isKeyPressed(int keynum)
{
	if (currentConfig->useGamepad)
		return (controller_buttons_prev[keynum] == SDL_RELEASED && controller_buttons[keynum] == SDL_PRESSED);
	else
		return (key_prev[keynum] == 0 && key[keynum] == 1);
}

bool handleEvents()
{
	bool focusGained = true;

	while (SDL_PollEvent(nullptr) || !focusGained)
	{
		SDL_Event event;
		SDL_WaitEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			return false;

		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
				focusGained = true;
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
				focusGained = false;

			break;

		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
			controller_buttons[event.cbutton.button] = event.cbutton.state;
			break;
		}
	}

	return true;
}
