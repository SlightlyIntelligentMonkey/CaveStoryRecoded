#include "npc.h"
#include "boss.h"
#include "game.h"
#include "sound.h"
#include "player.h"
#include "mathUtils.h"
#include "flash.h"

void monsterXSubNpcTreads(npc *NPC);
void monsterXSubNpcFishMissileGenerator(npc *NPC);
void monsterXSubNpcCover(npc *NPC);
void monsterXSubNpcOrb(npc *NPC);
void monsterXSubNpcHead(npc *NPC);

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
		boss->bits = -32248;
		boss->size = 3;
		boss->code_event = 1000;
		boss->ani_no = 0;

		//initialize the cover npcs
		bossObj[1].cond = npccond_alive;
		bossObj[1].size = 3;
		bossObj[1].direct = 0;
		bossObj[1].view.left = 12288;
		bossObj[1].view.top = 12288;
		bossObj[1].view.right = 12288;
		bossObj[1].view.bottom = 12288;
		bossObj[1].bits = npc_ignoreSolid;

		memcpy(&bossObj[2], &bossObj[1], sizeof(npc));
		bossObj[2].direct = 2;

		//initialize the orb npcs
		bossObj[3].cond = npccond_alive;
		bossObj[3].life = 60;
		bossObj[3].hit_voice = 54;
		bossObj[3].destroy_voice = 71;
		bossObj[3].size = 2;
		bossObj[3].view.left = 4096;
		bossObj[3].view.top = 4096;
		bossObj[3].view.right = 4096;
		bossObj[3].view.bottom = 4096;
		bossObj[3].hit.left = 2560;
		bossObj[3].hit.right = 2560;
		bossObj[3].hit.top = 2560;
		bossObj[3].hit.bottom = 2560;
		bossObj[3].bits = npc_ignoreSolid;
		bossObj[3].tgt_x = 0;

		memcpy(&bossObj[4], &bossObj[3], sizeof(npc));
		bossObj[4].tgt_x = 1;

		memcpy(&bossObj[5], &bossObj[3], sizeof(npc));
		bossObj[5].tgt_x = 2;
		bossObj[5].life = 100;

		memcpy(&bossObj[6], &bossObj[3], sizeof(npc));
		bossObj[6].tgt_x = 3;
		bossObj[6].life = 100;

		//initialize the head npc
		bossObj[7].cond = npccond_alive;
		bossObj[7].x = 0x100000;
		bossObj[7].y = 102400;
		bossObj[7].view.left = 26624;
		bossObj[7].view.top = 12288;
		bossObj[7].view.right = 26624;
		bossObj[7].view.bottom = 12288;
		bossObj[7].hit_voice = 52;
		bossObj[7].hit.left = 4096;
		bossObj[7].hit.top = 12288;
		bossObj[7].hit.right = 4096;
		bossObj[7].hit.bottom = 0x2000;
		bossObj[7].bits = 8;
		bossObj[7].size = 3;
		bossObj[7].ani_no = 0;

		//initialize the tread npcs
		bossObj[9].cond = npccond_alive;
		bossObj[9].act_no = 0;
		bossObj[9].direct = dirUp;
		bossObj[9].x = 1015808;
		bossObj[9].y = 73728;
		bossObj[9].view.left = 18432;
		bossObj[9].view.top = 4096;
		bossObj[9].view.right = 18432;
		bossObj[9].view.bottom = 12288;
		bossObj[9].hit_voice = 52;
		bossObj[9].hit.left = 14336;
		bossObj[9].hit.top = 4096;
		bossObj[9].hit.right = 14336;
		bossObj[9].hit.bottom = 0x2000;
		bossObj[9].bits = npc_solidSoft | npc_invulnerable | npc_ignoreSolid | npc_rearTop;
		bossObj[9].size = 3;

		memcpy(&bossObj[10], &bossObj[9], sizeof(npc));
		bossObj[10].x = 1081344;

		memcpy(&bossObj[11], &bossObj[9], sizeof(npc));
		bossObj[11].direct = dirDown;
		bossObj[11].x = 1015808;
		bossObj[11].y = 0x20000;
		bossObj[11].view.top = 12288;
		bossObj[11].view.bottom = 4096;
		bossObj[11].hit.top = 0x2000;
		bossObj[11].hit.bottom = 4096;

		memcpy(&bossObj[12], &bossObj[11], sizeof(npc));
		bossObj[12].x = 1081344;

		//initialize the fish missile generator npcs
		memcpy(&bossObj[13], &bossObj[9], sizeof(npc));
		bossObj[13].cond = npccond_alive;
		bossObj[13].view.top = 0x2000;
		bossObj[13].view.bottom = 0x2000;
		bossObj[13].view.left = 15360;
		bossObj[13].view.right = 21504;
		bossObj[13].count1 = 9;
		bossObj[13].ani_no = 0;
		bossObj[13].bits = npc_ignoreSolid;

		memcpy(&bossObj[14], &bossObj[13], sizeof(npc));
		bossObj[14].view.left = 21504;
		bossObj[14].view.right = 15360;
		bossObj[14].count1 = 10;
		bossObj[14].ani_no = 1;
		bossObj[14].bits = npc_ignoreSolid;

		memcpy(&bossObj[15], &bossObj[13], sizeof(npc));
		bossObj[15].view.top = 0x2000;
		bossObj[15].view.bottom = 0x2000;
		bossObj[15].count1 = 11;
		bossObj[15].ani_no = 2;
		bossObj[15].bits = npc_ignoreSolid;

		memcpy(&bossObj[16], &bossObj[15], sizeof(npc));
		bossObj[16].view.left = 21504;
		bossObj[16].view.right = 15360;
		bossObj[16].count1 = 12;
		bossObj[16].ani_no = 3;
		bossObj[16].bits = npc_ignoreSolid;

		boss->act_no = 2;
		break;
	case 10:
		boss->act_no = 11;
		boss->act_wait = 0;
		boss->count1 = 0;
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
	case 101:
		if (++boss->act_wait == 4)
			bossObj[9].act_no = 100;
		if (boss->act_wait == 8)
			bossObj[10].act_no = 100;
		if (boss->act_wait == 10)
			bossObj[11].act_no = 100;
		if (boss->act_wait == 12)
			bossObj[12].act_no = 100;
		if (boss->act_wait > 120 && boss->count1 > 2)
			boss->act_no = 300;
		if (boss->act_wait > 121 && currentPlayer.x > boss->x)
			boss->act_no = 200;
		break;
	case 200:
		boss->act_wait = 0;
		boss->act_no = 201;
		++boss->count1;
	case 201:
		if (++boss->act_wait == 4)
			bossObj[9].act_no = 200;
		if (boss->act_wait == 8)
			bossObj[10].act_no = 200;
		if (boss->act_wait == 10)
			bossObj[11].act_no = 200;
		if (boss->act_wait == 12)
			bossObj[12].act_no = 200;
		if (boss->act_wait > 120 && boss->count1 > 2)
			boss->act_no = 400;
		if (boss->act_wait > 121 && currentPlayer.x < boss->x)
			boss->act_no = 100;
		break;
	case 300:
		boss->act_wait = 0;
		boss->act_no = 301;
	case 301:
		if (++boss->act_wait == 4)
			bossObj[9].act_no = 300;
		if (boss->act_wait == 8)
			bossObj[10].act_no = 300;
		if (boss->act_wait == 10)
			bossObj[11].act_no = 300;
		if (boss->act_wait == 12)
			bossObj[12].act_no = 300;
		if (boss->act_wait > 50)
		{
			if (bossObj[3].cond || bossObj[4].cond || bossObj[5].cond || bossObj[6].cond)
				boss->act_no = 500;
			else
				boss->act_no = 600;
		}
		break;
	case 400:
		boss->act_wait = 0;
		boss->act_no = 401;
	case 401:
		if (++boss->act_wait == 4)
			bossObj[9].act_no = 400;
		if (boss->act_wait == 8)
			bossObj[10].act_no = 400;
		if (boss->act_wait == 10)
			bossObj[11].act_no = 400;
		if (boss->act_wait == 12)
			bossObj[12].act_no = 400;
		if (boss->act_wait > 50)
		{
			if (bossObj[3].cond || bossObj[4].cond || bossObj[5].cond || bossObj[6].cond)
				boss->act_no = 500;
			else
				boss->act_no = 600;
		}
		break;
	case 500:
		boss->act_no = 501;
		boss->act_wait = 0;
		bossObj[1].act_no = 10;
		bossObj[2].act_no = 10;
	case 501:
		if (++boss->act_wait > 300)
		{
			boss->act_no = 502;
			boss->act_wait = 0;
		}
		if (!bossObj[3].cond && !bossObj[4].cond && !bossObj[5].cond && !bossObj[6].cond)
		{
			boss->act_no = 502;
			boss->act_wait = 0;
		}
		break;
	case 502:
		boss->act_no = 503;
		boss->act_wait = 0;
		boss->count1 = 0;
		bossObj[1].act_no = 20;
		bossObj[2].act_no = 20;
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
		bossObj[1].act_no = 30;
		bossObj[2].act_no = 30;
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
		bossObj[1].act_no = 40;
		bossObj[2].act_no = 40;
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
		viewport.quake = 2;
		if (!(++boss->act_wait % 8))
			playSound(SFX_BehemothLargeHurt);
		createSmokeLeft(boss->x + (random(-72, 72) << 9), boss->y + (random(-64, 64) << 9), 1, 1);
		if (boss->act_wait > 100)
		{
			boss->act_wait = 0;
			boss->act_no = 1001;
			setFlash(boss->x, boss->y, explosion, 0);
			playSound(SFX_LargeExplosion);
		}
		break;
	case 1001:
		viewport.quake = 40;
		if (++boss->act_wait > 50)
		{
			for (int i = 0; i < 20; ++i)
				bossObj[i].cond = 0;
			killNpcsByType(158, 1);
			createNpc(NPC_BossMonsterXDefeated, boss->x, boss->y - 12288);
		}
		break;
	default:
		break;
	}

	//run through sub npc ais
	monsterXSubNpcTreads(&bossObj[9]);
	monsterXSubNpcTreads(&bossObj[10]);
	monsterXSubNpcTreads(&bossObj[11]);
	monsterXSubNpcTreads(&bossObj[12]);
	boss->x += ((bossObj[12].x + bossObj[11].x + bossObj[10].x + bossObj[9].x) / 4 - boss->x) / 16;

	monsterXSubNpcHead(&bossObj[7]);
	
	monsterXSubNpcFishMissileGenerator(&bossObj[13]);
	monsterXSubNpcFishMissileGenerator(&bossObj[14]);
	monsterXSubNpcFishMissileGenerator(&bossObj[15]);
	monsterXSubNpcFishMissileGenerator(&bossObj[16]);

	monsterXSubNpcCover(&bossObj[1]);
	monsterXSubNpcCover(&bossObj[2]);

	if (bossObj[3].cond)
		monsterXSubNpcOrb(&bossObj[3]);
	if (bossObj[4].cond)
		monsterXSubNpcOrb(&bossObj[4]);
	if (bossObj[5].cond)
		monsterXSubNpcOrb(&bossObj[5]);
	if (bossObj[6].cond)
		monsterXSubNpcOrb(&bossObj[6]);

	//check if dead
	if (!boss->life && boss->act_no < 1000)
	{
		boss->act_no = 1000;
		boss->act_wait = 0;
		boss->shock = -106;
		bossObj[9].act_no = 300;
		bossObj[10].act_no = 300;
		bossObj[11].act_no = 300;
		bossObj[12].act_no = 300;
	}
	return;
}

