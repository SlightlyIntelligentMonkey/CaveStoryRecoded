#include "npcCollision.h"

void npcJudgeBlock(RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->y - rcHit->top < ((2 * ty + 1) << 12) - 0x600
		&& NPC->y + rcHit->bottom > ((2 * ty - 1) << 12) + 0x600
		&& NPC->x - rcHit->right < (2 * tx + 1) << 12
		&& NPC->x - rcHit->right > tx << 13)
	{
		NPC->x = ((2 * tx + 1) << 12) + rcHit->right;

		NPC->hit |= leftWall;
	}

	if (NPC->y - rcHit->top < ((2 * ty + 1) << 12) - 0x600
		&& NPC->y + rcHit->bottom > ((2 * ty - 1) << 12) + 0x600
		&& NPC->x + rcHit->right > (2 * tx - 1) << 12
		&& NPC->x + rcHit->right < tx << 13)
	{
		NPC->x = ((2 * tx - 1) << 12) - rcHit->right;

		NPC->hit |= rightWall;
	}

	if (NPC->x - rcHit->right < ((2 * tx + 1) << 12) - 0x600
		&& NPC->x + rcHit->right > ((2 * tx - 1) << 12) + 0x600
		&& NPC->y - rcHit->top < (2 * ty + 1) << 12
		&& NPC->y - rcHit->top > ty << 13)
	{
		NPC->y = ((2 * ty + 1) << 12) + rcHit->top;
		NPC->ysp = 0;

		NPC->hit |= ceiling;
	}

	if (NPC->x - rcHit->right < ((2 * tx + 1) << 12) - 0x600
		&& NPC->x + rcHit->right > ((2 * tx - 1) << 12) + 0x600
		&& NPC->y + rcHit->bottom > (2 * ty - 1) << 12
		&& NPC->y + rcHit->bottom < ty << 13)
	{
		NPC->y = ((2 * ty - 1) << 12) - rcHit->bottom;
		NPC->ysp = 0;

		NPC->hit |= ground;
	}
}

//Ceiling slopes
void npcJudgeTriangleA(RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->x > (2 * tx - 1) << 12
		&& NPC->x < (2 * tx + 1) << 12
		&& NPC->y - rcHit->top < (ty << 13) - (-0x2000 * tx + NPC->x) / 2 + 0x800
		&& NPC->y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		if (NPC->ysp < 0)
			NPC->ysp = 0;

		NPC->y = (ty << 13) - (-0x2000 * tx + NPC->x) / 2 + 0x800 + rcHit->top;

		NPC->hit |= ceiling;
	}
}

void npcJudgeTriangleB(RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->x > (2 * tx - 1) << 12
		&& NPC->x < (2 * tx + 1) << 12
		&& NPC->y - rcHit->top < (ty << 13) - (-0x2000 * tx + NPC->x) / 2 - 0x800
		&& NPC->y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		if (NPC->ysp < 0)
			NPC->ysp = 0;

		NPC->y = (ty << 13) - (-0x2000 * tx + NPC->x) / 2 - 0x800 + rcHit->top;

		NPC->hit |= ceiling;
	}
}

void npcJudgeTriangleC(RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->x > (2 * tx - 1) << 12
		&& NPC->x < (2 * tx + 1) << 12
		&& NPC->y - rcHit->top < (ty << 13) + (-0x2000 * tx + NPC->x) / 2 - 0x800
		&& NPC->y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		if (NPC->ysp < 0)
			NPC->ysp = 0;

		NPC->y = (ty << 13) + (-0x2000 * tx + NPC->x) / 2 - 0x800 + rcHit->top;

		NPC->hit |= ceiling;
	}
}

void npcJudgeTriangleD(RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->x > (2 * tx - 1) << 12
		&& NPC->x < (2 * tx + 1) << 12
		&& NPC->y - rcHit->top < (ty << 13) + (-0x2000 * tx + NPC->x) / 2 + 0x800
		&& NPC->y + rcHit->bottom >(2 * ty - 1) << 12)
	{
		if (NPC->ysp < 0)
			NPC->ysp = 0;

		NPC->y = (ty << 13) + (-0x2000 * tx + NPC->x) / 2 + 0x800 + rcHit->top;

		NPC->hit |= ceiling;
	}
}

//Floor slopes
void npcJudgeTriangleE(RECT *rcHit, npc *NPC, int tx, int ty)
{
	NPC->hit |= slopeE;

	if (NPC->x > (2 * tx - 1) << 12
		&& NPC->x < (2 * tx + 1) << 12
		&& NPC->y + rcHit->bottom >(ty << 13) + (-0x2000 * tx + NPC->x) / 2 - 0x800
		&& NPC->y - rcHit->top < (2 * ty + 1) << 12)
	{
		NPC->y = (ty << 13) + (-0x2000 * tx + NPC->x) / 2 - 0x800 - rcHit->bottom;

		if (NPC->ysp > 0)
			NPC->ysp = 0;

		NPC->hit |= (ground | slopeLeft);
	}
}

void npcJudgeTriangleF(RECT *rcHit, npc *NPC, int tx, int ty)
{
	NPC->hit |= slopeF;

	if (NPC->x < (2 * tx + 1) << 12
		&& NPC->x >(2 * tx - 1) << 12
		&& NPC->y + rcHit->bottom > (ty << 13) + (-0x2000 * tx + NPC->x) / 2 + 0x800
		&& NPC->y - rcHit->top < (2 * ty + 1) << 12)
	{
		NPC->y = (ty << 13) + (-0x2000 * tx + NPC->x) / 2 + 0x800 - rcHit->bottom;

		NPC->hit |= (ground | slopeLeft);
	}
}

