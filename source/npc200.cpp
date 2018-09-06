#include "npc200.h"

#include <vector>
#include "sound.h"
#include "player.h"
#include "mathUtils.h"
#include "caret.h"
#include "render.h"
#include "bullet.h"

using std::vector;

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
		createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 8);
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
			const int yVel = 3 * getSin(angle);
			const int xVel = 3 * getCos(angle);
			if (NPC->direct != dirLeft)
				createNpc(NPC_ProjectileDragonZombie, NPC->x + 0x1C00, NPC->y, xVel, yVel);
			else
				createNpc(NPC_ProjectileDragonZombie, NPC->x - 0x1C00, NPC->y, xVel, yVel);

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

void npcAct201(npc * NPC) // Dragon Zombie, dead
{
	if (NPC->direct != dirLeft)
		NPC->rect = { 200, 40, 240, 80 };
	else
		NPC->rect = { 200, 0, 240, 40 };
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

	constexpr RECT rcNPC[3] = { {184, 216, 200, 240}, {200, 216, 216, 240}, {216, 216, 232, 240} };

	NPC->animate(1, 0, 2);

	NPC->rect = rcNPC[NPC->ani_no];

	if (++NPC->count1 > 300)
	{
		NPC->cond = 0;
		createCaret(NPC->x, NPC->y, effect_RisingDisc);
	}
}

void npcAct203(npc * NPC) // Critter, Hopping Aqua (enemy)
{
	constexpr RECT rcLeft[3] = { {0, 80, 16, 96}, {16, 80, 32, 96}, {32, 80, 48, 96} };
	constexpr RECT rcRight[3] = { {0, 96, 16, 112}, {16, 96, 32, 112}, {32, 96, 48, 112} };

	enum
	{
		init = 0,
		waitForAttack = 1,
		startJump = 2,
		jumping = 3,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->y += 0x600;
		NPC->act_no = waitForAttack;
		// Fallthrough
	case waitForAttack:
		NPC->facePlayer();

		if (NPC->act_no < 8 || NPC->isPlayerAligned(0xE000, 0xA000))
		{
			if (NPC->act_wait < 8)
				++NPC->act_wait;
			NPC->ani_no = 0;
		}
		else
			NPC->ani_no = 1;

		if (NPC->shock)
		{
			NPC->act_no = startJump;
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}

		if (NPC->act_wait >= 8 && NPC->isPlayerWithinDistance(0x6000, 0xA000, 0x6000))
		{
			NPC->act_no = startJump;
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}
		break;

	case startJump:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = jumping;
			NPC->ani_no = 2;
			NPC->ym = -0x5FF;
			if (!(currentPlayer.cond & player_removed))
				playSound(SFX_CritterHop);

			NPC->moveTowardsPlayer(0x100);
		}
		break;

	case jumping:
		if (NPC->flag & ground)
		{
			NPC->xm = 0;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->act_no = waitForAttack;
			if (!(currentPlayer.cond & player_removed))
				playSound(SFX_QuoteHitGround);
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

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct204(npc * NPC) // Falling Spike, small
{
	vector<RECT> rcNPC = { {240, 80, 256, 96}, {240, 144, 256, 160} };

	enum
	{
		init = 0,
		normal = 1,
		shaky = 2,
		falling = 3,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = normal;
		NPC->tgt_x = NPC->x;
		// Fallthrough
	case normal:
		if (currentPlayer.x > NPC->x - pixelsToUnits(12) && currentPlayer.x < NPC->x + pixelsToUnits(12) && currentPlayer.y > NPC->y)
			NPC->act_no = shaky;
		break;

	case shaky:
		if (++NPC->act_wait / 6 % 2)
			NPC->x = NPC->tgt_x - 0x200;
		else
			NPC->x = NPC->tgt_x;

		if (NPC->act_wait > 30)
		{
			NPC->act_no = falling;
			NPC->ani_no = 1;
		}
		break;

	case falling:
		NPC->ym += 0x20;
		if (!(NPC->flag & 0xFF))
			break;
		if (!(currentPlayer.cond & player_removed))
			playSound(SFX_DestroyBreakableBlock);
		createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 4);
		NPC->cond = 0;
		return;

	default:
		break;
	}

	NPC->doGravity(0, 0xC00);
	NPC->y += NPC->ym;

	NPC->doRects(rcNPC);
}

void npcAct205(npc *NPC) // Falling Spike, large
{
	vector<RECT> rcNPC = {{112, 80, 128, 112}, {128, 80, 144, 112}};

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->tgt_x = NPC->x;
		NPC->y += pixelsToUnits(4);
	// Fallthrough
	case 1:
		if (currentPlayer.x > NPC->x - pixelsToUnits(12) && currentPlayer.x < NPC->y + pixelsToUnits(12)
		        && currentPlayer.y > NPC->y)
			NPC->act_no = 2;
		break;

	case 2:
		if (++NPC->act_wait / 6 % 2)
			NPC->x = NPC->tgt_x - pixelsToUnits(1);
		else
			NPC->x = NPC->tgt_x;

		if (NPC->act_wait > 30)
		{
			NPC->act_no = 3;
			NPC->ani_no = 1;
			NPC->act_wait = 0;
		}
		break;

	case 3:
		NPC->ym += 0x20;
		if (currentPlayer.y <= NPC->y)
		{
			NPC->bits |= npc_solidHard;
			NPC->damage = 0;
		}
		else
		{
			NPC->bits &= ~npc_solidHard;
			NPC->damage = 127;
		}

		if (++NPC->act_wait <= 8 || !(NPC->flag & 0xFF))
			break;

		NPC->bits |= npc_solidHard;
		NPC->act_no = 4;
		NPC->act_wait = 0;
		NPC->ym = 0;
		NPC->damage = 0;
		playSound(SFX_DestroyBreakableBlock);
		createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 4);
		createBullet(bullet_Unused, NPC->x, NPC->y, dirLeft, 0);
		return;

	case 4:
		if (++NPC->act_wait > 4)
		{
			NPC->act_no = 5;
			NPC->bits |= npc_shootable;
		}
		break;

	default:
		break;
	}

	NPC->doGravity(0, 0xC00);
	NPC->y += NPC->ym;

	NPC->doRects(rcNPC);
}

void npcAct210(npc *NPC) // Beetle, Follow Aqua (enemy)
{

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

