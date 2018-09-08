#include "npc080.h"

#include "player.h"
#include "mathUtils.h"
#include "caret.h"
#include "sound.h"
#include "game.h"
#include "render.h"

using std::vector;

void npcAct080(npc *NPC) //Gravekeeper
{
	RECT rcLeft[7];
	RECT rcRight[7];

	rcLeft[0].left = 0; //Sorry I'm lazy
	rcLeft[0].top = 64;
	rcLeft[0].right = 24;
	rcLeft[0].bottom = 88;
	rcLeft[1].left = 24;
	rcLeft[1].top = 64;
	rcLeft[1].right = 48;
	rcLeft[1].bottom = 88;
	rcLeft[2].left = 0;
	rcLeft[2].top = 64;
	rcLeft[2].right = 24;
	rcLeft[2].bottom = 88;
	rcLeft[3].left = 48;
	rcLeft[3].top = 64;
	rcLeft[3].right = 72;
	rcLeft[3].bottom = 88;
	rcLeft[4].left = 72;
	rcLeft[4].top = 64;
	rcLeft[4].right = 96;
	rcLeft[4].bottom = 88;
	rcLeft[5].left = 96;
	rcLeft[5].top = 64;
	rcLeft[5].right = 120;
	rcLeft[5].bottom = 88;
	rcLeft[6].left = 120;
	rcLeft[6].top = 64;
	rcLeft[6].right = 144;
	rcLeft[6].bottom = 88;
	rcRight[0].left = 0;
	rcRight[0].top = 88;
	rcRight[0].right = 24;
	rcRight[0].bottom = 112;
	rcRight[1].left = 24;
	rcRight[1].top = 88;
	rcRight[1].right = 48;
	rcRight[1].bottom = 112;
	rcRight[2].left = 0;
	rcRight[2].top = 88;
	rcRight[2].right = 24;
	rcRight[2].bottom = 112;
	rcRight[3].left = 48;
	rcRight[3].top = 88;
	rcRight[3].right = 72;
	rcRight[3].bottom = 112;
	rcRight[4].left = 72;
	rcRight[4].top = 88;
	rcRight[4].right = 96;
	rcRight[4].bottom = 112;
	rcRight[5].left = 96;
	rcRight[5].top = 88;
	rcRight[5].right = 120;
	rcRight[5].bottom = 112;
	rcRight[6].left = 120;
	rcRight[6].top = 88;
	rcRight[6].right = 144;
	rcRight[6].bottom = 112;

	switch (NPC->act_no)
	{
	case 0:
		NPC->bits &= ~npc_shootable;
		NPC->act_no = 1;
		NPC->damage = 0;
		NPC->hit.left = 0x800;
//Fallthrough
	case 1:
		NPC->ani_no = 0;
		if (NPC->x - 0x10000 < currentPlayer.x && NPC->x + 0x10000 > currentPlayer.x && NPC->y - 0x6000 < currentPlayer.y && NPC->y + 0x4000 > currentPlayer.y)
		{
			NPC->ani_wait = 0;
			NPC->act_no = 2;
		}

		if (NPC->shock)
		{
			NPC->ani_no = 1;
			NPC->ani_wait = 0;
			NPC->act_no = 2;
			NPC->bits &= ~npc_shootable;
		}

		NPC->facePlayer();
		break;

	case 2:
		if (++NPC->ani_wait > 6)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 3)
			NPC->ani_no = 0;

		if (NPC->x - 0x2000 < currentPlayer.x && NPC->x + 0x2000 > currentPlayer.x)
		{
			NPC->hit.left = 0x2400;
			NPC->act_wait = 0;
			NPC->act_no = 3;
			NPC->bits |= npc_shootable;

			NPC->moveTowardsPlayer(pixelsToUnits(2));
		}

		if (currentPlayer.x >= NPC->x)
		{
			NPC->direct = 2;
			NPC->xm = 0x100;
		}
		else
		{
			NPC->direct = 0;
			NPC->xm = -0x100;
		}
		break;

	case 3:
		NPC->xm = 0;

		if (++NPC->act_wait > 40)
		{
			NPC->act_wait = 0;
			NPC->act_no = 4;
		}

		NPC->ani_no = 4;
		break;

	case 4:
		NPC->damage = 10;

		if (++NPC->act_wait > 2)
		{
			NPC->act_wait = 0;
			NPC->act_no = 5;
		}

		NPC->ani_no = 5;
		break;

	case 5:
		NPC->ani_no = 6;
		if (++NPC->act_wait > 60)
			NPC->act_no = 0;
		break;

	default:
		break;
	}

	if (NPC->xm < 0 && NPC->flag & leftWall)
		NPC->xm = 0;
	if (NPC->xm > 0 && NPC->flag & rightWall)
		NPC->xm = 0;

	NPC->ym += 0x20;

	if (NPC->xm > 0x400)
		NPC->xm = 0x400;
	if (NPC->xm < -0x400)
		NPC->xm = -0x400;

	if (NPC->ym > 0x5FF)
		NPC->xm = 0x5FF;
	if (NPC->ym < -0x5FF)
		NPC->xm = -0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct081(npc *NPC) //Big pignon
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0].left = 144;
	rcLeft[0].top = 64;
	rcLeft[0].right = 168;
	rcLeft[0].bottom = 88;
	rcLeft[1].left = 168;
	rcLeft[1].top = 64;
	rcLeft[1].right = 192;
	rcLeft[1].bottom = 88;
	rcLeft[2].left = 192;
	rcLeft[2].top = 64;
	rcLeft[2].right = 216;
	rcLeft[2].bottom = 88;
	rcLeft[3].left = 216;
	rcLeft[3].top = 64;
	rcLeft[3].right = 240;
	rcLeft[3].bottom = 88;
	rcLeft[4].left = 144;
	rcLeft[4].top = 64;
	rcLeft[4].right = 168;
	rcLeft[4].bottom = 88;
	rcLeft[5].left = 240;
	rcLeft[5].top = 64;
	rcLeft[5].right = 264;
	rcLeft[5].bottom = 88;
	rcRight[0].left = 144;
	rcRight[0].top = 88;
	rcRight[0].right = 168;
	rcRight[0].bottom = 112;
	rcRight[1].left = 168;
	rcRight[1].top = 88;
	rcRight[1].right = 192;
	rcRight[1].bottom = 112;
	rcRight[2].left = 192;
	rcRight[2].top = 88;
	rcRight[2].right = 216;
	rcRight[2].bottom = 112;
	rcRight[3].left = 216;
	rcRight[3].top = 88;
	rcRight[3].right = 240;
	rcRight[3].bottom = 112;
	rcRight[4].left = 144;
	rcRight[4].top = 88;
	rcRight[4].right = 168;
	rcRight[4].bottom = 112;
	rcRight[5].left = 240;
	rcRight[5].top = 88;
	rcRight[5].right = 264;
	rcRight[5].bottom = 112;

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
//Fallthrough
	case 1:
		if (random(0, 100) == 1)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		else
		{
			if (random(0, 150) == 1)
			{
				if (NPC->direct)
					NPC->direct = 0;
				else
					NPC->direct = 2;
			}

			if (random(0, 150) == 1)
			{
				NPC->act_no = 3;
				NPC->act_wait = 50;
				NPC->ani_no = 0;
			}
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
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
//Fallthrough
	case 4:
		if (!--NPC->act_wait)
			NPC->act_no = 0;

		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 2;

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
			NPC->xm = 0x100;
		else
			NPC->xm = -0x100;
		break;

	case 5:
		if (NPC->flag & ground)
			NPC->act_no = 0;
		break;
	default:
		break;
	}

	if (NPC->act_no < 5 && (1 << NPC->act_no) & 0x16 && NPC->shock)
	{
		NPC->ym = -512;
		NPC->ani_no = 5;
		NPC->act_no = 5;

		if (NPC->x >= currentPlayer.x)
			NPC->xm = -0x100;
		else
			NPC->xm = 0x100;
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
	// Fallthrough
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
	// Fallthrough
	case 16:
		if (++NPC->act_wait == 30)
		{
			playSound(SFX_Bubble);
			createNpc(NPC_ProjectileMiseryBubble, NPC->x, NPC->y - 0x2000);
		}

		if (NPC->act_wait == 50)
			NPC->act_no = 14;
		break;

	case 20: //Fly up
		NPC->act_no = 21;
		NPC->ani_no = 0;
		NPC->ym = 0;
		NPC->bits |= npc_ignoreSolid;
	// Fallthrough
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
	// Fallthrough
	case 26:
		if (++NPC->ani_no > 7)
			NPC->ani_no = 5;

		if (++NPC->act_wait == 30)
		{
			playSound(SFX_Lightning);
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
	// Fallthrough
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
	// Fallthrough
	case 41:
		NPC->ani_no = 4;

		something = ++NPC->act_wait - 30;

		if (something <= 0x14 && (1 << something) & 0x100401)
		{
			createNpc(NPC_ProjectileBalrogEnergyBallInvincible, NPC->x + 0x1000, NPC->y - 0x100, 0x600, random(-0x200, 0));
			playSound(SFX_FireballShoot);
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

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct083(npc *NPC) // Igor, standing
{
	RECT rcLeft[8];
	RECT rcRight[8];

	rcLeft[0] = { 0, 0, 40, 40 };
	rcLeft[1] = { 40, 0, 80, 40 };
	rcLeft[2] = { 80, 0, 120, 40 };
	rcLeft[3] = { 0, 0, 40, 40 };
	rcLeft[4] = { 120, 0, 160, 40 };
	rcLeft[5] = { 0, 0, 40, 40 };
	rcLeft[6] = { 160, 0, 200, 40 };
	rcLeft[7] = { 200, 0, 240, 40 };

	rcRight[0] = { 0, 40, 40, 80 };
	rcRight[1] = { 40, 40, 80, 80 };
	rcRight[2] = { 80, 40, 120, 80 };
	rcRight[3] = { 0, 40, 40, 80 };
	rcRight[4] = { 120, 40, 160, 80 };
	rcRight[5] = { 0, 40, 40, 80 };
	rcRight[6] = { 160, 40, 200, 80 };
	rcRight[7] = { 200, 40, 240, 80 };

	enum // Igor's states
	{
		standingPanting = 0,
		walking = 2,
		punch = 4,
	};

	switch (NPC->act_no)
	{
	case standingPanting:
		NPC->xm = 0;
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
	// Fallthrough
	case 1:
		// Animate from animation no 0 to 1 with a 5 frame delay
		if (++NPC->ani_wait > 5)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;
		break;

	case walking:
		NPC->act_no = 3;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
	// Fallthrough
	case 3:
		// Animate from animation no 2 to 5 with a 3 frame delay
		if (++NPC->ani_wait > 3)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 5)
			NPC->ani_no = 2;

		if (NPC->direct != dirLeft)
			NPC->xm = 0x200;
		else
			NPC->xm = -0x200;
		break;

	case punch:
		NPC->xm = 0;
		NPC->act_no = 5;
		NPC->act_wait = 0;
		NPC->ani_no = 6;
	// Fallthrough
	case 5:
		if (++NPC->act_wait > 10)
		{
			NPC->act_wait = 0;
			NPC->act_no = 6;
			NPC->ani_no = 7;
			playSound(SFX_EnemySmokePoof);
		}
		break;

	case 6:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = standingPanting;
			NPC->ani_no = 0;
		}
		break;

	case 7:
		NPC->act_no = 1;
		break;

	default:
		break;
	}

	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft[NPC->ani_no];
	else
		NPC->rect = rcRight[NPC->ani_no];
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
		createCaret(NPC->x, NPC->y, effect_RisingDisc);
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
		createCaret(NPC->x, NPC->y, effect_RisingDisc);
		NPC->cond = 0;
	}
}

