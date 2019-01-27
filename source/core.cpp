#include <string.h>

#include "boss.h"
#include "npc.h"
#include "player.h"
#include "sound.h"
#include "stage.h"
#include "mathUtils.h"
#include "game.h"
#include "render.h"

enum bossNpcSlots
{
	core = 0,
	face = 4,
	tail = 5,
	hit1 = 6,
	hit2 = 7,
	hit3 = 8,
	hit4 = 9,
	mini1 = 10,
	mini2 = 11,
	mini3 = 12,
	mini4 = 13,
	mini5 = 14
};

void coreFace(npc *NPC);
void coreTail(npc *NPC);
void miniCore(npc *NPC);
void coreHit(npc *NPC);

enum coreStates
{
	ini = 0,
	iniShootWisp = 210,
	shootWisp = 211,
	iniShootBigAssEnergyBall = 220,
	shootBigAssEnergyBall = 221,
	iniDeathAnimation = 500,
	deathAnimation = 501
};

void actBoss_Core(npc *boss)
{
	static int flash = 0;
	int deg;
	bool bShock = false;

	switch (boss->act_no)
	{
		// -- sets values for boss npcs -- //
	case 0:
		boss->act_no = 10;
		boss->exp = 1;
		boss->cond = npccond_alive;
		boss->bits = npc_showDamage | npc_ignoreSolid | npc_invulnerable;
		boss->life = 650;
		boss->hit_voice = 114;
		boss->x = tilesToUnits(77);
		boss->y = tilesToUnits(14);
		boss->xm = 0;
		boss->ym = 0;
		boss->code_event = 1000;
		boss->bits |= npc_eventDie;

		// initializes face and tail
		gBossObj[face].cond = npccond_alive;
		gBossObj[face].act_no = 10;
		gBossObj[tail].cond = npccond_alive;
		gBossObj[tail].act_no = 10;

		//initializes core hitboxes
		gBossObj[hit1].cond = npccond_alive;
		gBossObj[hit1].bits = npc_ignoreSolid | npc_invulnerable;
		gBossObj[hit1].view.left = 0;
		gBossObj[hit1].view.top = 0;
		gBossObj[hit1].hit.right = pixelsToUnits(40);
		gBossObj[hit1].hit.top = pixelsToUnits(16);
		gBossObj[hit1].hit.bottom = pixelsToUnits(16);
		gBossObj[hit1].count1 = 0;

		memcpy(&gBossObj[hit2], &gBossObj[hit1], sizeof(npc));
		gBossObj[hit2].hit.right = pixelsToUnits(36);
		gBossObj[hit2].hit.top = pixelsToUnits(24);
		gBossObj[hit2].hit.bottom = gBossObj[hit2].hit.top;
		gBossObj[hit2].count1 = 1;

		memcpy(&gBossObj[hit3], &gBossObj[hit1], sizeof(npc));
		gBossObj[hit3].hit.right = pixelsToUnits(44);
		gBossObj[hit3].hit.top = pixelsToUnits(8);
		gBossObj[hit3].hit.bottom = gBossObj[hit3].hit.top;
		gBossObj[hit3].count1 = 2;

		memcpy(&gBossObj[hit4], &gBossObj[hit1], sizeof(npc));
		gBossObj[hit4].cond |= npc_bouncy;
		gBossObj[hit4].hit.right = pixelsToUnits(20);
		gBossObj[hit4].hit.top = pixelsToUnits(20);
		gBossObj[hit4].hit.bottom = pixelsToUnits(20);
		gBossObj[hit4].count1 = 3;

		// initializes mini cores
		gBossObj[mini1].cond = npccond_alive;
		gBossObj[mini1].act_no = 10;
		gBossObj[mini1].bits = npc_invulnerable | npc_ignoreSolid | npc_shootable;
		gBossObj[mini1].life = 1000;
		gBossObj[mini1].hit_voice = 54;
		gBossObj[mini1].hit.right = pixelsToUnits(24);
		gBossObj[mini1].hit.top = pixelsToUnits(16);
		gBossObj[mini1].hit.bottom = pixelsToUnits(16);
		gBossObj[mini1].view.left = pixelsToUnits(32);
		gBossObj[mini1].view.top = pixelsToUnits(20);
		gBossObj[mini1].x = boss->x - pixelsToUnits(8);
		gBossObj[mini1].y = boss->y - pixelsToUnits(64);

		memcpy(&gBossObj[mini2], &gBossObj[mini1], sizeof(npc));
		gBossObj[mini2].x = boss->x + pixelsToUnits(16);
		gBossObj[mini2].y = boss->y;

		memcpy(&gBossObj[mini3], &gBossObj[mini1], sizeof(npc));
		gBossObj[mini3].x = boss->x - pixelsToUnits(8);
		gBossObj[mini3].y = boss->y + pixelsToUnits(64);

		memcpy(&gBossObj[mini4], &gBossObj[mini1], sizeof(npc));
		gBossObj[mini4].x = boss->x - pixelsToUnits(48);
		gBossObj[mini4].y = boss->y - pixelsToUnits(32);

		memcpy(&gBossObj[mini5], &gBossObj[mini1], sizeof(npc));
		gBossObj[mini5].x = boss->x - pixelsToUnits(48);
		gBossObj[mini5].y = boss->y + pixelsToUnits(32);
		break;
	case 200:
		boss->act_no = 201;
		boss->act_wait = 0;
		gBossObj[hit4].bits &= ~npc_shootable;
		gSuperYPos = 0;
		//CutNoise();
		// Fallthrough
	case 201:
		boss->tgt_x = gCurrentPlayer.x;
		boss->tgt_y = gCurrentPlayer.y;
		if (++boss->act_wait > 400)
		{
			++boss->count1;
			playSound(SFX_CoreThrust);
			if (boss->count1 <= 3)
			{
				boss->act_no = iniShootWisp;
			}
			else
			{
				boss->count1 = 0;
				boss->act_no = iniShootBigAssEnergyBall;
			}
			gBossObj[face].ani_no = 0;
			gBossObj[tail].ani_no = 0;
			bShock = 1;
		}
		break;
	case iniShootWisp:
		boss->act_no = shootWisp;
		boss->act_wait = 0;
		boss->count2 = boss->life;
		gBossObj[hit4].bits |= npc_shootable;
		// Fallthrough
	case shootWisp:
		boss->tgt_x = gCurrentPlayer.x;
		boss->tgt_y = gCurrentPlayer.y;
		if (boss->shock)
		{
			if ((++flash >> 1) & 1)
			{
				gBossObj[face].ani_no = 0;
				gBossObj[tail].ani_no = 0;
			}
			else
			{
				gBossObj[face].ani_no = 1;
				gBossObj[tail].ani_no = 1;
			}
		}
		else
		{
			gBossObj[face].ani_no = 0;
			gBossObj[tail].ani_no = 0;
		}
		if (++boss->act_wait % 100 == 1)
		{
			gCurlyShootWait = random(80, 100);
			gCurlyShootX = gBossObj[hit4].x;
			gCurlyShootY = gBossObj[hit4].y;
		}
		if (boss->act_wait <= 199 && boss->act_wait % 20 == 1)
		{
			createNpc(NPC_ProjectileCoreWisp, boss->x + (random(-48, -16) << 9),
				boss->y + (random(-64, 64) << 9), 0, 0, 0, nullptr, false);
		}
		if (boss->act_wait > 400 || boss->life < boss->count2 - 200)
		{
			boss->act_no = 200;
			gBossObj[face].ani_no = 2;
			gBossObj[tail].ani_no = 0;
			bShock = 1;
		}
		break;
	case iniShootBigAssEnergyBall:
		boss->act_no = shootBigAssEnergyBall;
		boss->act_wait = 0;
		gSuperYPos = 1;
		gBossObj[hit4].bits |= npc_shootable;
		gViewport.quake = 100;
		//SetNoise(1, 1000);
		// Fallthrough
	case shootBigAssEnergyBall:
		++boss->act_wait;
		createNpc(NPC_UnderwaterCurrent, gCurrentPlayer.x + (random(-50, 150) << 10),
			gCurrentPlayer.y + (random(-160, 160) << 9), 0, 0, 0, nullptr, false);
		gCurrentPlayer.xm -= 32;
		gCurrentPlayer.cond |= npc_shootable;

		if (boss->shock)
		{
			if ((++flash >> 1) & 1)
			{
				gBossObj[face].ani_no = 0;
				gBossObj[tail].ani_no = 0;
			}
			else
			{
				gBossObj[face].ani_no = 1;
				gBossObj[tail].ani_no = 1;
			}
		}
		else
		{
			gBossObj[face].ani_no = 0;
			gBossObj[tail].ani_no = 0;
		}
		//shoots massive balls at a certain interval
		if (boss->act_wait == 300 || boss->act_wait == 350 || boss->act_wait == 400)
		{
			deg = getAtan(boss->x - gCurrentPlayer.x, boss->y - gCurrentPlayer.y);
			createNpc(NPC_ProjectileCoreLargeEnergyBall,
				boss->x - 20480, boss->y, 3 * getCos(deg), 3 * getSin(deg));
			playSound(SFX_Lightning);
		}
		if (boss->act_wait > 400)
		{
			boss->act_no = 200;
			gBossObj[face].ani_no = 2;
			gBossObj[tail].ani_no = 0;
			bShock = 1;
		}
		break;
	case iniDeathAnimation:
		//CutNoise();
		boss->act_no = deathAnimation;
		boss->act_wait = 0;
		boss->xm = 0;
		boss->ym = 0;

		gBossObj[face].ani_no = 2;
		gBossObj[tail].ani_no = 0;

		gBossObj[mini1].act_no = 200;
		gBossObj[mini2].act_no = 200;
		gBossObj[mini3].act_no = 200;
		gBossObj[mini4].act_no = 200;
		gBossObj[mini5].act_no = 200;
		gViewport.quake = 20;
		for (int i = 0; i <= 31; ++i)
		{
			createNpc(NPC_Smoke, boss->x + (random(-128, 128) << 9), boss->y + (random(-64, 64) << 9),
				random(-128, 128) << 9, random(-128, 128) << 9, 0, nullptr, false);
		}
		for (int i = 0; i <= 11; ++i)
			gBossObj[i].bits &= ~(npc_invulnerable | npc_shootable);
		// Fallthrough
	case deathAnimation:
		if (++boss->act_wait & 0xF)
		{
			createNpc(NPC_Smoke, boss->x + (random(-64, 64) << 9), boss->y + (random(-32, 32) << 9),
				random(-128, 128) << 9, random(-128, 128) << 9, 0, nullptr, false);
		}

		if (boss->act_wait / 2 & 1)
			boss->x -= 512;
		else
			boss->x += 512;

		if (boss->x > tilesToUnits(63))
			boss->x -= 128;
		else
			boss->x += 128;

		if (boss->y > tilesToUnits(11))
			boss->y -= 128;
		else
			boss->y += 128;
		break;
	case 600:
		boss->act_no = 601;
		gBossObj[face].act_no = 50;
		gBossObj[tail].act_no = 50;
		gBossObj[hit1].bits &= ~npc_invulnerable;
		gBossObj[hit2].bits &= ~npc_invulnerable;
		gBossObj[hit3].bits &= ~npc_invulnerable;
		gBossObj[hit4].bits &= ~npc_invulnerable;
		break;
	case 601:
		break;
	default:
		break;
	}

	if (bShock)
	{
		gViewport.quake = 20;
		gBossObj[mini1].act_no = 100;
		gBossObj[mini2].act_no = 100;
		gBossObj[mini3].act_no = 100;
		gBossObj[mini4].act_no = 100;
		gBossObj[mini5].act_no = 100;
		playSound(26);
		for (int i = 0; i <= 7; ++i)
		{
			createNpc(NPC_Smoke, gBossObj[face].x + (random(-32, 16) << 9), gBossObj[face].y,
				random(-512, 512), random(-256, 256), 0, nullptr, false);
		}
	}
	if (boss->act_no >= 200 && boss->act_no < 300)
	{
		switch (boss->act_wait)
		{
		case 80:
			gBossObj[mini1].act_no = 120;
			break;
		case 110:
			gBossObj[mini2].act_no = 120;
			break;
		case 140:
			gBossObj[mini3].act_no = 120;
			break;
		case 170:
			gBossObj[mini4].act_no = 120;
			break;
		case 200:
			gBossObj[mini5].act_no = 120;
			break;
		}
		if (boss->x < boss->tgt_x + tilesToUnits(10))
			boss->xm += 4;
		if (boss->x > boss->tgt_x + tilesToUnits(10))
			boss->xm -= 4;
		if (boss->y < boss->tgt_y)
			boss->ym += 4;
		if (boss->y > boss->tgt_y)
			boss->ym -= 4;
	}

	if (boss->xm > 128)
		boss->xm = 128;
	if (boss->xm < -128)
		boss->xm = -128;
	if (boss->ym > 128)
		boss->ym = 128;
	if (boss->ym < -128)
		boss->ym = -128;
	boss->x += boss->xm;
	boss->y += boss->ym;

	coreFace(&gBossObj[face]);
	coreTail(&gBossObj[tail]);

	miniCore(&gBossObj[mini1]);
	miniCore(&gBossObj[mini2]);
	miniCore(&gBossObj[mini3]);
	miniCore(&gBossObj[mini4]);
	miniCore(&gBossObj[mini5]);

	coreHit(&gBossObj[hit1]);
	coreHit(&gBossObj[hit2]);
	coreHit(&gBossObj[hit3]);
	coreHit(&gBossObj[hit4]);
	return;
}

