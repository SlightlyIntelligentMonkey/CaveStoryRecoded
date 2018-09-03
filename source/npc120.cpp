#include "npc120.h"

#include "sound.h"

void npcAct125(npc *NPC) //Hidden item
{
	if (NPC->life <= 989)
	{
		createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 8);
		playSound(70);
		if (NPC->direct)
			createNpc(NPC_Missile, NPC->x, NPC->y, 0, 0, dirRight);
		else
			createNpc(NPC_Heart, NPC->x, NPC->y, 0, 0, dirRight);
		NPC->cond = 0;
	}

	if (NPC->direct)
	{
		NPC->rect.left = 16;
		NPC->rect.top = 96;
		NPC->rect.right = 32;
	}
	else
	{
		NPC->rect.left = 0;
		NPC->rect.top = 96;
		NPC->rect.right = 16;
	}

	NPC->rect.bottom = 112;
}

void npcAct127(npc *NPC)
{
	if (++NPC->ani_wait > 0)
	{
		NPC->ani_wait = 0;
		if (++NPC->ani_no > 2)
			NPC->cond = 0;
	}

	if (NPC->direct)
	{
		NPC->rect.left = 112;
		NPC->rect.top = 48 + (NPC->ani_no * 16);
	}
	else
	{
		NPC->rect.left = 64 + (NPC->ani_no * 16);
		NPC->rect.top = 80;
	}

	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 16;
}

void npcAct128(npc *NPC)
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		if (NPC->direct && NPC->direct != 2)
		{
			NPC->view.left = 4096;
			NPC->view.top = 2048;
		}
		else
		{
			NPC->view.left = 2048;
			NPC->view.top = 4096;
		}
	}
	if (++NPC->ani_no > 4)
	{
		NPC->cond = 0;
		return;
	}

	switch (NPC->direct)
	{
	case(dirLeft):
		NPC->rect.left = 176 + (8 * (NPC->ani_no - 1));
		NPC->rect.top = 16;
		NPC->rect.right = NPC->rect.left + 8;
		NPC->rect.bottom = NPC->rect.top + 16;
		break;
	case(dirUp):
		NPC->rect.left = 176 + (16 * ((NPC->ani_no - 1) / 2));
		NPC->rect.top = 32 + (8 * ((NPC->ani_no - 1) % 2));
		NPC->rect.right = NPC->rect.left + 16;
		NPC->rect.bottom = NPC->rect.top + 8;
		break;
	case(dirRight):
		NPC->rect.left = 232 - (8 * (NPC->ani_no - 1));
		NPC->rect.top = 16;
		NPC->rect.right = NPC->rect.left + 8;
		NPC->rect.bottom = NPC->rect.top + 16;
		break;
	case(dirDown):
		NPC->rect.left = 208 + (16 * ((NPC->ani_no - 1) / 2));
		NPC->rect.top = 32 + (8 * ((NPC->ani_no - 1) % 2));
		NPC->rect.right = NPC->rect.left + 16;
		NPC->rect.bottom = NPC->rect.top + 8;
		break;
	}
}

void npcAct129(npc *NPC) //Fireball Level 1/2 trail
{
	RECT rect[18];

	rect[0] = { 0x80, 0x30, 0x90, 0x40 };
	rect[1] = { 0x90, 0x30, 0xA0, 0x40 };
	rect[2] = { 0xA0, 0x30, 0xB0, 0x40 };
	rect[3] = { 0x80, 0x40, 0x90, 0x50 };
	rect[4] = { 0x90, 0x40, 0xA0, 0x50 };
	rect[5] = { 0xA0, 0x40, 0xB0, 0x50 };
	rect[6] = { 0x80, 0x50, 0x90, 0x60 };
	rect[7] = { 0x90, 0x50, 0xA0, 0x60 };
	rect[8] = { 0xA0, 0x50, 0xB0, 0x60 };
	rect[9] = { 0xB0, 0x30, 0xC0, 0x40 };
	rect[10] = { 0xC0, 0x30, 0xD0, 0x40 };
	rect[11] = { 0xD0, 0x30, 0xE0, 0x40 };
	rect[12] = { 0xB0, 0x40, 0xC0, 0x50 };
	rect[13] = { 0xC0, 0x40, 0xD0, 0x50 };
	rect[14] = { 0xD0, 0x40, 0xE0, 0x50 };
	rect[15] = { 0xB0, 0x50, 0xC0, 0x60 };
	rect[16] = { 0xC0, 0x50, 0xD0, 0x60 };
	rect[17] = { 0xD0, 0x50, 0xE0, 0x60 };

	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;

		if (++NPC->ani_no > 2)
			NPC->cond = 0;
	}

	NPC->y += NPC->ym;

	NPC->rect = rect[NPC->ani_no + 3 * NPC->direct];
}

void npcAct137(npc *NPC) // Large doorway frame
{
	NPC->rect = { 96, 136, 128, 188 };
}

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
	// Fallthrough
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
	// Fallthrough
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
	// Fallthrough
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
		playSound(SFX_Teleport);
	// Fallthrough
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
		playSound(SFX_Teleport);
	// Fallthrough
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

	if (NPC->direct != dirLeft)
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
