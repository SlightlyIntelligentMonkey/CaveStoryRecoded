#include "fade.h"

unsigned int fadeCounter = 0;
int fadeDirection = 0;

const int numFadeFrames = 16;
bool fadedOut = true;

enum fadeDirections
{
	left = 0,
	up = 1,
	right = 2,
	down = 3,
	center = 4
};

void fadeOut(const int direction)
{
	RECT rcFade = { 0, 0, 16, 16 };

	switch (direction)
	{
	case(left):
		for (int x = screenWidth >> 4; x >= 0; x--)
		{
			for (int y = 0; y < (screenHeight >> 4) + 1; y++)
			{
				rcFade.left = (fadeCounter - x) << 4;
				if (rcFade.left >= (numFadeFrames - 1) << 4) { rcFade.left = (numFadeFrames - 1) << 4; }
				rcFade.right = rcFade.left + 16;
				drawTexture(sprites[TEX_FADE], &rcFade, x << 4, y << 4);
			}
		}
		if (fadeCounter++ >(screenWidth >> 4) + 1 + numFadeFrames)
		{
			tscMode = PARSE;
			fadedOut = true;
			tscDisplayFlags &= ~FAO;
		}
		break;
	case(up):
		for (int y = screenHeight >> 4; y >= 0; y--)
		{
			for (int x = 0; x < (screenWidth >> 4) + 1; x++)
			{
				rcFade.left = (fadeCounter - y) << 4;
				if (rcFade.left >= (numFadeFrames - 1) << 4) { rcFade.left = (numFadeFrames - 1) << 4; }
				rcFade.right = rcFade.left + 16;
				drawTexture(sprites[TEX_FADE], &rcFade, x << 4, y << 4);
			}
		}
		if (fadeCounter++ >(screenHeight >> 4) + 1 + numFadeFrames)
		{
			tscMode = PARSE;
			fadedOut = true;
			tscDisplayFlags &= ~FAO;
		}
		break;
	case(right):
		for (int x = screenWidth >> 4; x >= 0; x--)
		{
			for (int y = 0; y < (screenHeight >> 4) + 1; y++)
			{
				rcFade.left = ((x - (screenWidth >> 4)) + fadeCounter) << 4;
				if (rcFade.left >= (numFadeFrames - 1) << 4) { rcFade.left = (numFadeFrames - 1) << 4; }
				rcFade.right = rcFade.left + 16;
				drawTexture(sprites[TEX_FADE], &rcFade, x << 4, y << 4);
			}
		}
		if (fadeCounter++ >(screenWidth >> 4) + 1 + numFadeFrames)
		{
			tscMode = PARSE;
			fadedOut = true;
			tscDisplayFlags &= ~FAO;
		}
		break;
	case(down):
		for (int y = screenHeight >> 4; y >= 0; y--)
		{
			for (int x = 0; x < (screenWidth >> 4) + 1; x++)
			{
				rcFade.left = ((y - (screenHeight >> 4)) + fadeCounter) << 4;
				if (rcFade.left >= (numFadeFrames - 1) << 4) { rcFade.left = (numFadeFrames - 1) << 4; }
				rcFade.right = rcFade.left + 16;
				drawTexture(sprites[TEX_FADE], &rcFade, x << 4, y << 4);
			}
		}
		if (fadeCounter++ >(screenHeight >> 4) + 1 + numFadeFrames)
		{
			tscMode = PARSE;
			fadedOut = true;
			tscDisplayFlags &= ~FAO;
		}
		break;
	case(center):
		for (int x = 0; x < (screenWidth >> 4) + 1; x++)
		{
			for (int y = 0; y < (screenHeight >> 4) + 1; y++)
			{
				rcFade.left = 240 - (((((screenWidth + screenHeight) >> 5) + 1 + numFadeFrames) - fadeCounter) - abs(x - (screenWidth >> 5)) - abs(y - (screenHeight >> 5)) << 4);
				if (rcFade.left >= (numFadeFrames - 1) << 4) { rcFade.left = (numFadeFrames - 1) << 4; }
				rcFade.right = rcFade.left + 16;
				drawTexture(sprites[TEX_FADE], &rcFade, x << 4, y << 4);
			}
		}
		
		if (fadeCounter++ > ((screenWidth + screenHeight) >> 5) + 1 + numFadeFrames)
		{
			tscMode = PARSE;
			fadedOut = true;
			tscDisplayFlags &= ~FAO;
		}

		break;
	}
	return;
}

