#include "npc140.h"

#include "sound.h"

void npcAct145(npc *NPC) //King's blade
{
	RECT rcLeft = { 96, 32, 112, 48 };
	RECT rcRight = { 112, 32, 128, 48 };
	
	if (!NPC->act_no)
	{
		//Some orientation stuff
		if (NPC->pNpc->count2)
		{
			if (NPC->pNpc->direct != dirLeft)
				NPC->direct = dirLeft;
			else
				NPC->direct = dirRight;
		}
		else if (NPC->pNpc->direct != dirLeft)
		{
			NPC->direct = dirRight;
		}
		else
		{
			NPC->direct = dirLeft;
		}

		//Set position
		if (NPC->direct != dirLeft)
			NPC->x = NPC->pNpc->x + 0x1400;
		else
			NPC->x = NPC->pNpc->x - 0x1400;
		NPC->y = NPC->pNpc->y;
	}

	//Set framerect
	if (NPC->direct != dirLeft)
		NPC->rect = rcRight;
	else
		NPC->rect = rcLeft;
}

void npcAct146(npc *NPC) //Lightning
{
	RECT rect[5];

	rect[0] = { 0, 0, 0, 0 };
	rect[1] = { 260, 0, 275, 240 };
	rect[2] = { 275, 0, 290, 240 };
	rect[3] = { 290, 0, 305, 240 };
	rect[4] = { 305, 0, 320, 240 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;

		//if (NPC->direct == 2)
		//	SetFlash(0, 0, 2);
		// Fallthrough
	case 1:
		if (++NPC->act_wait > 10)
		{
			NPC->act_no = 2;
			playSound(SFX_Lightning);
		}
		break;

	case 2:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no == 2)
			NPC->damage = 10;

		if (NPC->ani_no > 4)
		{
			createSmoke(NPC->x, NPC->y, 0x1000, 8);
			NPC->cond = 0;
		}

	default:
		break;
	}

	NPC->rect = rect[NPC->ani_no];
}
