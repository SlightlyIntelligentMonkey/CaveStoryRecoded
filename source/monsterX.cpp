#include <string.h>

#include "npc.h"
#include "boss.h"
#include "game.h"
#include "sound.h"
#include "player.h"
#include "mathUtils.h"
#include "flash.h"
#include "stage.h"

void treads(npc *NPC);
void fishMissileGenerator(npc *NPC);
void cover(npc *NPC);
void orb(npc *NPC);
void head(npc *NPC);

enum states
{
	nop = 0,
	ini = 1
};

void actBoss_MonsterX(npc *boss)
{
	switch (boss->act_no)
	{
	case nop:
		boss->life = 1;
		boss->x = -163840;
		break;
	case ini:
		boss->life = 700;
		boss->exp = 1;
		boss->act_no = ini;
		boss->x = 0x100000;
		boss->y = 102400;
		boss->hit_voice = 54;
		boss->hit.left = 12288;
		boss->hit.top = 12288;
		boss->hit.right = 12288;
		boss->hit.bottom = 12288;
		boss->bits = 0x8208;
		boss->size = 3;
		boss->code_event = 1000;
		boss->ani_no = 0;

		//initialize the cover npcs
		gBossObj[1].cond = npccond_alive;
		gBossObj[1].size = 3;
		gBossObj[1].direct = 0;
		gBossObj[1].view.left = 12288;
		gBossObj[1].view.top = 12288;
		gBossObj[1].view.right = 12288;
		gBossObj[1].view.bottom = 12288;
		gBossObj[1].bits = npc_ignoreSolid;

		memcpy(&gBossObj[2], &gBossObj[1], sizeof(npc));
		gBossObj[2].direct = 2;

		//initialize the orb npcs
		gBossObj[3].cond = npccond_alive;
		gBossObj[3].life = 60;
		gBossObj[3].hit_voice = 54;
		gBossObj[3].destroy_voice = 71;
		gBossObj[3].size = 2;
		gBossObj[3].view.left = 4096;
		gBossObj[3].view.top = 4096;
		gBossObj[3].view.right = 4096;
		gBossObj[3].view.bottom = 4096;
		gBossObj[3].hit.left = 2560;
		gBossObj[3].hit.right = 2560;
		gBossObj[3].hit.top = 2560;
		gBossObj[3].hit.bottom = 2560;
		gBossObj[3].bits = npc_ignoreSolid;
		gBossObj[3].tgt_x = 0;

		memcpy(&gBossObj[4], &gBossObj[3], sizeof(npc));
		gBossObj[4].tgt_x = 1;

		memcpy(&gBossObj[5], &gBossObj[3], sizeof(npc));
		gBossObj[5].tgt_x = 2;
		gBossObj[5].life = 100;

		memcpy(&gBossObj[6], &gBossObj[3], sizeof(npc));
		gBossObj[6].tgt_x = 3;
		gBossObj[6].life = 100;

		//initialize the head npc
		gBossObj[7].cond = npccond_alive;
		gBossObj[7].x = 0x100000;
		gBossObj[7].y = 102400;
		gBossObj[7].view.left = 26624;
		gBossObj[7].view.top = 12288;
		gBossObj[7].view.right = 26624;
		gBossObj[7].view.bottom = 12288;
		gBossObj[7].hit_voice = 52;
		gBossObj[7].hit.left = 4096;
		gBossObj[7].hit.top = 12288;
		gBossObj[7].hit.right = 4096;
		gBossObj[7].hit.bottom = tilesToUnits(1);
		gBossObj[7].bits = 8;
		gBossObj[7].size = 3;
		gBossObj[7].ani_no = 0;

		//initialize the tread npcs
		gBossObj[9].cond = npccond_alive;
		gBossObj[9].act_no = 0;
		gBossObj[9].direct = dirUp;
		gBossObj[9].x = 1015808;
		gBossObj[9].y = 73728;
		gBossObj[9].view.left = 18432;
		gBossObj[9].view.top = 4096;
		gBossObj[9].view.right = 18432;
		gBossObj[9].view.bottom = 12288;
		gBossObj[9].hit_voice = 52;
		gBossObj[9].hit.left = 14336;
		gBossObj[9].hit.top = 4096;
		gBossObj[9].hit.right = 14336;
		gBossObj[9].hit.bottom = tilesToUnits(1);
		gBossObj[9].bits = npc_solidSoft | npc_invulnerable | npc_ignoreSolid | npc_rearTop;
		gBossObj[9].size = 3;

		memcpy(&gBossObj[10], &gBossObj[9], sizeof(npc));
		gBossObj[10].x = 1081344;

		memcpy(&gBossObj[11], &gBossObj[9], sizeof(npc));
		gBossObj[11].direct = dirDown;
		gBossObj[11].x = 1015808;
		gBossObj[11].y = 0x20000;
		gBossObj[11].view.top = 12288;
		gBossObj[11].view.bottom = 4096;
		gBossObj[11].hit.top = tilesToUnits(1);
		gBossObj[11].hit.bottom = 4096;

		memcpy(&gBossObj[12], &gBossObj[11], sizeof(npc));
		gBossObj[12].x = 1081344;

		//initialize the fish missile generator npcs
		memcpy(&gBossObj[13], &gBossObj[9], sizeof(npc));
		gBossObj[13].cond = npccond_alive;
		gBossObj[13].view.top = 0x2000;
		gBossObj[13].view.bottom = 0x2000;
		gBossObj[13].view.left = 15360;
		gBossObj[13].view.right = 21504;
		gBossObj[13].count1 = 9;
		gBossObj[13].ani_no = 0;
		gBossObj[13].bits = npc_ignoreSolid;

		memcpy(&gBossObj[14], &gBossObj[13], sizeof(npc));
		gBossObj[14].view.left = 21504;
		gBossObj[14].view.right = 15360;
		gBossObj[14].count1 = 10;
		gBossObj[14].ani_no = 1;
		gBossObj[14].bits = npc_ignoreSolid;

		memcpy(&gBossObj[15], &gBossObj[13], sizeof(npc));
		gBossObj[15].view.top = 0x2000;
		gBossObj[15].view.bottom = 0x2000;
		gBossObj[15].count1 = 11;
		gBossObj[15].ani_no = 2;
		gBossObj[15].bits = npc_ignoreSolid;

		memcpy(&gBossObj[16], &gBossObj[15], sizeof(npc));
		gBossObj[16].view.left = 21504;
		gBossObj[16].view.right = 15360;
		gBossObj[16].count1 = 12;
		gBossObj[16].ani_no = 3;
		gBossObj[16].bits = npc_ignoreSolid;

		boss->act_no = 2;
		break;
	case 10:
		boss->act_no = 11;
		boss->act_wait = 0;
		boss->count1 = 0;
		// Fallthrough
	case 11:
		if (++boss->act_wait > 100)
		{
			boss->act_wait = 0;
			if (boss->x <= currentPlayer.x)
				boss->act_no = 200;
			else
				boss->act_no = 100;
		}
		break;
	case 100:
		boss->act_wait = 0;
		boss->act_no = 101;
		++boss->count1;
		// Fallthrough
	case 101:
		if (++boss->act_wait == 4)
			gBossObj[9].act_no = 100;
		if (boss->act_wait == 8)
			gBossObj[10].act_no = 100;
		if (boss->act_wait == 10)
			gBossObj[11].act_no = 100;
		if (boss->act_wait == 12)
			gBossObj[12].act_no = 100;
		if (boss->act_wait > 120 && boss->count1 > 2)
			boss->act_no = 300;
		if (boss->act_wait > 121 && currentPlayer.x > boss->x)
			boss->act_no = 200;
		break;
	case 200:
		boss->act_wait = 0;
		boss->act_no = 201;
		++boss->count1;
		// Fallthrough
	case 201:
		if (++boss->act_wait == 4)
			gBossObj[9].act_no = 200;
		if (boss->act_wait == 8)
			gBossObj[10].act_no = 200;
		if (boss->act_wait == 10)
			gBossObj[11].act_no = 200;
		if (boss->act_wait == 12)
			gBossObj[12].act_no = 200;
		if (boss->act_wait > 120 && boss->count1 > 2)
			boss->act_no = 400;
		if (boss->act_wait > 121 && currentPlayer.x < boss->x)
			boss->act_no = 100;
		break;
	case 300:
		boss->act_wait = 0;
		boss->act_no = 301;
		// Fallthrough
	case 301:
		if (++boss->act_wait == 4)
			gBossObj[9].act_no = 300;
		if (boss->act_wait == 8)
			gBossObj[10].act_no = 300;
		if (boss->act_wait == 10)
			gBossObj[11].act_no = 300;
		if (boss->act_wait == 12)
			gBossObj[12].act_no = 300;
		if (boss->act_wait > 50)
		{
			if (gBossObj[3].cond || gBossObj[4].cond || gBossObj[5].cond || gBossObj[6].cond)
				boss->act_no = 500;
			else
				boss->act_no = 600;
		}
		break;
	case 400:
		boss->act_wait = 0;
		boss->act_no = 401;
		// Fallthrough
	case 401:
		if (++boss->act_wait == 4)
			gBossObj[9].act_no = 400;
		if (boss->act_wait == 8)
			gBossObj[10].act_no = 400;
		if (boss->act_wait == 10)
			gBossObj[11].act_no = 400;
		if (boss->act_wait == 12)
			gBossObj[12].act_no = 400;
		if (boss->act_wait > 50)
		{
			if (gBossObj[3].cond || gBossObj[4].cond || gBossObj[5].cond || gBossObj[6].cond)
				boss->act_no = 500;
			else
				boss->act_no = 600;
		}
		break;
	case 500:
		boss->act_no = 501;
		boss->act_wait = 0;
		gBossObj[1].act_no = 10;
		gBossObj[2].act_no = 10;
		// Fallthrough
	case 501:
		if (++boss->act_wait > 300)
		{
			boss->act_no = 502;
			boss->act_wait = 0;
		}
		if (!gBossObj[3].cond && !gBossObj[4].cond && !gBossObj[5].cond && !gBossObj[6].cond)
		{
			boss->act_no = 502;
			boss->act_wait = 0;
		}
		break;
	case 502:
		boss->act_no = 503;
		boss->act_wait = 0;
		boss->count1 = 0;
		gBossObj[1].act_no = 20;
		gBossObj[2].act_no = 20;
		// Fallthrough
	case 503:
		if (++boss->act_wait > 50)
		{
			if (boss->x <= currentPlayer.x)
				boss->act_no = 200;
			else
				boss->act_no = 100;
		}
		break;
	case 600:
		boss->act_no = 601;
		boss->act_wait = 0;
		boss->count2 = boss->life;
		gBossObj[1].act_no = 30;
		gBossObj[2].act_no = 30;
		// Fallthrough
	case 601:
		++boss->act_wait;
		if (boss->life < boss->count2 - 200 || boss->act_wait > 300)
		{
			boss->act_no = 602;
			boss->act_wait = 0;
		}
		break;
	case 602:
		boss->act_no = 603;
		boss->act_wait = 0;
		boss->count1 = 0;
		gBossObj[1].act_no = 40;
		gBossObj[2].act_no = 40;
		break;
	case 603:
		if (++boss->act_wait > 50)
		{
			if (boss->x <= currentPlayer.x)
				boss->act_no = 200;
			else
				boss->act_no = 100;
		}
		break;
	case 1000:
		gViewport.quake = 2;
		if (!(++boss->act_wait % 8))
			playSound(SFX_BehemothLargeHurt);
		createSmokeLeft(boss->x + (random(-72, 72) << 9), boss->y + (random(-64, 64) << 9), 1, 1);
		if (boss->act_wait > 100)
		{
			boss->act_wait = 0;
			boss->act_no = 1001;
			setFlash(boss->x, boss->y, flashModes::explosion, 0);
			playSound(SFX_LargeExplosion);
		}
		break;
	case 1001:
		gViewport.quake = 40;
		if (++boss->act_wait > 50)
		{
			for (int i = 0; i < 20; ++i)
				gBossObj[i].cond = 0;
			killNpcsByType(158, 1);
			createNpc(NPC_BossMonsterXDefeated, boss->x, boss->y - 12288);
		}
		break;
	default:
		break;
	}

	//run through sub npc ais
	treads(&gBossObj[9]);
	treads(&gBossObj[10]);
	treads(&gBossObj[11]);
	treads(&gBossObj[12]);
	boss->x += ((gBossObj[12].x + gBossObj[11].x + gBossObj[10].x + gBossObj[9].x) / 4 - boss->x) / 16;

	head(&gBossObj[7]);

	fishMissileGenerator(&gBossObj[13]);
	fishMissileGenerator(&gBossObj[14]);
	fishMissileGenerator(&gBossObj[15]);
	fishMissileGenerator(&gBossObj[16]);

	cover(&gBossObj[1]);
	cover(&gBossObj[2]);

	if (gBossObj[3].cond)
		orb(&gBossObj[3]);
	if (gBossObj[4].cond)
		orb(&gBossObj[4]);
	if (gBossObj[5].cond)
		orb(&gBossObj[5]);
	if (gBossObj[6].cond)
		orb(&gBossObj[6]);

	//check if dead
	if (!boss->life && boss->act_no < 1000)
	{
		boss->act_no = 1000;
		boss->act_wait = 0;
		boss->shock = 0x96;
		gBossObj[9].act_no = 300;
		gBossObj[10].act_no = 300;
		gBossObj[11].act_no = 300;
		gBossObj[12].act_no = 300;
	}
}

