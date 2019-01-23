#include "boss.h"
#include "sound.h"
#include "mathUtils.h"
#include "weapons.h"
#include "player.h"
#include "render.h"
#include "game.h"
#include "common.h"
#include "stage.h"
#include "flash.h"

#include <cstring>

void ActBoss01_12() // Knee things
{
	RECT rcRight[1];
	RECT rcLeft[1];

	rcLeft[0] = { 80, 56, 104, 72 };
	rcRight[0] = { 104, 56, 128, 72 };

	for (int i = 1; i <= 2; ++i)
	{
		if (bossObj[i].direct)
		{
			bossObj[i].rect = rcRight[bossObj[i].count2];
			bossObj[i].x = bossObj[0].x + 0x2000;
		}
		else
		{
			bossObj[i].rect = rcLeft[bossObj[i].count2];
			bossObj[i].x = bossObj[0].x - 0x2000;
		}

		bossObj[i].y = (bossObj[i].y + bossObj[i + 2].y - 0x1000) / 2;
	}
}

void ActBoss01_34() // Feet
{
	RECT rcRight[2];
	RECT rcLeft[2];

	rcLeft[0] = { 0, 56, 40, 88 };
	rcLeft[1] = { 40, 56, 80, 88 };
	rcRight[0] = { 0, 88, 40, 120 };
	rcRight[1] = { 40, 88, 80, 120 };
	rcRight[1].left = 40;
	rcRight[1].top = 88;
	rcRight[1].right = 80;
	rcRight[1].bottom = 120;

	for (int i = 3; i <= 4; ++i)
	{
		switch (bossObj[i].act_no)
		{
		case 0:
			bossObj[i].act_no = 1;
//Fallthrough
		case 1:
			if (i == 3)
				bossObj[i].x = bossObj[0].x - 0x2000;
			if (i == 4)
				bossObj[i].x = bossObj[0].x + 0x2000;

			bossObj[i].y = bossObj[0].y;
			break;

		case 3:
			if (i == 3)
				bossObj[i].x = bossObj[0].x - 0x2000;
			if (i == 4)
				bossObj[i].x = bossObj[0].x + 0x2000;

			bossObj[i].tgt_y = bossObj[0].y + 0x3000;
			bossObj[i].y += (bossObj[i].tgt_y - bossObj[i].y) / 2;
			break;

		default:
			break;
		}

		bossObj[i].count2 = !(bossObj[i].flag & ground && bossObj[i].y > bossObj[i].tgt_y);

		if (bossObj[i].direct)
			bossObj[i].rect = rcRight[bossObj[i].count2];
		else
			bossObj[i].rect = rcLeft[bossObj[i].count2];
	}
}

void ActBoss01_5()
{
	if (!bossObj[5].act_no)
	{
		bossObj[5].bits |= npc_solidSoft | npc_ignoreSolid;
		bossObj[5].hit = { 0x2800, 0x4800, 0x2800, 0x2000 };
		bossObj[5].act_no = 1;
	}

	if (bossObj[5].act_no == 1)
	{
		bossObj[5].x = bossObj[0].x;
		bossObj[5].y = bossObj[0].y;
	}
}