void coreFace(npc *NPC)
{
	RECT rect[4];
	rect[0] = { 0,   0,  72, 112 };
	rect[1] = { 0, 112,  72, 224 };
	rect[2] = { 160, 0, 232, 112 };
	rect[3] = { 0,   0,   0,   0 };

	switch (NPC->act_no)
	{
	case 10:
		NPC->act_no = 11;
		NPC->ani_no = 2;
		NPC->bits = npc_ignoreSolid;
		NPC->view.left = 18432;
		NPC->view.top = 28672;
		// Fallthrough
	case 11:
		NPC->x = gBossObj[core].x - 18432;
		NPC->y = gBossObj[core].y;
		break;
	case 50:
		NPC->act_no = 51;
		NPC->act_wait = 112;
		// Fallthrough
	case 51:
		if (!--NPC->act_wait)
		{
			NPC->act_no = 100;
			NPC->ani_no = 3;
		}
		NPC->x = gBossObj[core].x - 18432;
		NPC->y = gBossObj[core].y;
		break;
	case 100:
		NPC->ani_no = 3;
		break;
	default:
		break;
	}

	NPC->rect = rect[NPC->ani_no];
	if (NPC->act_no == 51)
		NPC->rect.bottom = NPC->act_wait + NPC->rect.top;
}