void treads(npc *NPC)
{
	constexpr int spriteX[] = { 0, 0, 72, 144, 72, 144 };
	constexpr int spriteY[] = { 0, 32, 0, 0, 32, 32 };

	switch (NPC->act_no)
	{
	case 10:
		NPC->ani_no = 0;
		NPC->bits &= ~npc_bouncy;
		break;
	case 100:
		NPC->bits |= npc_bouncy;
		NPC->act_no = 101;
		NPC->act_wait = 0;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
		// Fallthrough

	case 101:
		if (++NPC->act_wait > 30)
			NPC->act_no = 102;
		if (++NPC->ani_wait > 0)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 3)
			NPC->ani_no = 2;
		NPC->xm -= 32;
		break;
	case 102:
		NPC->bits &= ~npc_bouncy;
		NPC->act_no = 103;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		// Fallthrough

	case 103:
		++NPC->act_wait;
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;
		NPC->xm -= 32;
		break;

	case 200:
		NPC->bits |= npc_bouncy;
		NPC->bits |= npc_rearTop;
		NPC->act_no = 201;
		NPC->act_wait = 0;
		NPC->ani_no = 4;
		NPC->ani_wait = 0;
		// Fallthrough

	case 201:
		if (++NPC->act_wait > 30)
			NPC->act_no = 202;
		if (++NPC->ani_wait > 0)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 5)
			NPC->ani_no = 4;
		NPC->xm += 32;
		// Fallthrough

	case 202:
		NPC->bits &= ~npc_bouncy;
		NPC->act_no = 203;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		break;

	case 203:
		++NPC->act_wait;
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;
		NPC->xm += 32;
		break;

	case 300:
		NPC->act_no = 301;
		NPC->ani_no = 4;
		NPC->ani_wait = 0;
		NPC->bits |= npc_bouncy;
		// Fallthrough

	case 301:
		if (++NPC->ani_wait > 0)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 5)
			NPC->ani_no = 4;
		NPC->xm += 32;
		if (NPC->xm > 0)
		{
			NPC->xm = 0;
			NPC->act_no = 10;
		}
		break;

	case 400:
		NPC->act_no = 401;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
		NPC->bits |= npc_bouncy;
		// Fallthrough

	case 401:
		if (++NPC->ani_wait > 0)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 3)
			NPC->ani_no = 2;
		NPC->xm -= 32;
		if (NPC->xm < 0)
		{
			NPC->xm = 0;
			NPC->act_no = 10;
		}
		break;
	default:
		break;
	}

	if ((NPC->act_no == 101 || NPC->act_no == 201 || NPC->act_no == 301 || NPC->act_no == 401)
		&& NPC->act_wait % 2 == 1)
	{
		playSound(SFX_LargeThud);
	}
	if ((NPC->act_no == 103 || NPC->act_no == 203) && NPC->act_wait % 4 == 1)
		playSound(SFX_Thud);
	if (NPC->act_no < 100 || currentPlayer.y >= NPC->y + 2048 || currentPlayer.y <= NPC->y - 2048)
	{
		NPC->damage = 0;
		NPC->bits &= ~npc_rearTop;
	}
	else
	{
		NPC->damage = 10;
		NPC->bits |= npc_rearTop;
	}
	if (NPC->xm > 1024)
		NPC->xm = 1024;
	if (NPC->xm < -1024)
		NPC->xm = -1024;
	NPC->x += NPC->xm;

	NPC->rect.left = spriteX[NPC->ani_no];
	NPC->rect.top = spriteY[NPC->ani_no] + ((NPC->direct/2)*64);
	NPC->rect.right = NPC->rect.left + 72;
	NPC->rect.bottom = NPC->rect.top + 32;
}

