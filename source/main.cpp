#include "common.h"
#include "hud.h"

//get some variables
SDL_Window *window;
SDL_Renderer *renderer;

SDL_Rect DrawRect;
SDL_Rect ImageRect;

int screenWidth = 0;
int screenHeight = 0;

int screenScale = 0;

int viewX = 0;
int viewY = 0;
int viewGoalX = 0x1000;
int viewGoalY = 0x1000;

int viewXsp = 0;
int viewYsp = 0;
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

//View
int createView(int width, int height, int scale, bool fullscreen) {
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	int createWidth;
	int createHeight;

	if (fullscreen)
	{
		screenScale = (int)floor(DM.h / height);

		screenHeight = DM.h / screenScale;
		screenWidth = DM.w / screenScale;

		createWidth = screenWidth;
		createHeight = screenHeight;
	}
	else
	{
		screenWidth = width;
		screenHeight = height;
		screenScale = scale;

		createWidth = width * scale;
		createHeight = height * scale;
	}

	window = SDL_CreateWindow("Cave Story Remake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, createWidth, createHeight, 0);

	if (fullscreen)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, screenWidth, screenHeight);

	return 0;
}

SDL_Texture* sprites[0x28];

int init() {
	//Initiate SDL and window stuff
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	
	initSound();
	
	createView(320, 240, 2, false);

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
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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

		getKeys(&events);
		if (events.type == SDL_QUIT || exitGame) { return -1; }
		
		//////UPDATE//////
		if (keyPressed(SDL_SCANCODE_1)) {
			loadLevel(13);
			currentPlayer.init(10, 8, 2);

			//playOrg(8);
		}
		else if (keyPressed(SDL_SCANCODE_2)) {
			loadLevel(12);
			currentPlayer.init(37, 11, 0);

			//playOrg(8);
		}
		else if (keyPressed(SDL_SCANCODE_3)){
			loadLevel(2);
			currentPlayer.init(5, 6, 2);

			//playOrg(1);
		}
		else if (keyPressed(SDL_SCANCODE_4)) {
			loadLevel(11);
			currentPlayer.init(13, 34, 2);

			//playOrg(9);
		}
		else if (keyPressed(SDL_SCANCODE_5)) {
			loadLevel(49);
			currentPlayer.init(7, 6, 2);

			//playOrg(27);
		}
		else if (keyPressed(SDL_SCANCODE_6)) {
			loadLevel(67);
			currentPlayer.init(7, 52, 2);

			//playOrg(29);
		}
		else if (keyPressed(SDL_SCANCODE_7)) {
			loadLevel(48);
			currentPlayer.init(155, 1, 0);

			//playOrg(38);
		}

		updateNPC();
		currentPlayer.update();

		updateTsc();

		//Move camera
		viewXsp = (viewGoalX - viewX) / viewSpeed;
		viewYsp = (viewGoalY - viewY) / viewSpeed;

		viewX += viewXsp;
		viewY += viewYsp;

		//Keep view in level
		if ((levelWidth - 1) << 4 > screenWidth)
			viewX = clamp(viewX, 0, ((levelWidth - 1) << 13) - (screenWidth << 9));
		else
			viewX = ((levelWidth - 1) << 12) - (screenWidth << 8);

		if ((levelHeight - 1) << 4 > screenHeight)
			viewY = clamp(viewY, 0, ((levelHeight - 1) << 13) - (screenHeight << 9));
		else
			viewY = ((levelHeight - 1) << 12) - (screenHeight << 8);

		//////DRAW//////
		drawStart();

		//Draw background (and background tiles)
		drawLevel(false);

		//Draw stuff
		drawNPC();
		currentPlayer.draw();

		//Draw foreground tiles
		drawLevel(true);

		//Draw hud
		drawHud(false);

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
