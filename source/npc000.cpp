#include "npc000.h"

void npcAct000(npc *NPC) //Null
{
	if (!NPC->action)
	{
		NPC->action = 1;
		if (NPC->direction == 2)
			NPC->y += 0x2000;
	}

	NPC->frameRect = { 0, 0, 16, 16 };
}

void npcAct002(npc *NPC) //Behemoth
{
	int action = NPC->action;

	RECT rcLeft[7];
	RECT rcRight[7];
	RECT *setRect;

	if (NPC->hit & leftWall)
	{
		NPC->direction = 2;
	}
	else if (NPC->hit & rightWall)
	{
		NPC->direction = 0;
	}

	switch (action) {
	case 0: //Normal act
			//Start off moving in facing direction
		if (NPC->direction)
			NPC->xsp = 0x100;
		else
			NPC->xsp = -0x100;

		if (++NPC->animationWait > 8)
		{
			NPC->animationWait = 0;
			++NPC->animation;
		}

		NPC->animation %= 4;

		if (NPC->shock)
		{
			NPC->count1 = 0;
			NPC->action = 1;
			NPC->animation = 4;
		}

		break;

	case 1: //Hit
		NPC->xsp = 7 * NPC->xsp / 8;

		if (++NPC->count1 > 40)
		{
			if (NPC->shock) //Shot multiple times in a row, charge
			{
				NPC->count1 = 0;
				NPC->action = 2;
				NPC->animation = 6;
				NPC->animationWait = 0;
				NPC->damage = 5;
			}
			else
			{
				NPC->action = 0;
				NPC->animationWait = 0;
			}
		}

		break;

	case 2: //Charge
		if (NPC->direction)
			NPC->xsp = 0x400;
		else
			NPC->xsp = -0x400;

		if (++NPC->count1 > 200)
		{
			NPC->action = 0;
			NPC->damage = 1;
		}

		if (++NPC->animationWait > 5)
		{
			NPC->animationWait = 0;
			++NPC->animation;
		}

		if (NPC->animation > 6) {
			NPC->animation = 5;
		}
	}

	//Move
	NPC->ysp += 0x40;

	if (NPC->ysp > 0x5FF)
		NPC->ysp = 0x5FF;

	NPC->x += NPC->xsp;
	NPC->y += NPC->ysp;

	//Framerect
	rcLeft[0] = { 0, 0, 32, 24 };
	rcLeft[1] = { 32, 0, 64, 24 };
	rcLeft[2] = { 64, 0, 96, 24 };
	rcLeft[3] = { 32, 0, 64, 24 };
	rcLeft[4] = { 96, 0, 128, 24 };
	rcLeft[5] = { 128, 0, 160, 24 };
	rcLeft[6] = { 160, 0, 32, 24 };

	rcRight[0] = { 0, 24, 32, 48 };
	rcRight[1] = { 32, 24, 64, 48 };
	rcRight[2] = { 64, 24, 96, 48 };
	rcRight[3] = { 32, 24, 64, 48 };
	rcRight[4] = { 96, 24, 128, 48 };
	rcRight[5] = { 128, 24, 160, 48 };
	rcRight[6] = { 160, 24, 32, 48 };

	if (NPC->direction)
		setRect = &rcRight[NPC->animation];
	else
		setRect = &rcLeft[NPC->animation];

	NPC->frameRect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}

