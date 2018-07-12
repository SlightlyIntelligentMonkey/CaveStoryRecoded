#include "npc060.h"

void npcAct060(npc *NPC) //Toroko
{
	int action = NPC->action;
	int wait;
	int reset;

	int frameMap[11] = { 0, 1, 2, 1, 3, 4, 5, 4, 6, 7, 8 };

	switch(action)
	{
	case 0:
		NPC->action = 1;
		NPC->animation = 0;
		NPC->animationWait = 0;
		NPC->xsp = 0;
		
		goto npc060act1;

	case 1:
npc060act1:
		if (random(0, 120) == 10)
		{
			NPC->action = 2;
			NPC->actionWait = 0;
			NPC->animation = 1;
		}

		if (NPC->x - 0x2000 < currentPlayer.x && NPC->x + 0x2000 > currentPlayer.x && NPC->y - 0x2000 < currentPlayer.y && NPC->y + 0x2000 > currentPlayer.y)
		{
			if (NPC->x <= currentPlayer.x)
				NPC->direction = 2;
			else
				NPC->direction = 0;
		}

		break;

	case 2:
		if (++NPC->actionWait > 8)
		{
			NPC->action = 1;
			NPC->animation = 0;
		}

		break;

	case 3:
		NPC->action = 4;
		NPC->animation = 1;
		NPC->animationWait = 0;
		goto npc060act4;

	case 4:
npc060act4:
		if (++NPC->animationWait > 2)
		{
			NPC->animationWait = 0;
			++NPC->animation;
		}
		if (NPC->animation > 4)
			NPC->animation = 1;

		if (NPC->hit & leftWall)
		{
			NPC->direction = 2;
			NPC->xsp = 0x200;
		}
		if (NPC->hit & rightWall)
		{
			NPC->direction = 0;
			NPC->xsp = -0x200;
		}

		if (NPC->direction)
			NPC->xsp = 0x400;
		else
			NPC->xsp = -0x400;

		break;

	case 6:
		NPC->action = 7;
		NPC->actionWait = 0;
		NPC->animation = 1;
		NPC->animationWait = 0;
		NPC->ysp = -0x400;
		goto npc060act7;

	case 7:
npc060act7:
		if (++NPC->animationWait > 2)
		{
			NPC->animationWait = 0;
			++NPC->animation;
		}

		if (NPC->animation > 4)
			NPC->animation = 1;

		if (NPC->direction)
			NPC->xsp = 256;
		else
			NPC->xsp = -256;

		//Landing on the ground, I think?
		wait = NPC->actionWait;
		NPC->actionWait++;

		reset = wait != 0 && NPC->hit & ground;

		if (reset)
			NPC->action = 3;

		break;

	case 8:
		NPC->animation = 1;
		NPC->actionWait = 0;
		NPC->action = 9;
		NPC->ysp = -512;

		goto npc060act9;

	case 9:
npc060act9:
		//Same as above in action 7
		wait = NPC->actionWait;
		NPC->actionWait++;

		reset = wait != 0 && NPC->hit & ground;

		if (reset)
			NPC->action = 0;

		break;

	case 10:
		NPC->action = 11;
		NPC->animation = 9;
		NPC->ysp = -0x400;
		//PlaySoundObject(50, 1);

		if (NPC->direction)
			NPC->xsp = 0x100;
		else
			NPC->xsp = -0x100;

		break;

	case 11:
		wait = NPC->actionWait;
		NPC->actionWait++;

		reset = wait != 0 && NPC->hit & ground;
		if (reset)
		{
			NPC->action = 12;
			NPC->animation = 10;

			NPC->flags |= npc_shootable;
		}

		break;

	case 12:
		NPC->xsp = 0;
		break;

	default:
		break;
	}

	//Move
	NPC->ysp += 0x40;

	if (NPC->ysp > 0x5FF)
		NPC->ysp = 0x5FF;

	if (NPC->xsp > 0x400)
		NPC->xsp = 0x400;
	if (NPC->xsp < -0x400)
		NPC->xsp = -0x400;

	NPC->x += NPC->xsp;
	NPC->y += NPC->ysp;

	//Framerect
	int yOff = 64;

	if (NPC->direction)
		yOff += 16;

	NPC->frameRect = { frameMap[NPC->animation] << 4, yOff, 16 + (frameMap[NPC->animation] << 4), yOff + 16};
}

