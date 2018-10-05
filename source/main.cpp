#include "main.h"

#include <string>
#include <SDL_image.h>
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
	IMG_Quit();
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
	loadImage("data/Title.png", &sprites[TEX_TITLE]);
	loadImage("data/Pixel.png", &sprites[TEX_PIXEL]);

	loadImage("data/Fade.png", &sprites[TEX_FADE]);

	loadImage("data/ItemImage.png", &sprites[TEX_ITEMIMAGE]);

	loadImage("data/Arms.png", &sprites[TEX_ARMS]);
	loadImage("data/ArmsImage.png", &sprites[TEX_ARMSIMAGE]);

	loadImage("data/StageImage.png", &sprites[TEX_STAGEIMAGE]);

<<<<<<< HEAD
	loadImage("data/MyChar.png", &sprites[TEX_MYCHAR]);

	loadImage("data/Bullet.png", &sprites[TEX_BULLET]);
	loadImage("data/Caret.png", &sprites[TEX_CARET]);
=======
	loadImage("data/MyChar.png", &sprites[0x10]);
>>>>>>> e50bbb8c6038e8208e2111719c81505aac22f748

	loadImage("data/Npc/NpcSym.png", &sprites[TEX_NPC_SYM]);

	loadImage("data/Npc/NpcRegu.png", &sprites[TEX_NPC_REGU]);

	loadImage("data/TextBox.png", &sprites[TEX_TEXTBOX]);
	loadImage("data/Face.png", &sprites[TEX_FACE]);

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

	//Initiate SDL_image
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
		doCustomError("Couldn't initiate SDL Image");

	loadConfigFiles();
<<<<<<< HEAD
	loadConfig();

=======
	
	//draws loading
	loadImage("data/Loading.png", &sprites[TEX_LOADING]);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	RECT rcLoad = { 0, 0, 64, 8 };
	drawTexture(sprites[TEX_LOADING], &rcLoad, (screenWidth >> 1) - (rcLoad.right >> 1), (screenHeight >> 1) - (rcLoad.bottom >> 1));
	SDL_RenderPresent(renderer);
	
>>>>>>> e50bbb8c6038e8208e2111719c81505aac22f748
	// TBD : Load config data, initialise keybinds and screen resolution based on it
	// TBD : Init joypad

	//draws loading
	loadImage("data/Loading.png", &sprites[TEX_LOADING]);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	RECT rcLoad = { 0, 0, 64, 8 };
	drawTexture(sprites[TEX_LOADING], &rcLoad, (screenWidth >> 1) - (rcLoad.right >> 1), (screenHeight >> 1) - (rcLoad.bottom >> 1));
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
