#include "main.h"

#include <string>
#include <typeinfo>
#include "SDL.h"
#include "sound.h"
#include "script.h"
#include "input.h"
#include "flags.h"
#include "player.h"
#include "render.h"
#include "game.h"
#include "loadConfig.h"
#include "log.h"
#include "stage.h"

using std::string;
using std::to_string;

// Some global functions

static void doQuit()
{
	//sound::quit(); // TBD : Make a sound quit method, make the quit method a global destructor or remove this
	SDL_Quit();
	freeSounds();

	logInfo("Finished quit");
}

void doError()
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical Error", SDL_GetError(), nullptr);
	logError(SDL_GetError());
	SDL_ClearError();
	doQuit();
	exit(EXIT_FAILURE);
}

void doCustomError(const string& msg)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical Error", msg.c_str(), nullptr);
	logError(msg);
	doQuit();
	exit(EXIT_FAILURE);
}

SDL_Texture* sprites[0x28];

void loadInitialSprites()
{
	loadImage("Title", &sprites[TEX_TITLE]);
	loadImage("Pixel", &sprites[TEX_PIXEL]);

	loadImage("Fade", &sprites[TEX_FADE]);

	loadImage("ItemImage", &sprites[TEX_ITEMIMAGE]);

	loadImage("Arms", &sprites[TEX_ARMS]);
	loadImage("ArmsImage", &sprites[TEX_ARMSIMAGE]);

	loadImage("StageImage", &sprites[TEX_STAGEIMAGE]);

	loadImage("MyChar", &sprites[TEX_MYCHAR]);

	loadImage("Bullet", &sprites[TEX_BULLET]);
	loadImage("Caret", &sprites[TEX_CARET]);

	loadImage("Npc/NpcSym", &sprites[TEX_NPC_SYM]);

	loadImage("Npc/NpcRegu", &sprites[TEX_NPC_REGU]);

	loadImage("TextBox", &sprites[TEX_TEXTBOX]);
	loadImage("Face", &sprites[TEX_FACE]);

	loadImage("Font", &sprites[0x26]);
	loadImage("Missing", &sprites[0x27]);

}

void init()
{
	initLogFile();

#ifdef USE_ICONS_WINDOWS
	// Set the window icons. See icon.rc.
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, "101");
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "102");
#endif

	//Initiate SDL
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
		doCustomError("Couldn't initiate SDL");

	loadConfigFiles();
	CONFIG *config = loadConfigDat();

	if (config != nullptr)
	{
		if (config->attack_button_mode == 1)
		{
			keyJump = SDL_SCANCODE_X;
			keyShoot = SDL_SCANCODE_Z;
		}
		else
		{
			keyJump = SDL_SCANCODE_Z;
			keyShoot = SDL_SCANCODE_X;
		}
		/* no ok button/key currently implemented
		if (config->ok_button_mode == 1)
		{
			keyOk = keyShoot;
			keyCancel = keyJump;
		}
		else
		{
			keyOk = keyJump;
			keyCancel = keyShoot;
		}
		*/
		if (config->move_button_mode == 1)
		{
			keyLeft = SDL_SCANCODE_COMMA;
			keyRight = SDL_SCANCODE_SLASH;
			keyDown = SDL_SCANCODE_PERIOD;
			keyUp = SDL_SCANCODE_SEMICOLON;
		}
		else
		{
			keyLeft = SDL_SCANCODE_LEFT;
			keyRight = SDL_SCANCODE_RIGHT;
			keyDown = SDL_SCANCODE_DOWN;
			keyUp = SDL_SCANCODE_UP;
		}

		switch (config->display_mode)
		{
		case(0):
			createWindow(320, 240, 2);
			switchScreenMode();
			break;
		case(1):
			createWindow(320, 240, 1);
			break;
		case(2):
			createWindow(320, 240, 2);
			break;
		default:
			createWindow(320, 240, 2);
			switchScreenMode();
			break;
		}

		delete config;
	}
	else
		createWindow(screenWidth, screenHeight, screenScale);

	//draws loading
	loadImage("Loading", &sprites[TEX_LOADING]);   // Load the loading sprite now so that we can display it
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	drawTexture(sprites[TEX_LOADING], nullptr, screenWidth >> 1, screenHeight >> 1);
	SDL_RenderPresent(renderer);

	initTsc();
	initFlags();

	initGamepad();

	initAudio();
	loadSounds();

	currentPlayer.init();

	//Load assets
	loadNpcTable();
	loadStageTable();

	loadInitialSprites();

	logInfo("Finished init");
}

[[noreturn]] void handleExceptionAndAbort()
{
	try
	{
		throw;
	} catch (std::exception& e)
	{
        doCustomError("Exception thrown : " + string(e.what()));
	} catch (const int i)
	{
		doCustomError("int exception : " + to_string(i));
	} catch (const long l)
	{
		doCustomError("long exception : " + to_string(l));
	} catch (const char *p)
	{
	    if (p)
            doCustomError("char * exception : " + string(p));
        else
            doCustomError("Someone threw a nullptr char *");
	} catch (const void *p)
	{
        doCustomError("Someone threw a pointer to " + to_string(reinterpret_cast<uintptr_t>(p)));
	} catch (...)
	{
		doCustomError("Nope, sorry, no idea wtf happened");
	}
}

int main(int /*argc*/, char ** /*argv*/) // TBD : Do something with command-line parameters
{
	try
	{
		init();

		mainGameLoop();

		doQuit();
		return 0;
	}
	catch (...)
	{
        handleExceptionAndAbort();
	}
}
