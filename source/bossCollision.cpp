#include "boss.h"

#include "bossCollision.h"
#include "npcCollision.h"
#include "stage.h"

void bossHitMap()
{
    for (size_t i = 0; i < 20; ++i)
    {
        RECT *rcHit = &gBossObj[i].hit;
        npc *NPC = &gBossObj[i];
		NPC->flag = 0; //clear
        if (gBossObj[i].cond & 0x80 && !(gBossObj[i].bits & npc_ignoreSolid))
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
						if ((gBossObj[i].bits & npc_ignore44))
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
