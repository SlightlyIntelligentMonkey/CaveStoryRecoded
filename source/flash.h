#pragma once

enum class flashModes
{
	none = 0,
	normal = 1,
	explosion = 2
};

void setFlash(int x, int y, flashModes mode, int length);
void setFlashColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void actFlash();