void fishMissileGenerator(npc *NPC)
{
	int xp = 0;
	int yp = 0;

	if (NPC->act_no == 10)
	{
		NPC->act_no = 11;
		NPC->act_wait = 30 * NPC->ani_no + 30;
	}
	else if (NPC->act_no != 11)
	{
		goto LABEL_13;
	}
	if (NPC->act_wait)
	{
		--NPC->act_wait;
	}
	else
	{
		switch (NPC->ani_no)
		{
		case 0:
			xp = -15360;
			yp = 3072;
			break;
		case 1:
			xp = 15360;
			yp = 3072;
			break;
		case 2:
			xp = -15360;
			yp = -3072;
			break;
		case 3:
			xp = 15360;
			yp = -3072;
			break;
		default:
			break;
		}
		createNpc(NPC_ProjectileMonsterX, xp + NPC->x, yp + NPC->y, 0, 0, NPC->direct);
		playSound(SFX_EnemyShootProjectile);
		NPC->act_wait = 120;
	}

LABEL_13:
	NPC->x = (gBossObj[NPC->count1].x + gBossObj[0].x) / 2;
	NPC->y = (gBossObj[NPC->count1].y + gBossObj[0].y) / 2;

	NPC->rect.left = (NPC->ani_no % 2) * 72;
	NPC->rect.top = 128 + ((NPC->ani_no / 2) * 32);
	NPC->rect.right = NPC->rect.left + 72;
	NPC->rect.bottom = NPC->rect.top + 32;
}