void npcAct085(npc * NPC) // Terminal
{
	constexpr RECT rcLeft[3] = { {256, 96, 272, 120}, {256, 96, 272, 120}, {272, 96, 288, 120} };
	constexpr RECT rcRight[3] = { {256, 96, 272, 120}, {288, 96, 304, 120}, {304, 96, 320, 120} };

	if (NPC->act_no == 0)
	{
		NPC->ani_no = 0;
		if (NPC->x - 0x1000 < currentPlayer.x
			&& NPC->x + 0x1000 > currentPlayer.x
			&& NPC->y - 0x2000 < currentPlayer.y
			&& NPC->y + 0x1000 > currentPlayer.y)
		{
			playSound(SFX_ComputerScreenOn);
			NPC->act_no = 1;
		}
	}
	else if (NPC->act_no == 1 && ++NPC->ani_no > 2)
		NPC->ani_no = 1;

	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft[NPC->ani_no];
	else
		NPC->rect = rcRight[NPC->ani_no];
}

void npcAct086(npc *NPC) //Dropped missiles
{
	RECT rcMissile1[2];
	RECT rcMissile3[2];
	RECT rcLast;

	rcMissile1[0] = { 0, 80, 16, 96 };
	rcMissile1[1] = { 16, 80, 32, 96 };
	rcMissile3[0] = { 0, 112, 16, 128 };
	rcMissile3[1] = { 16, 112, 32, 128 };
	rcLast = { 16, 0, 32, 16 };

	if (!NPC->direct)
	{
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 1)
			NPC->ani_no = 0;
	}

	if (backgroundScroll == 5 || backgroundScroll == 6)
	{
		if (!NPC->act_no)
		{
			NPC->act_no = 1;
			NPC->ym = random(-32, 32);
			NPC->xm = random(127, 256);
		}

		NPC->xm -= 8;

		if (NPC->x <= 40959)
			NPC->cond = 0;
		if (NPC->x < -1536)
			NPC->x = -1536;
		if (NPC->flag & 1)
			NPC->xm = 256;
		if (NPC->flag & 2)
			NPC->ym = 64;
		if (NPC->flag & 8)
			NPC->ym = -64;

		NPC->x += NPC->xm;
		NPC->y += NPC->ym;
	}

	if (NPC->exp == 1)
		NPC->rect = rcMissile1[NPC->ani_no];
	else if (NPC->exp == 3)
		NPC->rect = rcMissile3[NPC->ani_no];

	if (!NPC->direct)
		++NPC->count1;

	if (NPC->count1 > 550)
		NPC->cond = 0;

	if (NPC->count1 > 500 && NPC->count1 / 2 & 1)
		NPC->rect.right = 0;

	if (NPC->count1 > 547)
		NPC->rect = rcLast;
}

