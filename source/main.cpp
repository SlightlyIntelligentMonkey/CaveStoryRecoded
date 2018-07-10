#include "common.h"
#include "game.h"

//get some variables
SDL_Window *window;
SDL_Renderer *renderer;

SDL_Rect DrawRect;
SDL_Rect ImageRect;

int viewX = 0;
int viewY = 0;
int viewGoalX = 0;
int viewGoalY = 0;

int viewSpeed = 16;

SDL_Event events;

//Game variables
int framerate = 20; //17 for 60-ish fps
unsigned int framerateTicks;

int mode;

bool exitGame = false;

// Some global functions

static void doQuit() {
	endSound();
	SDL_Quit();
}

void doError() {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical Error", SDL_GetError(), NULL);
	SDL_ClearError();
	doQuit();
	exit(EXIT_FAILURE);
}

void doCustomError(const char *msg) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical Error", msg, NULL);
	doQuit();
	exit(EXIT_FAILURE);
}

SDL_Texture* sprites[0x28];

int init() {
	//Initiate SDL and window stuff
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	
	initSound();
	
	createWindow(240 * 16 / 9, 240, 2, false);

	//Load assets
	loadBMP("data/Title.bmp", &sprites[0x00]);

	loadBMP("data/Fade.bmp", &sprites[0x06]);

	loadBMP("data/ItemImage.bmp", &sprites[0x08]);

	loadBMP("data/Arms.bmp", &sprites[0x0B]);
	loadBMP("data/ArmsImage.bmp", &sprites[0x0C]);

	loadBMP("data/StageImage.bmp", &sprites[0x0E]);

	loadBMP("data/Loading.bmp", &sprites[0x0F]);

	loadBMP("data/MyChar.bmp", &sprites[0x10]);

	loadBMP("data/Bullet.bmp", &sprites[0x11]);
	loadBMP("data/Caret.bmp", &sprites[0x13]);

	loadBMP("data/Npc/NpcSym.bmp", &sprites[0x14]);
	loadBMP("data/Npc/NpcRegu.bmp", &sprites[0x17]);

	loadBMP("data/TextBox.bmp", &sprites[0x1A]);
	loadBMP("data/Face.bmp", &sprites[0x1B]);

	//Start game
	//init flags
	initFlags();

	//Load level
	loadLevel(13);
	currentPlayer.init(10, 8, 2);

	//playOrg(8);

	runScriptEvent(200);

	return 0;
}

int drawStart() {
	//CLEAR
	SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
	SDL_RenderClear(renderer);

	return 0;
}

int drawEnd() {
	//Draw to the screen
	SDL_RenderPresent(renderer);

	return 0;
}

int game() {
	while (mode > -1) {
		//Framerate limiter
		Uint32 timeNow = SDL_GetTicks();
		Uint32 timeNext = framerateTicks + framerate;

		if (timeNow >= timeNext) {
			framerateTicks = SDL_GetTicks();
		}
		else
		{
			SDL_Delay(timeNext - timeNow);
			continue;
		}

		//Handle events
		getKeys(&events);
		if (events.type == SDL_QUIT || exitGame) { return -1; }

		//////UPDATE//////
		gameUpdate();

		//////DRAW//////
		drawStart();
		gameDraw();
		drawEnd();
	}
	
	return -1;
}

int main(int argc, char **argv) {
	init();

	game();

	doQuit();
	return 0;
}
