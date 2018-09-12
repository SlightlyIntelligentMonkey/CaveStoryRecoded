#include "boss.h"

#include "bossCollision.h"
#include "npcCollision.h"
#include "level.h"

void bossHitMap()
{
	int offy[16];
	int offx[16];
	uint8_t atrb[16];

	offx[0] = 0;
	offx[1] = 1;
	offx[2] = 0;
	offx[3] = 1;
	offx[4] = 2;
	offx[5] = 2;
	offx[6] = 2;
	offx[7] = 0;
	offx[8] = 1;
	offx[9] = -1;
	offx[10] = -1;
	offx[11] = -1;
	offx[12] = -1;
	offx[13] = 0;
	offx[14] = 1;
	offx[15] = 2;

	offy[0] = 0;
	offy[1] = 0;
	offy[2] = 1;
	offy[3] = 1;
	offy[4] = 0;
	offy[5] = 1;
	offy[6] = 2;
	offy[7] = 2;
	offy[8] = 2;
	offy[9] = -1;
	offy[10] = 0;
	offy[11] = 1;
	offy[12] = 2;
	offy[13] = -1;
	offy[14] = -1;
	offy[15] = -1;

	for (size_t b = 0; b < BOSSNPCS; ++b)
	{
		if (bossObj[b].cond & npccond_alive && !(bossObj[b].bits & npc_ignoreSolid))
		{
			//Get amount of tiles to judge
			size_t judg = 16;
			if (bossObj[b].size < 3)
				judg = 4;

			int x = bossObj[b].x / 0x2000;
			int y = bossObj[b].y / 0x2000;

			bossObj[b].flag = 0;

			for (size_t j = 0; j < judg; ++j)
			{
				atrb[j] = getTileAttribute(x + offx[j], y + offy[j]);

				switch (atrb[j])
				{
				case 2u:
				case 0x60u:
				case 0x61u:
				case 0x64u:
					npcJudgeBlock(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					npcJudgeWater(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 5u:
				case 0x41u:
				case 0x43u:
					npcJudgeBlock(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
				case 0x44u:
					if (!(bossObj[b].bits & npc_ignore44))
						npcJudgeBlock(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x50u:
					npcJudgeTriangleA(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x51u:
					npcJudgeTriangleB(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x52u:
					npcJudgeTriangleC(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x53u:
					npcJudgeTriangleD(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x54u:
					npcJudgeTriangleE(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x55u:
					npcJudgeTriangleF(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x56u:
					npcJudgeTriangleG(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x57u:
					npcJudgeTriangleH(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x70u:
					npcJudgeTriangleA(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					npcJudgeWater(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x71u:
					npcJudgeTriangleB(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					npcJudgeWater(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x72u:
					npcJudgeTriangleC(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					npcJudgeWater(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x73u:
					npcJudgeTriangleD(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					npcJudgeWater(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x74u:
					npcJudgeTriangleE(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					npcJudgeWater(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x75u:
					npcJudgeTriangleF(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					npcJudgeWater(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x76u:
					npcJudgeTriangleG(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					npcJudgeWater(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				case 0x77u:
					npcJudgeTriangleH(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					npcJudgeWater(&bossObj[b].hit, &bossObj[b], x + offx[j], y + offy[j]);
					break;
				default:
					continue;
				}
			}
		}
	}
}
