#pragma once
#include "common.h"

extern std::vector<char> scriptEvents;

void loadTsc(const char * path);
void runScriptEvent(int event_num);

void updateTsc();
