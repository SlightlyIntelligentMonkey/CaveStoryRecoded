#include "npc340.h"

void npcAct359(npc *NPC)
{
	if (currentPlayer.x < NPC->x + 0x28000 && currentPlayer.x > NPC->x - 0x28000 && currentPlayer.y < NPC->y + 0x28000 && currentPlayer.y > NPC->y - 0x28000 && random(0, 100) == 2)
	{
		createNpc((random(-6, 6) << 9) + NPC->x, NPC->y - 0xE00, 0, 0, 0, 0, 0, 73, 0);
	}
}