void fadeIn(const int direction)
{
	RECT rcFade = { 0, 0, 16, 16 };
	fadedOut = false;

	switch (direction)
	{
	case(left):
		for (int x = screenWidth >> 4; x >= 0; x--)
		{
			for (int y = 0; y < (screenHeight >> 4) + 1; y++)
			{
				rcFade.left = 240 - ((fadeCounter - x) << 4);
				if (rcFade.left >= (numFadeFrames - 1) << 4) { rcFade.left = (numFadeFrames - 1) << 4; }
				rcFade.right = rcFade.left + 16;
				drawTexture(sprites[TEX_FADE], &rcFade, x << 4, y << 4);
			}
		}
		if (fadeCounter++ >(screenWidth >> 4) + 1 + numFadeFrames)
		{
			tscMode = PARSE;
			tscDisplayFlags &= ~FAI;
		}
		break;
	case(up):
		for (int y = screenHeight >> 4; y >= 0; y--)
		{
			for (int x = 0; x < (screenWidth >> 4) + 1; x++)
			{
				rcFade.left = 240 - ((fadeCounter - y) << 4);
				if (rcFade.left >= (numFadeFrames - 1) << 4) { rcFade.left = (numFadeFrames - 1) << 4; }
				rcFade.right = rcFade.left + 16;
				drawTexture(sprites[TEX_FADE], &rcFade, x << 4, y << 4);
			}
		}
		if (fadeCounter++ >(screenHeight >> 4) + 1 + numFadeFrames)
		{
			tscMode = PARSE;
			tscDisplayFlags &= ~FAI;
		}
		break;
	case(right):
		for (int x = screenWidth >> 4; x >= 0; x--)
		{
			for (int y = 0; y < (screenHeight >> 4) + 1; y++)
			{
				rcFade.left = 240 - (((x - (screenWidth >> 4)) + fadeCounter) << 4);
				if (rcFade.left >= (numFadeFrames - 1) << 4) { rcFade.left = (numFadeFrames - 1) << 4; }
				rcFade.right = rcFade.left + 16;
				drawTexture(sprites[TEX_FADE], &rcFade, x << 4, y << 4);
			}
		}
		if (fadeCounter++ > (screenWidth >> 4) + 1 + numFadeFrames)
		{
			tscMode = PARSE;
			tscDisplayFlags &= ~FAI;
		}
		break;
	case(down):
		for (int y = screenHeight >> 4; y >= 0; y--)
		{
			for (int x = 0; x < (screenWidth >> 4) + 1; x++)
			{
				rcFade.left = 240 - (((y - (screenHeight >> 4)) + fadeCounter) << 4);
				if (rcFade.left >= (numFadeFrames - 1) << 4) { rcFade.left = (numFadeFrames - 1) << 4; }
				rcFade.right = rcFade.left + 16;
				drawTexture(sprites[TEX_FADE], &rcFade, x << 4, y << 4);
			}
		}
		if (fadeCounter++ >(screenHeight >> 4) + 1 + numFadeFrames)
		{
			tscMode = PARSE;
			tscDisplayFlags &= ~FAI;
		}
		break;
	case(center):
		for (int x = 0; x < (screenWidth >> 4) + 1; x++)
		{
			for (int y = 0; y < (screenHeight >> 4) + 1; y++)
			{
				rcFade.left = 240 - (fadeCounter - abs(x - (screenWidth >> 5)) - abs(y - (screenHeight >> 5)) << 4);
				if (rcFade.left >= (numFadeFrames - 1) << 4) { rcFade.left = (numFadeFrames - 1) << 4; }
				rcFade.right = rcFade.left + 16;
				drawTexture(sprites[TEX_FADE], &rcFade, x << 4, y << 4);
			}
		}
		if (fadeCounter++ > ((screenWidth + screenHeight) >> 5) + 1 + numFadeFrames)
		{
			tscMode = PARSE;
			tscDisplayFlags &= ~FAI;
		}
		break;
	}
	return;
}

void drawFade()
{
	if (fadedOut == true)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
		SDL_RenderClear(renderer);
	}

	if (fadeCounter == 0xFFFFFFF) { tscDisplayFlags &= ~(FAI | FAO); }
	if (tscDisplayFlags & FAI)
	{
		fadeIn(fadeDirection);
	}
	if (tscDisplayFlags & FAO)
	{
		fadeOut(fadeDirection);
	}
	return;
}
