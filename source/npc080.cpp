#include "npc080.h"

#include "player.h"
#include "mathUtils.h"
#include "caret.h"
#include "sound.h"

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
			createNpc(NPC_ProjectileMiseryBubble, NPC->x, NPC->y - 0x2000, 0, 0, 0, NPC);
		}

		if (NPC->act_wait == 50)
			NPC->act_no = 14;
		break;

	case 20: //Fly up
		NPC->act_no = 21;
		NPC->ani_no = 0;
		NPC->ym = 0;
		NPC->bits |= npc_ignoresolid;
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
			createNpc(NPC_ProjectileBalrogEnergyBallInvincible, NPC->x + 0x1000, NPC->y - 0x100, 0x600, random(-0x200, 0), 0, nullptr);
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
		createCaret(NPC->x, NPC->y, effect_RisingDisc, 0);
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
		createCaret(NPC->x, NPC->y, effect_RisingDisc, 0);
		NPC->cond = 0;
	}
}

void npcAct086(npc *NPC) //Dropped missiles
{
	RECT rcMissile1[2];
	RECT rcMissile3[2];
	RECT rcLast;

	rcMissile1[0] = { 0, 80, 16, 96 };
	rcMissile1[1] = { 16, 80, 32, 96 };
	rcMissile3[0] = { 0, 122, 16, 128 };
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
		if (NPC->flag & npc_solidsoft)
			NPC->xm = 0x100;
		if (NPC->flag & npc_ignore44)
			NPC->ym = 0x40;
		if (NPC->flag & npc_ignoresolid)
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
		{
			createNpc(NPC_UnderwaterCurrent, NPC->x + (random(-8, 8) << 9), NPC->y, 0, 0, 0, nullptr);
		}

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
		{
			createNpc(NPC_UnderwaterCurrent, NPC->x + (random(-8, 8) << 9), NPC->y, 0, 0, 1, nullptr);
		}

		//Blow quote
		if (currentPlayer.x < NPC->x + 0x1000 && currentPlayer.x > NPC->x - 0x1000 && currentPlayer.y < NPC->y && currentPlayer.y > NPC->y - 0xC000)
		{
			currentPlayer.ym -= 0x88;
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
		{
			createNpc(NPC_UnderwaterCurrent, NPC->x + (random(-8, 8) << 9), NPC->y, 0, 0, 2, nullptr);
		}

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