void npcAct087(npc *NPC) //Health refill
{
	constexpr RECT rcHealth2[2] = { {32, 80, 48, 96}, { 48, 80, 64, 96 } };
	constexpr RECT rcHealth6[2] = { {64, 80, 80, 96}, { 80, 80, 96, 96 } };
	constexpr RECT rcLast = { 16, 0, 32, 16 };

	if (NPC->direct == dirLeft)
	{
		if(++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;
	}

	if (backgroundScroll == 5 || backgroundScroll == 6)
	{
		if (NPC->act_no == 0)
		{
			NPC->act_no = 1;
			NPC->ym = random(-0x20, 0x20);
			NPC->xm = random(0x7F, 0x100);
		}

		NPC->xm -= 8;

		if (NPC->x < 0xA000)
			NPC->cond = 0;
		if (NPC->x < -0x600)
			NPC->x = -0x600;
		if (NPC->flag & npc_solidSoft)
			NPC->xm = 0x100;
		if (NPC->flag & npc_ignore44)
			NPC->ym = 0x40;
		if (NPC->flag & npc_ignoreSolid)
			NPC->ym = -0x40;

		NPC->x += NPC->xm;
		NPC->y += NPC->ym;
	}

	if (NPC->exp == 2)
		NPC->rect = rcHealth2[NPC->ani_no];
	else if (NPC->exp == 6)
		NPC->rect = rcHealth6[NPC->ani_no];

	if (NPC->direct == dirLeft)
		++NPC->count1;

	if (NPC->count1 > 550)
		NPC->cond = 0;

	if (NPC->count1 > 500 && NPC->count1 / 2 % 2)
		NPC->rect.right = 0;

	if (NPC->count1 > 547)
		NPC->rect = rcLast;
}

void npcAct088(npc * NPC) // Igor (boss)
{
	RECT rcLeft[12];
	RECT rcRight[12];

	rcLeft[0] = { 0, 0, 40, 40 };
	rcLeft[1] = { 40, 0, 80, 40 };
	rcLeft[2] = { 80, 0, 120, 40 };
	rcLeft[3] = rcLeft[0];
	rcLeft[4] = { 120, 0, 160, 40 };
	rcLeft[5] = rcLeft[0];
	rcLeft[6] = { 160, 0, 200, 40 };
	rcLeft[7] = { 200, 0, 240, 40 };
	rcLeft[8] = { 0, 80, 40, 120 };
	rcLeft[9] = { 40, 80, 80, 120 };
	rcLeft[10] = { 240, 0, 280, 40 };
	rcLeft[11] = { 280, 0, 320, 40 };

	rcRight[0] = { 0, 40, 40, 80 };
	rcRight[1] = { 40, 40, 80, 80 };
	rcRight[2] = { 80, 40, 120, 80 };
	rcRight[3] = rcRight[0];
	rcRight[4] = { 120, 40, 160, 80 };
	rcRight[5] = rcRight[0];
	rcRight[6] = { 160, 40, 200, 80 };
	rcRight[7] = { 200, 40, 240, 80 };
	rcRight[8] = { 120, 80, 160, 120 };
	rcRight[9] = { 160, 80, 200, 120 };
	rcRight[10] = { 240, 40, 280, 80 };
	rcRight[11] = { 280, 40, 320, 80 };

	enum
	{
		init = 0,
		stand = 1,
		startAttack = 2,
		walk = 3,
		startPunch = 4,
		waitPunch = 5,
		punch = 6,
		jump = 7,
		land = 8,
		startMouthBlast = 9,
		mouthBlast = 10,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->xm = 0;
		NPC->act_no = stand;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		// Fallthrough
	case stand:
		NPC->animate(5, 0, 1);
		if (++NPC->act_wait > 50)
			NPC->act_no = startAttack;
		break;

	case startAttack:
		NPC->act_no = walk;
		NPC->act_wait = 0;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;

		NPC->facePlayer();

		if (++NPC->count1 >= 3 && NPC->life <= 150)
		{
			NPC->count2 = 1;
			NPC->direct = (NPC->direct == dirLeft) ? dirRight : dirLeft;
		}
		else
			NPC->count2 = 0;
		// Fallthrough
	case walk:
		++NPC->act_wait;
		NPC->animate(3, 2, 5);
		NPC->moveTowardsPlayer(0x200);

		if (NPC->count2)
		{
			if (NPC->act_wait > 0x10)
			{
				NPC->act_no = startMouthBlast;
				NPC->xm = 0;
				NPC->ani_no = 10;
			}
		}
		else if (NPC->act_wait <= 50)
		{
			if (NPC->direct != dirLeft)
			{
				if (NPC->x + 0x3000 > currentPlayer.x)
					NPC->act_no = startPunch;
			}
			else if (NPC->x - 0x3000 < currentPlayer.x)
				NPC->act_no = startPunch;
		}
		else
		{
			NPC->ani_no = 8;
			NPC->ym = -0x400;
			NPC->act_no = jump;
			NPC->act_wait = 0;
			NPC->xm = 3 * NPC->xm / 2;
			NPC->damage = 2;
		}
		break;

	case startPunch:
		NPC->xm = 0;
		NPC->act_no = waitPunch;
		NPC->act_wait = 0;
		NPC->ani_no = 6;
		// Fallthrough
	case waitPunch:
		if (++NPC->act_wait > 12)
		{
			NPC->act_wait = 0;
			NPC->act_no = punch;
			NPC->ani_no = 7;
			playSound(SFX_EnemySmokePoof);
			NPC->damage = 5;
			NPC->hit.left = 0x3000;
			NPC->hit.top = 1;
		}
		break;

	case punch:
		if (++NPC->act_wait > 10)
		{
			NPC->act_no = init;
			NPC->ani_no = 0;
			NPC->damage = 0;
			NPC->hit.left = 0x1000;
			NPC->hit.top = 0x2000;
		}
		break;

	case jump:
		if (NPC->flag & ground)
		{
			NPC->act_no = land;
			NPC->ani_no = 9;
			playSound(SFX_LargeObjectHitGround);
			viewport.quake = 30;
			NPC->damage = 0;

			for (size_t i = 0; i < 4; ++i)
				createNpc(NPC_Smoke,
					NPC->x + (random(-12, 12) << 9),
					NPC->y + (random(-12, 12) << 9),
					random(-0x155, 0x155),
					random(-0x600, 0));
		}
		break;

	case land:
		NPC->xm = 0;
		if (++NPC->act_wait > 10)
		{
			NPC->act_no = 0;
			NPC->ani_no = 0;
			NPC->damage = 0;
		}
		break;

	case startMouthBlast:
		NPC->act_no = mouthBlast;
		NPC->act_wait = 0;
		NPC->facePlayer();
		// Fallthrough
	case mouthBlast:
		if (++NPC->act_wait > 100 && NPC->act_wait % 6 == 1)
		{
			uint8_t angle = (NPC->direct != dirLeft) ? 248 : 136;
			angle += random(-0x10, 0x10);

			createNpc(NPC_ProjectileBalrogEnergyBallInvincible
				, NPC->x
				, NPC->y + 0x800
				, 3 * getCos(angle)
				, 3 * getSin(angle));
			playSound(SFX_DestroyBreakableBlock);
		}

		if (NPC->act_wait > 50 && NPC->act_wait / 2 % 2)
			NPC->ani_no = 11;
		else
			NPC->ani_no = 10;

		if (NPC->act_wait > 132)	// After having fired 6 shots
		{
			NPC->act_no = init;
			NPC->ani_no = 0;
			NPC->count1 = 0;
		}
		break;

	default:
		break;
	}

	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft[NPC->ani_no];
	else
		NPC->rect = rcRight[NPC->ani_no];
}

void npcAct089(npc * NPC) // Igor, dying
{
	RECT rcLeft[4];
	RECT rcRight[4];

	rcLeft[0].left = 80;
	rcLeft[0].top = 80;
	rcLeft[0].right = 120;
	rcLeft[0].bottom = 120;
	rcLeft[1].left = 240;
	rcLeft[1].top = 80;
	rcLeft[1].right = 264;
	rcLeft[1].bottom = 104;
	rcLeft[2].left = 264;
	rcLeft[2].top = 80;
	rcLeft[2].right = 288;
	rcLeft[2].bottom = 104;
	rcLeft[3].left = 288;
	rcLeft[3].top = 80;
	rcLeft[3].right = 312;
	rcLeft[3].bottom = 104;
	rcRight[0].left = 200;
	rcRight[0].top = 80;
	rcRight[0].right = 240;
	rcRight[0].bottom = 120;
	rcRight[1].left = 240;
	rcRight[1].top = 104;
	rcRight[1].right = 264;
	rcRight[1].bottom = 128;
	rcRight[2].left = 264;
	rcRight[2].top = 104;
	rcRight[2].right = 288;
	rcRight[2].bottom = 128;
	rcRight[3].left = 288;
	rcRight[3].top = 104;
	rcRight[3].right = 312;
	rcRight[3].bottom = 128;

	switch (NPC->act_no)
	{
	case 0:
		playSound(72);

		if (NPC->x <= currentPlayer.x)
			NPC->direct = 2;
		else
			NPC->direct = 0;

		for (int i = 0; i < 8; ++i)
			createNpc(4, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0), 0, nullptr);

		NPC->act_no = 1;
//Fallthrough
	case 1:
		if (++NPC->act_wait > 100)
		{
			NPC->act_wait = 0;
			NPC->act_no = 2;
		}

		if (!(NPC->act_wait % 5))
			createNpc(4, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0), 0, nullptr);

		if (NPC->direct)
			NPC->rect = rcRight[0];
		else
			NPC->rect = rcLeft[0];

		if (NPC->act_wait / 2 & 1)
			--NPC->rect.left;
		break;

	case 2:
		if (++NPC->act_wait / 2 & 1 && NPC->act_wait <= 99)
		{
			NPC->ani_no = 0;
			NPC->view.right = 0x2800;
			NPC->view.left = 0x2800;
			NPC->view.top = 0x2800;
		}
		else
		{
			NPC->ani_no = 1;
			NPC->view.right = 0x1800;
			NPC->view.left = 0x1800;
			NPC->view.top = 0x1000;
		}

		if (NPC->act_wait > 150)
		{
			NPC->act_wait = 0;
			NPC->act_no = 3;
			NPC->ani_no = 1;
		}

		if (!(NPC->act_wait % 9))
			createNpc(4, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0), 0, nullptr);

		if (NPC->direct)
			NPC->rect = rcRight[NPC->ani_no];
		else
			NPC->rect = rcLeft[NPC->ani_no];
		break;

	case 3:
		if (++NPC->ani_wait > 50)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no == 3)
			NPC->act_no = 4;

		if (NPC->direct)
			NPC->rect = rcRight[NPC->ani_no];
		else
			NPC->rect = rcLeft[NPC->ani_no];
		break;

	default:
		break;
	}

	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;
}

