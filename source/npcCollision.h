#pragma once

#include "npc.h"
#include "level.h"

class npc;

void npcHitMap(size_t NPCid);
void npcJudgeBlock(const RECT *rcHit, npc *NPC, int tx, int ty);
void npcJudgeTriangleA(const RECT *rcHit, npc *NPC, int tx, int ty);
void npcJudgeTriangleB(const RECT *rcHit, npc *NPC, int tx, int ty);
void npcJudgeTriangleC(const RECT *rcHit, npc *NPC, int tx, int ty);
void npcJudgeTriangleD(const RECT *rcHit, npc *NPC, int tx, int ty);
void npcJudgeTriangleE(const RECT *rcHit, npc *NPC, int tx, int ty);
void npcJudgeTriangleF(const RECT *rcHit, npc *NPC, int tx, int ty);
void npcJudgeTriangleG(const RECT *rcHit, npc *NPC, int tx, int ty);
void npcJudgeTriangleH(const RECT *rcHit, npc *NPC, int tx, int ty);
void npcJudgeWater(const RECT *rcHit, npc *NPC, int tx, int ty);
