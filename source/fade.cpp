#include "fade.h"

FADE_STATE fade;

const int fadeFrames = 16;

void initFade()
{
	memset(&fade, 0, sizeof(FADE_STATE));
}

void updateFade()
{
	if (fade.mode)
	{
		fade.bMask = false;

		if (++fade.count > (std::max(screenWidth, screenHeight) / 16 + fadeFrames))
		{
			fade.bMask = (fade.mode == 2);
			fade.mode = 0;
		}
	}
}

void drawFade()
{
	RECT rect = { 0, 0, 0, 16 };

	int xt;
	int yt;
	int frame = 0;

	const int w = screenWidth / 16 + 1;
	const int h = screenHeight / 16 + 1;
	const int frames = (std::max(screenWidth, screenHeight) / 16 + fadeFrames);

	if (fade.bMask)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
		SDL_RenderClear(renderer);
	}
	else if (fade.mode == 1)
	{
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				switch (fade.dir)
				{
				case 0:
					xt = (frames - fade.count);
					frame = xt - x;
					break;

				case 1:
					yt = (frames - fade.count);
					frame = yt - y;
					break;

				case 2:
					xt = w - (frames - fade.count);
					frame = x - xt;
					break;

				case 3:
					yt = h - (frames - fade.count);
					frame = y - yt;
					break;

				case 4:
					frame = fadeFrames - (fade.count - abs(x - (screenWidth >> 5)) - abs(y - (screenHeight >> 5)));
					break;
				}

				if (frame > 0)
				{
					rect.left = std::min(frame, fadeFrames - 1) * 16;
					rect.right = rect.left + 16;

					const int fixX = (w * 16) - screenWidth;
					const int fixY = (h * 16) - screenHeight;
					drawTexture(sprites[TEX_FADE], &rect, x * 16 - fixX, y * 16 - fixY);
				}
			}
		}
	}
	else if (fade.mode == 2)
	{
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				switch (fade.dir)
				{
				case 0:
					xt = w - fade.count;
					frame = x - xt;
					break;

				case 1:
					yt = h - fade.count;
					frame = y - yt;
					break;

				case 2:
					xt = fade.count;
					frame = xt - x;
					break;

				case 3:
					yt = fade.count;
					frame = yt - y;
					break;

				case 4:
					frame = fadeFrames - ((frames - fade.count) - abs(x - (screenWidth >> 5)) - abs(y - (screenHeight >> 5)));
					break;
				}

				if (frame > 0)
				{
					rect.left = std::min(frame, fadeFrames - 1) * 16;
					rect.right = rect.left + 16;

					const int fixX = (w * 16) - screenWidth;
					const int fixY = (h * 16) - screenHeight;
					drawTexture(sprites[TEX_FADE], &rect, x * 16 - fixX, y * 16 - fixY);
				}
			}
		}
	}
}