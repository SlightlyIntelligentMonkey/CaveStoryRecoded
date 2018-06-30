#include "npcCollision.h"

//Judging functions
int npcJudgeBlock(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;

	if (*y - rcHit->top < (2 * (2 * ty + 1) - 1) << 11
		&& *y + rcHit->bottom >(2 * (2 * ty - 1) + 1) << 11
		&& *x - rcHit->right < (2 * tx + 1) << 12
		&& *x - rcHit->right > tx << 13)
	{
		*x = ((2 * tx + 1) << 12) + rcHit->right;

		hit |= leftWall;
	}

	if (*y - rcHit->top < (2 * (2 * ty + 1) - 1) << 11
		&& *y + rcHit->bottom >(2 * (2 * ty - 1) + 1) << 11
		&& *x + rcHit->right >(2 * tx - 1) << 12
		&& *x + rcHit->right < tx << 13)
	{
		*x = ((2 * tx - 1) << 12) - rcHit->right;

		hit |= rightWall;
	}

	if (*x - rcHit->right < ((2 * tx + 1) << 12) - 0x600
		&& *x + rcHit->right >((2 * tx - 1) << 12) + 0x600
		&& *y - rcHit->top < (2 * ty + 1) << 12
		&& *y - rcHit->top > ty << 13)
	{
		*y = ((2 * ty + 1) << 12) + rcHit->top;

		hit |= ceiling;
	}

	if (*x - rcHit->right < ((2 * tx + 1) << 12) - 0x600
		&& *x + rcHit->right >((2 * tx - 1) << 12) + 0x600
		&& *y + rcHit->bottom >(2 * ty - 1) << 12
		&& *y + rcHit->bottom < ty << 13)
	{
		*y = ((2 * ty - 1) << 12) - rcHit->bottom;

		hit |= ground;
	}

	return hit;
}

//Ceiling slopes
int npcJudgeTriangleA(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y - rcHit->top < (ty << 13) - (-0x2000 * tx + *x) / 2 + 0x800
		&& *y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		*y = (ty << 13) - (-0x2000 * tx + *x) / 2 + 0x800 + rcHit->top;

		hit = ceiling;
	}

	return hit;
}

int npcJudgeTriangleB(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y - rcHit->top < (ty << 13) - (-0x2000 * tx + *x) / 2 - 0x800
		&& *y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		*y = (ty << 13) - (-0x2000 * tx + *x) / 2 - 0x800 + rcHit->top;

		hit = ceiling;
	}

	return hit;
}

int npcJudgeTriangleC(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y - rcHit->top < (ty << 13) + (-0x2000 * tx + *x) / 2 - 0x800
		&& *y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		*y = (ty << 13) + (-0x2000 * tx + *x) / 2 - 0x800 + rcHit->top;

		hit = ceiling;
	}

	return hit;
}

int npcJudgeTriangleD(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y - rcHit->top < (ty << 13) + (-0x2000 * tx + *x) / 2 + 0x800
		&& *y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		*y = (ty << 13) + (-0x2000 * tx + *x) / 2 + 0x800 + rcHit->top;

		hit = ceiling;
	}

	return hit;
}

//Floor slopes
int npcJudgeTriangleE(RECT *rcHit, int tx, int ty, int *x, int *y, int wasGround)
{
	int hit = slopeE;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y + rcHit->bottom >(ty << 13) + (-0x2000 * tx + *x) / 2 - 0x800
		&& *y - rcHit->top < (2 * ty + 1) << 12)
	{
		*y = (ty << 13) + (-0x2000 * tx + *x) / 2 - 0x800 - rcHit->bottom;

		hit |= (ground | slopeLeft);
	}

	return hit;
}

int npcJudgeTriangleF(RECT *rcHit, int tx, int ty, int *x, int *y, int wasGround)
{
	int hit = slopeF;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y + rcHit->bottom >(ty << 13) + (-0x2000 * tx + *x) / 2 + 0x800
		&& *y - rcHit->top < (2 * ty + 1) << 12)
	{
		*y = (ty << 13) + (-0x2000 * tx + *x) / 2 + 0x800 - rcHit->bottom;

		hit |= (ground | slopeLeft);
	}
	return hit;
}

int npcJudgeTriangleG(RECT *rcHit, int tx, int ty, int *x, int *y, int wasGround)
{
	int hit = slopeG;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y + rcHit->bottom >(ty << 13) - (-0x2000 * tx + *x) / 2 + 0x800
		&& *y - rcHit->top < (2 * ty + 1) << 12)
	{
		*y = (ty << 13) - (-0x2000 * tx + *x) / 2 + 0x800 - rcHit->bottom;

		hit |= (ground | slopeRight);
	}

	return hit;
}

