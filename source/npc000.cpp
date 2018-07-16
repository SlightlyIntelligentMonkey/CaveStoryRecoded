#include "npc000.h"

void npcActNone(npc *NPC)
{
	NPC->surf = 0x27;
	NPC->rect = { 0, 0, NPC->view.left >> 8, NPC->view.top >> 8 };
}

void npcAct000(npc *NPC) //Null
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;

		if (NPC->direct == 2)
			NPC->y += 0x2000;
	}

	NPC->rect = { 0, 0, 16, 16 };
}

void npcAct002(npc *NPC) //Behemoth
{
	int act_no = NPC->act_no;

	RECT rcLeft[7];
	RECT rcRight[7];
	RECT setRect;

	if (NPC->flag & leftWall)
	{
		NPC->direct = 2;
	}
	else if (NPC->flag & rightWall)
	{
		NPC->direct = 0;
	}

	switch (act_no) {
	case 0: //Normal act
		if (NPC->direct)
			NPC->xm = 0x100;
		else
			NPC->xm = -0x100;

		if (++NPC->ani_wait > 8)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 3)
			NPC->ani_no = 0;

		if (NPC->shock)
		{
			NPC->count1 = 0;
			NPC->act_no = 1;
			NPC->ani_no = 4;
		}

		break;

	case 1: //Hit
		NPC->xm = 7 * NPC->xm / 8;

		if (++NPC->count1 > 40)
		{
			if (NPC->shock) //Shot multiple times in a row, charge
			{
				NPC->count1 = 0;
				NPC->act_no = 2;
				NPC->ani_no = 6;
				NPC->ani_wait = 0;
				NPC->damage = 5;
			}
			else
			{
				NPC->act_no = 0;
				NPC->ani_wait = 0;
			}
		}

		break;

	case 2: //Charge
		if (NPC->direct)
			NPC->xm = 0x400;
		else
			NPC->xm = -0x400;

		if (++NPC->count1 > 200)
		{
			NPC->act_no = 0;
			NPC->damage = 1;
		}

		if (++NPC->ani_wait > 5)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 6) {
			NPC->ani_no = 5;
		}
	}

	//Move
	NPC->ym += 0x40;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Framerect
	rcLeft[0] = { 32, 0, 64, 24 };
	rcLeft[1] = { 0, 0, 32, 24 };
	rcLeft[2] = { 32, 0, 64, 24 };
	rcLeft[3] = { 64, 0, 96, 24 };
	rcLeft[4] = { 96, 0, 128, 24 };
	rcLeft[5] = { 128, 0, 160, 24 };
	rcLeft[6] = { 160, 0, 192, 24 };

	rcRight[0] = { 32, 24, 64, 48 };
	rcRight[1] = { 0, 24, 32, 48 };
	rcRight[2] = { 32, 24, 64, 48 };
	rcRight[3] = { 64, 24, 96, 48 };
	rcRight[4] = { 96, 24, 128, 48 };
	rcRight[5] = { 128, 24, 160, 48 };
	rcRight[6] = { 160, 24, 192, 48 };

	if (NPC->direct)
		setRect = rcRight[NPC->ani_no];
	else
		setRect = rcLeft[NPC->ani_no];

	NPC->rect = setRect;
}

void npcAct004(npc *NPC) //Smoke
{
	RECT rcLeft[8];
	RECT rcUp[8];

	rcLeft[0] = { 16, 0, 17, 1 };
	rcLeft[1] = { 16, 0, 32, 16 };
	rcLeft[2] = { 32, 0, 48, 16 };
	rcLeft[3] = { 48, 0, 64, 16 };
	rcLeft[4] = { 64, 0, 80, 16 };
	rcLeft[5] = { 80, 0, 96, 16 };
	rcLeft[6] = { 96, 0, 112, 16 };
	rcLeft[7] = { 112, 0, 128, 16 };

	rcUp[0] = { 16, 0, 17, 1 };
	rcUp[1] = { 80, 48, 96, 64 };
	rcUp[2] = { 0, 128, 16, 144 };
	rcUp[3] = { 16, 128, 32, 144 };
	rcUp[4] = { 32, 128, 48, 144 };
	rcUp[5] = { 48, 128, 64, 144 };
	rcUp[6] = { 64, 128, 80, 144 };
	rcUp[7] = { 80, 128, 96, 144 };

	if (NPC->act_no)
	{
		NPC->xm = 20 * NPC->xm / 21;
		NPC->ym = 20 * NPC->ym / 21;

		NPC->x += NPC->xm;
		NPC->y += NPC->ym;
	}
	else
	{
		if (!NPC->direct || NPC->direct == 1)
		{
			BYTE deg = random(0, 255);

			int degCos = getCos(deg);
			NPC->xm = degCos * random(0x200, 0x5FF) / 0x200;

			int degSin = getSin(deg);
			NPC->ym = degSin * random(0x200, 0x5FF) / 0x200;
		}

		NPC->ani_no = random(0, 4);
		NPC->ani_wait = random(0, 3);

		NPC->act_no = 1;
	}

	if (++NPC->ani_wait > 4)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no <= 7)
	{
		if (NPC->direct != 1)
		{
			NPC->rect = rcLeft[NPC->ani_no];
		}
		else
		{
			NPC->rect = rcUp[NPC->ani_no]; //Really broken
		}
	}
	else
		NPC->cond = 0;
}

