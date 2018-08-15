#include "npc180.h"

void npcAct194(npc *NPC) // Blue Robot, destroyed
{
	if (NPC->act_no == 0)
	{
		NPC->act_no = 1;
		NPC->y += 0x800;
	}

	NPC->rect = { 192, 120, 224, 128 };
}

void npcAct195(npc *NPC) // Grate mouth
{
	NPC->rect = { 112, 64, 128, 80 };
}

void npcAct199(npc *NPC) //Current / fan effect
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->ani_no = random(0, 2);

		const int direction = NPC->direct;

		if (direction == 1)
		{
			NPC->ym = -1;
		}
		else if (direction > 1)
		{
			if (direction == 2)
			{
				NPC->xm = 1;
			}
			else if (direction == 3)
			{
				NPC->ym = 1;
			}
		}
		else if (!direction)
		{
			NPC->xm = -1;
		}

		NPC->xm = (random(4, 8) << 9) / 2 * NPC->xm;
		NPC->ym = (random(4, 8) << 9) / 2 * NPC->ym;
	}

	if (++NPC->ani_wait > 6)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 4)
		NPC->cond = 0;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->rect = { 72 + (NPC->ani_no << 1),16,74 + (NPC->ani_no << 1),18 };
}
