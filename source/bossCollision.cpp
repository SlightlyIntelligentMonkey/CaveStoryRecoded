#include "boss.h"

#include "bossCollision.h"
#include "npcCollision.h"
#include "level.h"

void bossHitMap()
{
    constexpr char offsetX[0x10] = {0, 1, 0, 1, 2, 2, 2, 0, 1, -1, -1, -1, -1, 0, 1, 2};
    constexpr char offsetY[0x10] = {0, 0, 1, 1, 0, 1, 2, 2, 2, -1, 0, 1, 2, -1, -1, -1};
    // see https://en.wikipedia.org/wiki/Cartesian_product for understanding what the fuck these even are

    for (size_t i = 0; i < 20; ++i)
    {
        RECT *rcHit = &bossObj[i].rect;
        npc *NPC = &bossObj[i];
        if (bossObj[i].cond & 0x80 && !(bossObj[i].bits & npc_ignoreSolid))
        {
            size_t judg;
            auto xPosBlocks = unitsToTiles(bossObj[i].x);
            auto yPosBlocks = unitsToTiles(bossObj[i].y);
            if (bossObj[i].size < 3)
                judg = 4;
            else
                judg = 0x10;

            bossObj[i].flag = 0;

            int8_t attributes[0x10];
            for (size_t j = 0; j < judg; ++j)
            {
                int currentX = offsetX[j] + xPosBlocks;
                int currentY = offsetY[j] + yPosBlocks;
                attributes[j] = getTileAttribute(offsetX[j] + xPosBlocks, offsetY[j] + yPosBlocks);
                switch (attributes[j])
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