void npcAct005(npc *NPC) //Egg Corridor critter
{
	int action = NPC->action;

	//Idle
	if (action == 1)
	{
	npc005act:
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
		goto npc005end;
	}

	//Initialize
	if (action <= 1)
	{
		if (action)
			goto npc005end;

		NPC->y += 0x5FF;
		NPC->action = 1;
		goto npc005act;
	}

	//Going to jump.. (squished, ready to jump)
	if (action == 2)
	{
		if (++NPC->actionWait > 8)
		{
			//Jump
			NPC->action = 3;
			NPC->animation = 2;

			NPC->hit ^= ground;
			NPC->ysp = -0x5FF;

			//Jump in direction facing
			if (NPC->direction)
				NPC->xsp = 0x100;
			else
				NPC->xsp = -0x100;
		}
	}
	else if (action == 3 && NPC->hit & ground)
	{
		//Landed on the ground after jumping
		NPC->action = 1;
		NPC->actionWait = 0;

		NPC->animation = 0;

		NPC->xsp = 0;
	}
npc005end:
	//Gravity
	NPC->ysp += 0x40;
	if (NPC->ysp > 0x5FF)
		NPC->ysp = 0x5FF;

	//Move critter
	NPC->x += NPC->xsp;
	NPC->y += NPC->ysp;

	//Change framerect
	if (NPC->direction)
	{
		switch (NPC->animation) {
		case(0):
			NPC->frameRect = { 0,64,16,80 };
			break;

		case(1):
			NPC->frameRect = { 16,64,32,80 };
			break;

		case(2):
			NPC->frameRect = { 32,64,48,80 };
			break;
		}
	}
	else
	{
		switch (NPC->animation) {
		case(0):
			NPC->frameRect = { 0,48,16,64 };
			break;

		case(1):
			NPC->frameRect = { 16,48,32,64 };
			break;

		case(2):
			NPC->frameRect = { 32,48,48,64 };
			break;
		}
	}
}

void npcAct006(npc *NPC) //Beetle
{
	int action = NPC->action;

	switch (action) {
	case 0: //Init
		if (NPC->direction)
			NPC->action = 3;
		else
			NPC->action = 1;

		break;

	case 1: //Fly left
		NPC->xsp -= 16;

		if (NPC->xsp < -0x400)
			NPC->xsp = -0x400;

		//Move
		if (NPC->shock)
			NPC->x += NPC->xsp / 2;
		else
			NPC->x += NPC->xsp;

		//Animate
		if (++NPC->animationWait > 1)
		{
			NPC->animationWait = 0;
			++NPC->animation;
		}

		if (NPC->animation > 2)
			NPC->animation = 1;

		//Hit wall, turn
		if (NPC->hit & leftWall)
		{
			NPC->action = 2;
			NPC->actionWait = 0;
			NPC->animation = 0;
			NPC->xsp = 0;
			NPC->direction = 2;
		}

		break;

	case 2:
		//Wait 60 frames and then fly in facing direction
		if (++NPC->actionWait > 60)
		{
			if (NPC->direction)
				NPC->action = 3;
			else
				NPC->action = 1;

			NPC->animationWait = 0;
			NPC->animation = 1;
		}

		break;

	case 3: //Fly right
		NPC->xsp += 16;

		if (NPC->xsp > 0x400)
			NPC->xsp = 0x400;

		//Move
		if (NPC->shock)
			NPC->x += NPC->xsp / 2;
		else
			NPC->x += NPC->xsp;

		//Animate
		if (++NPC->animationWait > 1)
		{
			NPC->animationWait = 0;
			++NPC->animation;
		}
		
		if (NPC->animation > 2)
			NPC->animation = 1;

		//Hit wall, turn
		if (NPC->hit & rightWall)
		{
			NPC->action = 2;
			NPC->actionWait = 0;
			NPC->animation = 0;
			NPC->xsp = 0;
			NPC->direction = 0;
		}

		break;
	}

	//Set framerect
	if (NPC->direction)
		NPC->frameRect = { (NPC->animation << 4), 96, (NPC->animation << 4) + 16, 112 };
	else
		NPC->frameRect = { (NPC->animation << 4), 80, (NPC->animation << 4) + 16, 96 };
}

