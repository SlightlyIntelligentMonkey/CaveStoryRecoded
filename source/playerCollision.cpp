#include "npcCollision.h"

//Judging functions
int playerJudgeBlock(RECT *rcHit, player *me, int tx, int ty)
{
	int hit = 0; // [esp+1Ch] [ebp-Ch]

	if (me->y - rcHit->top < (2 * (2 * ty + 1) - 1) << 11
		&& rcHit->bottom + me->y > (2 * (2 * ty - 1) + 1) << 11
		&& me->x - rcHit->left < (2 * tx + 1) << 12
		&& me->x - rcHit->left > tx << 13)
	{
		me->x = ((2 * tx + 1) << 12) + rcHit->left;

		if (me->xsp < -0x180)
			me->xsp = -0x180;

		if (!(me->key && keyDown(SDL_SCANCODE_LEFT)) && me->xsp < 0)
			me->xsp = 0;

		hit |= leftWall;
	}

	if (me->y - rcHit->top < (2 * (2 * ty + 1) - 1) << 11
		&& rcHit->bottom + me->y > (2 * (2 * ty - 1) + 1) << 11
		&& rcHit->right + me->x > (2 * tx - 1) << 12
		&& rcHit->right + me->x < tx << 13)
	{
		me->x = ((2 * tx - 1) << 12) - rcHit->right;

		if (me->xsp > 0x180)
			me->xsp = 0x180;

		if (!(me->key && keyDown(SDL_SCANCODE_RIGHT)) && me->xsp > 0)
			me->xsp = 0;

		hit |= rightWall;
	}

	if (me->x - rcHit->left < ((2 * tx + 1) << 12) - 0x600
		&& rcHit->right + me->x > ((2 * tx - 1) << 12) + 0x600
		&& me->y - rcHit->top < (2 * ty + 1) << 12
		&& me->y - rcHit->top > ty << 13)
	{
		me->y = ((2 * ty + 1) << 12) + rcHit->top;

		//if (!(me->cond & player_removed) && me->ysp < -512)
			//_PutlittleStar();

		if (me->ysp < 0)
			me->ysp = 0;

		hit |= ceiling;
	}

	if (me->x - rcHit->left < ((2 * tx + 1) << 12) - 0x600
		&& rcHit->right + me->x > ((2 * tx - 1) << 12) + 0x600
		&& rcHit->bottom + me->y > (2 * ty - 1) << 12
		&& rcHit->bottom + me->y < ty << 13)
	{
		me->y = ((2 * ty - 1) << 12) - rcHit->bottom;

		//if (me->ysp > 1024)
			//PlaySoundObject(23, 1);

		if (me->ysp > 0)
			me->ysp = 0;

		hit |= ground;
	}

	return hit;
}

//Ceiling slopes
int playerJudgeTriangleA(RECT *rcHit, player *me, int tx, int ty)
{
	int hit = 0;
	
	if (me->x > (2 * tx - 1) << 12
		&& me->x < (2 * tx + 1) << 12
		&& me->y - rcHit->top < (ty << 13) - (-0x2000 * tx + me->x) / 2 + 0x800
		&& me->y + rcHit->bottom > (2 * ty - 1) << 12)
	{
		if (me->ysp < 0)
			me->ysp = 0;

		me->y = (ty << 13) - (-0x2000 * tx + me->x) / 2 + 0x800 + rcHit->top;
		
		hit = ceiling;
	}

	return hit;
}

int playerJudgeTriangleB(RECT *rcHit, player *me, int tx, int ty)
{
	int hit = 0;

	if (me->x > (2 * tx - 1) << 12
		&& me->x < (2 * tx + 1) << 12
		&& me->y - rcHit->top < (ty << 13) - (-0x2000 * tx + me->x) / 2 - 0x800
		&& me->y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		if (me->ysp < 0)
			me->ysp = 0;

		me->y = (ty << 13) - (-0x2000 * tx + me->x) / 2 - 0x800 + rcHit->top;

		hit = ceiling;
	}

	return hit;
}

