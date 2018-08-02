#include "npc120.h"

void npcAct139(npc *NPC) //Doctor with the crown
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = { 0, 128, 24, 160 };
	rcLeft[1] = { 24, 128, 48, 160 };
	rcLeft[2] = { 48, 128, 72, 160 };

	rcRight[0] = { 0, 160, 24, 192 };
	rcRight[1] = { 24, 160, 48, 192 };
	rcRight[2] = { 48, 160, 72, 192 };
	
	switch (NPC->act_no)
	{
	case 0: //Stand
		NPC->act_no = 1;
		NPC->xm = 0;
		NPC->ym = 0;
		NPC->y -= 0x1000;

	case 1:
		if (NPC->flag & ground)
			NPC->ani_no = 0;
		else
			NPC->ani_no = 2;

		NPC->ym += 0x40;
		break;

	case 10: //Laughing his ass off
		NPC->act_no = 11;
		NPC->ani_no = 1;
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
			++NPC->count1;
		}

		if (NPC->count1 > 8)
		{
			NPC->ani_no = 0;
			NPC->act_no = 1;
		}

		break;

	case 20: //Floating
		NPC->act_no = 21;
		NPC->act_wait = 0;
		NPC->ani_no = 2;
		NPC->tgt_y = NPC->y - 0x4000;

	case 21:
		if (NPC->y >= NPC->tgt_y)
			NPC->ym -= 0x20;
		else
			NPC->ym += 0x20;

		if (NPC->ym > 0x200)
			NPC->ym = 0x200;
		if (NPC->ym < -0x200)
			NPC->ym = -0x200;

		break;

	case 30: //Teleport away and then stop existing altogether
		NPC->act_no = 31;
		NPC->xm = 0;
		NPC->ym = 0;
		NPC->act_wait = 2 * (NPC->rect.bottom - NPC->rect.top);
		//PlaySoundObject(29, 1);

	case 31:
		--NPC->act_wait;
		NPC->ani_no = 0;

		if (!NPC->act_wait)
			NPC->cond = 0;

		break;

	case 40: //Teleport in, then float
		NPC->act_no = 41;
		NPC->act_wait = 0;
		NPC->xm = 0;
		NPC->ym = 0;
		//PlaySoundObject(29, 1);

	case 41:
		NPC->ani_no = 2;

		if (++NPC->act_wait >= 64)
			NPC->act_no = 20;

		break;

	default:
		break;
	}

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];

	if (NPC->act_no == 31 || NPC->act_no == 41)
	{
		NPC->rect.bottom = NPC->act_wait / 2 + NPC->rect.top;

		if (NPC->act_wait / 2 & 1)
			++NPC->rect.left;
	}
}
