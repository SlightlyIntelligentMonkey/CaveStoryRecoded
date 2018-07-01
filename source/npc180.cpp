#include "npc180.h"

void npcAct199(npc *NPC)
{
	if (!NPC->action)
	{
		NPC->action = 1;
		NPC->animation = random(0, 2);

		int direction = NPC->direction;

		if (direction == 1)
		{
			NPC->ysp = -1;
		}
		else if (direction > 1)
		{
			if (direction == 2)
			{
				NPC->xsp = 1;
			}
			else if (direction == 3)
			{
				NPC->ysp = 1;
			}
		}
		else if (!direction)
		{
			NPC->xsp = -1;
		}

		NPC->xsp = (random(4, 8) << 9) / 2 * NPC->xsp;
		NPC->ysp = (random(4, 8) << 9) / 2 * NPC->ysp;
	}

	if (++NPC->animationWait > 6)
	{
		NPC->animationWait = 0;
		++NPC->animation;
	}

	if (NPC->animation > 4)
		NPC->cond = 0;

	NPC->x += NPC->xsp;
	NPC->y += NPC->ysp;

	NPC->frameRect = { 72 + (NPC->animation << 1),16,74 + (NPC->animation << 1),18 };
}