void npcAct007(npc *NPC) //Basil
{
	RECT *setRect;
	RECT rcRight[3];
	RECT rcLeft[3];

	rcLeft[0] = { 256, 64, 288, 80 };
	rcLeft[1] = { 256, 80, 288, 96 };
	rcLeft[2] = { 256, 96, 288, 112 };

	rcRight[0] = { 288, 64, 320, 80 };
	rcRight[1] = { 288, 80, 320, 96 };
	rcRight[2] = { 288, 96, 320, 112 };

	int action = NPC->action;
	switch (action)
	{
	case 1:
		//Move left
		NPC->xsp -= 64;

		//Turn when far from player
		if (NPC->x < currentPlayer.x - 98304)
			NPC->action = 2;

		//Turn when hit wall
		if (NPC->hit & leftWall)
		{
			NPC->xsp = 0;
			NPC->action = 2;
		}

		break;

	case 2:
		//Move right
		NPC->xsp += 64;

		//Turn when far from player
		if (NPC->x > currentPlayer.x + 98304)
			NPC->action = 1;

		//Turn when hit wall
		if (NPC->hit & rightWall)
		{
			NPC->xsp = 0;
			NPC->action = 1;
		}

		break;

	case 0:
		//Start at player's x
		NPC->x = currentPlayer.x;

		//Move in facing direction
		if (NPC->direction)
			NPC->action = 2;
		else
			NPC->action = 1;

		break;
	}

	//Face in moving direction
	if (NPC->xsp >= 0)
		NPC->direction = 2;
	else
		NPC->direction = 0;

	//Limit speed
	if (NPC->xsp > 0x5FF)
		NPC->xsp = 0x5FF;

	if (NPC->xsp < -0x5FF)
		NPC->xsp = -0x5FF;

	//Move
	NPC->x += NPC->xsp;

	//Animate
	if (++NPC->animationWait > 1)
	{
		NPC->animationWait = 0;
		++NPC->animation;
	}

	NPC->animation %= 3;

	//Set framerect
	if (NPC->direction)
	{
		setRect = &rcRight[NPC->animation];
	}
	else
	{
		setRect = &rcLeft[NPC->animation];
	}

	NPC->frameRect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}

void npcAct008(npc *NPC) //Follow beetle (egg corridor)
{
	RECT rcRight[2];
	RECT rcLeft[2];
	RECT *setRect;

	rcLeft[0] = { 80, 80, 96, 96 };
	rcLeft[1] = { 96, 80, 112, 96 };

	rcRight[0] = { 80, 96, 96, 112 };
	rcRight[1] = { 96, 96, 112, 112 };

	int action = NPC->action;
	if (action)
	{
		if (action == 1)
		{
			if (NPC->x <= currentPlayer.x)
			{
				NPC->direction = 2;
				NPC->xsp += 0x10;
			}
			else
			{
				NPC->direction = 0;
				NPC->xsp -= 0x10;
			}

			if (NPC->xsp > 0x2FF)
				NPC->xsp = 0x2FF;
			if (NPC->xsp < -0x2FF)
				NPC->xsp = -0x2FF;

			if (NPC->y >= NPC->targetY)
				NPC->ysp -= 0x8;
			else
				NPC->ysp += 0x8;

			if (NPC->ysp > 0x100)
				NPC->ysp = 0x100;
			if (NPC->ysp < -0x100)
				NPC->ysp = -0x100;

			if (NPC->shock)
			{
				NPC->x += NPC->xsp / 2;
				NPC->y += NPC->ysp / 2;
			}
			else
			{
				NPC->x += NPC->xsp;
				NPC->y += NPC->ysp;
			}
		}
	}
	else
	{
		if (currentPlayer.x >= NPC->x + 0x2000 || currentPlayer.x <= NPC->x - 0x2000)
		{
			NPC->flags &= 0xFFDFu;
			NPC->frameRect.right = 0;
			NPC->damage = 0;
			NPC->xsp = 0;
			NPC->ysp = 0;

			return;
		}

		NPC->flags |= npc_shootable;
		NPC->ysp = -256;
		NPC->targetY = NPC->y;
		NPC->action = 1;
		NPC->damage = 2;

		if (NPC->direction)
		{
			NPC->x = currentPlayer.x - 0x20000;
			NPC->xsp = 767;
		}
		else
		{
			NPC->x = currentPlayer.x + 0x20000;
			NPC->xsp = -767;
		}
	}

	if (++NPC->animationWait > 1)
	{
		NPC->animationWait = 0;
		++NPC->animation;
	}

	NPC->animation %= 2;

	if (NPC->direction)
	{
		setRect = &rcRight[NPC->animation];
	}
	else
	{
		setRect = &rcLeft[NPC->animation];
	}

	NPC->frameRect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}

