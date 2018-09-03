#include "bullet.h"
#include "machineGun.h"
#include "caret.h"
#include "sound.h"
#include "npc.h"
#include "player.h"
#include "mathUtils.h"

void actBulletMachineGun1(bullet *bul)
{
	const int move = 4096;

	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}
	if (bul->act_no)
	{
		bul->x += bul->xm;
		bul->y += bul->ym;

		bul->rect.left = 64 + (bul->direct*16);
		bul->rect.top = 0;
		bul->rect.right = bul->rect.left + 16;
		bul->rect.bottom = bul->rect.top + 16;
	}
	else
	{
		bul->act_no = 1;

		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -move;
			bul->ym = random(-170, 170);
			break;
		case(dirUp):
			bul->ym = -move;
			bul->xm = random(-170, 170);
			break;
		case(dirRight):
			bul->xm = move;
			bul->ym = random(-170, 170);
			break;
		case(dirDown):
			bul->ym = move;
			bul->xm = random(-170, 170);
			break;
		}
	}
	return;
}

void actBulletMachineGun2(bullet *bul)
{
	const int move = 4096;

	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}
	if (bul->act_no)
	{
		bul->x += bul->xm;
		bul->y += bul->ym;

		bul->rect.left = 64 + (bul->direct * 16);
		bul->rect.top = 16;
		bul->rect.right = bul->rect.left + 16;
		bul->rect.bottom = bul->rect.top + 16;
		if (bul->direct != 1 && bul->direct != 3)
			createNpc(NPC_ProjectileMachineGunTrailLevel2, bul->x, bul->y, 0, 0, 0);
		else
			createNpc(NPC_ProjectileMachineGunTrailLevel2, bul->x, bul->y, 0, 0, 1);
	}
	else
	{
		bul->act_no = 1;

		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -move;
			bul->ym = random(-170, 170);
			break;
		case(dirUp):
			bul->ym = -move;
			bul->xm = random(-170, 170);
			break;
		case(dirRight):
			bul->xm = move;
			bul->ym = random(-170, 170);
			break;
		case(dirDown):
			bul->ym = move;
			bul->xm = random(-170, 170);
			break;
		}
	}

	return;
}

void actBulletMachineGun3(bullet *bul)
{
	const int move = 4096;

	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}
	if (bul->act_no)
	{
		bul->x += bul->xm;
		bul->y += bul->ym;

		bul->rect.left = 64 + (bul->direct * 16);
		bul->rect.top = 32;
		bul->rect.right = bul->rect.left + 16;
		bul->rect.bottom = bul->rect.top + 16;

		createNpc(NPC_ProjectileMachineGunTrailLevel3, bul->x, bul->y, 0, 0, bul->direct);
	}
	else
	{
		bul->act_no = 1;

		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -move;
			bul->ym = random(-170, 170);
			break;
		case(dirUp):
			bul->ym = -move;
			bul->xm = random(-170, 170);
			break;
		case(dirRight):
			bul->xm = move;
			bul->ym = random(-170, 170);
			break;
		case(dirDown):
			bul->ym = move;
			bul->xm = random(-170, 170);
			break;
		}
	}

	return;
}