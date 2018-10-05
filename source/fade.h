#pragma once

struct FADE_STATE
{
	int mode;
	bool bMask;
	int count;
	char dir;
};

extern FADE_STATE fade;

void initFade();

void updateFade();
void drawFade();

enum flashModes
{
	none = 0,
	normal = 1,
	explosion = 2
};

void setFlash(int x, int y, flashModes mode, int length);
void setFlashColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void actFlash();