void npcAct005(npc *NPC) //Egg Corridor critter
{
	int act_no = NPC->act_no;

	switch (act_no)
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

			//Jump in direct facing
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

	if (act_no == 1)
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
		NPC->rect = { (NPC->ani_no << 4), 64, ((NPC->ani_no + 1) << 4), 80 };
	else
		NPC->rect = { (NPC->ani_no << 4), 48, ((NPC->ani_no + 1) << 4), 64 };
}

void npcAct006(npc *NPC) //Beetle
{
	int act_no = NPC->act_no;

	switch (act_no) {
	case 0: //Init
		if (NPC->direct)
			NPC->act_no = 3;
		else
			NPC->act_no = 1;

		break;

	case 1: //Fly left
		NPC->xm -= 16;

		if (NPC->xm < -0x400)
			NPC->xm = -0x400;

		//Move
		if (NPC->shock)
			NPC->x += NPC->xm / 2;
		else
			NPC->x += NPC->xm;

		//Animate
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 2)
			NPC->ani_no = 1;

		//Hit wall, turn
		if (NPC->flag & leftWall)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->xm = 0;
			NPC->direct = 2;
		}

		break;

	case 2:
		//Wait 60 frames and then fly in facing direct
		if (++NPC->act_wait > 60)
		{
			if (NPC->direct)
				NPC->act_no = 3;
			else
				NPC->act_no = 1;

			NPC->ani_wait = 0;
			NPC->ani_no = 1;
		}

		break;

	case 3: //Fly right
		NPC->xm += 16;

		if (NPC->xm > 0x400)
			NPC->xm = 0x400;

		//Move
		if (NPC->shock)
			NPC->x += NPC->xm / 2;
		else
			NPC->x += NPC->xm;

		//Animate
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		
		if (NPC->ani_no > 2)
			NPC->ani_no = 1;

		//Hit wall, turn
		if (NPC->flag & rightWall)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->xm = 0;
			NPC->direct = 0;
		}

		break;
	}

	//Set framerect
	if (NPC->direct)
		NPC->rect = { (NPC->ani_no << 4), 96, (NPC->ani_no << 4) + 16, 112 };
	else
		NPC->rect = { (NPC->ani_no << 4), 80, (NPC->ani_no << 4) + 16, 96 };
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

	int act_no = NPC->act_no;
	switch (act_no)
	{
	case 1:
		//Move left
		NPC->xm -= 64;

		//Turn when far from player
		if (NPC->x < currentPlayer.x - 98304)
			NPC->act_no = 2;

		//Turn when hit wall
		if (NPC->flag & leftWall)
		{
			NPC->xm = 0;
			NPC->act_no = 2;
		}

		break;

	case 2:
		//Move right
		NPC->xm += 64;

		//Turn when far from player
		if (NPC->x > currentPlayer.x + 98304)
			NPC->act_no = 1;

		//Turn when hit wall
		if (NPC->flag & rightWall)
		{
			NPC->xm = 0;
			NPC->act_no = 1;
		}

		break;

	case 0:
		//Start at player's x
		NPC->x = currentPlayer.x;

		//Move in facing direct
		if (NPC->direct)
			NPC->act_no = 2;
		else
			NPC->act_no = 1;

		break;
	}

	//Face in moving direct
	if (NPC->xm >= 0)
		NPC->direct = 2;
	else
		NPC->direct = 0;

	//Limit speed
	if (NPC->xm > 0x5FF)
		NPC->xm = 0x5FF;

	if (NPC->xm < -0x5FF)
		NPC->xm = -0x5FF;

	//Move
	NPC->x += NPC->xm;

	//Animate
	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 2)
		NPC->ani_no = 0;

	//Set framerect
	if (NPC->direct)
	{
		setRect = &rcRight[NPC->ani_no];
	}
	else
	{
		setRect = &rcLeft[NPC->ani_no];
	}

	NPC->rect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
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

	int act_no = NPC->act_no;
	if (act_no)
	{
		if (act_no == 1)
		{
			if (NPC->x <= currentPlayer.x)
			{
				NPC->direct = 2;
				NPC->xm += 0x10;
			}
			else
			{
				NPC->direct = 0;
				NPC->xm -= 0x10;
			}

			if (NPC->xm > 0x2FF)
				NPC->xm = 0x2FF;
			if (NPC->xm < -0x2FF)
				NPC->xm = -0x2FF;

			if (NPC->y >= NPC->tgt_y)
				NPC->ym -= 0x8;
			else
				NPC->ym += 0x8;

			if (NPC->ym > 0x100)
				NPC->ym = 0x100;
			if (NPC->ym < -0x100)
				NPC->ym = -0x100;

			if (NPC->shock)
			{
				NPC->x += NPC->xm / 2;
				NPC->y += NPC->ym / 2;
			}
			else
			{
				NPC->x += NPC->xm;
				NPC->y += NPC->ym;
			}
		}
	}
	else
	{
		if (currentPlayer.x >= NPC->x + 0x2000 || currentPlayer.x <= NPC->x - 0x2000)
		{
			NPC->bits &= ~npc_shootable;
			NPC->rect.right = 0;
			NPC->damage = 0;
			NPC->xm = 0;
			NPC->ym = 0;

			return;
		}

		NPC->bits |= npc_shootable;
		NPC->ym = -256;
		NPC->tgt_y = NPC->y;
		NPC->act_no = 1;
		NPC->damage = 2;

		if (NPC->direct)
		{
			NPC->x = currentPlayer.x - 0x20000;
			NPC->xm = 767;
		}
		else
		{
			NPC->x = currentPlayer.x + 0x20000;
			NPC->xm = -767;
		}
	}

	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 1)
		NPC->ani_no = 0;

	if (NPC->direct)
	{
		setRect = &rcRight[NPC->ani_no];
	}
	else
	{
		setRect = &rcLeft[NPC->ani_no];
	}

	NPC->rect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}

