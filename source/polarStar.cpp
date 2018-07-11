#include "polarStar.h"

void actPolarStar(bullet *BULLET)
{
	int frame;

	if (++BULLET->count1 <= BULLET->range)
	{
		if (BULLET->action)
		{
			BULLET->x += BULLET->xsp;
			BULLET->y += BULLET->ysp;
		}
		else
		{
			BULLET->action = 1;

			switch (BULLET->direction)
			{
			case 0:
				BULLET->xsp = -0x1000;
				break;

			case 1:
				BULLET->ysp = -0x1000;
				break;

			case 2:
				BULLET->xsp = 0x1000;
				break;

			case 3:
				BULLET->ysp = 0x1000;
				break;
			}

			switch (BULLET->level)
			{
			case 1:
				if (BULLET->direction == 1 || BULLET->direction == 3)
				{
					BULLET->frameRect = { 144, 32, 160, 48 };
					BULLET->enemyWidth = 0x400;
				}
				else
				{
					BULLET->frameRect = { 128, 32, 144, 48 };
					BULLET->enemyHeight = 0x400;
				}

				break;

			case 2:
				if (BULLET->direction == 1 || BULLET->direction == 3)
				{
					BULLET->frameRect = { 176, 32, 192, 48 };
					BULLET->enemyWidth = 0x800;
				}
				else
				{
					BULLET->frameRect = { 160, 32, 176, 48 };
					BULLET->enemyHeight = 0x800;
				}

				break;

			case 3:
				if (BULLET->direction == 1 || BULLET->direction == 3)
					BULLET->frameRect = { 144, 48, 160, 64 };
				else
					BULLET->frameRect = { 128, 48, 144, 64 };

				break;
			}
		}

		if (BULLET->ysp == 0)
		{
			frame = 1;
		}
		else
		{
			frame = 2;
		}
	}
	else
	{
		BULLET->cond = 0;
	}
}
