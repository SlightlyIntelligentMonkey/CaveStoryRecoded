#pragma once
#include "common.h"

extern std::vector<char> scriptEvents;
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

enum displayFlags
{
	TSCINVIS = 0,
	TSCVIS = 0b0001,
	MSGbox = 0b0010,
	FAI = 0b0100,
	FAO = 0b1000,
	invisMSG = 0x10
};

extern int tscDisplayFlags;

void loadTsc(const char * path);

void runScriptEvent(int event_num);
void endTsc();

int updateTsc();
void updateMessageBox();

void drawTSC();
