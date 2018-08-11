#include "npc080.h"

void npcAct082(npc *NPC) //Misery standing
{
	int something; //This is like, set to act_wait - 30 after act_wait is increased by 1? Then it does weird shit in an if statement???
	RECT rcLeft[9];
	RECT rcRight[9];

	rcLeft[0] = { 0x50, 0x00, 0x60, 0x10 };
	rcLeft[1] = { 0x60, 0x00, 0x70, 0x10 };
	rcLeft[2] = { 0x70, 0x00, 0x80, 0x10 };
	rcLeft[3] = { 0x80, 0x00, 0x90, 0x10 };
	rcLeft[4] = { 0x90, 0x00, 0xA0, 0x10 };
	rcLeft[5] = { 0xA0, 0x00, 0xB0, 0x10 };
	rcLeft[6] = { 0xB0, 0x00, 0xC0, 0x10 };
	rcLeft[7] = { 0x90, 0x00, 0xA0, 0x10 };
	rcLeft[8] = { 0xD0, 0x40, 0xE0, 0x50 };

	rcRight[0] = { 0x50, 0x10, 0x60, 0x20 };
	rcRight[1] = { 0x60, 0x10, 0x70, 0x20 };
	rcRight[2] = { 0x70, 0x10, 0x80, 0x20 };
	rcRight[3] = { 0x80, 0x10, 0x90, 0x20 };
	rcRight[4] = { 0x90, 0x10, 0xA0, 0x20 };
	rcRight[5] = { 0xA0, 0x10, 0xB0, 0x20 };
	rcRight[6] = { 0xB0, 0x10, 0xC0, 0x20 };
	rcRight[7] = { 0x90, 0x10, 0xA0, 0x20 };
	rcRight[8] = { 0xD0, 0x50, 0xE0, 0x60 };

	switch (NPC->act_no)
	{
	case 0: //Stand and blink
		NPC->act_no = 1;
		NPC->ani_no = 2;
		
	case 1:
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 3;
		}
		break;

	case 2: //Blinking
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 2;
		}
		break;

	case 15: //Cast spell
		NPC->act_no = 16;
		NPC->act_wait = 0;
		NPC->ani_no = 4;
		
	case 16:
		if (++NPC->act_wait == 30)
		{
			playSound(21);
			createNpc(66, NPC->x, NPC->y - 0x2000, 0, 0, 0, NPC);
		}

		if (NPC->act_wait == 50)
			NPC->act_no = 14;
		break;

	case 20: //Fly up
		NPC->act_no = 21;
		NPC->ani_no = 0;
		NPC->ym = 0;
		NPC->bits |= npc_ignoresolid;
		
	case 21:
		NPC->ym -= 0x20;

		if (NPC->y < -0x1000)
			NPC->cond = 0;
		break;

	case 25: //Cast big spell
		NPC->act_no = 26;
		NPC->act_wait = 0;
		NPC->ani_no = 5;
		NPC->ani_wait = 0;
		
	case 26:
		if (++NPC->ani_no > 7)
			NPC->ani_no = 5;

		if (++NPC->act_wait == 30)
		{
			playSound(101);
			//SetFlash(0, 0, 2);
			NPC->act_no = 27;
			NPC->ani_no = 7;
		}
		break;

	case 27:
		if (++NPC->act_wait == 50)
		{
			NPC->act_no = 0;
			NPC->ani_no = 0;
		}

		break;

	case 30: //Cast spell
		NPC->act_no = 31;
		NPC->ani_no = 3;
		NPC->ani_wait = 0;
		
	case 31:
		if (++NPC->ani_wait > 10)
		{
			NPC->act_no = 32;
			NPC->ani_no = 4;
			NPC->ani_wait = 0;
		}
		break;

	case 32:
		if (++NPC->ani_wait > 100)
		{
			NPC->act_no = 1;
			NPC->ani_no = 2;
		}
		break;

	case 40: //Cast bubbles
		NPC->act_no = 41;
		NPC->act_wait = 0;
		
	case 41:
		NPC->ani_no = 4;

		something = ++NPC->act_wait - 30;

		if (something <= 0x14 && (1 << something) & 0x100401)
		{
			createNpc(11, NPC->x + 0x1000, NPC->y - 0x100, 0x600, random(-0x200, 0), 0, nullptr);
			playSound(33);
		}

		if (NPC->act_wait > 50)
			NPC->act_no = 0;
		break;

	case 50: //Defeated
		NPC->ani_no = 8;
		break;

	default:
		break;
	}

	//Move
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;
	
	if (NPC->act_no == 11) //"Sitting"??
	{
		if (NPC->ani_wait)
		{
			--NPC->ani_wait;
			NPC->ani_no = 1;
		}
		else
		{
			if (random(0, 100) == 1)
				NPC->ani_wait = 30;

			NPC->ani_no = 0;
		}
	}

	if (NPC->act_no == 14) //Something
	{
		if (NPC->ani_wait)
		{
			--NPC->ani_wait;
			NPC->ani_no = 3;
		}
		else
		{
			if (random(0, 100) == 1)
				NPC->ani_wait = 30;

			NPC->ani_no = 2;
		}
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

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
			currentPlayer.flag |= windLeft;
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
			currentPlayer.ym -= 0x88;
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