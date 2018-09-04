#include "npc340.h"

#include "player.h"
#include "mathUtils.h"

void npcAct349(npc *NPC)
{
	if (NPC->act_no == 0)
	{
		NPC->act_no = 1;
		if (NPC->direct == dirLeft)
			NPC->x += 0x1000;
		else if (NPC->direct == dirRight)
			NPC->y += 0x2000;
	}

	NPC->rect = { 0, 0, 16, 16 };
}

void npcAct359(npc *NPC)
{
	if (currentPlayer.x < NPC->x + 0x28000 && currentPlayer.x > NPC->x - 0x28000 && currentPlayer.y < NPC->y + 0x28000 && currentPlayer.y > NPC->y - 0x28000 && random(0, 100) == 2)
		createNpc(NPC_Waterdrop, (random(-6, 6) << 9) + NPC->x, NPC->y - 0xE00);
}