void cover(npc *NPC)
{
	switch (NPC->act_no)
	{
	case 0xA:
		NPC->tgt_x += 512;
		if (NPC->tgt_x > 0x4000)
		{
			NPC->tgt_x = 0x4000;
			NPC->act_no = 0;
			gBossObj[3].act_no = 10;
			gBossObj[4].act_no = 10;
			gBossObj[5].act_no = 10;
			gBossObj[6].act_no = 10;
		}
		break;
	case 0x14:
		NPC->tgt_x -= 512;
		if (NPC->tgt_x < 0)
		{
			NPC->tgt_x = 0;
			NPC->act_no = 0;
			gBossObj[3].act_no = 0;
			gBossObj[4].act_no = 0;
			gBossObj[5].act_no = 0;
			gBossObj[6].act_no = 0;
		}
		break;
	case 0x1E:
		NPC->tgt_x += 512;
		if (NPC->tgt_x > 10240)
		{
			NPC->tgt_x = 10240;
			NPC->act_no = 0;
			gBossObj[7].act_no = 10;
			gBossObj[13].act_no = 10;
			gBossObj[14].act_no = 10;
			gBossObj[15].act_no = 10;
			gBossObj[16].act_no = 10;
		}
		break;
	case 0x28:
		NPC->tgt_x -= 512;
		if (NPC->tgt_x < 0)
		{
			NPC->tgt_x = 0;
			NPC->act_no = 0;
			gBossObj[7].act_no = 0;
			gBossObj[13].act_no = 0;
			gBossObj[14].act_no = 0;
			gBossObj[15].act_no = 0;
			gBossObj[16].act_no = 0;
		}
		break;
	default:
		break;
	}
	if (NPC->direct)
	{
		NPC->rect.left = 264;
		NPC->rect.top = 96;
		NPC->rect.right = 312;
		NPC->rect.bottom = 144;
		NPC->x = gBossObj[0].x + NPC->tgt_x + 12288;
	}
	else
	{
		NPC->rect.left = 216;
		NPC->rect.top = 96;
		NPC->rect.right = 264;
		NPC->rect.bottom = 144;
		NPC->x = gBossObj[0].x - 12288 - NPC->tgt_x;
	}
	NPC->y = gBossObj[0].y;
}

