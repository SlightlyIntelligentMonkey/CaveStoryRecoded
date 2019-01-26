#include "npc120.h"

#include <array>
#include "sound.h"
#include "caret.h"
#include "player.h"
#include "weapons.h"
#include "input.h"
#include "mathUtils.h"
#include "game.h"

void npcAct120(npc *NPC) //Colon 1
{
	if (NPC->direct)
		NPC->rect = { 64, 16, 80, 32 };
	else
		NPC->rect = { 64, 0, 80, 16 };
}

void npcAct121(npc *NPC) //Colon 2
{
	RECT rect[3];

	rect[0] = { 0, 0, 16, 16 };
	rect[1] = { 16, 0, 32, 16 };
	rect[2] = { 112, 0, 128, 16 };

	if (!NPC->direct)
	{
		switch (NPC->act_no)
		{
		case 0:
			NPC->act_no = 1;
			NPC->ani_no = 0;
			NPC->ani_wait = 0;
//Fallthrough
		case 1:
			if (random(0, 120) == 10)
			{
				NPC->act_no = 2;
				NPC->act_wait = 0;
				NPC->ani_no = 1;
			}
			break;

		case 2:
			if (++NPC->act_wait > 8)
			{
				NPC->act_no = 1;
				NPC->ani_no = 0;
			}
			break;

		default:
			break;
		}

		NPC->rect = rect[NPC->ani_no];
		return;
	}

	NPC->rect = rect[2];

	if (++NPC->act_wait > 100)
	{
		NPC->act_wait = 0;
		createCaret(NPC->x, NPC->y, 5, 0);
	}
}

