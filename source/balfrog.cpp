#include "mapboss.h"
#include "sound.h"
#include "mathUtils.h"
#include "player.h"
#include "render.h"
#include "game.h"

//balfrog's mouth
void balfrogMouth(npc *sub)
{
	int minus;

	if (bossObj[0].direct)
		minus = -1;
	else
		minus = 1;

	switch (bossObj[0].act_no)
	{
	case 0:	//ini
		sub->hit_voice = 52;
		sub->hit.left = 0x2000;
		sub->hit.top = 0x2000;
		sub->hit.right = 0x2000;
		sub->hit.bottom = 0x2000;
		sub->size = 3;
		sub->bits = 4;
		break;
	case 1:
		sub->x = bossObj[0].x - 12288 * minus;
		sub->y = bossObj[0].y - 12288;
		break;
	case 2:
		sub->x = bossObj[0].x - 12288 * minus;
		sub->y = bossObj[0].y - 10240;
		break;
	case 3:
	case 4:
		sub->x = bossObj[0].x - 12288 * minus;
		sub->y = bossObj[0].y - 8192;
		break;
	case 5:
		sub->x = bossObj[0].x - 12288 * minus;
		sub->y = bossObj[0].y - 22016;
		break;
	default:
		return;
	}
}

//second half of balfrog or something
void actBoss_Frog2(npc *sub)
{
	if (bossObj[0].act_no)
	{
		if (bossObj[0].act_no >= 0 && bossObj[0].act_no <= 5)
		{
			sub->x = bossObj[0].x;
			sub->y = bossObj[0].y;
		}
	}
	else
	{
		sub->hit_voice = 52;
		sub->hit.left = 12288;
		sub->hit.top = 8192;
		sub->hit.right = 12288;
		sub->hit.bottom = 8192;
		sub->size = 3;
		sub->bits = 4;
	}
}

