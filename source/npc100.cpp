#include "npc100.h"

void npcAct111(npc *NPC) //Quote teleport out
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 16, 0, 32, 16 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 16, 16, 32, 32 };
	
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->y -= 0x2000;
		break;

	case 1:
		if (++NPC->act_wait > 20)
		{
			NPC->act_wait = 0;
			NPC->act_no = 2;
			NPC->ani_no = 1;
			NPC->ym = -0x2FF;
		}
		break;

	case 2:
		if (NPC->ym > 0)
			NPC->hit.bottom = 0x2000;

		if (NPC->flag & ground)
		{
			NPC->act_no = 3;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
		}

		break;
	case 3:
		if (++NPC->act_wait > 40)
		{
			NPC->act_no = 4;
			NPC->act_wait = 64;
			//PlaySoundObject(29, 1);
		}
		break;
	case 4:
		--NPC->act_wait;
		NPC->ani_no = 0;

		if (!NPC->act_wait)
			NPC->cond = 0;
		break;
	default:
		break;
	}

	NPC->ym += 0x40;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];

	if (currentPlayer.equip & equip_mimigaMask)
	{
		NPC->rect.top += 32;
		NPC->rect.bottom += 32;
	}

	if (NPC->act_no == 4)
	{
		NPC->rect.bottom = NPC->act_wait / 4 + NPC->rect.top;

		if (NPC->act_wait / 2 & 1)
			++NPC->rect.left;
	}
}

void npcAct112(npc *NPC) //Quote teleport in
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 16, 0, 32, 16 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 16, 16, 32, 32 };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->x += 0x2000;
		NPC->y += 4096;
		//PlaySoundObject(29, 1);
	}

	switch (NPC->act_no)
	{
		case 1:
			if (++NPC->act_wait >= 64)
			{
				NPC->act_no = 2;
				NPC->act_wait = 0;
			}
			break;

		case 2:
			if (++NPC->act_wait > 20)
			{
				NPC->act_no = 3;
				NPC->ani_no = 1;
				NPC->hit.bottom = 0x1000;
			}
			break;

		case 3:
			if (NPC->flag & ground)
			{
				NPC->act_no = 4;
				NPC->act_wait = 0;
				NPC->ani_no = 0;
			}
			break;
	}

	NPC->ym += 64;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];

	if (currentPlayer.equip & equip_mimigaMask)
	{
		NPC->rect.top += 32;
		NPC->rect.bottom += 32;
	}

	if (NPC->act_no == 1)
	{
		NPC->rect.bottom = NPC->act_wait / 4 + NPC->rect.top;

		if (NPC->act_wait / 2 & 1)
			++NPC->rect.left;
	}

	/*if (v1 == 1)
	{
	LABEL_9:
		if (++npc->act_wait == 64)
		{
			npc->act_no = 2;
			npc->act_wait = 0;
		}
		goto LABEL_15;
	}
	if (v1 <= 1)
	{
		if (v1)
			goto LABEL_15;
		npc->act_no = 1;
		npc->ani_no = 0;
		npc->ani_wait = 0;
		npc->x += 0x2000;
		npc->y += 4096;
		PlaySoundObject(29, 1);
		goto LABEL_9;
	}
	if (v1 == 2)
	{
		if (++npc->act_wait > 20)
		{
			npc->act_no = 3;
			npc->ani_no = 1;
			npc->hit.bottom = 4096;
		}
	}
	else if (v1 == 3 && npc->flag & 8)
	{
		npc->act_no = 4;
		npc->act_wait = 0;
		npc->ani_no = 0;
	}
LABEL_15:
	npc->ym += 64;
	npc->y += npc->ym;
	if (npc->direct)
	{
		v2 = npc;
		v3 = (Sint32 *)&v6[16 * npc->ani_no - 64];
	}
	else
	{
		v2 = npc;
		v3 = (Sint32 *)&v6[16 * npc->ani_no - 32];
	}
	npc->rect.left = *v3;
	v2->rect.top = v3[1];
	v2->rect.right = v3[2];
	v2->rect.bottom = v3[3];
	if (unk_81C8598 & 0x40)
	{
		npc->rect.top += 32;
		npc->rect.bottom += 32;
	}
	if (npc->act_no == 1)
	{
		npc->rect.bottom = npc->act_wait / 4 + npc->rect.top;
		if (npc->act_wait / 2 & 1)
			++npc->rect.left;
	}
	*/
}