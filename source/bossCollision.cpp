#include "boss.h"

#include "bossCollision.h"
#include "npcCollision.h"
#include "level.h"

void bossHitMap()
{
<<<<<<< HEAD
    for (size_t i = 0; i < 20; ++i)
    {
        RECT *rcHit = &bossObj[i].hit;
        npc *NPC = &bossObj[i];
		NPC->flag = 0; //clear
        if (bossObj[i].cond & 0x80 && !(bossObj[i].bits & npc_ignoreSolid))
        {
			const size_t fromX = unitsToTiles(NPC->x - rcHit->right + 0x1000);
			const size_t fromY = unitsToTiles(NPC->y - rcHit->top + 0x1000);

			const size_t toX = unitsToTiles(NPC->x + rcHit->right + 0x1000);
			const size_t toY = unitsToTiles(NPC->y + rcHit->bottom + 0x1000);

            for (size_t currentX = fromX; currentX <= toX; ++currentX)
            {
				for (size_t currentY = fromY; currentY <= toY; currentY++)
				{
					switch (getTileAttribute(currentX, currentY))
					{
					case 0x44:
						if ((bossObj[i].bits & npc_ignore44))
							break;
						// Fallthrough
					case 0x5:
					case 0x41:
					case 0x43:
						npcJudgeBlock(rcHit, NPC, currentX, currentY);
						break;

					case 0x50:
						npcJudgeTriangleA(rcHit, NPC, currentX, currentY);
						break;

					case 0x51:
						npcJudgeTriangleB(rcHit, NPC, currentX, currentY);
						break;

					case 0x52:
						npcJudgeTriangleC(rcHit, NPC, currentX, currentY);
						break;

					case 0x53:
						npcJudgeTriangleD(rcHit, NPC, currentX, currentY);
						break;

					case 0x54:
						npcJudgeTriangleE(rcHit, NPC, currentX, currentY);
						break;

					case 0x55:
						npcJudgeTriangleF(rcHit, NPC, currentX, currentY);
						break;

					case 0x56:
						npcJudgeTriangleG(rcHit, NPC, currentX, currentY);
						break;

					case 0x57:
						npcJudgeTriangleH(rcHit, NPC, currentX, currentY);
						break;

					case 0x2:
					case 0x60:
					case 0x61:
					case 0x64:
						npcJudgeBlock(rcHit, NPC, currentX, currentY);
						npcJudgeWater(rcHit, NPC, currentX, currentY);
						break;

					case 0x70:
						npcJudgeTriangleA(rcHit, NPC, currentX, currentY);
						npcJudgeWater(rcHit, NPC, currentX, currentY);
						break;

					case 0x71:
						npcJudgeTriangleB(rcHit, NPC, currentX, currentY);
						npcJudgeWater(rcHit, NPC, currentX, currentY);
						break;

					case 0x72:
						npcJudgeTriangleC(rcHit, NPC, currentX, currentY);
						npcJudgeWater(rcHit, NPC, currentX, currentY);
						break;

					case 0x73:
						npcJudgeTriangleD(rcHit, NPC, currentX, currentY);
						npcJudgeWater(rcHit, NPC, currentX, currentY);
						break;

					case 0x74:
						npcJudgeTriangleE(rcHit, NPC, currentX, currentY);
						npcJudgeWater(rcHit, NPC, currentX, currentY);
						break;

					case 0x75:
						npcJudgeTriangleF(rcHit, NPC, currentX, currentY);
						npcJudgeWater(rcHit, NPC, currentX, currentY);
						break;

					case 0x76:
						npcJudgeTriangleG(rcHit, NPC, currentX, currentY);
						npcJudgeWater(rcHit, NPC, currentX, currentY);
						break;

					case 0x77:
						npcJudgeTriangleH(rcHit, NPC, currentX, currentY);
						npcJudgeWater(rcHit, NPC, currentX, currentY);
						break;

					default:
						continue;
					}
				}
            }
        }
    }
=======
	constexpr int offy[16] = {0, 1, 0, 1, 2, 2, 2, 0, 1, -1, -1, -1, -1, 0, 1, 2};
	constexpr int offx[16] = {0, 0, 1, 1, 0, 1, 2, 2, 2, -1, 0, 1, 2, -1, -1, -1};
    // see https://en.wikipedia.org/wiki/Cartesian_product for understanding what the fuck these even are

	uint8_t atrb[16];

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
					break;
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
>>>>>>> upstream/master
}
