#include "npc080.h"

void npcAct084(npc *NPC) //Basu 1 projectile
{
	RECT rect[4];

	rect[0] = { 48, 48, 64, 64 };
	rect[1] = { 64, 48, 80, 64 };
	rect[2] = { 48, 64, 64, 80 };
	rect[3] = { 64, 64, 80, 80 };

	//Remove if hit anything
	if (NPC->flag & 0xFF)
	{
		createCaret(NPC->x, NPC->y, 2, 0);
		NPC->cond = 0;
	}

	//Move
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Animate
	if (++NPC->ani_wait > 2)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 3)
		NPC->ani_no = 0;

	NPC->rect = rect[NPC->ani_no];
	
	if (++NPC->count1 > 300)
	{
		createCaret(NPC->x, NPC->y, 2, 0);
		NPC->cond = 0;
	}
}

void npcAct096(npc *NPC) //Fan left
{	
	int action = NPC->act_no;

	switch (action)
	{
	case 1:
		goto npc096end;
	case 2:
		//Animate
		if (++NPC->ani_wait > 0)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		//Animate
		if (NPC->ani_no > 2)
			NPC->ani_no = 0;

		//Current effect
		if (currentPlayer.x > NPC->x - 0x28000 && currentPlayer.x < NPC->x + 0x28000 && currentPlayer.y > NPC->y - 0x28000 && currentPlayer.y < NPC->y + 0x28000 && random(0, 5) == 1)
		{
			createNpc(199, NPC->x + (random(-8, 8) << 9), NPC->y, 0, 0, 0, nullptr);
		}

		//Blow quote
		if (currentPlayer.y < NPC->y + 0x1000 && currentPlayer.y > NPC->y - 0x1000 && currentPlayer.x < NPC->x && currentPlayer.x > NPC->x - 0xC000)
		{
			currentPlayer.flags |= windLeft;
			currentPlayer.cond |= player_noFriction;
		}

		break;
	case 0:
		if (NPC->direct == 2)
			NPC->act_no = 2;
		else
			NPC->ani_no = 1;
	npc096end:
		NPC->ani_no = 0;
		break;
	}
	
	NPC->rect = { 272 + (NPC->ani_no << 4), 120, 288 + (NPC->ani_no << 4), 136 };
}

void npcAct097(npc *NPC) //Fan up
{
	int action = NPC->act_no;

	switch (action)
	{
	case 1:
		goto npc096end;
	case 2:
		//Animate
		if (++NPC->ani_wait > 0)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		//Animate
		if (NPC->ani_no > 2)
			NPC->ani_no = 0;

		//Current effect
		if (currentPlayer.x > NPC->x - 0x28000 && currentPlayer.x < NPC->x + 0x28000 && currentPlayer.y > NPC->y - 0x28000 && currentPlayer.y < NPC->y + 0x28000 && random(0, 5) == 1)
		{
			createNpc(199, NPC->x + (random(-8, 8) << 9), NPC->y, 0, 0, 1, nullptr);
		}

		//Blow quote
		if (currentPlayer.x < NPC->x + 0x1000 && currentPlayer.x > NPC->x - 0x1000 && currentPlayer.y < NPC->y && currentPlayer.y > NPC->y - 0xC000)
		{
			currentPlayer.ysp -= 0x88;
		}

		break;
	case 0:
		if (NPC->direct == 2)
			NPC->act_no = 2;
		else
			NPC->ani_no = 1;
	npc096end:
		NPC->ani_no = 0;
		break;
	}

	NPC->rect = { 272 + (NPC->ani_no << 4), 136, 288 + (NPC->ani_no << 4), 152 };
}