#pragma once

struct FADE_STATE
{
	int mode;
	bool bMask;
	int count;
	char dir;
};

extern FADE_STATE gFade;

void initFade();

void updateFade();
void drawFade();
