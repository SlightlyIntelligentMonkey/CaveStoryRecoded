#include "npc060.h"

void npcAct060(npc *NPC) //Toroko
{
	int action = NPC->act_no;
	int wait;
	int reset;

	int frameMap[11] = { 0, 1, 2, 1, 3, 4, 5, 4, 6, 7, 8 };

	switch(action)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
		
		goto npc060act1;

	case 1:
npc060act1:
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}

		if (NPC->x - 0x2000 < currentPlayer.x && NPC->x + 0x2000 > currentPlayer.x && NPC->y - 0x2000 < currentPlayer.y && NPC->y + 0x2000 > currentPlayer.y)
		{
			if (NPC->x <= currentPlayer.x)
				NPC->direct = 2;
			else
				NPC->direct = 0;
		}

		break;

	case 2:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}

		break;

	case 3:
		NPC->act_no = 4;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
		goto npc060act4;

	case 4:
npc060act4:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		if (NPC->flag & leftWall)
		{
			NPC->direct = 2;
			NPC->xm = 0x200;
		}
		if (NPC->flag & rightWall)
		{
			NPC->direct = 0;
			NPC->xm = -0x200;
		}

		if (NPC->direct)
			NPC->xm = 0x400;
		else
			NPC->xm = -0x400;

		break;

	case 6:
		NPC->act_no = 7;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
		NPC->ym = -0x400;
		goto npc060act7;

	case 7:
npc060act7:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		if (NPC->direct)
			NPC->xm = 256;
		else
			NPC->xm = -256;

		//Landing on the ground, I think?
		wait = NPC->act_wait;
		NPC->act_wait++;

		reset = wait != 0 && NPC->flag & ground;

		if (reset)
			NPC->act_no = 3;

		break;

	case 8:
		NPC->ani_no = 1;
		NPC->act_wait = 0;
		NPC->act_no = 9;
		NPC->ym = -512;

		goto npc060act9;

	case 9:
npc060act9:
		//Same as above in action 7
		wait = NPC->act_wait;
		NPC->act_wait++;

		reset = wait != 0 && NPC->flag & ground;

		if (reset)
			NPC->act_no = 0;

		break;

	case 10:
		NPC->act_no = 11;
		NPC->ani_no = 9;
		NPC->ym = -0x400;
		//PlaySoundObject(50, 1);

		if (NPC->direct)
			NPC->xm = 0x100;
		else
			NPC->xm = -0x100;

		break;

	case 11:
		wait = NPC->act_wait;
		NPC->act_wait++;

		reset = wait != 0 && NPC->flag & ground;
		if (reset)
		{
			NPC->act_no = 12;
			NPC->ani_no = 10;

			NPC->bits |= npc_shootable;
		}

		break;

	case 12:
		NPC->xm = 0;
		break;

	default:
		break;
	}

	//Move
	NPC->ym += 0x40;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	if (NPC->xm > 0x400)
		NPC->xm = 0x400;
	if (NPC->xm < -0x400)
		NPC->xm = -0x400;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Framerect
	int yOff = 64;

	if (NPC->direct)
		yOff += 16;

	NPC->rect = { frameMap[NPC->ani_no] << 4, yOff, 16 + (frameMap[NPC->ani_no] << 4), yOff + 16};
}