void monsterXSubNpcTreads(npc *NPC)
{
	int spriteX[] = { 0, 0, 72, 144, 72, 144 };
	int spriteY[] = { 0, 32, 0, 0, 32, 32 };

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

void monsterXSubNpcFishMissileGenerator(npc *NPC)
{
	int xp = 0;
	int yp = 0;
	int direct = 0;

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
			direct = 3;
			xp = -15360;
			yp = 3072;
			break;
		case 1:
			direct = 2;
			xp = 15360;
			yp = 3072;
			break;
		case 2:
			direct = 0;
			xp = -15360;
			yp = -3072;
			break;
		case 3:
			direct = 1;
			xp = 15360;
			yp = -3072;
			break;
		default:
			break;
		}
		createNpc(NPC_ProjectileMonsterX, xp + NPC->x, yp + NPC->y, 0, 0, NPC->direct, 0, false);
		playSound(SFX_EnemyShootProjectile);
		NPC->act_wait = 120;
	}

LABEL_13:
	NPC->x = (bossObj[NPC->count1].x + bossObj[0].x) / 2;
	NPC->y = (bossObj[NPC->count1].y + bossObj[0].y) / 2;

	NPC->rect.left = (NPC->ani_no % 2) * 72;
	NPC->rect.top = 128 + ((NPC->ani_no / 2) * 32);
	NPC->rect.right = NPC->rect.left + 72;
	NPC->rect.bottom = NPC->rect.top + 32;
}

