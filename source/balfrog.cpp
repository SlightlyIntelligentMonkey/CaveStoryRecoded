#include "boss.h"
#include "sound.h"
#include "mathUtils.h"
#include "player.h"
#include "render.h"
#include "game.h"
#include "common.h"
#include "stage.h"

namespace balfrogStates
{
    //states enum so this is more readable
    enum
    {
        ini = 0,
        start = 10,
        ini_flicker = 20,
        flicker = 21,
        wait = 100,
        ini_hop_1 = 101,
        ini_hop_2 = 102,
        hop = 103,
        midair = 104,
        ini_land = 110,
        land = 111,
        ini_shoot = 112,
        shoot = 113,
        ini_leap = 120,
        ini_leap_2 = 121,
        ini_leap_3 = 122,
        leap = 123,
        leap_midair = 124,
        die = 130,
        die_flashing = 131,
        revert = 132,
        nop_start = 140,
        nop = 141,
        go_into_ceilng = 142,
        gone_into_ceiling = 143
    };
}   // balfrogStates

//balfrog's mouth
void balfrogMouth(npc *sub)
{
	int minus;

	if (gBossObj[0].direct)
		minus = -1;
	else
		minus = 1;

	switch (gBossObj[0].ani_no)
	{
	case -1:
		sub->hit_voice = 52;
		sub->hit.left = pixelsToUnits(16);
		sub->hit.top = pixelsToUnits(16);
		sub->hit.right = pixelsToUnits(16);
		sub->hit.bottom = pixelsToUnits(16);
		sub->size = 3;
		sub->bits = 4;
		break;
	case 0:
		sub->x = gBossObj[0].x - pixelsToUnits(24) * minus;
		sub->y = gBossObj[0].y - pixelsToUnits(24);
		break;
	case 1:
		sub->x = gBossObj[0].x - pixelsToUnits(24) * minus;
		sub->y = gBossObj[0].y - pixelsToUnits(20);
		break;
	case 2:
	case 3:
		sub->x = gBossObj[0].x - pixelsToUnits(24) * minus;
		sub->y = gBossObj[0].y - pixelsToUnits(16);
		break;
	case 4:
		sub->x = gBossObj[0].x - pixelsToUnits(24) * minus;
		sub->y = gBossObj[0].y - pixelsToUnits(43);
		break;
	default:
		return;
	}
}

//second half of balfrog or something and has the exact same hitbox and is at the exact same coordinates
//so really this does fucking nothing
void balfrogOtherHalf(npc *sub)
{
	if (gBossObj[0].ani_no != -1)
	{
		if (gBossObj[0].ani_no >= 0 && gBossObj[0].ani_no <= 5)
		{
			sub->x = gBossObj[0].x;
			sub->y = gBossObj[0].y;
		}
	}
	else
	{
		sub->hit_voice = 52;
		sub->hit.left = pixelsToUnits(24);
		sub->hit.top = pixelsToUnits(16);
		sub->hit.right = pixelsToUnits(24);
		sub->hit.bottom = pixelsToUnits(16);
		sub->size = 3;
		sub->bits = npc_invulnerable;
	}
}