void npcAct122(npc *NPC) //Colon attack
{
	RECT rcLeft[10];
	RECT rcRight[10];

	rcLeft[0] = { 0x00, 0x00, 0x10, 0x10 };
	rcLeft[1] = { 0x10, 0x00, 0x20, 0x10 };
	rcLeft[2] = { 0x20, 0x00, 0x30, 0x10 };
	rcLeft[3] = { 0x00, 0x00, 0x10, 0x10 };
	rcLeft[4] = { 0x30, 0x00, 0x40, 0x10 };
	rcLeft[5] = { 0x00, 0x00, 0x10, 0x10 };
	rcLeft[6] = { 0x50, 0x00, 0x60, 0x10 };
	rcLeft[7] = { 0x60, 0x00, 0x70, 0x10 };
	rcLeft[8] = { 0x70, 0x00, 0x80, 0x10 };
	rcLeft[9] = { 0x80, 0x00, 0x90, 0x10 };

	rcRight[0] = { 0x00, 0x10, 0x10, 0x20 };
	rcRight[1] = { 0x10, 0x10, 0x20, 0x20 };
	rcRight[2] = { 0x20, 0x10, 0x30, 0x20 };
	rcRight[3] = { 0x00, 0x10, 0x10, 0x20 };
	rcRight[4] = { 0x30, 0x10, 0x40, 0x20 };
	rcRight[5] = { 0x00, 0x10, 0x10, 0x20 };
	rcRight[6] = { 0x50, 0x10, 0x60, 0x20 };
	rcRight[7] = { 0x60, 0x10, 0x70, 0x20 };
	rcRight[8] = { 0x70, 0x10, 0x80, 0x20 };
	rcRight[9] = { 0x80, 0x10, 0x90, 0x20 };

	switch (NPC->act_no)
	{
	case 0: //Init
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
//Fallthrough
	case 1: //Stand
		//Blink
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}

		//Look towards Quote if nearby
		if (NPC->x - 0x4000 < currentPlayer.x && NPC->x + 0x4000 > currentPlayer.x && NPC->y - 0x4000 < currentPlayer.y && NPC->y + 0x2000 > currentPlayer.y)
		{
			if (NPC->x <= currentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}
		break;

	case 2: //Blink
		//Blink for 8 frames
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;

	case 10: //Init attacking
		NPC->life = 1000;
		NPC->act_no = 11;
		NPC->act_wait = random(0, 50);
		NPC->ani_no = 0;
		NPC->damage = 0;
//Fallthrough
	case 11: //Standing
		if (NPC->act_wait)
			--NPC->act_wait;
		else
			NPC->act_no = 13;
		break;

	case 13: //Run towards player
		NPC->act_no = 14;
		NPC->act_wait = random(0, 50);

		if (NPC->x <= currentPlayer.x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;
//Fallthrough
	case 14: //Running (will jump after a few frames)
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 5)
			NPC->ani_no = 2;

		if (NPC->direct)
			NPC->xm += 64;
		else
			NPC->xm -= 64;

		if (NPC->act_wait)
		{
			--NPC->act_wait;
		}
		else
		{
			NPC->bits |= npc_shootable;
			NPC->act_no = 15;
			NPC->ani_no = 2;
			NPC->ym = -0x200;
			NPC->damage = 2;
		}
		break;

	case 15: //Jumped
		if (NPC->flag & ground)
		{
			NPC->bits |= npc_shootable;
			NPC->xm = 0;
			NPC->act_no = 10;
			NPC->damage = 0;
		}
		break;

	case 20: //Shot (lay on the ground for 300 - 400 frames)
		if (NPC->flag & ground)
		{
			NPC->xm = 0;
			NPC->act_no = 21;
			NPC->damage = 0;

			if (NPC->ani_no == 6)
				NPC->ani_no = 8;
			else
				NPC->ani_no = 9;

			NPC->act_wait = random(300, 400);
		}
		break;

	case 21: //Laying on the ground
		if (NPC->act_wait)
		{
			--NPC->act_wait;
		}
		else
		{
			NPC->bits |= npc_shootable;
			NPC->life = 1000;
			NPC->act_no = 11;
			NPC->act_wait = random(0, 50);
			NPC->ani_no = 0;
		}
		break;

	default:
		break;
	}

	//When shot
	if (NPC->act_no > 10 && NPC->act_no < 20 && NPC->life != 1000)
	{
		NPC->act_no = 20;
		NPC->ym = -0x200;
		NPC->ani_no = random(6, 7);
		NPC->bits &= ~npc_shootable;
	}

	//Limit run speed
	if (NPC->xm > 0x1FF)
		NPC->xm = 0x1FF;
	if (NPC->xm < -0x1FF)
		NPC->xm = -0x1FF;

	//Gravity
	NPC->ym += 0x20;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	//Move
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Set framerect
	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct123(npc *NPC) //Curly projectile
{
	RECT rect[4];
	bool bBreak = false;

	rect[0] = { 192, 0, 208, 16 };
	rect[1] = { 208, 0, 224, 16 };
	rect[2] = { 224, 0, 240, 16 };
	rect[3] = { 240, 0, 256, 16 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		createCaret(NPC->x, NPC->y, 3, 0);
		playSound(32, 1);

		switch (NPC->direct)
		{
		case dirLeft:
			NPC->xm = -0x1000;
			NPC->ym = random(-0x80, 0x80);
			break;
		case dirUp:
			NPC->xm = random(-0x80, 0x80);
			NPC->ym = -0x1000;
			break;
		case dirRight:
			NPC->xm = 0x1000;
			NPC->ym = random(-0x80, 0x80);
			break;
		case dirDown:
			NPC->xm = random(-0x80, 0x80);
			NPC->ym = 0x1000;
			break;
		default:
			break;
		}
		break;

	case 1:
		switch (NPC->direct)
		{
		case dirLeft:
			bBreak = (NPC->flag & leftWall) != 0;
			break;
		case dirUp:
			bBreak = (NPC->flag & ceiling) != 0;
			break;
		case dirRight:
			bBreak = (NPC->flag & rightWall) != 0;
			break;
		case dirDown:
			bBreak = (NPC->flag & ground) != 0;
			break;
		default:
			break;
		}

		NPC->x += NPC->xm;
		NPC->y += NPC->ym;
		break;

	default:
		break;
	}

	if (bBreak)
	{
		createCaret(NPC->x, NPC->y, 2, 2);
		playSound(28);
		NPC->cond = 0;
	}

	NPC->rect = rect[NPC->direct];
}

void npcAct124(npc *NPC) //Sunstone
{
	RECT rect[2];
	rect[0] = { 160, 0, 192, 32 };
	rect[1] = { 192, 0, 224, 32 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->x += 0x1000;
		NPC->y += 0x1000;
//Fallthrough
	case 1:
		break;

	case 10:
		NPC->act_no = 11;
		NPC->ani_no = 1;
		NPC->act_wait = 0;
		NPC->bits |= npc_ignoreSolid;
//Fallthrough
	case 11:
		switch (NPC->direct)
		{
		case dirLeft:
			NPC->x -= 0x80;
			break;
		case dirUp:
			NPC->y -= 0x80;
			break;
		case dirRight:
			NPC->x += 0x80;
			break;
		case dirDown:
			NPC->y += 0x80;
			break;
		default:
			break;
		}
		if (!(++NPC->act_wait & 7))
			playSound(26, 1);
		gViewport.quake = 20;
		break;

	default:
		break;
	}

	NPC->rect = rect[NPC->ani_no];
}

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

	NPC->doRects({0, 96, 16, 112}, {16, 96, 32, 112});
}