void npcAct064(npc *NPC) //First Cave critter
{
	int action = NPC->act_no;

	switch (action)
	{
	case 0: //Initialize
		NPC->y += 0x5FF;
		NPC->act_no = 1;

		break;

	case 2: //Going to jump
		if (++NPC->act_wait > 8)
		{
			//Jump
			NPC->act_no = 3;
			NPC->ani_no = 2;

			NPC->flag &= ~ground;
			NPC->ym = -0x5FF;

			//Jump in direction facing
			if (NPC->direct)
				NPC->xm = 0x100;
			else
				NPC->xm = -0x100;
		}

		break;

	case 3: //In air
		if (NPC->flag & ground) //Landed on the ground after jumping
		{
			NPC->act_no = 1;
			NPC->act_wait = 0;

			NPC->ani_no = 0;

			NPC->xm = 0;
		}
	}

	if (action == 1)
	{
		//Face towards player
		if (NPC->x <= currentPlayer.x)
			NPC->direct = 2;
		else
			NPC->direct = 0;

		//TargetX being used as timer (10/10 pixel code)
		if (NPC->tgt_x < 100)
			++NPC->tgt_x;

		//Timer for looking at Quote
		if (NPC->act_wait < 8 || NPC->x - 0xE000 >= currentPlayer.x || NPC->x + 0xE000 <= currentPlayer.x || NPC->y - 0xA000 >= currentPlayer.y || NPC->y + 0xA000 <= currentPlayer.y)
		{
			if (NPC->act_wait <= 7)
				++NPC->act_wait;
			NPC->ani_no = 0;
		}
		else
		{
			NPC->ani_no = 1;
		}

		//If shocked, jump
		if (NPC->shock)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;

			NPC->ani_no = 0;
		}

		//Go into "going to jump" state
		if (NPC->act_wait >= 8
			&& NPC->tgt_x >= 100
			&& NPC->x - 0x8000 < currentPlayer.x
			&& NPC->x + 0x8000 > currentPlayer.x
			&& NPC->y - 0xA000 < currentPlayer.y
			&& NPC->y + 0x6000 > currentPlayer.y)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;

			NPC->ani_no = 0;
		}
	}

	//Gravity
	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	//Move critter
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Change framerect
	if (NPC->direct)
		NPC->rect = { (NPC->ani_no << 4), 16, ((NPC->ani_no + 1) << 4), 32 };
	else
		NPC->rect = { (NPC->ani_no << 4), 0, ((NPC->ani_no + 1) << 4), 16 };
}

void npcAct065(npc *NPC) //First Cave Bat
{
	int action = NPC->act_no;

	switch (action)
	{
	case 0:
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
		NPC->act_no = 1;
		NPC->act_wait = random(0, 50);

	case 2:
		//Face towards player
		if (currentPlayer.x >= NPC->x)
			NPC->direct = 2;
		else
			NPC->direct = 0;

		//Fly up and down
		if (NPC->tgt_y < NPC->y)
			NPC->ym -= 0x10;
		if (NPC->tgt_y > NPC->y)
			NPC->ym += 0x10;

		//Limit speed
		if (NPC->ym > 0x300)
			NPC->ym = 0x300;
		if (NPC->ym < -0x300)
			NPC->ym = -0x300;
	}

	if (action == 1)
	{
		if (++NPC->act_wait >= 50)
		{
			NPC->act_wait = 0;
			NPC->act_no = 2;
			NPC->ym = 0x300;
		}
	}

	//Move bat
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Animate
	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	NPC->ani_no %= 3;

	if (NPC->direct)
		NPC->rect = { 32 + (NPC->ani_no << 4), 48, 48 + (NPC->ani_no << 4), 64 };
	else
		NPC->rect = { 32 + (NPC->ani_no << 4), 32, 48 + (NPC->ani_no << 4), 48 };
}

void npcAct073(npc *NPC) //Water drop
{
	NPC->ym += 0x20;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->ani_no = random(0, 4);

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;
	
	//Set frameRect
	NPC->rect = { 72 + (NPC->ani_no << 1),16,74 + (NPC->ani_no << 1),18 };

	if (NPC->direct == 2) //Blood
	{
		NPC->rect.top += 2;
		NPC->rect.bottom += 2;
	}

	if (++NPC->act_wait > 10)
	{
		if (NPC->flag & leftWall)
			NPC->cond = 0;
		if (NPC->flag & rightWall)
			NPC->cond = 0;
		if (NPC->flag & ground)
			NPC->cond = 0;
		if (NPC->flag & water)
			NPC->cond = 0;
	}

	if (NPC->y > levelHeight << 13)
		NPC->cond = 0;
}