void npcAct015(npc *NPC) //Closed chest
{
	int action = NPC->action;

	switch (action) {
	case 0:
		NPC->action = 1;

		if (NPC->direction == 2)
		{
			NPC->ysp = -512;
		}

		if (random(0, 30) == 0)
			NPC->action = 2;

		break;

	default: //case 1:
		NPC->animation = 0;

		if (random(0, 30) == 0)
			NPC->action = 2;

		break;

	case 2:
		if (++NPC->animationWait > 1)
		{
			NPC->animationWait = 0;
			++NPC->animation;
		}
		if (NPC->animation > 2)
		{
			NPC->action = 1;
			NPC->animation = 0;
		}

		break;
	}

	//Gravity
	NPC->ysp += 0x40;
	if (NPC->ysp > 0x5FF)
		NPC->ysp = 0x5FF;

	//Move
	NPC->y += NPC->ysp;

	//Framerect
	NPC->frameRect = { 240 + (NPC->animation << 4), 0, 256 + (NPC->animation << 4), 16 };
}

void npcAct016(npc *NPC) //Save point
{
	//First created
	if (!NPC->action)
	{
		NPC->action = 1;

		//Spawn with smoke and stuff
		if (NPC->direction) {
			NPC->ysp = -0x200;
		}
	}

	//Animate
	if (++NPC->animationWait > 2)
	{
		NPC->animationWait = 0;
		++NPC->animation;
	}

	NPC->animation %= 8;

	//Set framerect
	NPC->frameRect = { 96 + (NPC->animation << 4), 16, 112 + (NPC->animation << 4), 32 };

	//Fall down
	NPC->ysp += 0x40;
	if (NPC->ysp > 0x5FF)
		NPC->ysp = 0x5FF;

	NPC->y += NPC->ysp;
}

void npcAct017(npc *NPC) //Health refill
{
	int rand;
	switch (NPC->action)
	{
	case 0:
		NPC->action = 1;

		//Spawn with smoke and stuff
		if (NPC->direction) {
			NPC->ysp = -0x200;
		}

		goto npc017start;

	case 1:
	npc017start:
		rand = random(0, 30);

		if (rand > 9)
		{
			if (rand > 24)
				NPC->action = 4;
			else
				NPC->action = 3;
		}
		else
		{
			NPC->action = 2;
		}

		NPC->actionWait = random(16, 64);
		NPC->animationWait = 0;

		break;

	case 2:
		NPC->frameRect = { 288, 0, 304, 16 };

		if (!--NPC->actionWait)
			NPC->action = 1;

		break;

	case 3:
		++NPC->animationWait;

		if (NPC->animationWait & 1)
		{
			NPC->frameRect = { 288, 0, 304, 16 };
		}
		else
		{
			NPC->frameRect = { 304, 0, 320, 16 };
		}

		if (!--NPC->actionWait)
			NPC->action = 1;

		break;

	case 4:
		NPC->frameRect = { 304, 0, 320, 16 };

		if (!--NPC->actionWait)
			NPC->action = 1;

		break;

	default:
		break;
	}
	NPC->ysp += 0x40;

	if (NPC->ysp > 0x5FF)
		NPC->ysp = 0x5FF;

	NPC->y += NPC->ysp;
}

void npcAct018(npc *NPC) //Door
{
	int action = NPC->action;
	if (action)
	{
		if (action == 1)
		{
			NPC->action = 0;
			NPC->frameRect = { 224, 16, 240, 40 };
		}
	}
	else if (NPC->direction)
	{
		NPC->frameRect = { 192, 112, 208, 136 };
	}
	else
	{
		NPC->frameRect = { 224, 16, 240, 40 };
	}
}