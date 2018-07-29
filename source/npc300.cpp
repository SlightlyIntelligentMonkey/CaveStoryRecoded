#include "npc300.h"

void npcAct300(npc *NPC) //Demon crown
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->y += 0xC00;
	}

	if (++NPC->ani_wait % 8 == 1)
	{
		createCaret(NPC->x + (random(-8, 8) << 9), NPC->y + 4096, 13, 1);
	}

	NPC->rect = { 192, 80, 208, 96 };
}
