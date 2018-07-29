#include "npc280.h"

void npcAct298(npc *NPC) //Doctor without crown (intro)
{
	RECT rc[8];

	rc[0] = { 0x48, 0x80, 0x58, 0xA0 };
	rc[1] = { 0x58, 0x80, 0x60, 0xA0 };
	rc[2] = { 0x68, 0x80, 0x78, 0xA0 };
	rc[3] = { 0x48, 0x80, 0x58, 0xA0 };
	rc[4] = { 0x78, 0x80, 0x88, 0xA0 };
	rc[5] = { 0x48, 0x80, 0x58, 0xA0 };
	rc[6] = { 0x68, 0xA0, 0x78, 0xC0 };
	rc[7] = { 0x78, 0xA0, 0x88, 0xC0 };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->y -= 0x1000;
	}

	switch (NPC->act_no)
	{
	case 1:
		NPC->ani_no = 0;
		break;

	case 10:
		NPC->act_no = 11;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->count1 = 0;

	case 11:
		if (++NPC->ani_wait > 6)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 1)
		{
			NPC->ani_no = 0;

			if (++NPC->count1 > 7)
			{
				NPC->ani_no = 0;
				NPC->act_no = 1;
			}
		}
		break;

	case 20:
		NPC->act_no = 21;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
		
	case 21:
		if (++NPC->ani_wait > 10)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 5)
			NPC->ani_no = 2;

		NPC->x += 0x100;
		break;

	case 30:
		NPC->ani_no = 6;
		break;

	case 40:
		NPC->act_no = 41;
		NPC->ani_no = 6;
		NPC->ani_wait = 0;
		NPC->count1 = 0;
		
	case 41:
		if (++NPC->ani_wait > 6)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 7)
		{
			NPC->ani_no = 6;

			if (++NPC->count1 > 7)
			{
				NPC->ani_no = 6;
				NPC->act_no = 30;
			}
		}
		break;

	default:
		break;
	}
	
	NPC->rect = rc[NPC->ani_no];
}

void npcAct299(npc *NPC) //Balrog / Misery in bubble (intro)
{
	RECT rc[2]; // [esp+0h] [ebp-20h]

	rc[0] = { 0, 0, 48, 48 };
	rc[1] = { 48, 0, 96, 48 };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		if (NPC->direct)
		{
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}
		else
		{
			NPC->ani_no = 1;
			NPC->act_wait = 25;
			NPC->y -= 0x640;
		}
	}

	if (++NPC->act_wait / 50 & 1)
		NPC->y += 64;
	else
		NPC->y -= 64;

	NPC->rect = rc[NPC->ani_no];
}
