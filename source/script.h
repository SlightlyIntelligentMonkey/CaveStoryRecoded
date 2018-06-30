#pragma once
#include "common.h"

extern std::vector<char> scriptEvents;

void loadTsc(char* path);
void runScriptEvent(int event_num);

void updateTsc();