void npcAct126(npc *NPC) //Running Puppy
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0] = { 48, 144, 64, 160 };
	rcLeft[1] = { 64, 144, 80, 160 };
	rcLeft[2] = { 48, 144, 64, 160 };
	rcLeft[3] = { 80, 144, 96, 160 };
	rcLeft[4] = { 96, 144, 112, 160 };
	rcLeft[5] = { 112, 144, 128, 160 };

	rcRight[0] = { 48, 160, 64, 176 };
	rcRight[1] = { 64, 160, 80, 176 };
	rcRight[2] = { 48, 160, 64, 176 };
	rcRight[3] = { 80, 160, 96, 176 };
	rcRight[4] = { 96, 160, 112, 176 };
	rcRight[5] = { 112, 160, 128, 176 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
//Fallthrough
	case 1:
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}

		if (NPC->x - 0xC000 < currentPlayer.x && NPC->x + 0xC000 > currentPlayer.x && NPC->y - 0x4000 < currentPlayer.y && NPC->y + 0x2000 > currentPlayer.y)
		{
			if (NPC->x <= currentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}

		if (NPC->x - 0x4000 < currentPlayer.x && NPC->x + 0x4000 > currentPlayer.x && NPC->y - 0x4000 < currentPlayer.y && NPC->y + 0x2000 > currentPlayer.y)
		{
			if (NPC->x <= currentPlayer.x)
				NPC->direct = dirLeft;
			else
				NPC->direct = dirRight;

			NPC->act_no = 10;
		}
		break;

	case 2:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;

	case 10:
		NPC->act_no = 11;
		NPC->ani_no = 4;
		NPC->ani_wait = 0;
//Fallthrough
	case 11:
		if (NPC->flag & ground)
		{
			if (++NPC->ani_wait > 2)
			{
				NPC->ani_wait = 0;
				++NPC->ani_no;
			}

			if (NPC->ani_no > 5)
				NPC->ani_no = 4;
		}
		else
		{
			NPC->ani_no = 5;
			NPC->ani_wait = 0;
		}

		if (NPC->xm < 0 && NPC->flag & leftWall)
		{
			NPC->xm /= -2;
			NPC->direct = dirRight;
		}

		if (NPC->xm > 0 && NPC->flag & rightWall)
		{
			NPC->xm /= -2;
			NPC->direct = dirLeft;
		}

		if (NPC->direct)
			NPC->xm += 0x40;
		else
			NPC->xm -= 0x40;

		if (NPC->xm > 0x5FF)
			NPC->xm = 0x400;
		if (NPC->xm < -0x5FF)
			NPC->xm = -0x400;
		break;

	default:
		break;
	}

	if (isKeyPressed(gKeyDown))
		NPC->bits |= npc_interact;
	else
		NPC->bits &= ~npc_interact;

	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
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

