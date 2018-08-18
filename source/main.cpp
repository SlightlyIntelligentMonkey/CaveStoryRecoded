#include "common.h"

//Debug flags
int debugFlags = showSlots;

//Rendering and view related variables
SDL_Window *window;
SDL_Renderer *renderer;

SDL_Rect DrawRect;
SDL_Rect ImageRect;

//Game variables
SDL_Event events;

int gameFlags = 0;

int framerate = 20; //17 for 60-ish fps
unsigned int framerateTicks;

int mode;

bool exitGame = false;

// Some global functions

static void doQuit() 
{
	//sound::quit();
	SDL_Quit();
	IMG_Quit();
	freeSounds();
}

void doError() 
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical Error", SDL_GetError(), nullptr);
	SDL_ClearError();
	doQuit();
	exit(EXIT_FAILURE);
}

void doCustomError(const char *msg) 
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical Error", msg, nullptr);
	doQuit();
	exit(EXIT_FAILURE);
}

SDL_Texture* sprites[0x28];

int init() 
{
	//Initiate SDL and window stuff
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
		doCustomError("Couldn't initiate SDL");

	//Initiate SDL_image
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
		doCustomError("Couldn't initiate SDL Image");
	
	// TBD : Load config data, initialise keybinds and screen resolution based on it
	// TBD : Init joypad

	initTsc();
	initFlags();

	ini_audio();
	loadSounds();

	currentPlayer.init();
	
	createWindow(320, 240, 2, true);

	//Load assets
	loadNpcTable();
	loadStageTable();

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

	loadImage("data/TextBox.png", &sprites[0x1A]);
	loadImage("data/Face.png", &sprites[0x1B]);

	loadImage("data/Font.png", &sprites[0x26]);
	loadImage("data/Missing.png", &sprites[0x27]);

	return 0;
}

int main(int argc, char **argv) // TDB : Do something with command-line parameters
{
	init();

	mainGameLoop();

	doQuit();
	return 0;
}