int npcJudgeTriangleH(RECT *rcHit, int tx, int ty, int *x, int *y, int wasGround)
{
	int hit = slopeH;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y + rcHit->bottom >(ty << 13) - (-0x2000 * tx + *x) / 2 - 0x800
		&& *y - rcHit->top < (2 * ty + 1) << 12)
	{
		*y = (ty << 13) - (-0x2000 * tx + *x) / 2 - 0x800 - rcHit->bottom;

		hit |= (ground | slopeRight);
	}

	return hit;
}

int npcJudgeWater(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;

	if (*x - rcHit->right < (4 * (2 * tx + 1) - 1) << 10
		&& *x + rcHit->right > (4 * (2 * tx - 1) + 1) << 10
		&& *y - rcHit->top < (4 * (2 * ty + 1) - 1) << 10
		&& *y + rcHit->bottom > (4 * (2 * ty - 1) + 1) << 10)
	{
		hit = water;
	}

	return hit;
}

int npcJudgeSpike(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;

	if (*x - 0x800 < (4 * tx + 1) << 11
		&& *x + 0x800 > (4 * tx - 1) << 11
		&& *y - 0x800 < (ty << 13) + 0x600
		&& *y + 0x800 > (ty << 13) - 0x600)
	{
		hit = spike;
	}

	return hit;
}

int npcJudgeWaterSpike(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;

	if (*x - 0x800 < (4 * tx + 1) << 11
		&& *x + 0x800 > (4 * tx - 1) << 11
		&& *y - 0x800 < (ty << 13) + 0x600
		&& *y + 0x800 > (ty << 13) - 0x600)
	{
		hit = (spike | water | 0x800);
	}

	return hit;
}

int npcHitMap(RECT *rcHit, int *x, int *y, bool b44, bool wasGround)
{
	int hit = 0;
	
	int fromX = (*x - rcHit->right + 0x1000) >> 13;
	int fromY = (*y - rcHit->top + 0x1000) >> 13;
	int toX = (*x + rcHit->right + 0x1000) >> 13;
	int toY = (*y + rcHit->bottom + 0x1000) >> 13;
	
	for (int current_X = fromX; current_X <= toX; current_X++)
	{
		for (int current_Y = fromY; current_Y <= toY; current_Y++)
		{
			switch (getTileAttribute(current_X, current_Y))
			{
				//Solid
			case(0x41): case(0x43):
				hit |= npcJudgeBlock(rcHit, current_X, current_Y, x, y);
				continue;

				//No NPC
			case(0x03): case(0x04): case(0x44): case(0x46):
				if (b44 == true) { hit |= npcJudgeBlock(rcHit, current_X, current_Y, x, y); }
				continue;

				//Water solid
			case(0x61):
				hit |= npcJudgeBlock(rcHit, current_X, current_Y, x, y);
				hit |= npcJudgeWater(rcHit, current_X, current_Y, x, y);
				continue;

				//Water
			case(0x02): case(0x60):
				hit |= npcJudgeWater(rcHit, current_X, current_Y, x, y);
				continue;

				//Spikes
			case(0x42):
				hit |= npcJudgeSpike(rcHit, current_X, current_Y, x, y);
				continue;

				//Water spikes
			case(0x62):
				hit |= npcJudgeWaterSpike(rcHit, current_X, current_Y, x, y);
				continue;

				//Slopes
			case(0x50):
				hit |= npcJudgeTriangleA(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x51):
				hit |= npcJudgeTriangleB(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x52):
				hit |= npcJudgeTriangleC(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x53):
				hit |= npcJudgeTriangleD(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x54):
				hit |= npcJudgeTriangleE(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x55):
				hit |= npcJudgeTriangleF(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x56):
				hit |= npcJudgeTriangleG(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x57):
				hit |= npcJudgeTriangleH(rcHit, current_X, current_Y, x, y, wasGround);
				continue;

				//Water slopes
			case(0x70):
				hit |= npcJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= npcJudgeTriangleA(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x71):
				hit |= npcJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= npcJudgeTriangleB(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x72):
				hit |= npcJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= npcJudgeTriangleC(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x73):
				hit |= npcJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= npcJudgeTriangleD(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x74):
				hit |= npcJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= npcJudgeTriangleE(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x75):
				hit |= npcJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= npcJudgeTriangleF(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x76):
				hit |= npcJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= npcJudgeTriangleG(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x77):
				hit |= npcJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= npcJudgeTriangleH(rcHit, current_X, current_Y, x, y, wasGround);
				continue;

				//Default
			default:
				continue;
			}
		}
	}

	return hit;
}