void actBoss_Omega(npc *boss)
{
	switch (boss->act_no)
	{
	case 0:
		boss->x = 0x1B6000;
		boss->y = 0x20000;

		boss->view = { 0x5000, 0x5000, 0x5000, 0x2000 };

		boss->tgt_x = 0x1B6000;
		boss->tgt_y = 0x20000;

		boss->hit_voice = 52;

		boss->hit = { 0x1000, 0x3000, 0x1000, 0x2000 };

		boss->bits = (npc_ignoreSolid | npc_eventDie | npc_showDamage);
		boss->size = 3;
		boss->exp = 1;
		boss->code_event = 210;
		boss->life = 400;

		bossObj[1].cond = npccond_alive;
		bossObj[1].view = { 0x1800, 0x1000, 0x1800, 0x1000 };
		bossObj[1].bits = npc_ignoreSolid;

		memcpy(&bossObj[2], &bossObj[1], sizeof(bossObj[2]));

		bossObj[1].direct = dirLeft;
		bossObj[2].direct = dirRight;

		bossObj[3].cond = npccond_alive;
		bossObj[3].view = { 0x3000, 0x2000, 0x2000, 0x2000 };
		bossObj[3].hit_voice = 52;
		bossObj[3].hit = { 0x1000, 0x1000, 0x1000, 0x1000 };
		bossObj[3].bits = npc_ignoreSolid;

		bossObj[3].y = boss->y;
		bossObj[3].direct = dirLeft;

		memcpy(&bossObj[4], &bossObj[3], sizeof(bossObj[4]));
		bossObj[4].direct = dirRight;
		bossObj[3].x = boss->x + 0x2000;
		bossObj[5].cond = npccond_alive;
		break;

	case 20: //Rising out of the ground
		boss->act_no = 30;
		boss->act_wait = 0;
		boss->ani_no = 0;
		//Fallthrough
	case 30:
		gViewport.quake = 2;
		boss->y -= 0x200;

		if (!(++boss->act_wait & 3))
			playSound(SFX_LargeObjectHitGround);

		if (boss->act_wait >= 48)
		{
			boss->act_no = 40;
			boss->act_wait = 0;

			if (boss->life <= 280)
			{
				boss->act_no = 110;

				boss->bits |= npc_shootable;
				boss->bits &= ~npc_ignoreSolid;
				bossObj[3].bits &= ~npc_ignoreSolid;
				bossObj[4].bits &= ~npc_ignoreSolid;

				bossObj[3].act_no = 3;
				bossObj[4].act_no = 3;
				bossObj[5].hit.top = 0x2000;
			}
		}
		break;

	case 40:
		if (++boss->act_wait >= 48)
		{
			boss->act_wait = 0;
			boss->act_no = 50;
			boss->count1 = 0;
			bossObj[5].hit.top = 0x2000;
			playSound(SFX_SandCroc);
		}
		break;

	case 50: //Open mouth
		if (++boss->count1 > 2)
		{
			boss->count1 = 0;
			++boss->count2;
		}

		if (boss->count2 == 3)
		{
			boss->act_no = 60;
			boss->act_wait = 0;
			boss->bits |= npc_shootable;
			boss->hit.left = 0x2000;
			boss->hit.right = 0x2000;
		}
		break;

	case 60: //Shoot out of mouth
		if (++boss->act_wait > 20 && boss->act_wait < 80 && !(boss->act_wait % 3))
		{
			if (random(0, 9) <= 7)
				createNpc(48, boss->x, boss->y - 0x2000, random(-0x100, 0x100), -0x333, dirLeft, nullptr, true);
			else
				createNpc(48, boss->x, boss->y - 0x2000, random(-0x100, 0x100), -0x333, dirRight, nullptr, true);

			playSound(SFX_EnemyShootProjectile);
		}

		if (boss->act_wait >= 200 || weaponBullets(6))
		{
			boss->count1 = 0;
			boss->act_no = 70;
			playSound(SFX_SandCroc);
		}
		break;

	case 70: //Close mouth
		if (++boss->count1 > 2)
		{
			boss->count1 = 0;
			--boss->count2;
		}

		if (boss->count2 == 1)
			boss->damage = 20;

		if (!boss->count2)
		{
			playSound(SFX_SandCroc);
			playSound(SFX_DestroyBreakableBlock);

			boss->act_no = 80;
			boss->act_wait = 0;

			boss->bits &= ~npc_shootable;

			boss->hit.left = 0x3000;
			boss->hit.right = 0x3000;
			bossObj[5].hit.top = 0x4800;

			boss->damage = 0;
		}
		break;

	case 80:
		if (++boss->act_wait >= 48)
		{
			boss->act_wait = 0;
			boss->act_no = 90;
		}
		break;

	case 90: //Go back into the ground
		gViewport.quake = 2;
		boss->y += 0x200;

		if (!(++boss->act_wait & 3))
			playSound(SFX_LargeObjectHitGround);

		if (boss->act_wait >= 48)
		{
			boss->act_wait = 0;
			boss->act_no = 100;
		}
		break;

	case 100: //Move to proper position for coming out of the ground
		if (++boss->act_wait >= 120)
		{
			boss->act_wait = 0;
			boss->act_no = 30;

			boss->x = boss->tgt_x + (random(-64, 64) << 9);
			boss->y = boss->tgt_y;
		}
		break;

	case 110:
		if (++boss->count1 > 2)
		{
			boss->count1 = 0;
			++boss->count2;
		}

		if (boss->count2 == 3)
		{
			boss->act_no = 120;
			boss->act_wait = 0;
			boss->hit.left = 0x2000;
			boss->hit.right = 0x2000;
		}
		break;

	case 120:
		if (++boss->act_wait >= 50 || weaponBullets(6))
		{
			boss->act_no = 130;
			playSound(SFX_SandCroc);
			boss->act_wait = 0;
			boss->count1 = 0;
		}

		if (boss->act_wait < 30 && !(boss->act_wait % 5))
		{
			createNpc(48, boss->x, boss->y - 0x2000, random(-0x155, 0x155), -0x333, 0, nullptr, true);
			playSound(SFX_EnemyShootProjectile);
		}
		break;

	case 130:
		if (++boss->count1 > 2)
		{
			boss->count1 = 0;
			--boss->count2;
		}

		if (boss->count2 == 1)
			boss->damage = 20;

		if (!boss->count2)
		{
			boss->act_no = 140;
			boss->bits |= npc_shootable;

			boss->hit.left = 0x2000;
			boss->hit.right = 0x2000;

			boss->ym = -0x5FF;

			playSound(SFX_SandCroc);
			playSound(SFX_DestroyBreakableBlock);
			playSound(SFX_SillyExplosion);

			if (boss->x < currentPlayer.x)
				boss->xm = 0x100;
			if (boss->x > currentPlayer.x)
				boss->xm = -0x100;

			boss->damage = 0;
			bossObj[5].hit.top = 0x4800;
		}
		break;

	case 140:
		if (currentPlayer.flag & ground && boss->ym > 0)
			bossObj[5].damage = 20;
		else
			bossObj[5].damage = 0;

		boss->ym += 0x24;
		if (boss->ym > 0x5FF)
			boss->ym = 0x5FF;

		boss->x += boss->xm;
		boss->y += boss->ym;

		if (boss->flag & ground)
		{
			boss->act_no = 110;
			boss->act_wait = 0;
			boss->count1 = 0;

			bossObj[5].hit.top = 0x2000;
			bossObj[5].damage = 0;

			playSound(SFX_LargeObjectHitGround);
			playSound(SFX_DestroyBreakableBlock);

			gViewport.quake = 30;
		}
		break;

	case 150:
		gViewport.quake = 2;

		if (!(++boss->act_wait % 12))
			playSound(SFX_BehemothLargeHurt);

		createSmokeLeft(boss->x + (random(-48, 48) << 9), boss->y + (random(-48, 24) << 9), 1, 1);

		if (boss->act_wait > 100)
		{
			boss->act_wait = 0;
			boss->act_no = 160;
			setFlash(boss->x, boss->y, flashModes::explosion, 0);
			playSound(SFX_LargeExplosion);
		}
		break;

	case 160:
		gViewport.quake = 40;

		if (++boss->act_wait > 50)
		{
			boss->cond = 0;
			bossObj[1].cond = 0;
			bossObj[2].cond = 0;
			bossObj[3].cond = 0;
			bossObj[4].cond = 0;
			bossObj[5].cond = 0;
		}
		break;

	default:
		break;
	}

	RECT rect[4];

	rect[0].left = 0;
	rect[0].top = 0;
	rect[0].right = 80;
	rect[0].bottom = 56;
	rect[1].left = 80;
	rect[1].top = 0;
	rect[1].right = 160;
	rect[1].bottom = 56;
	rect[2].left = 160;
	rect[2].top = 0;
	rect[2].right = 240;
	rect[2].bottom = 56;
	rect[3].left = 80;
	rect[3].top = 0;
	rect[3].right = 160;
	rect[3].bottom = 56;

	boss->rect = rect[boss->count2];

	bossObj[1].shock = boss->shock;
	bossObj[2].shock = boss->shock;
	bossObj[3].shock = boss->shock;
	bossObj[4].shock = boss->shock;

	ActBoss01_34();
	ActBoss01_12();
	ActBoss01_5();

	if (!boss->life && boss->act_no < 150)
	{
		boss->act_no = 150;
		boss->act_wait = 0;
		boss->damage = 0;
		bossObj[5].damage = 0;

		for (size_t i = 0; i < gNPC.size(); i++)
		{
			if (gNPC[i].cond & npccond_alive)
			{
				if (gNPC[i].code_char == 48)
					gNPC[i].cond = 0;
			}
		}
	}
}