void npcAct015(npc *NPC) //Closed chest
{
	int act_no = NPC->act_no;

	switch (act_no) {
	case 0:
		NPC->act_no = 1;

		NPC->bits |= npc_interact;

		//Spawn with smoke and stuff
		if (NPC->direct == 2)
		{
			NPC->ym = -0x200;

			for (int i = 0; i < 4; ++i)
				createNpc(4, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0), 0, nullptr);
		}

		if (random(0, 30) == 0)
			NPC->act_no = 2;

		break;

	default: //case 1:
		NPC->ani_no = 0;

		if (random(0, 30) == 0)
			NPC->act_no = 2;

		break;

	case 2:
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 2)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}

		break;
	}

	//Gravity
	NPC->ym += 0x40;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	//Move
	NPC->y += NPC->ym;

	//Framerect
	NPC->rect = { 240 + (NPC->ani_no << 4), 0, 256 + (NPC->ani_no << 4), 16 };
}

void npcAct016(npc *NPC) //Save point
{
	//First created
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->bits |= npc_interact;

		//Spawn with smoke and stuff
		if (NPC->direct) {
			NPC->ym = -0x200;
			NPC->bits &= ~npc_interact;

			for (int i = 0; i < 4; ++i)
				createNpc(4, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0), 0, nullptr);
		}
	}

	if (NPC->flag & ground)
		NPC->bits |= npc_interact;

	//Animate
	if (++NPC->ani_wait > 2)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 7)
		NPC->ani_no = 0;

	//Set framerect
	NPC->rect = { 96 + (NPC->ani_no << 4), 16, 112 + (NPC->ani_no << 4), 32 };

	//Fall down
	NPC->ym += 0x40;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->y += NPC->ym;
}

void npcAct017(npc *NPC) //Health refill
{
	int rand;
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;

		//Spawn with smoke and stuff
		if (NPC->direct) {
			NPC->ym = -0x200;

			for (int i = 0; i < 4; ++i)
				createNpc(4, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0), 0, nullptr);
		}

		goto npc017start;

	case 1:
	npc017start:
		rand = random(0, 30);

		if (rand > 9)
		{
			if (rand > 24)
				NPC->act_no = 4;
			else
				NPC->act_no = 3;
		}
		else
		{
			NPC->act_no = 2;
		}

		NPC->act_wait = random(16, 64);
		NPC->ani_wait = 0;

		break;

	case 2:
		NPC->rect = { 288, 0, 304, 16 };

		if (!--NPC->act_wait)
			NPC->act_no = 1;

		break;

	case 3:
		++NPC->ani_wait;

		if (NPC->ani_wait & 1)
		{
			NPC->rect = { 288, 0, 304, 16 };
		}
		else
		{
			NPC->rect = { 304, 0, 320, 16 };
		}

		if (!--NPC->act_wait)
			NPC->act_no = 1;

		break;

	case 4:
		NPC->rect = { 304, 0, 320, 16 };

		if (!--NPC->act_wait)
			NPC->act_no = 1;

		break;

	default:
		break;
	}

	NPC->ym += 0x40;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->y += NPC->ym;
}

void npcAct018(npc *NPC) //Door
{
	int act_no = NPC->act_no;
	if (act_no)
	{
		if (act_no == 1)
		{
			NPC->act_no = 0;
			NPC->rect = { 224, 16, 240, 40 };
		}
	}
	else if (NPC->direct)
	{
		NPC->rect = { 192, 112, 208, 136 };
	}
	else
	{
		NPC->rect = { 224, 16, 240, 40 };
	}
}
