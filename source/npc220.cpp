#include "npc220.h"

void npcAct222(npc *NPC) // Prison bars
{
	if (!NPC->act_no)
	{
		++NPC->act_no;
		NPC->y -= 0x1000;
	}

	NPC->rect = { 96, 168, 112, 200 };
}

void npcAct227(npc *NPC) // Bucket
{
	NPC->rect = { 208, 32, 224, 48 };
}

void npcAct231(npc *NPC) //Momorin's rocket
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
		// fallthrough
	case 11:
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
			playSound(12);
		}
		// fallthrough
	case 13:
		NPC->ym -= 8;

		if (!(++NPC->act_wait & 1))
			createCaret(NPC->x - 0x1400, NPC->y + 0x1000, 7, 3);

		if (NPC->act_wait % 2 == 1)
			createCaret(NPC->x + 0x1400, NPC->y + 0x1000, 7, 3);

		if (NPC->act_wait % 4 == 1)
			playSound(34);

		if (NPC->flag & ceiling || currentPlayer.flag & ceiling || NPC->act_wait > 450)
		{
			if (NPC->flag & ceiling || currentPlayer.flag & ceiling)
				NPC->ym = 0;

			NPC->act_no = 15;

			for (int i = 0; i < 6; ++i)
			{
				createNpc(4, NPC->x + (random(-16, 16) << 9), NPC->y + (random(-8, 8) << 9), 0, 0, 0, nullptr);
				playSound(12);
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

			if (NPC->act_wait % 16 == 1)
				playSound(34);
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

void npcAct238(npc *NPC) //Killer press
{
	RECT rc[3];

	rc[0] = { 184, 200, 208, 216 };
	rc[1] = { 208, 200, 232, 216 };
	rc[2] = { 232, 200, 256, 216 };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
		NPC->view.left = 0x2000;
		NPC->view.right = 0x1000;
	}

	switch (NPC->act_no)
	{
	case 1:
		if (!NPC->direct && currentPlayer.x < NPC->x && currentPlayer.x > NPC->x - 0x18000 && currentPlayer.y > NPC->y - 0x800 && currentPlayer.y < NPC->y + 0x1000)
		{
			NPC->act_no = 10;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}

		if (NPC->direct == 2 && currentPlayer.x > NPC->x && currentPlayer.x < NPC->x + 0x18000 && currentPlayer.y > NPC->y - 0x800 && currentPlayer.y < NPC->y + 0x1000)
		{
			NPC->act_no = 10;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}

		break;

	case 10:
		NPC->damage = 127;

		if (NPC->direct)
			NPC->x += 0xC00;
		else
			NPC->x -= 0xC00;

		if (++NPC->act_wait == 8)
		{
			NPC->act_no = 20;
			NPC->act_wait = 0;

			for (int i = 0; i < 4; ++i)
				createNpc(4, NPC->x + (random(-16, 16) << 9), NPC->y + (random(-8, 8) << 9), 0, 0, 0, nullptr);
		}
		break;

	case 20:
		NPC->damage = 0;

		if (++NPC->act_wait > 50)
		{
			NPC->act_wait = 0;
			NPC->act_no = 30;
		}

		break;

	case 30:
		NPC->damage = 0;
		NPC->ani_no = 1;

		if (++NPC->act_wait == 12)
		{
			NPC->act_no = 1;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
		}

		if (NPC->direct)
			NPC->x -= 0x800;
		else
			NPC->x += 0x800;

		break;

	default:
		break;
	}

	if (NPC->direct || currentPlayer.x >= NPC->x)
	{
		if (NPC->direct != 2 || currentPlayer.x <= NPC->x)
			NPC->hit.right = 0x1000;
		else
			NPC->hit.right = 0x2000;
	}
	else
		NPC->hit.right = 0x2000;

	NPC->rect = rc[NPC->ani_no];
}
