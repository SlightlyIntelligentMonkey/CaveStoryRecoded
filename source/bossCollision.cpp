#include "boss.h"

#include "bossCollision.h"
#include "npcCollision.h"
#include "stage.h"

void bossHitMap()
{
    for (size_t i = 0; i < 20; ++i)
    {
        RECT *rcHit = &bossObj[i].hit;
        npc *NPC = &bossObj[i];
		NPC->flag = 0; //clear
        if (bossObj[i].cond & 0x80 && !(bossObj[i].bits & npc_ignoreSolid))
        {
			const int fromX = unitsToTiles(NPC->x - rcHit->right + 0x1000);
			const int fromY = unitsToTiles(NPC->y - rcHit->top + 0x1000);

			const int toX = unitsToTiles(NPC->x + rcHit->right + 0x1000);
			const int toY = unitsToTiles(NPC->y + rcHit->bottom + 0x1000);

            for (int currentX = fromX; currentX <= toX; ++currentX)
            {
				for (int currentY = fromY; currentY <= toY; currentY++)
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
}