void npcAct090(npc *NPC) // Background
{
	NPC->rect = { 280, 80, 296, 104 };
}

void npcAct091(npc *NPC) // Cage
{
	if (!NPC->act_no)
	{
		++NPC->act_no;
		NPC->y += 0x2000;
	}

	NPC->rect = { 96, 88, 128, 112 };
}

void npcAct092(npc * NPC) // Sue (Computer)
{
	constexpr RECT rcNPC[3] = { {272, 216, 288, 240}, {288, 216, 304, 240}, {304, 216, 320, 240} };

	enum
	{
		init = 0,
		typing = 1,
		slouch = 2,
		upright = 3,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->x -= 0x800;
		NPC->y += 0x2000;
		NPC->act_no = typing;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		// Fallthrough
	case typing:
		NPC->animate(2, 0, 1);

		if (!random(0, 80))
		{
			NPC->act_no = slouch;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		if (!random(0, 120))
		{
			NPC->act_no = upright;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}
		break;

	case slouch:
		if (++NPC->act_wait > 40)
		{
			NPC->act_no = upright;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}
		break;

	case upright:
		if (++NPC->act_wait > 80)
		{
			NPC->act_no = typing;
			NPC->ani_no = 0;
		}
		break;

	default:
		break;
	}

	NPC->rect = rcNPC[NPC->ani_no];
}

void npcAct093(npc * NPC)
{
	vector<RECT> rcLeft(7);
	vector<RECT> rcRight(7);

	rcLeft[0] = { 128, 0, 144, 16 };
	rcLeft[1] = { 144, 0, 160, 16 };
	rcLeft[2] = { 160, 0, 176, 16 };
	rcLeft[3] = rcLeft[0];
	rcLeft[4] = { 176, 0, 192, 16 };
	rcLeft[5] = rcLeft[0];
	rcLeft[6] = { 32, 32, 48, 48 };

	rcRight[0] = { 128, 16, 144, 32 };
	rcRight[1] = { 144, 16, 160, 32 };
	rcRight[2] = { 160, 16, 176, 32 };
	rcRight[3] = rcRight[0];
	rcRight[4] = { 176, 16, 192, 32 };
	rcRight[5] = rcRight[0];
	rcRight[6] = { 32, 32, 48, 48 };

	enum
	{
		init = 0,
		stand = 1,
		waitBlink = 2,
		startWalk = 3,
		walking = 4,
		sleeping = 10,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = stand;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		// Fallthrough
	case stand:
		if (!random(0, 120))
		{
			NPC->act_no = waitBlink;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		if (NPC->isPlayerWithinDistance(tilesToUnits(2), tilesToUnits(2), tilesToUnits(1)))
			NPC->facePlayer();
		break;

	case waitBlink:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = stand;
			NPC->ani_no = 0;
		}
		break;

	case startWalk:
		NPC->act_no = walking;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
		// Fallthrough
	case walking:
		NPC->animate(4, 2, 5);
		if (NPC->direct != dirLeft)
			NPC->x += 0x200;
		else
			NPC->x -= 0x200;
		break;

	case sleeping:
		NPC->ani_no = 6;
		if (++NPC->act_wait > 200)
		{
			NPC->act_wait = 0;
			createCaret(NPC->x, NPC->y, effect_ZzZ);
		}
		break;

	default:
		break;
	}

	NPC->doRects(rcLeft, rcRight);
}

void npcAct094(npc * NPC)
{
	vector<RECT> rcNPC = { {272, 0, 320, 24}, {272, 24, 320, 48}, {272, 48, 320, 73}, {272, 72, 320, 96}, {272, 96, 320, 120} };

	enum
	{
		waitingForShoot = 0,
		falling = 10,
		animateThrust = 11,
		thrusting = 12,
		gotShot = 20,
	};

	switch (NPC->act_no)
	{
	case waitingForShoot:
		NPC->ani_no = 4;
		if (NPC->shock)
		{
			NPC->ani_no = 0;
			NPC->act_no = falling;
			NPC->act_wait = 0;
		}
		break;

	case falling:
		NPC->bits |= npc_shootable;
		NPC->bits &= ~npc_invulnerable;
		if (++NPC->act_wait > 40)
		{
			NPC->act_wait = 0;
			NPC->ani_wait = 0;
			NPC->act_no = animateThrust;
		}
		break;

	case animateThrust:
		if (++NPC->ani_wait > 5)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 2)
		{
			NPC->act_no = thrusting;
			NPC->ani_no = 3;
		}
		break;

	case thrusting:
		NPC->ym = -0x155;
		if (++NPC->act_wait > 20)
		{
			NPC->act_wait = 0;
			NPC->act_no = falling;
			NPC->ani_no = 0;
		}
		break;

	case gotShot:
		NPC->xm /= 2;
		NPC->ym += 0x20;
		if (!NPC->shock)
		{
			NPC->act_wait = 30;
			NPC->act_no = falling;
			NPC->ani_no = 0;
		}
		break;

	default:
		break;
	}

	if (NPC->shock)
	{
		if (++NPC->count2 > 12)
		{
			NPC->act_no = 20;
			NPC->ani_no = 4;
			NPC->bits &= ~npc_shootable;
			NPC->bits |= npc_invulnerable;
		}
	}
	else
		NPC->count2 = 0;

	if (NPC->act_no >= falling)
	{
		if (NPC->flag & leftWall)
		{
			NPC->count1 = 50;
			NPC->direct = dirRight;
		}
		if (NPC->flag & rightWall)
		{
			NPC->count1 = 50;
			NPC->direct = 0;
		}

		if (NPC->count1)
		{
			--NPC->count1;
			NPC->moveInDir(0x80);
		}
		else
		{
			NPC->count1 = 50;
			NPC->facePlayer();
		}
		NPC->ym += 0x10;
		if (NPC->flag & ground)
			NPC->ym = -0x400;
	}

	NPC->limitXVel(0x100);
	NPC->limitYVel(0x300);
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rcNPC);
}

