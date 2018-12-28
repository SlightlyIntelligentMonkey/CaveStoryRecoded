#include "npcCollision.h"

void npcJudgeBlock(const RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->y - rcHit->top < ((2 * ty + 1) << 12) - 0x600
	        && NPC->y + rcHit->bottom > ((2 * ty - 1) << 12) + 0x600
	        && NPC->x - rcHit->right < (2 * tx + 1) << 12
	        && NPC->x - rcHit->right > tilesToUnits(tx))
	{
		NPC->x = ((2 * tx + 1) << 12) + rcHit->right;

		NPC->flag |= leftWall;
	}

	if (NPC->y - rcHit->top < ((2 * ty + 1) << 12) - 0x600
	        && NPC->y + rcHit->bottom > ((2 * ty - 1) << 12) + 0x600
	        && NPC->x + rcHit->right > (2 * tx - 1) << 12
	        && NPC->x + rcHit->right < tilesToUnits(tx))
	{
		NPC->x = ((2 * tx - 1) << 12) - rcHit->right;

		NPC->flag |= rightWall;
	}

	if (NPC->x - rcHit->right < ((2 * tx + 1) << 12) - 0x600
	        && NPC->x + rcHit->right > ((2 * tx - 1) << 12) + 0x600
	        && NPC->y - rcHit->top < (2 * ty + 1) << 12
	        && NPC->y - rcHit->top > tilesToUnits(ty))
	{
		NPC->y = ((2 * ty + 1) << 12) + rcHit->top;
		NPC->ym = 0;

		NPC->flag |= ceiling;
	}

	if (NPC->x - rcHit->right < ((2 * tx + 1) << 12) - 0x600
	        && NPC->x + rcHit->right > ((2 * tx - 1) << 12) + 0x600
	        && NPC->y + rcHit->bottom > (2 * ty - 1) << 12
	        && NPC->y + rcHit->bottom < tilesToUnits(ty))
	{
		NPC->y = ((2 * ty - 1) << 12) - rcHit->bottom;
		NPC->ym = 0;

		NPC->flag |= ground;
	}
}

//Ceiling slopes
void npcJudgeTriangleA(const RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->x > (2 * tx - 1) << 12
	        && NPC->x < (2 * tx + 1) << 12
	        && NPC->y - rcHit->top < tilesToUnits(ty) - (-0x2000 * tx + NPC->x) / 2 + 0x800
	        && NPC->y + rcHit->bottom > (2 * ty - 1) << 12)
	{
		if (NPC->ym < 0)
			NPC->ym = 0;

		NPC->y = tilesToUnits(ty) - (-0x2000 * tx + NPC->x) / 2 + 0x800 + rcHit->top;

		NPC->flag |= ceiling;
	}
}

void npcJudgeTriangleB(const RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->x > (2 * tx - 1) << 12
	        && NPC->x < (2 * tx + 1) << 12
	        && NPC->y - rcHit->top < tilesToUnits(ty) - (-0x2000 * tx + NPC->x) / 2 - 0x800
	        && NPC->y + rcHit->bottom > (2 * ty - 1) << 12)
	{
		if (NPC->ym < 0)
			NPC->ym = 0;

		NPC->y = tilesToUnits(ty) - (-0x2000 * tx + NPC->x) / 2 - 0x800 + rcHit->top;

		NPC->flag |= ceiling;
	}
}

void npcJudgeTriangleC(const RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->x > (2 * tx - 1) << 12
	        && NPC->x < (2 * tx + 1) << 12
	        && NPC->y - rcHit->top < tilesToUnits(ty) + (-0x2000 * tx + NPC->x) / 2 - 0x800
	        && NPC->y + rcHit->bottom > (2 * ty - 1) << 12)
	{
		if (NPC->ym < 0)
			NPC->ym = 0;

		NPC->y = tilesToUnits(ty) + (-0x2000 * tx + NPC->x) / 2 - 0x800 + rcHit->top;

		NPC->flag |= ceiling;
	}
}

void npcJudgeTriangleD(const RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->x > (2 * tx - 1) << 12
	        && NPC->x < (2 * tx + 1) << 12
	        && NPC->y - rcHit->top < tilesToUnits(ty) + (-0x2000 * tx + NPC->x) / 2 + 0x800
	        && NPC->y + rcHit->bottom > (2 * ty - 1) << 12)
	{
		if (NPC->ym < 0)
			NPC->ym = 0;

		NPC->y = tilesToUnits(ty) + (-0x2000 * tx + NPC->x) / 2 + 0x800 + rcHit->top;

		NPC->flag |= ceiling;
	}
}

//Floor slopes
void npcJudgeTriangleE(const RECT *rcHit, npc *NPC, int tx, int ty)
{
	NPC->flag |= slopeE;

	if (NPC->x > (2 * tx - 1) << 12
	        && NPC->x < (2 * tx + 1) << 12
	        && NPC->y + rcHit->bottom > tilesToUnits(ty) + (-0x2000 * tx + NPC->x) / 2 - 0x800
	        && NPC->y - rcHit->top < (2 * ty + 1) << 12)
	{
		NPC->y = tilesToUnits(ty) + (-0x2000 * tx + NPC->x) / 2 - 0x800 - rcHit->bottom;

		if (NPC->ym > 0)
			NPC->ym = 0;

		NPC->flag |= (ground | slopeLeft);
	}
}

