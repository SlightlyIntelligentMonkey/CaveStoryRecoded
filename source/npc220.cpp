#include "npc220.h"

void npcAct231(npc *NPC)
{
	RECT rc[2];

	rc[0] = { 176, 32, 208, 48 };
	rc[1] = { 176, 48, 208, 64 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		break;

	case 1:
		NPC->ani_no = 0;
		break;

	case 10:
		NPC->act_no = 11;
		NPC->act_wait = 0;
		goto npc231down;

	case 11:
npc231down:
		++NPC->act_wait;
		NPC->ym += 8;
		if (NPC->flag & ground)
		{
			if (NPC->act_wait > 9)
				NPC->act_no = 1;
			else
				NPC->act_no = 12;
		}
		break;

	case 12:
		NPC->bits &= ~npc_interact;
		
		NPC->act_no = 13;
		NPC->act_wait = 0;
		NPC->ani_no = 1;

		for (int i = 0; i < 10; ++i)
		{
			createNpc(4, NPC->x + (random(-16, 16) << 9), NPC->y + (random(-8, 8) << 9), 0, 0, 0, nullptr);
			//PlaySoundObject(12, 1);
		}
		goto npc231up;
		
	case 13:
npc231up:
		NPC->ym -= 8;

		if (!(++NPC->act_wait & 1))
			createCaret(NPC->x - 0x1400, NPC->y + 0x1000, 7, 3);

		if (NPC->act_wait % 2 == 1)
			createCaret(NPC->x + 0x1400, NPC->y + 0x1000, 7, 3);

		//if (npc->act_wait % 4 == 1)
		//	PlaySoundObject(34, 1);

		if (NPC->flag & ceiling || currentPlayer.flag & ceiling || NPC->act_wait > 450)
		{
			if (NPC->flag & ceiling || currentPlayer.flag & ceiling)
				NPC->ym = 0;

			NPC->act_no = 15;

			for (int i = 0; i < 6; ++i)
			{
				createNpc(4, NPC->x + (random(-16, 16) << 9), NPC->y + (random(-8, 8) << 9), 0, 0, 0, nullptr);
				//PlaySoundObject(12, 1);
			}
		}
		break;

	case 0xF:
		NPC->ym += 8;
		++NPC->act_wait;

		if (NPC->ym < 0)
		{
			if (!(NPC->act_wait & 7))
				createCaret(NPC->x - 0x1400, NPC->y + 0x1000, 7, 3);

			if (NPC->act_wait % 8 == 4)
				createCaret(NPC->x + 0x1400, NPC->y + 0x1000, 7, 3);

			//if (NPC->act_wait % 16 == 1)
			//	PlaySoundObject(34, 1);
		}

		if (NPC->flag & ground)
		{
			NPC->bits |= npc_interact;
			
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;
	default:
		break;
	}

	if (NPC->ym < -0x5FF)
		NPC->ym = -0x5FF;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->y += NPC->ym;

	NPC->rect = rc[NPC->ani_no];
}
