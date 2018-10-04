#include "main.h"

#include <string>
#include "SDL.h"
#include "sound.h"
#include "script.h"
#include "input.h"
#include "flags.h"
#include "player.h"
#include "render.h"
#include "filesystem.h"
#include "game.h"
#include "loadConfig.h"
#include "log.h"
#include "level.h"

using std::string;

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
	loadImage("data/Title.png", &sprites[0x00]);
	loadImage("data/Pixel.png", &sprites[0x01]);

	loadImage("data/Fade.png", &sprites[0x06]);

	loadImage("data/ItemImage.png", &sprites[0x08]);

	loadImage("data/Arms.png", &sprites[0x0B]);
	loadImage("data/ArmsImage.png", &sprites[0x0C]);

	loadImage("data/StageImage.png", &sprites[0x0E]);

	loadImage("data/Loading.png", &sprites[0x0F]);

	loadImage("data/MyChar.png", &sprites[0x10]);

	loadImage("data/Bullet.png", &sprites[0x11]);
	loadImage("data/Caret.png", &sprites[0x13]);

	loadImage("data/Npc/NpcSym.png", &sprites[0x14]);

	loadImage("data/Npc/NpcRegu.png", &sprites[0x17]);

	loadImage("data/TextBox.png", &sprites[0x1A]);  // Escape menu
	loadImage("data/Face.png", &sprites[0x1B]);

	loadImage("data/Font.png", &sprites[0x26]);
	loadImage("data/Missing.png", &sprites[0x27]);

}

void init()
{
    initLogFile();

#ifdef USE_ICONS_WINDOWS
	// Set the window icons. See icon.rc.
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, "101");
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "102");
#endif

	//Initiate SDL and window stuff
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
		doCustomError("Couldn't initiate SDL");

	loadConfigFiles();

	// TBD : Load config data, initialise keybinds and screen resolution based on it
	// TBD : Init joypad
	loadConfig();

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

int main(int /*argc*/, char ** /*argv*/) // TDB : Do something with command-line parameters
{
	try
	{
		init();

		mainGameLoop();

		doQuit();
		return 0;
	}
	catch (const std::exception& e)
	{
		doCustomError(e.what());
	}
}