//main boss ai
void actBoss_Frog(npc *boss)
{
	int xm = 0;
	int ym = 0;
	uint8_t deg = 0;

	//states enum so this is more readable
	enum
	{
		ini = 0,
		start = 10,
		hop = 103
	};

	switch (boss->act_no)
	{
	case ini:
		boss->x = 49152;
		boss->y = 102400;
		boss->direct = 2;
		boss->view.left = 24576;
		boss->view.top = 24576;
		boss->view.right = 16384;
		boss->view.bottom = 8192;

		boss->hit_voice = 52;

		boss->hit.left = 12288;
		boss->hit.top = 8192;

		boss->hit.right = 12288;
		boss->hit.bottom = 8192;

		boss->size = 3;
		boss->exp = 1;
		boss->code_event = 1000;
		boss->bits |= 0x8200;
		boss->life = 300;
		break;
	case start:
		boss->act_no = 11;
		boss->ani_no = 3;
		boss->cond = -128;

		boss->rect.left = 80;
		boss->rect.top = 48;
		boss->rect.right = 160;
		boss->rect.bottom = 112;

		boss->cond = -112;
		boss->code_event = 1000;
		bossObj[2].cond = 0x80;
		boss->damage = 5;
		bossObj[2].damage = 5;
		for (int i = 0; i <= 7; ++i)
			createNpc(4, boss->x + (random(-12, 12) << 9), boss->y + (random(-12, 12) << 9), random(-341, 341), random(-1536, 0));
		break;
	case 20:
		boss->act_no = 21;
		boss->act_wait = 0;
	case 21:
		if (++boss->act_wait / 2 & 1)
			boss->ani_no = 3;
		else
			boss->ani_no = 0;
		break;
	case 100:
		boss->act_no = 101;
		boss->act_wait = 0;
		boss->ani_no = 1;
		boss->xm = 0;
	case 101:
		if (++boss->act_wait > 50)
		{
			boss->act_no = 102;
			boss->ani_wait = 0;
			boss->ani_no = 2;
		}
		break;
	case 102:
		if (++boss->ani_wait > 10)
		{
			boss->act_no = 103;
			boss->ani_wait = 0;
			boss->ani_no = 1;
		}
		break;
	case hop:
		if (++boss->ani_wait > 4)
		{
			boss->act_no = 104;
			boss->ani_no = 5;
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
	case 104:
		if (!boss->direct && boss->flag & 1)
		{
			boss->direct = 2;
			boss->xm = 512;
		}
		if (boss->direct == 2 && boss->flag & 4)
		{
			boss->direct = 0;
			boss->xm = -512;
		}
		if (boss->flag & 8)
		{
			playSound(26, 1);
			viewport.quake = 30;
			boss->act_no = 100;
			boss->ani_no = 1;
			boss->view.top = 24576;
			boss->view.bottom = 0x2000;
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
				createNpc(4, boss->x + (random(-12, 12) << 9), boss->y + boss->hit.bottom, random(-341, 341), random(-1536, 0));
		}
		break;
	case 110:
		boss->ani_no = 1;
		boss->act_wait = 0;
		boss->act_no = 111;
	case 111:
		++boss->act_wait;
		boss->xm = 8 * boss->xm / 9;
		if (boss->act_wait > 50)
		{
			boss->ani_no = 2;
			boss->ani_wait = 0;
			boss->act_no = 112;
		}
		break;
	case 112:
		if (++boss->ani_wait > 4)
		{
			boss->act_no = 113;
			boss->act_wait = 0;
			boss->ani_no = 3;
			boss->count1 = 16;
			bossObj[1].bits |= 0x20;
			boss->tgt_x = boss->life;
		}
		break;
	case 113:
		if (boss->shock)
		{
			if ((boss->count2++ / 2) & 1)
				boss->ani_no = 4;
			else
				boss->ani_no = 3;
		}
		else
		{
			boss->count2 = 0;
			boss->ani_no = 3;
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
				createNpc(108, boss->x + 0x4000, boss->y - 4096, xm, ym);
			else
				createNpc(108, boss->x - 0x4000, boss->y - 4096, xm, ym);
			playSound(39, 1);
			if (!boss->count1 || boss->life < boss->tgt_x - 90)
			{
				boss->act_no = 114;
				boss->act_wait = 0;
				boss->ani_no = 2;
				boss->ani_wait = 0;

				bossObj[1].bits &= 0xFFDF;
			}
		}
		break;
	case 114:
		if (++boss->ani_wait > 10)
		{
			if (++bossObj[1].count1 <= 2)
			{
				boss->act_no = 100;
			}
			else
			{
				bossObj[1].count1 = 0;
				boss->act_no = 120;
			}
			boss->ani_wait = 0;
			boss->ani_no = 1;
		}
		break;
	case 120:
		boss->act_no = 121;
		boss->act_wait = 0;
		boss->ani_no = 1;
		boss->xm = 0;
	case 121:
		if (++boss->act_wait > 50)
		{
			boss->act_no = 122;
			boss->ani_wait = 0;
			boss->ani_no = 2;
		}
		break;
	case 122:
		if (++boss->ani_wait > 20)
		{
			boss->act_no = 123;
			boss->ani_wait = 0;
			boss->ani_no = 1;
		}
		break;
	case 123:
		if (++boss->ani_wait > 4)
		{
			boss->act_no = 124;
			boss->ani_no = 5;
			boss->ym = -2560;
			boss->view.top = 0x8000;
			boss->view.bottom = 12288;
			playSound(25, 1);
		}
		break;
	case 124:
		if (boss->flag & 8)
		{
			playSound(26, 1);
			viewport.quake = 60;
			boss->act_no = 100;
			boss->ani_no = 1;
			boss->view.top = 24576;
			boss->view.bottom = 0x2000;
			for (int i = 0; i <= 1; ++i)
				createNpc(104, random(4, 16) << 13, random(0, 4) << 13, 0, 0, 4, 0);
			for (int i = 0; i <= 5; ++i)
				createNpc(110, random(4, 16) << 13, random(0, 4) << 13, 0, 0, 4, 0);
			for (int i = 0; i <= 7; ++i)
				createNpc(4, boss->x + (random(-12, 12) << 9), boss->y + boss->hit.bottom, random(-341, 341), random(-1536, 0), 0, 0);
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
	case 130:
		boss->act_no = 131;
		boss->ani_no = 3;
		boss->act_wait = 0;
		boss->xm = 0;
		playSound(72, 1);
		for (int i = 0; i <= 7; ++i)
			createNpc(4, boss->x + (random(-12, 12) << 9), boss->y + (random(-12, 12) << 9), random(-341, 341), random(-1536, 0));
		bossObj[1].cond = 0;
		bossObj[2].cond = 0;
	case 131:
		if (!(++boss->act_wait % 5))
			createNpc(4, boss->x + (random(-12, 12) << 9), boss->y + (random(-12, 12) << 9), random(-341, 341), random(-1536, 0));
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
	case 132:
		if (++boss->act_wait / 2 & 1)
		{
			boss->view.left = 10240;
			boss->view.top = 6144;
			boss->view.right = 10240;
			boss->view.bottom = 6144;
			boss->ani_no = 6;
		}
		else
		{
			boss->view.left = 24576;
			boss->view.top = 24576;
			boss->view.right = 0x4000;
			boss->view.bottom = 0x2000;
			boss->ani_no = 3;
		}
		if (!(boss->act_wait % 9))
			createNpc(4, boss->x + (random(-12, 12) << 9), boss->y + (random(-12, 12) << 9), random(-341, 341), random(-1536, 0));
		if (boss->act_wait > 150)
		{
			boss->act_no = 140;
			boss->hit.bottom = 6144;
		}
		break;
	case 140:
		boss->act_no = 141;
	case 141:
		if (boss->flag & 8)
		{
			boss->act_no = 142;
			boss->act_wait = 0;
			boss->ani_no = 7;
		}
		break;
	case 142:
		if (++boss->act_wait > 30)
		{
			boss->ani_no = 8;
			boss->ym = -2560;
			boss->bits |= 8;
			boss->act_no = 143;
		}
		break;
	case 143:
		boss->ym = -2560;
		if (boss->y < 0)
		{
			boss->cond = 0;
			playSound(26, 1);
			viewport.quake = 30;
		}
		break;
	default:
		break;
	}

	boss->doGravity(64, 1535);

	boss->x += boss->xm;
	boss->y += boss->ym;
	 
	boss->rect.left = ((boss->ani_no / 3) * 160) + (boss->direct * 80);
	boss->rect.top = 48 + ((boss->ani_no % 3) * 64);
	boss->rect.right = boss->rect.left + 80;
	boss->rect.bottom = boss->rect.top + 64;

	balfrogMouth(&bossObj[1]);
	actBoss_Frog2(&bossObj[2]);
	return;
}