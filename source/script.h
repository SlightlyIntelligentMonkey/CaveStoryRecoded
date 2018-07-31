#pragma once
#include "common.h"

extern std::vector<char> scriptEvents;
extern BYTE tscFlags[1000];
extern SDL_Texture *font;

extern int tscMode;

void loadTsc(const char * path);
void runScriptEvent(int event_num);
int updateTsc();
void updateMessageBox();

void drawTSC();