//main boss ai
void actBoss_Balfrog(npc *boss)
{
    using namespace balfrogStates;

	int xm = 0;
	int ym = 0;
	uint8_t deg = 0;

	switch (boss->act_no)
	{
	case ini:
		boss->x = tilesToUnits(6);
		boss->y = tilesToUnits(12);
		boss->direct = 2;

		boss->view.left = pixelsToUnits(48);
		boss->view.top = boss->view.left;
		boss->view.right = pixelsToUnits(32);
		boss->view.bottom = pixelsToUnits(16);

		boss->hit_voice = 52;

		boss->hit.left = pixelsToUnits(24);
		boss->hit.top = pixelsToUnits(16);
		boss->hit.right = pixelsToUnits(24);
		boss->hit.bottom = pixelsToUnits(16);

		boss->ani_no = -1;
		boss->size = 3;
		boss->exp = 1;
		boss->code_event = 1000;
		boss->bits = npc_showDamage | npc_eventDie;
		boss->life = 300;
		break;
	case start:
		boss->cond = npccond_alive;
		gBossObj[1].cond = npccond_alive | npccond_dmgboss;
		gBossObj[2].cond = npccond_alive;
		boss->bits |= npc_invulnerable;

		boss->act_no = 11;
		boss->ani_no = 2;

		boss->rect.left = 80;
		boss->rect.top = 48;
		boss->rect.right = 160;
		boss->rect.bottom = 112;

		boss->damage = 5;
		gBossObj[2].damage = 5;

		for (int i = 0; i <= 7; ++i)
			createNpc(NPC_Smoke, boss->x + (random(-12, 12) << 9), boss->y + (random(-12, 12) << 9), random(-341, 341), random(-1536, 0));
		break;
	case ini_flicker:
		boss->act_no = 21;
		boss->act_wait = 0;
		// Fallthrough
	case flicker:
		if (++boss->act_wait / 2 & 1)
			boss->ani_no = 2;
		else
			boss->ani_no = -1;
		break;
	case wait:
		boss->act_no = ini_hop_1;
		boss->act_wait = 0;
		boss->ani_no = 0;
		boss->xm = 0;
		// Fallthrough
	case ini_hop_1:
		if (++boss->act_wait > 50)
		{
			boss->act_no = ini_hop_2;
			boss->ani_wait = 0;
			boss->ani_no = 1;
		}
		break;
	case ini_hop_2:
		if (++boss->ani_wait > 10)
		{
			boss->act_no = hop;
			boss->ani_wait = 0;
			boss->ani_no = 0;
		}
		break;
	case hop:
		if (++boss->ani_wait > 4)
		{
			boss->act_no = midair;
			boss->ani_no = 4;
			playSound(25);

			boss->ym = -1024;
			if (boss->direct)
				boss->xm = 512;
			else
				boss->xm = -512;

			boss->view.top = 0x8000;
			boss->view.bottom = 12288;
		}
		break;
	case midair:
		if (!boss->direct && boss->flag & leftWall)
		{
			boss->direct = 2;
			boss->xm = 512;
		}
		if (boss->direct == 2 && boss->flag & rightWall)
		{
			boss->direct = 0;
			boss->xm = -512;
		}
		if (boss->flag & ground)
		{
			playSound(26, 1);
			gViewport.quake = 30;
			boss->act_no = 100;
			boss->ani_no = 1;
			boss->view.top = pixelsToUnits(48);
			boss->view.bottom = pixelsToUnits(16);
			if (!boss->direct && boss->x < currentPlayer.x)
			{
				boss->direct = 2;
				boss->act_no = 110;
			}
			if (boss->direct == 2 && boss->x > currentPlayer.x)
			{
				boss->direct = 0;
				boss->act_no = 110;
			}
			createNpc(110, random(4, 16) << 13, random(0, 4) << 13, 0, 0, 4);
			for (int i = 0; i <= 3; ++i)
				createNpc(NPC_Smoke, boss->x + (random(-12, 12) << 9), boss->y + boss->hit.bottom, random(-341, 341), random(-1536, 0));
		}
		break;
	case ini_land:
		boss->ani_no = 0;
		boss->act_wait = 0;
		boss->act_no = land;
		// Fallthrough
	case land:
		boss->xm = 8 * boss->xm / 9;
		if (boss->act_wait++ > 50)
		{
			boss->ani_no = 1;
			boss->ani_wait = 0;
			boss->act_no = ini_shoot;
		}
		break;
	case ini_shoot:
		if (++boss->ani_wait > 4)
		{
			boss->act_no = shoot;
			boss->act_wait = 0;
			boss->ani_no = 2;
			boss->count1 = 16;
			gBossObj[1].bits |= npc_shootable;
			boss->tgt_x = boss->life;
		}
		break;
	case shoot:
		if (boss->shock)
		{
			if ((boss->count2++ / 2) & 1)
				boss->ani_no = 3;
			else
				boss->ani_no = 2;
		}
		else
		{
			boss->count2 = 0;
			boss->ani_no = 2;
		}
		boss->xm = 10 * boss->xm / 11;
		if (++boss->act_wait > 16)
		{
			boss->act_wait = 0;
			--boss->count1;
			deg = boss->direct ? getAtan(boss->x + 0x4000 - currentPlayer.x, boss->y - 4096 - currentPlayer.y) :
				getAtan(boss->x - 0x4000 - currentPlayer.x, boss->y - 4096 - currentPlayer.y);
			deg += random(-16, 16);
			ym = getSin(deg);
			xm = getCos(deg);
			if (boss->direct)
				createNpc(NPC_ProjectileBalfrogSpitball, boss->x + 0x4000, boss->y - 4096, xm, ym);
			else
				createNpc(NPC_ProjectileBalfrogSpitball, boss->x - 0x4000, boss->y - 4096, xm, ym);
			playSound(39, 1);
			if (!boss->count1 || boss->life < boss->tgt_x - 90)
			{
				boss->act_no = 114;
				boss->act_wait = 0;
				boss->ani_no = 2;
				boss->ani_wait = 0;

				gBossObj[1].bits &= ~npc_shootable;
			}
		}
		break;
	case 114:
		if (++boss->ani_wait > 10)
		{
			if (++gBossObj[1].count1 <= 2)
			{
				boss->act_no = 100;
			}
			else
			{
				gBossObj[1].count1 = 0;
				boss->act_no = 120;
			}
			boss->ani_wait = 0;
			boss->ani_no = 1;
		}
		break;
	case ini_leap:
		boss->act_no = 121;
		boss->act_wait = 0;
		boss->ani_no = 0;
		boss->xm = 0;
		// Fallthrough
	case ini_leap_2:
		if (++boss->act_wait > 50)
		{
			boss->act_no = 122;
			boss->ani_wait = 0;
			boss->ani_no = 1;
		}
		break;
	case ini_leap_3:
		if (++boss->ani_wait > 20)
		{
			boss->act_no = 123;
			boss->ani_wait = 0;
			boss->ani_no = 0;
		}
		break;
	case leap:
		if (++boss->ani_wait > 4)
		{
			boss->act_no = 124;
			boss->ani_no = 4;
			boss->ym = -2560;
			boss->view.top = 0x8000;
			boss->view.bottom = 12288;
			playSound(25, 1);
		}
		break;
	case leap_midair:
		if (boss->flag & ground)
		{
			playSound(26, 1);
			gViewport.quake = 60;
			boss->act_no = 100;
			boss->ani_no = 1;
			boss->view.top = tilesToUnits(3);
			boss->view.bottom = tilesToUnits(1);
			for (int i = 0; i <= 1; ++i)
				createNpc(NPC_EnemyFrog, random(4, 16) << 13, random(0, 4) << 13, 0, 0, dirAuto);
			for (int i = 0; i <= 5; ++i)
				createNpc(NPC_EnemyPuchi, random(4, 16) << 13, random(0, 4) << 13, 0, 0, dirAuto);
			for (int i = 0; i <= 7; ++i)
				createNpc(NPC_Smoke, boss->x + (random(-12, 12) << 9), boss->y + boss->hit.bottom, random(-341, 341), random(-1536, 0));
			if (!boss->direct && boss->x < currentPlayer.x)
			{
				boss->direct = 2;
				boss->act_no = 110;
			}
			if (boss->direct == 2 && boss->x > currentPlayer.x)
			{
				boss->direct = 0;
				boss->act_no = 110;
			}
		}
		break;
	case die:
		boss->act_no = 131;
		boss->ani_no = 2;
		boss->act_wait = 0;
		boss->xm = 0;
		playSound(72, 1);
		for (int i = 0; i <= 7; ++i)
			createNpc(NPC_Smoke, boss->x + (random(-12, 12) << 9), boss->y + (random(-12, 12) << 9), random(-341, 341), random(-1536, 0));
		gBossObj[1].cond = 0;
		gBossObj[2].cond = 0;
		// Fallthrough
	case die_flashing:
		if (!(++boss->act_wait % 5))
			createNpc(NPC_Smoke, boss->x + (random(-12, 12) << 9), boss->y + (random(-12, 12) << 9), random(-341, 341), random(-1536, 0));
		if (boss->act_wait / 2 & 1)
			boss->x -= 512;
		else
			boss->x += 512;
		if (boss->act_wait > 100)
		{
			boss->act_wait = 0;
			boss->act_no = 132;
		}
		break;
	case revert:
		if (++boss->act_wait / 2 & 1)
		{
			boss->view.left = pixelsToUnits(20);
			boss->view.top = pixelsToUnits(12);
			boss->view.right = pixelsToUnits(20);
			boss->view.bottom = pixelsToUnits(12);
			boss->ani_no = 8;
		}
		else
		{
			boss->view.left = pixelsToUnits(48);
			boss->view.top = boss->view.left;
			boss->view.right = pixelsToUnits(32);
			boss->view.bottom = pixelsToUnits(16);
			boss->ani_no = 2;
		}
		if (!(boss->act_wait % 9))
			createNpc(NPC_Smoke, boss->x + (random(-12, 12) << 9), boss->y + (random(-12, 12) << 9), random(-341, 341), random(-1536, 0));
		if (boss->act_wait > 150)
		{
			boss->act_no = 140;
			boss->hit.bottom = 6144;
		}
		break;
	case nop_start:
		boss->act_no = 141;
		// Fallthrough
	case nop:
		if (boss->flag & 8)
		{
			boss->act_no = 142;
			boss->act_wait = 0;
			boss->ani_no = 5;
		}
		break;
	case go_into_ceilng:
		if (++boss->act_wait > 30)
		{
			boss->ani_no = 6;
			boss->ym = -2560;
			boss->bits |= npc_ignoreSolid;
			boss->act_no = 143;
		}
		break;
	case gone_into_ceiling:
		boss->ym = -2560;
		if (boss->y < 0)
		{
			boss->cond = 0;
			playSound(26, 1);
			gViewport.quake = 30;
		}
		break;
	default:
		break;
	}

	boss->doGravity(0x40, 0x5FF);

	boss->x += boss->xm;
	boss->y += boss->ym;

	//when balrog is a frog
	if (boss->ani_no <= 4)
	{
		boss->rect.left = ((boss->ani_no / 3) * 160) + ((boss->direct / 2) * 80);
		boss->rect.top = 48 + ((boss->ani_no % 3) * 64);
		boss->rect.right = boss->rect.left + 80;
		boss->rect.bottom = boss->rect.top + 64 + ((boss->ani_no / 4) * 24);
	}
	//when balrog reverts and goes into ceiling
	if (boss->ani_no > 4)
	{
		boss->rect.left = 80 + ((boss->ani_no-5)*40);
		boss->rect.top = (boss->direct/2)*24;
		boss->rect.right = boss->rect.left + 40;
		boss->rect.bottom = boss->rect.top + 24;
	}


	if (boss->ani_no == -1)
		boss->rect.right = boss->rect.left;

	balfrogMouth(&gBossObj[1]);
	balfrogOtherHalf(&gBossObj[2]);
}