void npcAct130(npc *NPC) //Puppy wag
{
	RECT rcLeft[4];
	RECT rcRight[4];

	rcLeft[0].left = 48;
	rcLeft[0].top = 144;
	rcLeft[0].right = 64;
	rcLeft[0].bottom = 160;
	rcLeft[1].left = 64;
	rcLeft[1].top = 144;
	rcLeft[1].right = 80;
	rcLeft[1].bottom = 160;
	rcLeft[2].left = 48;
	rcLeft[2].top = 144;
	rcLeft[2].right = 64;
	rcLeft[2].bottom = 160;
	rcLeft[3].left = 80;
	rcLeft[3].top = 144;
	rcLeft[3].right = 96;
	rcLeft[3].bottom = 160;
	rcRight[0].left = 48;
	rcRight[0].top = 160;
	rcRight[0].right = 64;
	rcRight[0].bottom = 176;
	rcRight[1].left = 64;
	rcRight[1].top = 160;
	rcRight[1].right = 80;
	rcRight[1].bottom = 176;
	rcRight[2].left = 48;
	rcRight[2].top = 160;
	rcRight[2].right = 64;
	rcRight[2].bottom = 176;
	rcRight[3].left = 80;
	rcRight[3].top = 160;
	rcRight[3].right = 96;
	rcRight[3].bottom = 176;

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->bits |= npc_interact;
//Fallthrough
	case 1:
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		if (NPC->x - 0x8000 < currentPlayer.x && NPC->x + 0x8000 > currentPlayer.x && NPC->y - 0x4000 < currentPlayer.y && NPC->y + 0x2000 > currentPlayer.y)
		    NPC->animate(3, 2, 3);

		if (NPC->x - 0xC000 < currentPlayer.x && NPC->x + 0xC000 > currentPlayer.x && NPC->y - 0x4000 < currentPlayer.y && NPC->y + 0x2000 > currentPlayer.y)
            NPC->facePlayer();
		break;

	case 2:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;
	}

	NPC->doGravity(0x40, 0x5FF);

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct131(npc *NPC) //Puppy sleep
{
	RECT rcLeft[1];
	RECT rcRight[1];

	rcLeft[0] = { 144, 144, 160, 160 };
	rcRight[0] = { 144, 160, 160, 176 };

	if (++NPC->act_wait > 100)
	{
		NPC->act_wait = 0;
		createCaret(NPC->x, NPC->y, 5, 0);
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct132(npc *NPC) //Puppy bark
{
	RECT rcLeft[5];
	RECT rcRight[5];

	rcLeft[0] = { 48, 144, 64, 160 };
	rcLeft[1] = { 64, 144, 80, 160 };
	rcLeft[2] = { 96, 144, 112, 160 };
	rcLeft[3] = { 96, 144, 112, 160 };
	rcLeft[4] = { 128, 144, 144, 160 };
	rcRight[0] = { 48, 160, 64, 176 };
	rcRight[1] = { 64, 160, 80, 176 };
	rcRight[2] = { 96, 160, 112, 176 };
	rcRight[3] = { 96, 160, 112, 176 };
	rcRight[4] = { 128, 160, 144, 176 };

	if (NPC->act_no < 100)
	{
		if (currentPlayer.x >= NPC->x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;
	}

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
//Fallthrough
	case 1:
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}

		if (NPC->x - 0x8000 >= currentPlayer.x || NPC->x + 0x8000 <= currentPlayer.x || NPC->y - 0x2000 >= currentPlayer.y || NPC->y + 0x2000 <= currentPlayer.y)
		{
			if (NPC->ani_no == 4)
				NPC->ani_no = 2;
		}
		else
		{
			if (++NPC->ani_wait > 4)
			{
				NPC->ani_wait = 0;
				++NPC->ani_no;
			}

			if (NPC->ani_no > 4)
				NPC->ani_no = 2;

			if (NPC->ani_no == 4 && !NPC->ani_wait)
				playSound(SFX_PuppyBark);
		}
		break;

	case 2:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;

	case 10:
		NPC->act_no = 11;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
//Fallthrough
	case 11:
		if (random(0, 120) == 10)
		{
			NPC->act_no = 12;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 12:
		if (++NPC->act_wait > 8 )
		{
			NPC->act_no = 11;
			NPC->ani_no = 0;
		}
		break;

	case 100:
		NPC->act_no = 101;
		NPC->count1 = 0;
//Fallthrough
	case 101:
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
		{
			if (NPC->count1 > 2)
			{
				NPC->ani_no = 0;
				NPC->count1 = 0;
			}
			else
			{
				NPC->ani_no = 2;
				++NPC->count1;
			}
		}

		if (NPC->ani_no == 4 && !NPC->ani_wait)
			playSound(SFX_PuppyBark);
		break;

	case 120:
		NPC->ani_no = 0;
		break;

	default:
		break;
	}

	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct133(npc *NPC) //Jenka
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = { 176, 32, 192, 48 };
	rcLeft[1] = { 192, 32, 208, 48 };
	rcRight[0] = { 176, 48, 192, 64 };
	rcRight[1] = { 192, 48, 208, 64 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
//Fallthrough
	case 1:
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 2:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct134(npc *NPC) //Armadillo
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0].left = 224;
	rcLeft[0].top = 0;
	rcLeft[0].right = 256;
	rcLeft[0].bottom = 16;
	rcLeft[1].left = 256;
	rcLeft[1].top = 0;
	rcLeft[1].right = 288;
	rcLeft[1].bottom = 16;
	rcLeft[2].left = 288;
	rcLeft[2].top = 0;
	rcLeft[2].right = 320;
	rcLeft[2].bottom = 16;
	rcRight[0].left = 224;
	rcRight[0].top = 16;
	rcRight[0].right = 256;
	rcRight[0].bottom = 32;
	rcRight[1].left = 256;
	rcRight[1].top = 16;
	rcRight[1].right = 288;
	rcRight[1].bottom = 32;
	rcRight[2].left = 288;
	rcRight[2].top = 16;
	rcRight[2].right = 320;
	rcRight[2].bottom = 32;

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 2;
		NPC->bits &= ~npc_shootable;
		NPC->bits |= npc_invulnerable;
//Fallthrough
	case 1:
		if (currentPlayer.x > NPC->x - 0x28000 && currentPlayer.x < NPC->x + 0x28000 && currentPlayer.y > NPC->y - 81920 && currentPlayer.y < NPC->y + 0x8000)
		{
			NPC->act_no = 10;
			NPC->bits |= npc_shootable;
			NPC->bits &= ~npc_invulnerable;
		}
		break;

	case 10:
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 1)
			NPC->ani_no = 0;

		if (!NPC->direct && NPC->flag & leftWall)
			NPC->direct = dirRight;
		if (NPC->direct == 2 && NPC->flag & rightWall)
			NPC->direct = dirLeft;

		if (NPC->direct)
			NPC->x += 0x100;
		else
			NPC->x -= 0x100;

		if (weaponBullets(6))
		{
			NPC->act_no = 20;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
			NPC->bits &= ~npc_shootable;
			NPC->bits |= npc_invulnerable;
		}
		break;

	case 20:
		if (++NPC->act_wait > 100)
		{
			NPC->act_no = 10;
			NPC->ani_no = 0;
			NPC->ani_wait = 0;
			NPC->bits |= npc_shootable;
			NPC->bits &= ~npc_invulnerable;
		}
		break;

	default:
		break;
	}

	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct135(npc *NPC)
{
	RECT rcRight[2];
	RECT rcLeft[2];

	rcLeft[0].left = 256;
	rcLeft[0].top = 32;
	rcLeft[0].right = 288;
	rcLeft[0].bottom = 64;
	rcLeft[1].left = 288;
	rcLeft[1].top = 32;
	rcLeft[1].right = 320;
	rcLeft[1].bottom = 64;
	rcRight[0].left = 256;
	rcRight[0].top = 64;
	rcRight[0].right = 288;
	rcRight[0].bottom = 96;
	rcRight[1].left = 288;
	rcRight[1].top = 64;
	rcRight[1].right = 320;
	rcRight[1].bottom = 96;

	uint8_t deg;
	int xm;
	int ym;

	if (currentPlayer.x < NPC->x - 0x2C000 || currentPlayer.x > NPC->x + 0x2C000 || currentPlayer.y < NPC->y - 0x14000 || currentPlayer.y > NPC->y + 0x8000)
		NPC->act_no = 0;

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->xm = 0;
//Fallthrough
	case 1:
		if (currentPlayer.x > NPC->x - 0x28000 && currentPlayer.x < NPC->x + 0x28000 && currentPlayer.y > NPC->y - 0x14000 && currentPlayer.y < NPC->y + 0x8000)
			NPC->act_no = 10;

		if (NPC->flag & ground)
			NPC->ani_no = 0;
		break;

	case 10:
		NPC->xm = 0;
		NPC->act_no = 11;
		NPC->act_wait = 0;
		NPC->ani_no = 0;
//Fallthrough
	case 11:
		if (++NPC->act_wait > 4 && NPC->flag & ground)
		{
			NPC->act_no = 20;
			NPC->ani_no = 1;
			NPC->count1 = 0;
			NPC->ym = -0x200 * random(1, 3);

			if (NPC->shock)
			{
				if (NPC->x >= currentPlayer.x)
					NPC->xm += 0x100;
				else
					NPC->xm -= 0x100;
			}
			else
			{
				if (NPC->x >= currentPlayer.x)
					NPC->xm -= 0x100;
				else
					NPC->xm += 0x100;
			}
		}
		break;

	case 20:
		if (NPC->ym > 0 && !NPC->count1)
		{
			++NPC->count1;
			deg = getAtan(NPC->x - currentPlayer.x, NPC->y + 0x800 - currentPlayer.y);
			ym = 2 * getSin(deg);
			xm = 2 * getCos(deg);
			createNpc(50, NPC->x, NPC->y, xm, ym, 0, nullptr, true);
			playSound(SFX_EnemyShootProjectile);
		}

		if (NPC->flag & ground)
		{
			NPC->act_no = 10;
			NPC->ani_no = 0;
		}
		break;

	default:
		break;
	}

	if (NPC->act_no >= 10)
	{
		if (NPC->x <= currentPlayer.x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;
	}

	NPC->ym += 0x33;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	if (NPC->xm > 0x5FF)
		NPC->xm = 0x5FF;
	if (NPC->xm < -0x5FF)
		NPC->xm = -0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct136(npc *NPC) //Puppy on Quote's head
{
	RECT rcRight[2];
	RECT rcLeft[2];

	rcLeft[0] = { 48, 144, 64, 160 };
	rcLeft[1] = { 64, 144, 80, 160 };
	rcRight[0] = { 48, 160, 64, 176 };
	rcRight[1] = { 64, 160, 80, 176 };

	switch (NPC->act_no)
	{
	case 0: //Init
		NPC->bits &= ~npc_interact;
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
//Fallthrough
	case 1: //Sitting
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 2: //Blink
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;

	default:
		break;
	}

	//Face in same direction player is facing
	if (currentPlayer.direct)
		NPC->direct = 2;
	else
		NPC->direct = 0;

	//Attach to player (and set framerects)
	NPC->y = currentPlayer.y - 0x1400;
	if (NPC->direct)
	{
		NPC->x = currentPlayer.x - 0x800;
		NPC->rect = rcRight[NPC->ani_no];
	}
	else
	{
		NPC->x = currentPlayer.x + 0x800;
		NPC->rect = rcLeft[NPC->ani_no];
	}

	//Bob up when walking
	if (currentPlayer.ani_no & 1)
		++NPC->rect.top;
}

void npcAct137(npc *NPC) // Large doorway frame
{
	NPC->doRects({ 96, 136, 128, 188 });
}

void npcAct138(npc *NPC)
{
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;

		if (NPC->direct)
		{
			NPC->rect = { 112, 112, 128, 136 };
			NPC->x -= 0x1000;
		}
		else
		{
			NPC->rect = { 96, 112, 112, 136 };
			NPC->x += 0x1000;
		}

		NPC->tgt_x = NPC->x;
//Fallthrough
	case 1:
		break;

	case 10:
		NPC->act_no = 11;
		NPC->ani_no = 1;
		NPC->act_wait = 0;
		NPC->bits |= npc_ignoreSolid;
//Fallthrough
	case 11:
		if (!(++NPC->act_wait & 7))
			playSound(SFX_LargeObjectHitGround);

		if (NPC->direct)
		{
			NPC->x = (NPC->act_wait / 8 << 9) + NPC->tgt_x;
			NPC->rect = { 112, 112, 128, 136 };
			NPC->rect.right -= NPC->act_wait / 8;
		}
		else
		{
			NPC->rect = { 96, 112, 112, 136 };
			NPC->rect.left += NPC->act_wait / 8;
		}

		if (NPC->act_wait == 104)
			NPC->cond = 0;
		break;
	}
}

void npcAct139(npc *NPC) //Doctor with the crown
{
	std::array<RECT, 3> rcLeft;
	std::array<RECT, 3> rcRight;

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

	NPC->doRects(rcLeft, rcRight);

	if (NPC->act_no == 31 || NPC->act_no == 41)
	{
		NPC->rect.bottom = NPC->act_wait / 2 + NPC->rect.top;

		if (NPC->act_wait / 2 & 1)
			++NPC->rect.left;
	}
}
