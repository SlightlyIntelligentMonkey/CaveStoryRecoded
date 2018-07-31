#pragma once
#include "common.h"

extern std::vector<char> scriptEvents;
extern BYTE tscFlags[1000];
extern SDL_Texture *font;

enum displayFlags
{
	TSCINVIS = 0,
	TSCVIS = 0b0001,
	MSGbox = 0b0100
};

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

extern int tscMode;

void loadTsc(const char * path);
void runScriptEvent(int event_num);
int updateTsc();
void updateMessageBox();

void drawTSC();