int playerJudgeTriangleC(RECT *rcHit, player *me, int tx, int ty)
{
	int hit = 0;

	if (me->x > (2 * tx - 1) << 12
		&& me->x < (2 * tx + 1) << 12
		&& me->y - rcHit->top < (ty << 13) + (-0x2000 * tx + me->x) / 2 - 0x800
		&& me->y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		if (me->ysp < 0)
			me->ysp = 0;

		me->y = (ty << 13) + (-0x2000 * tx + me->x) / 2 - 0x800 + rcHit->top;

		hit = ceiling;
	}

	return hit;
}

int playerJudgeTriangleD(RECT *rcHit, player *me, int tx, int ty)
{
	int hit = 0;

	if (me->x > (2 * tx - 1) << 12
		&& me->x < (2 * tx + 1) << 12
		&& me->y - rcHit->top < (ty << 13) + (-0x2000 * tx + me->x) / 2 + 0x800
		&& me->y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		if (me->ysp < 0)
			me->ysp = 0;

		me->y = (ty << 13) + (-0x2000 * tx + me->x) / 2 + 0x800 + rcHit->top;

		hit = ceiling;
	}

	return hit;
}

//Floor slopes
int playerJudgeTriangleE(RECT *rcHit, player *me, int tx, int ty)
{
	if (me->x > (2 * tx - 1) << 12
		&& me->x < (2 * tx + 1) << 12
		&& me->y + rcHit->bottom > (ty << 13) + (-0x2000 * tx + me->x) / 2 - 0x800
		&& me->y - rcHit->top < (2 * ty + 1) << 12)
	{
		me->y = (ty << 13) + (-0x2000 * tx + me->x) / 2 - 0x800 - rcHit->bottom;

		if (me->ysp > 0)
			me->ysp = 0;
		
		return (ground | slopeLeft | slopeE);
	}

	return slopeE;
}

int playerJudgeTriangleF(RECT *rcHit, player *me, int tx, int ty)
{
	if (me->x < (2 * tx + 1) << 12
		&& me->x >(2 * tx - 1) << 12
		&& me->y + rcHit->bottom > (ty << 13) + (-0x2000 * tx + me->x) / 2 + 0x800
		&& me->y - rcHit->top < (2 * ty + 1) << 12)
	{
		me->y = (ty << 13) + (-0x2000 * tx + me->x) / 2 + 0x800 - rcHit->bottom;

		return (ground | slopeLeft | slopeF);
	}

	return slopeF;
}

int playerJudgeTriangleG(RECT *rcHit, player *me, int tx, int ty)
{
	if (me->x > (2 * tx - 1) << 12
		&& me->x < (2 * tx + 1) << 12
		&& me->y + rcHit->bottom > (ty << 13) - (-0x2000 * tx + me->x) / 2 + 0x800
		&& me->y - rcHit->top < (2 * ty + 1) << 12)
	{
		me->y = (ty << 13) - (-0x2000 * tx + me->x) / 2 + 0x800 - rcHit->bottom;

		if (me->ysp > 0)
			me->ysp = 0;

		return (ground | slopeRight | slopeG);
	}

	return slopeG;
}

int playerJudgeTriangleH(RECT *rcHit, player *me, int tx, int ty)
{
	if (me->x > (2 * tx - 1) << 12
		&& me->x < (2 * tx + 1) << 12
		&& me->y + rcHit->bottom > (ty << 13) - (-0x2000 * tx + me->x) / 2 - 0x800
		&& me->y - rcHit->top < (2 * ty + 1) << 12)
	{
		me->y = (ty << 13) - (-0x2000 * tx + me->x) / 2 - 0x800 - rcHit->bottom;

		if (me->ysp > 0)
			me->ysp = 0;

		return (ground | slopeRight | slopeH);
	}

	return slopeH;
}