void npcAct064(npc *NPC) //First Cave critter
{
	int action = NPC->action;

	switch (action)
	{
	case 0: //Initialize
		NPC->y += 0x5FF;
		NPC->action = 1;

		break;

	case 2: //Going to jump
		if (++NPC->actionWait > 8)
		{
			//Jump
			NPC->action = 3;
			NPC->animation = 2;

			NPC->hit &= ~ground;
			NPC->ysp = -0x5FF;

			//Jump in direction facing
			if (NPC->direction)
				NPC->xsp = 0x100;
			else
				NPC->xsp = -0x100;
		}

		break;

	case 3: //In air
		if (NPC->hit & ground) //Landed on the ground after jumping
		{
			NPC->action = 1;
			NPC->actionWait = 0;

			NPC->animation = 0;

			NPC->xsp = 0;
		}
	}

	if (action == 1)
	{
		//Face towards player
		if (NPC->x <= currentPlayer.x)
			NPC->direction = 2;
		else
			NPC->direction = 0;

		//TargetX being used as timer (10/10 pixel code)
		if (NPC->targetX < 100)
			++NPC->targetX;

		//Timer for looking at Quote
		if (NPC->actionWait < 8 || NPC->x - 0xE000 >= currentPlayer.x || NPC->x + 0xE000 <= currentPlayer.x || NPC->y - 0xA000 >= currentPlayer.y || NPC->y + 0xA000 <= currentPlayer.y)
		{
			if (NPC->actionWait <= 7)
				++NPC->actionWait;
			NPC->animation = 0;
		}
		else
		{
			NPC->animation = 1;
		}

		//If shocked, jump
		if (NPC->shock)
		{
			NPC->action = 2;
			NPC->actionWait = 0;

			NPC->animation = 0;
		}

		//Go into "going to jump" state
		if (NPC->actionWait >= 8
			&& NPC->targetX >= 100
			&& NPC->x - 0x8000 < currentPlayer.x
			&& NPC->x + 0x8000 > currentPlayer.x
			&& NPC->y - 0xA000 < currentPlayer.y
			&& NPC->y + 0x6000 > currentPlayer.y)
		{
			NPC->action = 2;
			NPC->actionWait = 0;

			NPC->animation = 0;
		}
	}

	//Gravity
	NPC->ysp += 0x40;
	if (NPC->ysp > 0x5FF)
		NPC->ysp = 0x5FF;

	//Move critter
	NPC->x += NPC->xsp;
	NPC->y += NPC->ysp;

	//Change framerect
	if (NPC->direction)
		NPC->frameRect = { (NPC->animation << 4), 16, ((NPC->animation + 1) << 4), 32 };
	else
		NPC->frameRect = { (NPC->animation << 4), 0, ((NPC->animation + 1) << 4), 16 };
}

void npcAct065(npc *NPC) //First Cave Bat
{
	int action = NPC->action;

	switch (action)
	{
	case 0:
		NPC->targetX = NPC->x;
		NPC->targetY = NPC->y;
		NPC->action = 1;
		NPC->actionWait = random(0, 50);

	case 2:
		//Face towards player
		if (currentPlayer.x >= NPC->x)
			NPC->direction = 2;
		else
			NPC->direction = 0;

		//Fly up and down
		if (NPC->targetY < NPC->y)
			NPC->ysp -= 0x10;
		if (NPC->targetY > NPC->y)
			NPC->ysp += 0x10;

		//Limit speed
		if (NPC->ysp > 0x300)
			NPC->ysp = 0x300;
		if (NPC->ysp < -0x300)
			NPC->ysp = -0x300;
	}

	if (action == 1)
	{
		if (++NPC->actionWait >= 50)
		{
			NPC->actionWait = 0;
			NPC->action = 2;
			NPC->ysp = 0x300;
		}
	}

	//Move bat
	NPC->x += NPC->xsp;
	NPC->y += NPC->ysp;

	//Animate
	if (++NPC->animationWait > 1)
	{
		NPC->animationWait = 0;
		++NPC->animation;
	}

	NPC->animation %= 3;

	if (NPC->direction)
		NPC->frameRect = { 32 + (NPC->animation << 4), 48, 48 + (NPC->animation << 4), 64 };
	else
		NPC->frameRect = { 32 + (NPC->animation << 4), 32, 48 + (NPC->animation << 4), 48 };
}

void npcAct073(npc *NPC) //Water drop
{
	NPC->ysp += 0x20;

	if (NPC->ysp > 0x5FF)
		NPC->ysp = 0x5FF;

	NPC->animation = random(0, 4);

	NPC->x += NPC->xsp;
	NPC->y += NPC->ysp;
	
	//Set frameRect
	NPC->frameRect = { 72 + (NPC->animation << 1),16,74 + (NPC->animation << 1),18 };

	if (NPC->direction == 2) //Blood
	{
		NPC->frameRect.top += 2;
		NPC->frameRect.bottom += 2;
	}

	if (++NPC->actionWait > 10)
	{
		if (NPC->hit & leftWall)
			NPC->cond = 0;
		if (NPC->hit & rightWall)
			NPC->cond = 0;
		if (NPC->hit & ground)
			NPC->cond = 0;
		if (NPC->hit & water)
			NPC->cond = 0;
	}

	if (NPC->y > levelHeight << 13)
		NPC->cond = 0;
}