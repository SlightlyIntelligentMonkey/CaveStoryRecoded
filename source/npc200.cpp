#include "npc200.h"

#include "sound.h"
#include "player.h"
#include "mathUtils.h"
#include "caret.h"

void npcAct200(npc *NPC) // Dragon Zombie (enemy)
{
	constexpr RECT rcLeft[6] = { {0, 0, 40, 40}, {40, 0, 80, 40}, {80, 0, 120, 40}, {120, 0, 160, 40},
	{160, 0, 200, 40}, {200, 0, 240, 40} };
	constexpr RECT rcRight[6] = { {0, 40, 40, 80}, {40, 40, 80, 80}, {80, 40, 120, 80}, {120, 40, 160, 80}, {160, 40, 200, 80},
	{200, 40, 240, 80} };

	enum
	{
		init = 0,
		wait = 10,
		startShoot = 20,
		shoot = 30,
		dead = 100,
	};

	if (NPC->act_no < dead && NPC->life < 950)
	{
		playSound(SFX_Explosion);
		createSmoke(NPC->x, NPC->y, NPC->view.right, 8);
		dropExperience(NPC->x, NPC->y, NPC->exp);

		NPC->act_no = dead;
		NPC->bits &= ~npc_shootable;
		NPC->damage = 0;
	}

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = wait;
		NPC->count1 = 0;
		// Fallthrough
	case wait:
		NPC->animate(30, 0, 1);

		if (NPC->count1)	// count1 determines how much time till the dragon can shoot again
			--NPC->count1;
		if (!NPC->count1 && currentPlayer.x > NPC->x - 0xE000 && currentPlayer.x < NPC->x + 0xE000)	// Player also has to be in range
			NPC->act_no = startShoot;
		break;

	case startShoot:
		NPC->act_no = startShoot + 1;
		NPC->act_wait = 0;
		// Fallthrough
	case startShoot + 1:
		if (++NPC->act_wait / 2 % 2)
			NPC->ani_no = 2;
		else
			NPC->ani_no = 3;
		if (NPC->act_wait > 30)
			NPC->act_no = shoot;
		NPC->facePlayer();
		break;

	case shoot:
		NPC->act_no = shoot + 1;
		NPC->act_wait = 0;
		NPC->ani_no = 4;
		NPC->tgt_x = currentPlayer.x;
		NPC->tgt_y = currentPlayer.y;	// We're using the same target for all of our shots
		// Fallthrough
	case shoot + 1:
		if (++NPC->act_wait < 40 && NPC->act_wait % 8 == 1)
		{
			uint8_t angle = (NPC->direct != dirLeft) ? getAtan(NPC->x + 0x1C00 - NPC->tgt_x, NPC->y - NPC->tgt_y)
				: getAtan(NPC->x - 0x1C00 - NPC->tgt_x, NPC->y - NPC->tgt_y);
			angle += random(-6, 6);
			int yVel = 3 * getSin(angle);
			int xVel = 3 * getCos(angle);
			if (NPC->direct != dirLeft)
				createNpc(NPC_ProjectileDragonZombie, NPC->x + 0x1C00, NPC->y, xVel, yVel, dirLeft, nullptr);
			else
				createNpc(NPC_ProjectileDragonZombie, NPC->x - 0x1C00, NPC->y, xVel, yVel, dirLeft, nullptr);

			if (!(currentPlayer.cond & player_removed))	// wot ?
				playSound(SFX_FireballShoot);
		}
		if (NPC->act_wait > 60)
		{
			NPC->act_no = wait;
			NPC->count1 = random(100, 200);
			NPC->ani_wait = 0;
		}
		break;

	case dead:
		NPC->ani_no = 5;
		break;

	default:
		break;
	}

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct202(npc * NPC) // Dragon Zombie fire (projectile)
{
	if (NPC->flag & 0xFF)
	{
		NPC->cond = 0;
		createCaret(NPC->x, NPC->y, effect_RisingDisc);
	}
	
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	constexpr RECT rcNPC = { {184, 216, 200, 240}, {200, 216, 216, 240}, {216, 216, 232, 240} };

	NPC->animate(1, 0, 2);

	NPC->rect = rcNPC[NPC->ani_no];

	if (++NPC->count1 > 300)
	{
		NPC->cond = 0;
		createCaret(NPC->x, NPC->y, effect_RisingDisc);
	}
}

void npcAct211(npc *NPC) //Spikes
{
	RECT rcNPC[4];

	rcNPC[0].left = 256;
	rcNPC[0].top = 200;
	rcNPC[0].right = 272;
	rcNPC[0].bottom = 216;
	rcNPC[1].left = 272;
	rcNPC[1].top = 200;
	rcNPC[1].right = 288;
	rcNPC[1].bottom = 216;
	rcNPC[2].left = 288;
	rcNPC[2].top = 200;
	rcNPC[2].right = 304;
	rcNPC[2].bottom = 216;
	rcNPC[3].left = 304;
	rcNPC[3].top = 200;
	rcNPC[3].right = 320;
	rcNPC[3].bottom = 216;

	NPC->rect = rcNPC[NPC->code_event];
}

void npcAct216(npc *NPC) // Debug cat
{
	NPC->rect = { 256, 192, 272, 216 };
}