void npcJudgeTriangleF(const RECT *rcHit, npc *NPC, int tx, int ty)
{
	NPC->flag |= slopeF;

	if (NPC->x < (2 * tx + 1) << 12
	        && NPC->x >(2 * tx - 1) << 12
	        && NPC->y + rcHit->bottom > tilesToUnits(ty) + (-0x2000 * tx + NPC->x) / 2 + 0x800
	        && NPC->y - rcHit->top < (2 * ty + 1) << 12)
	{
		NPC->y = tilesToUnits(ty) + (-0x2000 * tx + NPC->x) / 2 + 0x800 - rcHit->bottom;

		NPC->flag |= (ground | slopeLeft);
	}
}

void npcJudgeTriangleG(const RECT *rcHit, npc *NPC, int tx, int ty)
{
	NPC->flag |= slopeG;

	if (NPC->x > (2 * tx - 1) << 12
	        && NPC->x < (2 * tx + 1) << 12
	        && NPC->y + rcHit->bottom > tilesToUnits(ty) - (-0x2000 * tx + NPC->x) / 2 + 0x800
	        && NPC->y - rcHit->top < (2 * ty + 1) << 12)
	{
		NPC->y = tilesToUnits(ty) - (-0x2000 * tx + NPC->x) / 2 + 0x800 - rcHit->bottom;

		if (NPC->ym > 0)
			NPC->ym = 0;

		NPC->flag |= (ground | slopeRight);
	}
}

void npcJudgeTriangleH(const RECT *rcHit, npc *NPC, int tx, int ty)
{
	NPC->flag |= slopeH;

	if (NPC->x > (2 * tx - 1) << 12
	        && NPC->x < (2 * tx + 1) << 12
	        && NPC->y + rcHit->bottom > tilesToUnits(ty) - (-0x2000 * tx + NPC->x) / 2 - 0x800
	        && NPC->y - rcHit->top < (2 * ty + 1) << 12)
	{
		NPC->y = tilesToUnits(ty) - (-0x2000 * tx + NPC->x) / 2 - 0x800 - rcHit->bottom;

		if (NPC->ym > 0)
			NPC->ym = 0;

		NPC->flag |= (ground | slopeRight);
	}
}

void npcJudgeWater(const RECT *rcHit, npc *NPC, int tx, int ty)
{
	if (NPC->x - rcHit->right < (4 * (2 * tx + 1) - 1) << 10
	        && NPC->x + rcHit->right > (4 * (2 * tx - 1) + 1) << 10
	        && NPC->y - rcHit->top < (4 * (2 * ty + 1) - 1) << 10
	        && NPC->y + rcHit->bottom > (4 * (2 * ty - 1) + 1) << 10)
	{
		NPC->flag |= water;
	}
}

void npcHitMap(size_t NPCid)
{
	npc *NPC = &npcs[NPCid];
	NPC->flag = 0; //clear
	const RECT *rcHit = &NPC->hit;

	const int fromX = unitsToTiles(NPC->x - rcHit->right + 0x1000);
	const int fromY = unitsToTiles(NPC->y - rcHit->top + 0x1000);

	const int toX = unitsToTiles(NPC->x + rcHit->right + 0x1000);
	const int toY = unitsToTiles(NPC->y + rcHit->bottom + 0x1000);

	if (!(NPC->bits & npc_ignoreSolid))
	{
		for (int currentX = fromX; currentX <= toX; currentX++)
		{
			for (int currentY = fromY; currentY <= toY; currentY++)
			{
				switch (getTileAttribute(currentX, currentY))
				{
				//Solid
				case 0x03:
				case 0x05:
				case 0x41:
				case 0x43:
					npcJudgeBlock(rcHit, NPC, currentX, currentY);
					break;

				//Npc Only solid
				case 0x44u:
					if (!(NPC->bits & npc_ignore44))
						npcJudgeBlock(rcHit, NPC, currentX, currentY);
					break;

				//Water
				case 0x02:
				case 0x60:
				case 0x62:
					npcJudgeWater(rcHit, NPC, currentX, currentY);
					break;

				//Water solid
				case 0x04:
				case 0x61:
				case 0x64:
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
					NPC->flag |= (windLeft);
					break;

				case 0x81u:
					NPC->flag |= (windUp);
					break;

				case 0x82u:
					NPC->flag |= (windRight);
					break;

				case 0x83u:
					NPC->flag |= (windDown);
					break;

				//Water current
				case 0xA0u:
					NPC->flag |= (water | windLeft);
					break;

				case 0xA1u:
					NPC->flag |= (water | windUp);
					break;

				case 0xA2u:
					NPC->flag |= (water | windRight);
					break;

				case 0xA3u:
					NPC->flag |= (water | windDown);
					break;

				//Default
				default:
					break;
				}
			}
		}
	}
}