int playerJudgeWater(RECT *rcHit, player *me, int tx, int ty)
{
	int hit = 0;

	if (me->x - rcHit->left < ((2 * tx + 1) << 12) - 0x600
		&& me->x + rcHit->right > ((2 * tx - 1) << 12) + 0x600
		&& me->y - rcHit->top < ((2 * ty + 1) << 12) - 0x600
		&& me->y + rcHit->bottom > ty << 13)
	{
		hit = water;
	}

	return hit;
}

int playerJudgeSpike(RECT *rcHit, player *me, int tx, int ty)
{
	if (me->x - 0x800 < (4 * tx + 1) << 11
		&& me->x + 0x800 > (4 * tx - 1) << 11
		&& me->y - 0x800 < (ty << 13) + 0x600
		&& me->y + 0x800 > (ty << 13) - 0x600)
	{
		return spike;
	}

	return 0;
}

int playerJudgeWaterSpike(RECT *rcHit, player *me, int tx, int ty)
{
	if (me->x - 0x800 < (4 * tx + 1) << 11
		&& me->x + 0x800 > (4 * tx - 1) << 11
		&& me->y - 0x800 < (ty << 13) + 0x600
		&& me->y + 0x800 > (ty << 13) - 0x600)
	{
		return (spike | water | bloodWater);
	}

	return 0;
}

//Current
int playerJudgeCurrentLeft(RECT *rcHit, player *me, int tx, int ty)
{
	if (me->x - rcHit->left < (4 * (2 * tx + 1) - 1) << 10
		&& me->x + rcHit->right > (4 * (2 * tx - 1) + 1) << 10
		&& me->y - rcHit->top < (4 * (2 * ty + 1) - 1) << 10
		&& me->y + rcHit->bottom > (4 * (2 * ty - 1) + 1) << 10)
	{
		return windLeft;
	}

	return 0;
}

int playerJudgeCurrentUp(RECT *rcHit, player *me, int tx, int ty)
{
	if (me->x - rcHit->left < (4 * (2 * tx + 1) - 1) << 10
		&& me->x + rcHit->right >(4 * (2 * tx - 1) + 1) << 10
		&& me->y - rcHit->top < (4 * (2 * ty + 1) - 1) << 10
		&& me->y + rcHit->bottom >(4 * (2 * ty - 1) + 1) << 10)
	{
		return windUp;
	}

	return 0;
}

int playerJudgeCurrentRight(RECT *rcHit, player *me, int tx, int ty)
{
	if (me->x - rcHit->left < (4 * (2 * tx + 1) - 1) << 10
		&& me->x + rcHit->right >(4 * (2 * tx - 1) + 1) << 10
		&& me->y - rcHit->top < (4 * (2 * ty + 1) - 1) << 10
		&& me->y + rcHit->bottom >(4 * (2 * ty - 1) + 1) << 10)
	{
		return windRight;
	}

	return 0;
}

int playerJudgeCurrentDown(RECT *rcHit, player *me, int tx, int ty)
{
	if (me->x - rcHit->left < (4 * (2 * tx + 1) - 1) << 10
		&& me->x + rcHit->right >(4 * (2 * tx - 1) + 1) << 10
		&& me->y - rcHit->top < (4 * (2 * ty + 1) - 1) << 10
		&& me->y + rcHit->bottom >(4 * (2 * ty - 1) + 1) << 10)
	{
		return windDown;
	}

	return 0;
}