void npcJudgeTriangleG(RECT *rcHit, npc *NPC, int tx, int ty)
{
	NPC->hit |= slopeG;

	if (NPC->x > (2 * tx - 1) << 12
		&& NPC->x < (2 * tx + 1) << 12
		&& NPC->y + rcHit->bottom >(ty << 13) - (-0x2000 * tx + NPC->x) / 2 + 0x800
		&& NPC->y - rcHit->top < (2 * ty + 1) << 12)
	{
		NPC->y = (ty << 13) - (-0x2000 * tx + NPC->x) / 2 + 0x800 - rcHit->bottom;

		if (NPC->ysp > 0)
			NPC->ysp = 0;

		NPC->hit |= (ground | slopeRight);
	}
}

void npcJudgeTriangleH(RECT *rcHit, npc *NPC, int tx, int ty)
{
	NPC->hit |= slopeH;

	if (NPC->x > (2 * tx - 1) << 12
		&& NPC->x < (2 * tx + 1) << 12
		&& NPC->y + rcHit->bottom >(ty << 13) - (-0x2000 * tx + NPC->x) / 2 - 0x800
		&& NPC->y - rcHit->top < (2 * ty + 1) << 12)
	{
		NPC->y = (ty << 13) - (-0x2000 * tx + NPC->x) / 2 - 0x800 - rcHit->bottom;

		if (NPC->ysp > 0)
			NPC->ysp = 0;

		NPC->hit |= (ground | slopeRight);
	}
}

void npcJudgeWater(RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->x - rcHit->right < (4 * (2 * tx + 1) - 1) << 10
		&& NPC->x + rcHit->right > (4 * (2 * tx - 1) + 1) << 10
		&& NPC->y - rcHit->top < (4 * (2 * ty + 1) - 1) << 10
		&& NPC->y + rcHit->bottom > (4 * (2 * ty - 1) + 1) << 10)
	{
		NPC->hit |= water;
	}
}

void npcHitMap(int NPCid)
{
	npc *NPC = &npcs[NPCid];

	NPC->hit = 0; //clear

	RECT *rcHit = &NPC->collideRect;
	
	int fromX = (NPC->x - rcHit->right + 0x1000) >> 13;
	int fromY = (NPC->y - rcHit->top + 0x1000) >> 13;

	int toX = (NPC->x + rcHit->right + 0x1000) >> 13;
	int toY = (NPC->y + rcHit->bottom + 0x1000) >> 13;
	
	if (!(NPC->flags & npc_ignoresolid))
	{
		for (int currentX = fromX; currentX <= toX; currentX++)
		{
			for (int currentY = fromY; currentY <= toY; currentY++)
			{
				switch (getTileAttribute(currentX, currentY))
				{
					//Solid
				case 0x03: case 0x05: case 0x41: case 0x43:
					npcJudgeBlock(rcHit, NPC, currentX, currentY);
					break;

					//Npc Only solid
				case 0x44u:
					if (!(NPC->flags & npc_ignore44))
						npcJudgeBlock(rcHit, NPC, currentX, currentY);
					break;

					//Water
				case 0x02: case 0x60: case 0x62:
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

					//Water solid
				case 0x04: case 0x61: case 0x64:
					npcJudgeBlock(rcHit, NPC, currentX, currentY);
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

					//Ceiling slopes
				case 0x50u:
					npcJudgeTriangleA(rcHit, NPC, currentX, currentY);
					break;

				case 0x51u:
					npcJudgeTriangleB(rcHit, NPC, currentX, currentY);
					break;

				case 0x52u:
					npcJudgeTriangleC(rcHit, NPC, currentX, currentY);
					break;

				case 0x53u:
					npcJudgeTriangleD(rcHit, NPC, currentX, currentY);
					break;

					//Floor slopes
				case 0x54u:
					npcJudgeTriangleE(rcHit, NPC, currentX, currentY);
					break;

				case 0x55u:
					npcJudgeTriangleF(rcHit, NPC, currentX, currentY);
					break;

				case 0x56u:
					npcJudgeTriangleG(rcHit, NPC, currentX, currentY);
					break;

				case 0x57u:
					npcJudgeTriangleH(rcHit, NPC, currentX, currentY);
					break;

					//Water slopes
					//Ceiling slopes
				case 0x70u:
					npcJudgeTriangleA(rcHit, NPC, currentX, currentY);
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

				case 0x71u:
					npcJudgeTriangleB(rcHit, NPC, currentX, currentY);
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

				case 0x72u:
					npcJudgeTriangleC(rcHit, NPC, currentX, currentY);
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

				case 0x73u:
					npcJudgeTriangleD(rcHit, NPC, currentX, currentY);
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

					//Floor slopes
				case 0x74u:
					npcJudgeTriangleE(rcHit, NPC, currentX, currentY);
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

				case 0x75u:
					npcJudgeTriangleF(rcHit, NPC, currentX, currentY);
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

				case 0x76u:
					npcJudgeTriangleG(rcHit, NPC, currentX, currentY);
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

				case 0x77u:
					npcJudgeTriangleH(rcHit, NPC, currentX, currentY);
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

					//Currents
				case 0x80u:
					NPC->hit |= (windLeft);
					break;

				case 0x81u:
					NPC->hit |= (windUp);
					break;

				case 0x82u:
					NPC->hit |= (windRight);
					break;

				case 0x83u:
					NPC->hit |= (windDown);
					break;

					//Water current
				case 0xA0u:
					NPC->hit |= (water | windLeft);
					break;

				case 0xA1u:
					NPC->hit |= (water | windUp);
					break;

				case 0xA2u:
					NPC->hit |= (water | windRight);
					break;

				case 0xA3u:
					NPC->hit |= (water | windDown);
					break;

					//Default
				default:
					break;
				}
			}
		}
	}
}