void npcAct096(npc *NPC) //Fan left
{
	const int action = NPC->act_no;

	switch (action)
	{
	case 1:
		NPC->ani_no = 0;
		break;

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
			createNpc(NPC_UnderwaterCurrent, NPC->x + (random(-8, 8) << 9), NPC->y, 0, 0, dirLeft);

		//Blow quote
		if (currentPlayer.y < NPC->y + 0x1000 && currentPlayer.y > NPC->y - 0x1000 && currentPlayer.x < NPC->x && currentPlayer.x > NPC->x - 0xC000)
		{
			currentPlayer.xm -= 0x88;
			currentPlayer.cond |= player_noFriction;
		}

		break;
	case 0:
		if (NPC->direct == dirRight)
			NPC->act_no = 2;
		else
			NPC->ani_no = 1; // Overriden by the next statement. Pixel's fault
		NPC->ani_no = 0;
		break;
	}

	NPC->rect = { 272 + (NPC->ani_no << 4), 120, 288 + (NPC->ani_no << 4), 136 };
}

void npcAct097(npc *NPC) //Fan up
{
	const int action = NPC->act_no;

	switch (action)
	{
	case 1:
		NPC->ani_no = 0;
		break;

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
			createNpc(NPC_UnderwaterCurrent, NPC->x + (random(-8, 8) << 9), NPC->y, 0, 0, dirUp);

		//Blow quote
		if (currentPlayer.x < NPC->x + 0x1000 && currentPlayer.x > NPC->x - 0x1000 && currentPlayer.y < NPC->y && currentPlayer.y > NPC->y - 0xC000)
			currentPlayer.ym -= 0x88;

		break;
	case 0:
		if (NPC->direct == dirRight)
			NPC->act_no = 2;
		else
			NPC->ani_no = 1; // Overriden by the next statement. Pixel's fault
		NPC->ani_no = 0;
		break;
	}

	NPC->rect = { 272 + (NPC->ani_no << 4), 136, 288 + (NPC->ani_no << 4), 152 };
}

void npcAct098(npc *NPC) //Fan right
{
	const int action = NPC->act_no;

	switch (action)
	{
	case 1:
		NPC->ani_no = 0;
		break;

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
			createNpc(NPC_UnderwaterCurrent, NPC->x + (random(-8, 8) << 9), NPC->y, 0, 0, dirRight);

		//Blow quote
		if (currentPlayer.y < NPC->y + 0x1000 && currentPlayer.y > NPC->y - 0x1000 && currentPlayer.x > NPC->x && currentPlayer.x < NPC->x + 0xC000)
		{
			currentPlayer.xm += 0x88;
			currentPlayer.cond |= player_noFriction;
		}

		break;
	case 0:
		if (NPC->direct == dirRight)
			NPC->act_no = 2;
		else
			NPC->ani_no = 1; // Overriden by the next statement. Pixel's fault
		NPC->ani_no = 0;
		break;
	}

	NPC->rect = { 272 + (NPC->ani_no << 4), 152, 288 + (NPC->ani_no << 4), 168 };
}
