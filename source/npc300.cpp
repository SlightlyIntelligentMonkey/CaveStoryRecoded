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

void npcAct302(npc *NPC) //Camera Helper NPC
{
	if (NPC->act_no != 20)
	{
		if (NPC->act_no <= 20)
		{
			if (NPC->act_no == 10)
			{
				NPC->x = currentPlayer.x;
				NPC->y = currentPlayer.y - 0x4000;
			}

			return;
		}

		if (NPC->act_no == 100)
		{
			NPC->act_no = 101;

			if (NPC->direct)
			{
				int n;

				for (n = 170; n < 512; ++n)
				{
					if ((npcs[n].cond & npccond_alive) != 0 && npcs[n].code_event == NPC->direct)
					{
						NPC->pNpc = &npcs[n];
						break;
					}
				}

				if (n == 512)
				{
					NPC->cond = 0;
					return;
				}
			}
			else
			{
				//npc->pNpc = gBoss;
			}
		}
		else if (NPC->act_no != 101)
		{
			if (NPC->act_no == 30)
			{
				NPC->x = currentPlayer.x;
				NPC->y = currentPlayer.y + 0xA000;
			}

			return;
		}

		NPC->x = (currentPlayer.x + NPC->pNpc->x) / 2;
		NPC->y = (currentPlayer.y + NPC->pNpc->y) / 2;
		return;
	}
	
	if (NPC->direct == 1)
		NPC->y -= 0x400;
	else if (NPC->direct > 1)
	{
		if (NPC->direct == 2)
			NPC->x += 0x400;
		else if (NPC->direct == 3)
			NPC->y += 0x400;
	}
	else if (!NPC->direct)
		NPC->x -= 1024;
	
	currentPlayer.x = NPC->x;
	currentPlayer.y = NPC->y;
}
