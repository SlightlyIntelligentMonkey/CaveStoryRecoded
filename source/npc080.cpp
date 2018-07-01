#include "npc080.h"

void npcAct096(npc *NPC)
{	
	int action = NPC->action;

	switch (action)
	{
	case 1:
		goto npc096end;
	case 2:
		//Animate
		if (++NPC->animationWait > 0)
		{
			NPC->animationWait = 0;
			++NPC->animation;
		}

		//Animate
		if (NPC->animation > 2)
			NPC->animation = 0;

		//Current effect
		if (currentPlayer.x > NPC->x - 0x28000 && currentPlayer.x < NPC->x + 0x28000 && currentPlayer.y > NPC->y - 0x28000 && currentPlayer.y < NPC->y + 0x28000 && random(0, 5) == 1)
		{
			createNpc(NPC->x, NPC->y + (random(-8, 8) << 9), 0, 0, 0, 0, 0, 199, 0);
		}

		//Blow quote
		if (currentPlayer.y < NPC->y + 0x1000 && currentPlayer.y > NPC->y - 0x1000 && currentPlayer.x < NPC->x && currentPlayer.x > NPC->x - 0xC000)
		{
			currentPlayer.flags |= windLeft;
			//gMC.cond |= 0x20u;
		}
		break;
	case 0:
		if (NPC->direction == 2)
			NPC->action = 2;
		else
			NPC->animation = 1;
	npc096end:
		NPC->animation = 0;
		break;
	}
	
	NPC->frameRect = { 272 + (NPC->animation << 4), 120, 288 + (NPC->animation << 4), 136 };
}