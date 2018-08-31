#pragma once
#include "common.h"

#include "npc.h"

static inline void npcAnimate(npc *NPC, int aniWait, int aniStart, int aniMax)
{
	if (++NPC->ani_wait > aniWait)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}
	if (NPC->ani_no > aniMax)
		NPC->ani_no = aniStart;
}

static inline void npcFacePlayer(npc *NPC)
{
	if (currentPlayer.x >= NPC->x)
		NPC->direct = dirRight;
	else
		NPC->direct = dirLeft;
}

static inline void npcMove(npc *NPC, int *velPtr, int vel)
{
	if (NPC->direct != dirLeft)
		*velPtr = vel;
	else
		*velPtr = -vel;
}