void monsterXSubNpcCover(npc *NPC)
{
	RECT *v1; // eax
	RECT *v2; // edx

	switch (NPC->act_no)
	{
	case 0xA:
		NPC->tgt_x += 512;
		if (NPC->tgt_x > 0x4000)
		{
			NPC->tgt_x = 0x4000;
			NPC->act_no = 0;
			bossObj[3].act_no = 10;
			bossObj[4].act_no = 10;
			bossObj[5].act_no = 10;
			bossObj[6].act_no = 10;
		}
		break;
	case 0x14:
		NPC->tgt_x -= 512;
		if (NPC->tgt_x < 0)
		{
			NPC->tgt_x = 0;
			NPC->act_no = 0;
			bossObj[3].act_no = 0;
			bossObj[4].act_no = 0;
			bossObj[5].act_no = 0;
			bossObj[6].act_no = 0;
		}
		break;
	case 0x1E:
		NPC->tgt_x += 512;
		if (NPC->tgt_x > 10240)
		{
			NPC->tgt_x = 10240;
			NPC->act_no = 0;
			bossObj[7].act_no = 10;
			bossObj[13].act_no = 10;
			bossObj[14].act_no = 10;
			bossObj[15].act_no = 10;
			bossObj[16].act_no = 10;
		}
		break;
	case 0x28:
		NPC->tgt_x -= 512;
		if (NPC->tgt_x < 0)
		{
			NPC->tgt_x = 0;
			NPC->act_no = 0;
			bossObj[7].act_no = 0;
			bossObj[13].act_no = 0;
			bossObj[14].act_no = 0;
			bossObj[15].act_no = 0;
			bossObj[16].act_no = 0;
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
		NPC->x = bossObj[0].x + NPC->tgt_x + 12288;
	}
	else
	{
		NPC->rect.left = 216;
		NPC->rect.top = 96;
		NPC->rect.right = 264;
		NPC->rect.bottom = 144;
		NPC->x = bossObj[0].x - 12288 - NPC->tgt_x;
	}
	NPC->y = bossObj[0].y;
}

void monsterXSubNpcOrb(npc *NPC)
{
	int Y_Vel;
	int X_Vel;
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
		Y_Vel = 3 * getSin(temp);
		X_Vel = 3 * getCos(temp);
		createNpc(NPC_ProjectileGaudiFlying, NPC->x, NPC->y, X_Vel, Y_Vel, 0, 0, 256);
		playSound(SFX_EnemyShootProjectile);
		NPC->act_wait = 40;
	}
LABEL_14:
	switch (NPC->tgt_x)
	{
	case 0:
		NPC->x = bossObj[0].x - 11264;
		NPC->y = bossObj[0].y - 0x2000;
		break;
	case 1:
		NPC->x = bossObj[0].x + 14336;
		NPC->y = bossObj[0].y - 0x2000;
		break;
	case 2:
		NPC->x = bossObj[0].x - 7680;
		NPC->y = bossObj[0].y + 7168;
		break;
	case 3:
		NPC->x = bossObj[0].x + 8704;
		NPC->y = bossObj[0].y + 7168;
		break;
	default:
		break;
	}

	NPC->rect.left = NPC->tgt_x * 16;
	NPC->rect.top = 192 + (NPC->ani_no * 16);
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 16;
}


void monsterXSubNpcHead(npc *NPC)
{
	switch (NPC->act_no)
	{
	case 0:
		bossObj[0].bits &= ~npc_shootable;
		NPC->ani_no = 0;
		break;
	case 10:
		NPC->act_no = 11;
		NPC->act_wait = 10 * NPC->tgt_x + 40;
		bossObj[0].bits |= npc_shootable;
		break;
	case 11:
		break;
	}

	if (bossObj[0].shock)
	{
		if (bossObj[0].shock / 2 % 2)
			NPC->ani_no = 1;
		else
			NPC->ani_no = 0;
	}
	else
	{
		NPC->ani_no = 0;
	}


	NPC->x = bossObj[0].x;
	NPC->y = bossObj[0].y;

	if (bossObj[0].act_no <= 10)
		NPC->ani_no = 3;

	NPC->rect.left = 216;
	NPC->rect.top = NPC->ani_no*48;
	NPC->rect.right = NPC->rect.left + 104;
	NPC->rect.bottom = NPC->rect.top + 48;
}