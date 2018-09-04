#include "npc240.h"
#include "mathUtils.h"
#include "render.h"

void npcAct243(npc *NPC) // Generator - Red Bat
{
	if (NPC->act_no)
	{
		if (NPC->act_no != 1)
			return;
	}
	else
	{
		NPC->act_no = 1;
		NPC->act_wait = random(0, 500);
	}
	if (NPC->act_wait)
		--NPC->act_wait;
	else
	{
		NPC->act_no = 0;
		createNpc(NPC_EnemyBatRed, NPC->x, NPC->y + pixelsToUnits(random(-0x20, 0x20)), 0, 0, NPC->direct);
	}
}

void npcAct259(npc *NPC) // Sleeping mimiga
{
	NPC->rect = { 48, 32, 64, 48 };
}
