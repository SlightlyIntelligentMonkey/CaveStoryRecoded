#include "npcCollision.h"

//Judging functions
int playerJudgeBlock(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;
	
	if (*y - rcHit->top < (2 * (2 * ty + 1) - 1) << 11
		&& *y + rcHit->bottom >(2 * (2 * ty - 1) + 1) << 11
		&& *x - rcHit->left < (2 * tx + 1) << 12
		&& *x - rcHit->left > tx << 13)
	{
		*x = ((2 * tx + 1) << 12) + rcHit->left;
		
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

	if (*x - rcHit->left < ((2 * tx + 1) << 12) - 0x600
		&& *x + rcHit->right >((2 * tx - 1) << 12) + 0x600
		&& *y - rcHit->top < (2 * ty + 1) << 12
		&& *y - rcHit->top > ty << 13)
	{
		*y = ((2 * ty + 1) << 12) + rcHit->top;
		
		hit |= ceiling;
	}

	if (*x - rcHit->left < ((2 * tx + 1) << 12) - 0x600
		&& *x + rcHit->right > ((2 * tx - 1) << 12) + 0x600
		&& *y + rcHit->bottom > (2 * ty - 1) << 12
		&& *y + rcHit->bottom < ty << 13)
	{
		*y = ((2 * ty - 1) << 12) - rcHit->bottom;
		
		hit |= ground;
	}

	return hit;
}

//Ceiling slopes
int playerJudgeTriangleA(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;
	
	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y - rcHit->top < (ty << 13) - (-0x2000 * tx + *x) / 2 + 0x800
		&& *y + rcHit->bottom > (2 * ty - 1) << 12)
	{
		*y = (ty << 13) - (-0x2000 * tx + *x) / 2 + 0x800 + rcHit->top;
		
		hit = ceiling;
	}

	return hit;
}

int playerJudgeTriangleB(RECT *rcHit, int tx, int ty, int *x, int *y)
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

int playerJudgeTriangleC(RECT *rcHit, int tx, int ty, int *x, int *y)
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

int playerJudgeTriangleD(RECT *rcHit, int tx, int ty, int *x, int *y)
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
int playerJudgeTriangleE(RECT *rcHit, int tx, int ty, int *x, int *y, int wasGround)
{
	int hit = slopeE;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y + rcHit->bottom > (ty << 13) + (-0x2000 * tx + *x) / 2 - 0x800
		&& *y - rcHit->top < (2 * ty + 1) << 12)
	{
		*y = (ty << 13) + (-0x2000 * tx + *x) / 2 - 0x800 - rcHit->bottom;
		
		hit |= (ground | slopeLeft);
	}

	return hit;
}

int playerJudgeTriangleF(RECT *rcHit, int tx, int ty, int *x, int *y, int wasGround)
{
	int hit = slopeF;
	
	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y + rcHit->bottom > (ty << 13) + (-0x2000 * tx + *x) / 2 + 0x800
		&& *y - rcHit->top < (2 * ty + 1) << 12)
	{
		*y = (ty << 13) + (-0x2000 * tx + *x) / 2 + 0x800 - rcHit->bottom;
		
		hit |= (ground | slopeLeft);
	}
	return hit;
}

int playerJudgeTriangleG(RECT *rcHit, int tx, int ty, int *x, int *y, int wasGround)
{
	int hit = slopeG;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y + rcHit->bottom > (ty << 13) - (-0x2000 * tx + *x) / 2 + 0x800
		&& *y - rcHit->top < (2 * ty + 1) << 12)
	{
		*y = (ty << 13) - (-0x2000 * tx + *x) / 2 + 0x800 - rcHit->bottom;

		hit |= (ground | slopeRight);
	}

	return hit;
}

int playerJudgeTriangleH(RECT *rcHit, int tx, int ty, int *x, int *y, int wasGround)
{
	int hit = slopeH;

	if (*x > (2 * tx - 1) << 12
		&& *x < (2 * tx + 1) << 12
		&& *y + rcHit->bottom > (ty << 13) - (-0x2000 * tx + *x) / 2 - 0x800
		&& *y - rcHit->top < (2 * ty + 1) << 12)
	{
		*y = (ty << 13) - (-0x2000 * tx + *x) / 2 - 0x800 - rcHit->bottom;

		hit |= (ground | slopeRight);
	}

	return hit;
}

int playerJudgeWater(RECT *rcHit, int tx, int ty, int *x, int *y)
{
	int hit = 0;

	if (*x - rcHit->left < ((2 * tx + 1) << 12) - 0x600
		&& *x + rcHit->right > ((2 * tx - 1) << 12) + 0x600
		&& *y - rcHit->top < ((2 * ty + 1) << 12) - 0x600
		&& *y + rcHit->bottom > ty << 13)
	{
		hit = water;
	}

	return hit;
}

int playerJudgeSpike(RECT *rcHit, int tx, int ty, int *x, int *y)
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

int playerJudgeWaterSpike(RECT *rcHit, int tx, int ty, int *x, int *y)
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

int playerHitMap(RECT *rcHit, int *x, int *y, bool b44, bool wasGround)
{
	int hit = 0;

	int fromX = (*x - rcHit->left + 0x1000) >> 13;
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
				hit |= playerJudgeBlock(rcHit, current_X, current_Y, x, y);
				continue;

				//No NPC
			case(0x03): case(0x04): case(0x44): case(0x46):
				if (b44 == true) { hit |= playerJudgeBlock(rcHit, current_X, current_Y, x, y); }
				continue;

				//Water solid
			case(0x61):
				hit |= playerJudgeBlock(rcHit, current_X, current_Y, x, y);
				hit |= playerJudgeWater(rcHit, current_X, current_Y, x, y);
				continue;

				//Water
			case(0x02): case(0x60):
				hit |= playerJudgeWater(rcHit, current_X, current_Y, x, y);
				continue;

				//Spikes
			case(0x42):
				hit |= playerJudgeSpike(rcHit, current_X, current_Y, x, y);
				continue;

				//Water spikes
			case(0x62):
				hit |= playerJudgeWaterSpike(rcHit, current_X, current_Y, x, y);
				continue;

				//Slopes
			case(0x50):
				hit |= playerJudgeTriangleA(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x51):
				hit |= playerJudgeTriangleB(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x52):
				hit |= playerJudgeTriangleC(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x53):
				hit |= playerJudgeTriangleD(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x54):
				hit |= playerJudgeTriangleE(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x55):
				hit |= playerJudgeTriangleF(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x56):
				hit |= playerJudgeTriangleG(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x57):
				hit |= playerJudgeTriangleH(rcHit, current_X, current_Y, x, y, wasGround);
				continue;

				//Water slopes
			case(0x70):
				hit |= playerJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= playerJudgeTriangleA(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x71):
				hit |= playerJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= playerJudgeTriangleB(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x72):
				hit |= playerJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= playerJudgeTriangleC(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x73):
				hit |= playerJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= playerJudgeTriangleD(rcHit, current_X, current_Y, x, y);
				continue;
			case(0x74):
				hit |= playerJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= playerJudgeTriangleE(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x75):
				hit |= playerJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= playerJudgeTriangleF(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x76):
				hit |= playerJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= playerJudgeTriangleG(rcHit, current_X, current_Y, x, y, wasGround);
				continue;
			case(0x77):
				hit |= playerJudgeWater(rcHit, current_X, current_Y, x, y);
				hit |= playerJudgeTriangleH(rcHit, current_X, current_Y, x, y, wasGround);
				continue;

				//Default
			default:
				continue;
			}
		}
	}

	return hit;
}