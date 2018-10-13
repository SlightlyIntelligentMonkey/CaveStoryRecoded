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

};

void actBoss_Core(npc *boss)
{
	static int flash = 0;
	int deg;
	bool bShock = false;

	switch (boss->act_no)
	{
	case 0:

		break;
	case 200:
		boss->act_no = 201;
		boss->act_wait = 0;
		bossObj[hit4].bits &= ~npc_shootable;
		superYPos = 0;
		//CutNoise();
	case 201:
		break;
	case 210:
		boss->act_no = 211;
		boss->act_wait = 0;
		boss->count2 = boss->life;
		bossObj[hit4].bits |= npc_shootable;
	case 211:
		break;
	case 220:
		boss->act_no = 221;
		boss->act_wait = 0;
		superYPos = 1;
		bossObj[hit4].bits |= npc_shootable;
		viewport.quake = 100;
		//SetNoise(1, 1000);
	case 221:
		++boss->act_wait;
		createNpc(NPC_UnderwaterCurrent, currentPlayer.x + (random(-50, 150) << 10),
			currentPlayer.y + (random(-160, 160) << 9), 0, 0, 0, 0, false);
		currentPlayer.xm -= 32;
		currentPlayer.cond |= npc_shootable;

		if (boss->shock)
		{
			if ((++flash >> 1) & 1)
			{
				bossObj[face].ani_no = 0;
				bossObj[tail].ani_no = 0;
			}
			else
			{
				bossObj[face].ani_no = 1;
				bossObj[tail].ani_no = 1;
			}
		}
		else
		{
			bossObj[face].ani_no = 0;
			bossObj[tail].ani_no = 0;
		}
		//shoots massive balls at a certain interval
		if (boss->act_wait == 300 || boss->act_wait == 350 || boss->act_wait == 400)
		{
			deg = getAtan(boss->x - currentPlayer.x, boss->y - currentPlayer.y);
			createNpc(NPC_ProjectileCoreLargeEnergyBall, 
				boss->x - 20480, boss->y, 3 * getCos(deg), 3 * getSin(deg), 0, 0, 256);
			playSound(SFX_Lightning);
		}
		if (boss->act_wait > 400)
		{
			boss->act_no = 200;
			bossObj[face].ani_no = 2;
			bossObj[tail].ani_no = 0;
			bShock = true;
		}
		break;
	case 500:
		//CutNoise();
		boss->act_no = 501;
		boss->act_wait = 0;
		boss->xm = 0;
		boss->ym = 0;

		bossObj[face].ani_no = 2;
		bossObj[tail].ani_no = 0;

		bossObj[mini1].act_no = 200;
		bossObj[mini2].act_no = 200;
		bossObj[mini3].act_no = 200;
		bossObj[mini4].act_no = 200;
		bossObj[mini5].act_no = 200;
		viewport.quake = 20;
	case 501:
		if (++boss->act_wait & 0xF)
		{
			createNpc(NPC_Smoke, boss->x + (random(-64, 64) << 9), boss->y + (random(-32, 32) << 9), 
				random(-128, 128) << 9, random(-128, 128) << 9, 0, 0, false);
		}

		if (boss->act_wait / 2 & 1)
			boss->x -= 512;
		else
			boss->x += 512;

		if (boss->x > 516095)
			boss->x -= 128;
		else
			boss->x += 128;

		if (boss->y > 90111)
			boss->y -= 128;
		else
			boss->y += 128;
		break;
	case 600:
		boss->act_no = 601;
		bossObj[face].act_no = 50;
		bossObj[tail].act_no = 50;
		bossObj[hit1].bits &= ~npc_invulnerable;
		bossObj[hit2].bits &= ~npc_invulnerable;
		bossObj[hit3].bits &= ~npc_invulnerable;
		bossObj[hit4].bits &= ~npc_invulnerable;
		break;
	case 601:
		break;
	default:
		break;
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

	coreFace(&bossObj[face]);
	coreTail(&bossObj[tail]);

	miniCore(&bossObj[mini1]);
	miniCore(&bossObj[mini2]);
	miniCore(&bossObj[mini3]);
	miniCore(&bossObj[mini4]);
	miniCore(&bossObj[mini5]);

	coreHit(&bossObj[hit1]);
	coreHit(&bossObj[hit2]);
	coreHit(&bossObj[hit3]);
	coreHit(&bossObj[hit4]);
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
	case 11:
		NPC->x = bossObj[core].x - 18432;
		NPC->y = bossObj[core].y;
		break;
	case 50:
		NPC->act_no = 51;
		NPC->act_wait = 112;
	case 51:
		if (!--NPC->act_wait)
		{
			NPC->act_no = 100;
			NPC->ani_no = 3;
		}
		NPC->x = bossObj[core].x - 18432;
		NPC->y = bossObj[core].y;
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
	case 11:
		NPC->x = bossObj[core].x + 22528;
		NPC->y = bossObj[core].y;
		break;
	case 50:
		NPC->act_no = 51;
		NPC->act_wait = 112;
	case 51:
		if (!--NPC->act_wait)
		{
			NPC->act_no = 100;
			NPC->ani_no = 2;
		}
		NPC->x = bossObj[core].x + 22528;
		NPC->y = bossObj[core].y;
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
		NPC->bits &= 0xFFDFu;
		break;
	case 100:
		NPC->act_no = 101;
		NPC->ani_no = 2;
		NPC->act_wait = 0;
		NPC->tgt_x = bossObj[core].x + (random(-128, 32) << 9);
		NPC->tgt_y = bossObj[core].y + (random(-64, 64) << 9);
		NPC->bits |= npc_shootable;
	case 101:
		NPC->x += (NPC->tgt_x - NPC->x) / 16;
		NPC->y += (NPC->tgt_y - NPC->y) / 16;
		if (++NPC->act_wait > 50)
			NPC->ani_no = 0;
		break;
	case 120:
		NPC->act_no = 121;
		NPC->act_wait = 0;
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
			deg = getAtan(NPC->x - currentPlayer.x, NPC->y - currentPlayer.y);
			deg += random(-2, 2);
			createNpc(NPC_ProjectileCoreSpinner, NPC->x, NPC->y,
				2 * getCos(deg), 2 * getSin(deg), 0, 0, false);
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
	case 201:
		NPC->xm += 32;
		NPC->x += NPC->xm;
		if (NPC->x > (map.width << 13) + 0x4000)
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
		NPC->x = bossObj[core].x;
		NPC->y = bossObj[core].y - pixelsToUnits(32);
		break;
	case 1:
		NPC->x = bossObj[core].x + pixelsToUnits(28);
		NPC->y = bossObj[core].y;
		break;
	case 2:
		NPC->x = bossObj[core].x + pixelsToUnits(4);
		NPC->y = bossObj[core].y + pixelsToUnits(32);
		break;
	case 3:
		NPC->x = bossObj[core].x - pixelsToUnits(28);
		NPC->y = bossObj[core].y + pixelsToUnits(4);
		break;
	default:
		break;
	}
}