void coreTail(npc *NPC)
{
	RECT rect[3];
	rect[0] = { 72, 0, 160, 112 };
	rect[1] = { 72, 112, 160, 224 };
	rect[2] = { 0, 0, 0, 0 };

	switch (NPC->act_no)
	{
	case 10:
		NPC->act_no = 11;
		NPC->ani_no = 0;
		NPC->bits = 8;
		NPC->view.left = 22528;
		NPC->view.top = 28672;
		// Fallthrough
	case 11:
		NPC->x = gBossObj[core].x + 22528;
		NPC->y = gBossObj[core].y;
		break;
	case 50:
		NPC->act_no = 51;
		NPC->act_wait = 112;
		// Fallthrough
	case 51:
		if (!--NPC->act_wait)
		{
			NPC->act_no = 100;
			NPC->ani_no = 2;
		}
		NPC->x = gBossObj[core].x + 22528;
		NPC->y = gBossObj[core].y;
		break;
	case 100:
		NPC->ani_no = 2;
		break;
	}

	NPC->rect = rect[NPC->ani_no];

	if (NPC->act_no == 51)
		NPC->rect.bottom = NPC->act_wait + NPC->rect.top;
}

void miniCore(npc *NPC)
{
	int deg;

	RECT rect[3];
	rect[0] = { 256, 0, 320, 40 };
	rect[1] = { 256, 40, 320, 80 };
	rect[2] = { 256, 80, 320, 120 };

	NPC->life = 1000;
	switch (NPC->act_no)
	{
	case 10:
		NPC->ani_no = 2;
		NPC->bits &= ~npc_shootable;
		break;
	case 100:
		NPC->act_no = 101;
		NPC->ani_no = 2;
		NPC->act_wait = 0;
		NPC->tgt_x = gBossObj[core].x + (random(-128, 32) << 9);
		NPC->tgt_y = gBossObj[core].y + (random(-64, 64) << 9);
		NPC->bits |= npc_shootable;
		// Fallthrough
	case 101:
		NPC->x += (NPC->tgt_x - NPC->x) / 16;
		NPC->y += (NPC->tgt_y - NPC->y) / 16;
		if (++NPC->act_wait > 50)
			NPC->ani_no = 0;
		break;
	case 120:
		NPC->act_no = 121;
		NPC->act_wait = 0;
		// Fallthrough
	case 121:
		if (++NPC->act_wait / 2 % 2)
			NPC->ani_no = 0;
		else
			NPC->ani_no = 1;
		if (NPC->act_wait > 20)
			NPC->act_no = 130;
		break;
	case 130:
		NPC->act_no = 131;
		NPC->ani_no = 2;
		NPC->act_wait = 0;
		NPC->tgt_x = NPC->x + (random(24, 48) << 9);
		NPC->tgt_y = NPC->y + (random(-4, 4) << 9);
		// Fallthrough
	case 131:
		NPC->x += (NPC->tgt_x - NPC->x) / 16;
		NPC->y += (NPC->tgt_y - NPC->y) / 16;
		if (++NPC->act_wait > 50)
		{
			NPC->act_no = 140;
			NPC->ani_no = 0;
		}
		if (NPC->act_wait == 1 || NPC->act_wait == 3)
		{
			deg = getAtan(NPC->x - gCurrentPlayer.x, NPC->y - gCurrentPlayer.y);
			deg += random(-2, 2);
			createNpc(NPC_ProjectileCoreSpinner, NPC->x, NPC->y,
				2 * getCos(deg), 2 * getSin(deg), 0, nullptr, false);
			playSound(SFX_EnemyShootProjectile);
		}
		break;
	case 140:
		NPC->x += (NPC->tgt_x - NPC->x) / 16;
		NPC->y += (NPC->tgt_y - NPC->y) / 16;
		break;
	case 200:
		NPC->act_no = 201;
		NPC->ani_no = 2;
		NPC->xm = 0;
		NPC->ym = 0;
		// Fallthrough
	case 201:
		NPC->xm += 32;
		NPC->x += NPC->xm;
		if (NPC->x > (gMap.width << 13) + 0x4000)
			NPC->cond = 0;
		break;
	default:
		break;
	}
	if (NPC->shock)
		NPC->tgt_x += 1024;

	NPC->rect = rect[NPC->ani_no];
}

void coreHit(npc *NPC)
{
	switch (NPC->count1)
	{
	case 0:
		NPC->x = gBossObj[core].x;
		NPC->y = gBossObj[core].y - pixelsToUnits(32);
		break;
	case 1:
		NPC->x = gBossObj[core].x + pixelsToUnits(28);
		NPC->y = gBossObj[core].y;
		break;
	case 2:
		NPC->x = gBossObj[core].x + pixelsToUnits(4);
		NPC->y = gBossObj[core].y + pixelsToUnits(32);
		break;
	case 3:
		NPC->x = gBossObj[core].x - pixelsToUnits(28);
		NPC->y = gBossObj[core].y + pixelsToUnits(4);
		break;
	default:
		break;
	}
}
