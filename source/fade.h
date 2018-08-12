#pragma once
#include "render.h"

void fadeOut(const int direction);
void fadeIn(const int direction);

void drawFade();

extern unsigned int fadeCounter;
extern int fadeDirection;
extern bool fadedOut;
