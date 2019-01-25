#include "input.h"
#include "filesystem.h"

#include <cstring>
#include <SDL_keyboard.h>
#include <SDL_events.h>
#include "main.h"

bool gUseGamepad = false;
SDL_GameController* currentGamepad = NULL;

int gKeyLeft = SDL_SCANCODE_LEFT;
int gKeyRight = SDL_SCANCODE_RIGHT;
int gKeyUp = SDL_SCANCODE_UP;
int gKeyDown = SDL_SCANCODE_DOWN;
int gKeyJump = SDL_SCANCODE_Z;
int gKeyShoot = SDL_SCANCODE_X;
int gKeyMenu = SDL_SCANCODE_Q;
int gKeyMap = SDL_SCANCODE_W;
int gKeyRotateLeft = SDL_SCANCODE_A;
int gKeyRotateRight = SDL_SCANCODE_S;

static uint8_t gKeyPrev[SDL_NUM_SCANCODES] = { 0 };
static uint8_t gKey[SDL_NUM_SCANCODES] = { 0 };
static int gKeyArraySize = 0;

static bool gControllerButtonsPressed[SDL_CONTROLLER_BUTTON_MAX];
static bool gControllerButtonsHeldPrev[SDL_CONTROLLER_BUTTON_MAX];
static bool gControllerButtonsHeld[SDL_CONTROLLER_BUTTON_MAX];

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
	memcpy(gKeyPrev, gKey, gKeyArraySize);
	memcpy(gKey, SDL_GetKeyboardState(&gKeyArraySize), SDL_NUM_SCANCODES);

	for (unsigned int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
		gControllerButtonsPressed[i] = !gControllerButtonsHeldPrev[i] && gControllerButtonsHeld[i];

	memcpy(gControllerButtonsHeldPrev, gControllerButtonsHeld, sizeof(gControllerButtonsHeld));
}

bool isKeyDown(int keynum)
{
	if (gUseGamepad)
		return gControllerButtonsHeld[keynum];
	else
		return (gKey[keynum] == 1);
}

bool isKeyPressed(int keynum)
{
	if (gUseGamepad)
		return gControllerButtonsPressed[keynum];
	else
		return (gKeyPrev[keynum] == 0 && gKey[keynum] == 1);
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
			gControllerButtonsHeld[event.cbutton.button] = true;
			break;

		case SDL_CONTROLLERBUTTONUP:
			gControllerButtonsHeld[event.cbutton.button] = false;
			break;
		}
	}

	return true;
}