void orb(npc *NPC)
{
	int yVel;
	int xVel;
	int temp;

	switch (NPC->act_no)
	{
	case 0:
		NPC->bits &= ~npc_shootable;
		NPC->ani_no = 0;
		goto LABEL_14;
	case 10:
		NPC->act_no = 11;
		NPC->act_wait = 10 * NPC->tgt_x + 40;
		NPC->bits |= npc_shootable;
		break;
	case 11:
		break;
	default:
		goto LABEL_14;
	}

	NPC->ani_no = NPC->act_wait < 16 && NPC->act_wait / 2 % 2;

	if (NPC->act_wait)
	{
		--NPC->act_wait;
	}
	else
	{
		temp = getAtan(NPC->x - currentPlayer.x, NPC->y - currentPlayer.y);
		temp += random(-2, 2);
		yVel = 3 * getSin(temp);
		xVel = 3 * getCos(temp);
		createNpc(NPC_ProjectileGaudiFlying, NPC->x, NPC->y, xVel, yVel);
		playSound(SFX_EnemyShootProjectile);
		NPC->act_wait = 40;
	}
LABEL_14:
	switch (NPC->tgt_x)
	{
	case 0:
		NPC->x = gBossObj[0].x - 11264;
		NPC->y = gBossObj[0].y - 0x2000;
		break;
	case 1:
		NPC->x = gBossObj[0].x + 14336;
		NPC->y = gBossObj[0].y - 0x2000;
		break;
	case 2:
		NPC->x = gBossObj[0].x - 7680;
		NPC->y = gBossObj[0].y + 7168;
		break;
	case 3:
		NPC->x = gBossObj[0].x + 8704;
		NPC->y = gBossObj[0].y + 7168;
		break;
	default:
		break;
	}

	NPC->rect.left = NPC->tgt_x * 16;
	NPC->rect.top = 192 + (NPC->ani_no * 16);
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 16;
}


void head(npc *NPC)
{
	switch (NPC->act_no)
	{
	case 0:
		gBossObj[0].bits &= ~npc_shootable;
		NPC->ani_no = 0;
		break;
	case 10:
		NPC->act_no = 11;
		NPC->act_wait = 10 * NPC->tgt_x + 40;
		gBossObj[0].bits |= npc_shootable;
		break;
	case 11:
		break;
	}

	if (gBossObj[0].shock)
	{
		if (gBossObj[0].shock / 2 % 2)
			NPC->ani_no = 1;
		else
			NPC->ani_no = 0;
	}
	else
	{
		NPC->ani_no = 0;
	}


	NPC->x = gBossObj[0].x;
	NPC->y = gBossObj[0].y;

	if (gBossObj[0].act_no <= 10)
		NPC->ani_no = 3;

	NPC->rect.left = 216;
	NPC->rect.top = NPC->ani_no*48;
	NPC->rect.right = NPC->rect.left + 104;
	NPC->rect.bottom = NPC->rect.top + 48;
}