//Main function
int playerHitMap(RECT *rcHit)
{
	int hit = 0;

	player *me = &currentPlayer;

	int fromX = (me->x - rcHit->left + 0x1000) >> 13;
	int fromY = (me->y - rcHit->top + 0x1000) >> 13;
	int toX = (me->x + rcHit->right + 0x1000) >> 13;
	int toY = (me->y + rcHit->bottom + 0x1000) >> 13;

	for (int currentX = fromX; currentX <= toX; currentX++)
	{
		for (int currentY = fromY; currentY <= toY; currentY++)
		{
			switch (getTileAttribute(currentX, currentY))
			{
				//Solid
			case 0x05u: case 0x41u: case 0x43u: case 0x46u:
				hit |= playerJudgeBlock(rcHit, me, currentX, currentY);
				break;

				//Spike
			case 0x42u:
				hit |= playerJudgeSpike(rcHit, me, currentX, currentY);
				break;

				//Ceiling slopes
			case 0x50u:
				hit |= playerJudgeTriangleA(rcHit, me, currentX, currentY);
				break;

			case 0x51u:
				hit |= playerJudgeTriangleB(rcHit, me, currentX, currentY);
				break;

			case 0x52u:
				hit |= playerJudgeTriangleC(rcHit, me, currentX, currentY);
				break;

			case 0x53u:
				hit |= playerJudgeTriangleD(rcHit, me, currentX, currentY);
				break;

				//Floor slopes
			case 0x54u:
				hit |= playerJudgeTriangleE(rcHit, me, currentX, currentY);
				break;

			case 0x55u:
				hit |= playerJudgeTriangleF(rcHit, me, currentX, currentY);
				break;

			case 0x56u:
				hit |= playerJudgeTriangleG(rcHit, me, currentX, currentY);
				break;

			case 0x57u:
				hit |= playerJudgeTriangleH(rcHit, me, currentX, currentY);
				break;

				//Water, water solid, and water spike
			case 0x60u:
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0x61u:
				hit |= playerJudgeBlock(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0x62u:
				hit |= playerJudgeWaterSpike(rcHit, me, currentX, currentY);
				break;

					//Water slopes
				//Ceiling slopes
			case 0x70u:
				hit |= playerJudgeTriangleA(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0x71u:
				hit |= playerJudgeTriangleB(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0x72u:
				hit |= playerJudgeTriangleC(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0x73u:
				hit |= playerJudgeTriangleD(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;
				
				//Floor slopes
			case 0x74u:
				hit |= playerJudgeTriangleE(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0x75u:
				hit |= playerJudgeTriangleF(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0x76u:
				hit |= playerJudgeTriangleG(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0x77u:
				hit |= playerJudgeTriangleH(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

				//Wind
			case 0x80u:
				hit |= playerJudgeCurrentLeft(rcHit, me, currentX, currentY);
				break;

			case 0x81u:
				hit |= playerJudgeCurrentUp(rcHit, me, currentX, currentY);
				break;

			case 0x82u:
				hit |= playerJudgeCurrentRight(rcHit, me, currentX, currentY);
				break;

			case 0x83u:
				hit |= playerJudgeCurrentDown(rcHit, me, currentX, currentY);
				break;

				//Water current
			case 0xA0u:
				hit |= playerJudgeCurrentLeft(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0xA1u:
				hit |= playerJudgeCurrentUp(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0xA2u:
				hit |= playerJudgeCurrentRight(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

			case 0xA3u:
				hit |= playerJudgeCurrentDown(rcHit, me, currentX, currentY);
				hit |= playerJudgeWater(rcHit, me, currentX, currentY);
				break;

				//Default
			default:
				break;
			}
		}
	}

	return hit;
}

//Npc hit functions
int playerHitNpcSoftSolid(RECT *rcHit, player *me, npc *NPC)
{
	int hit = 0;

	if (me->y - 0x1000 < NPC->y + NPC->collideRect.bottom - 0x600
		&& me->y + 0x1000 > NPC->y - NPC->collideRect.top + 0x600
		&& me->x - 0xA00 < NPC->x + NPC->collideRect.right
		&& me->x - 0xA00 > NPC->x)
	{
		if (me->xsp <= 0x1FF)
			me->xsp += 0x200;

		hit = leftWall;
	}

	if (me->y - 0x1000 < NPC->y + NPC->collideRect.bottom - 0x600
		&& me->y + 0x1000 > NPC->y - NPC->collideRect.top + 0x600
		&& me->x + 0xA00 - 0x200 > NPC->x - NPC->collideRect.right
		&& me->x + 0xA00 - 0x200 < NPC->x)
	{
		if (me->xsp >= -0x1FF)
			me->xsp -= 0x200;

		hit |= rightWall;
	}

	if (me->x - 0xA00 < NPC->x + NPC->collideRect.right - 0x600
		&& me->x + 0xA00 > NPC->x - NPC->collideRect.right + 0x600
		&& me->y - 0x1000 < NPC->y + NPC->collideRect.bottom
		&& me->y - 0x1000 > NPC->y)
	{
		if (me->ysp < 0)
			me->ysp = 0;

		hit |= ceiling;
	}

	if (me->x - 0xA00 < NPC->x + NPC->collideRect.right - 0x600
		&& me->x + 0xA00 > NPC->x - NPC->collideRect.right + 0x600
		&& me->y + 0x1000 > NPC->y - NPC->collideRect.top
		&& me->y + 0x1000 < NPC->y + 0x600)
	{
		if (NPC->flags & npc_bouncy)
		{
			me->ysp = NPC->ysp - 0x200;

			hit |= ground;
		}
		else if (!(me->flags & 8) && me->ysp > NPC->ysp)
		{
			me->y = NPC->y - NPC->collideRect.top - 0x1000 + 0x200;
			me->ysp = NPC->ysp;

			me->x += NPC->xsp;
			hit |= ground;
		}
	}

	return hit;
}

int playerHitNpcHardSolid(RECT *rcHit, player *me, npc *NPC)
{
	long double xDiff = 0.0;
	long double yDiff = 0.0;
	int hit = 0;

	long double fx1 = 0.0;
	long double fx2 = 0.0;
	long double fy1 = 0.0;
	long double fy2 = 0.0;

	if (NPC->x <= me->x)
		xDiff = me->x - NPC->x;
	else
		xDiff = NPC->x - me->x;

	fx1 = xDiff;
	fx2 = NPC->collideRect.right;

	if (fx1 == 0.0)
		fx1 = 1.0;
	if (fx2 == 0.0)
		fx2 = 1.0;

	if (NPC->y <= me->y)
		yDiff = me->y - NPC->y;
	else
		yDiff = NPC->y - me->y;

	fy1 = yDiff;
	fy2 = NPC->collideRect.top;

	if (fy1 / fx1 <= fy2 / fx2)
	{
		if (me->y - rcHit->top < NPC->y + NPC->collideRect.bottom && me->y + rcHit->bottom > NPC->y - NPC->collideRect.top)
		{
			if (me->x - rcHit->left < NPC->x + NPC->collideRect.right && me->x - rcHit->left > NPC->x)
			{
				if (me->xsp < NPC->xsp)
					me->xsp = NPC->xsp;

				me->x = NPC->collideRect.right + NPC->x + rcHit->left;

				hit = leftWall;
			}

			if (me->x + rcHit->right > NPC->x - NPC->collideRect.right && me->x + rcHit->right < NPC->x)
			{
				if (me->xsp > NPC->xsp)
					me->xsp = NPC->xsp;

				me->x = NPC->x - NPC->collideRect.right - rcHit->right;

				hit |= rightWall;
			}
		}
	}
	else if (me->x - rcHit->left < NPC->x + NPC->collideRect.right && me->x + rcHit->right > NPC->x - NPC->collideRect.right)
	{
		if (me->y - rcHit->top < NPC->y + NPC->collideRect.bottom && me->y - rcHit->top > NPC->y)
		{
			if (me->ysp >= NPC->ysp)
			{
				if (me->ysp < 0)
					me->ysp = 0;
			}
			else
			{
				me->y = NPC->y + NPC->collideRect.bottom + rcHit->top + 0x200;

				me->ysp = NPC->ysp;
			}

			hit = ceiling;
		}

		if (me->y + rcHit->bottom > NPC->y - NPC->collideRect.top && me->y + rcHit->bottom < NPC->y + 0x600)
		{
			//if (gMC.ysp - npc->ysp > 1024)
			//	PlaySoundObject(23, 1);

			//if (unk_81C8594 == 1)
			//{
			//	y = npc->y - npc->collideRect.top - unk_81C85D0 + 512;
			//	hit |= 8u;
			//}

			if (NPC->flags & npc_bouncy)
			{
				me->ysp = NPC->ysp - 0x200;

				hit |= ground;
			}
			else if (!(me->flags & ground) && me->ysp > NPC->ysp)
			{
				me->y = NPC->y - NPC->collideRect.top - rcHit->bottom + 0x200;

				me->ysp = NPC->ysp;
				me->x += NPC->xsp;

				hit |= ground;
			}
		}
	}
	return hit;
}

int playerHitNpcNonSolid(RECT *rcHit, player *me, npc *NPC)
{
	if (NPC->direction)
	{
		if (me->x + 0x400 > NPC->x - NPC->collideRect.right
			&& me->x - 0x400 < NPC->x + NPC->collideRect.left
			&& me->y + 0x400 > NPC->y - NPC->collideRect.top
			&& me->y - 0x400 < NPC->y + NPC->collideRect.bottom)
		{
			return 1;
		}
	}

	if (me->x + 0x400 > NPC->x - NPC->collideRect.left
		&& me->x - 0x400 < NPC->x + NPC->collideRect.right
		&& me->y + 0x400 > NPC->y - NPC->collideRect.top
		&& me->y - 0x400 < NPC->y + NPC->collideRect.bottom)
	{
		return 1;
	}

	return 0;
}

int playerHitNpcs(RECT *rcHit)
{
	int hit = 0;

	player *me = &currentPlayer;

	if ((me->cond & player_visible) && !(me->cond & player_removed))
	{
		for (uint32_t i = 0; i < npcs.size(); ++i)
		{
			if ((npcs[i].cond & npccond_alive))
			{
				if (npcs[i].flags & npc_solidsoft)
				{
					hit = playerHitNpcSoftSolid(rcHit, me, &npcs[i]);

					me->flags |= hit;
				}
				else if (npcs[i].flags & npc_solidhard)
				{
					hit = playerHitNpcHardSolid(rcHit, me, &npcs[i]);

					me->flags |= hit;
				}
				else
				{
					hit = playerHitNpcNonSolid(rcHit, me, &npcs[i]);
				}

				if (hit && npcs[i].type == 1)
				{
					//PlaySoundObject(14, 1);
					//AddExpMyChar(gNPC[i].exp);
					npcs[i].cond = 0;
				}
				if (hit && npcs[i].type == 86)
				{
					//PlaySoundObject(42, 1);
					//AddBulletMyChar(gNPC[i].code_event, gNPC[i].exp);
					npcs[i].cond = 0;
				}

				if (hit && npcs[i].type == 87)
				{
					//PlaySoundObject(20, 1);
					//AddLifeMyChar(gNPC[i].exp);
					npcs[i].cond = 0;
				}

				if (npcs[i].flags & npc_eventtouch)//!(g_GameFlags & 4) && hit && gNPC[i].bits & 0x100)
				{
					//StartTextScript(gNPC[i].code_event);
				}

				if (!(npcs[i].flags & npc_interact))
				{
					if (npcs[i].flags & npc_reartop)
					{
						if (hit & rightWall && npcs[i].xsp < 0)
							me->hit(npcs[i].damage);
						if (hit & leftWall && npcs[i].xsp > 0)
							me->hit(npcs[i].damage);
						if (hit & ground && npcs[i].ysp < 0)
							me->hit(npcs[i].damage);
						if (hit & ceiling && npcs[i].ysp > 0)
							me->hit(npcs[i].damage);
					}
					else if (hit && npcs[i].damage)
					{
						me->hit(npcs[i].damage);
					}
				}

				if (hit && me->cond & player_inspect && npcs[i].flags & npc_interact)
				{
					//StartTextScript(gNPC[i].code_event);
					me->xsp = 0;
					me->interacting = false;
				}
			}
		}
	}

	return 1;
}
