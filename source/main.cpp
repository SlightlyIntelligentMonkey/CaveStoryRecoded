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
#include "stage.h"

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
	loadImage("data/Title", &sprites[TEX_TITLE]);
	loadImage("data/Pixel", &sprites[TEX_PIXEL]);

	loadImage("data/Fade", &sprites[TEX_FADE]);

	loadImage("data/ItemImage", &sprites[TEX_ITEMIMAGE]);

	loadImage("data/Arms", &sprites[TEX_ARMS]);
	loadImage("data/ArmsImage", &sprites[TEX_ARMSIMAGE]);

	loadImage("data/StageImage", &sprites[TEX_STAGEIMAGE]);

	loadImage("data/MyChar", &sprites[TEX_MYCHAR]);

	loadImage("data/Bullet", &sprites[TEX_BULLET]);
	loadImage("data/Caret", &sprites[TEX_CARET]);

	loadImage("data/Npc/NpcSym", &sprites[TEX_NPC_SYM]);

	loadImage("data/Npc/NpcRegu", &sprites[TEX_NPC_REGU]);

	loadImage("data/TextBox", &sprites[TEX_TEXTBOX]);
	loadImage("data/Face", &sprites[TEX_FACE]);

	loadImage("data/Font", &sprites[0x26]);
	loadImage("data/Missing", &sprites[0x27]);

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
	loadConfig();

	//draws loading
	loadImage("data/Loading", &sprites[TEX_LOADING]);   // Load the loading sprite now so that we can display it
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

int main(int /*argc*/, char ** /*argv*/) // TBD : Do something with command-line parameters
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
