#pragma once
#include "common.h"

extern std::vector<char> scriptEvents;
extern BYTE tscFlags[1000];
extern SDL_Texture *font;

enum modes
{
	END = 0,
	PARSE = 1,
	NOD = 2,
	MSG = 3,
	WAI = 4,
	FADE = 5,
	YNJ = 6,
	WAS = 7
};

extern int mapNameDisplayTimer;

extern int tscMode;

void loadTsc(const char * path);

void runScriptEvent(int event_num);
void endTsc();

int updateTsc();
void updateMessageBox();

void drawTSC();
