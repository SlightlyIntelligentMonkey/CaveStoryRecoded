#pragma once
#include "common.h"

struct FADE_STATE
{
	int mode;
	bool bMask;
	int count;
	char dir;
};

extern FADE_STATE fade;

void initFade() noexcept;

void updateFade() noexcept;
void drawFade();
