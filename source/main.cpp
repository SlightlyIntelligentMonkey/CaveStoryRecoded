#include "common.h"

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

//Organya init
HMODULE orgModule = NULL;

pstartOrganya startOrganya;
ploadOrganya loadOrganya;
psetOrganyaPosition setOrganyaPosition;
pgetOrganyaPosition getOrganyaPosition;
pplayOrganyaMusic playOrganyaMusic;
pchangeOrganyaVolume changeOrganyaVolume;
pstopOrganyaMusic stopOrganyaMusic;
pendOrganya endOrganya;

//Some global functions
void doError() {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical Error", SDL_GetError(), NULL);
	SDL_ClearError();
	endOrganya();
	SDL_Quit();
	exit(EXIT_FAILURE);
}

void doCustomError(const char *msg) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical Error", msg, NULL);
	endOrganya();
	SDL_Quit();
	exit(EXIT_FAILURE);
}

//View
int createView(int width, int height, int scale) {
	screenWidth = width;
	screenHeight = height;
	screenScale = scale;

	int createWidth = width * scale;
	int createHeight = height * scale;

	window = SDL_CreateWindow("Cave Story Remake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, createWidth, createHeight, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, screenWidth, screenHeight);

	return 0;
}

SDL_Texture* sprites[0x28];

int init() {
	//Create the window and renderer
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	createView(426, 240, 2);

	//Organya
	orgModule = LoadLibrary("Org001.dll");

	HWND handle = GetActiveWindow();

	startOrganya = (pstartOrganya)GetProcAddress(orgModule, "StartOrganya");
	loadOrganya = (ploadOrganya)GetProcAddress(orgModule, "LoadOrganya");
	setOrganyaPosition = (psetOrganyaPosition)GetProcAddress(orgModule, "SetOrganyaPosition");
	getOrganyaPosition = (pgetOrganyaPosition)GetProcAddress(orgModule, "GetOrganyaPosition");
	playOrganyaMusic = (pplayOrganyaMusic)GetProcAddress(orgModule, "PlayOrganyaMusic");
	changeOrganyaVolume = (pchangeOrganyaVolume)GetProcAddress(orgModule, "ChangeOrganyaVolume");
	stopOrganyaMusic = (pstopOrganyaMusic)GetProcAddress(orgModule, "StopOrganyaMusic");
	endOrganya = (pendOrganya)GetProcAddress(orgModule, "EndOrganya");

	startOrganya(handle);

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
		if (SDL_GetTicks() > framerateTicks + framerate)
		{
			framerateTicks = SDL_GetTicks();
		}
		else
		{
			continue;
		}

		getKeys(&events);
		if (events.type == SDL_QUIT || exitGame) { return -1; }
		
		//////UPDATE//////
		if (keyPressed(SDL_SCANCODE_1)) {
			loadLevel(13);
			currentPlayer.init(10, 8, 2);

			playOrg(8);
		}
		else if (keyPressed(SDL_SCANCODE_2)) {
			loadLevel(12);
			currentPlayer.init(37, 11, 0);

			playOrg(8);
		}
		else if (keyPressed(SDL_SCANCODE_3)){
			loadLevel(2);
			currentPlayer.init(5, 6, 2);

			playOrg(1);
		}
		else if (keyPressed(SDL_SCANCODE_4)) {
			loadLevel(11);
			currentPlayer.init(13, 34, 2);

			playOrg(9);
		}
		else if (keyPressed(SDL_SCANCODE_5)) {
			loadLevel(67);
			currentPlayer.init(7, 52, 2);

			playOrg(29);
		}
		else if (keyPressed(SDL_SCANCODE_6)) {
			loadLevel(56);
			currentPlayer.init(118, 83, 0);

			playOrg(24);
		}
		else if (keyPressed(SDL_SCANCODE_7)) {
			loadLevel(6);
			currentPlayer.init(4, 18, 0);

			playOrg(5);
		}

		updateNPC();
		currentPlayer.update();

		updateTsc();

		//Move camera
		viewXsp = (viewGoalX - viewX) / viewSpeed;
		viewYsp = (viewGoalY - viewY) / viewSpeed;

		viewX += viewXsp;
		viewY += viewYsp;

		viewX = clamp(viewX, 0, ((levelWidth - 1) << 13) - (screenWidth << 9));
		viewY = clamp(viewY, 0, ((levelHeight - 1) << 13) - (screenHeight << 9));

		//////DRAW//////
		drawStart();

		//Draw background (and background tiles)
		drawLevel(false);

		//Draw stuff
		drawNPC();
		currentPlayer.draw();

		//Draw foreground tiles
		drawLevel(true);

		drawEnd();
	}
	
	return -1;
}

int main(int argc, char **argv) {
	init();

	game();

	SDL_Quit();
	endOrganya();
	return 0;
}