#include "bullet.h"
#include "spur.h"
#include "caret.h"

void actBulletSpur1(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	if (bul->damage && bul->life != 100)
		bul->damage = 0;
	if (bul->act_no)
	{
		bul->x += bul->xm;
		bul->y += bul->ym;
	}
	else
	{
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -4096;
			bul->enemyYL = 1024;
			break;
		case(dirUp):
			bul->ym = -4096;
			bul->enemyXL = 1024;
			break;
		case(dirRight):
			bul->xm = 4096;
			bul->enemyYL = 1024;
			break;
		case(dirDown):
			bul->ym = 4096;
			bul->enemyXL = 1024;
			break;
		}
	}

	bul->damage = bul->life;
	createBullet(bul->code_bullet + 3, bul->x, bul->y, bul->direct, 14);

	bul->rect.left = 128 + ((bul->direct % 2) * 16);
	bul->rect.top = 32;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = 48;
	return;
}

void actBulletSpur2(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	if (bul->damage && bul->life != 100)
		bul->damage = 0;
	if (bul->act_no)
	{
		bul->x += bul->xm;
		bul->y += bul->ym;
	}
	else
	{
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -4096;
			bul->enemyYL = 2048;
			break;
		case(dirUp):
			bul->ym = -4096;
			bul->enemyXL = 2048;
			break;
		case(dirRight):
			bul->xm = 4096;
			bul->enemyYL = 2048;
			break;
		case(dirDown):
			bul->ym = 4096;
			bul->enemyXL = 2048;
			break;
		}
	}

	bul->damage = bul->life;
	createBullet(bul->code_bullet + 3, bul->x, bul->y, bul->direct, 14);

	bul->rect.left = 160 + ((bul->direct % 2) * 16);
	bul->rect.top = 32;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = 48;
	return;
}

void actBulletSpur3(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	if (bul->damage && bul->life != 100)
		bul->damage = 0;
	if (bul->act_no)
	{
		bul->x += bul->xm;
		bul->y += bul->ym;
	}
	else
	{
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -4096;
			break;
		case(dirUp):
			bul->ym = -4096;
			break;
		case(dirRight):
			bul->xm = 4096;
			break;
		case(dirDown):
			bul->ym = 4096;
			break;
		}
	}

	bul->damage = bul->life;
	createBullet(bul->code_bullet + 3, bul->x, bul->y, bul->direct, 14);

	bul->rect.left = 128 + ((bul->direct % 2) * 16);
	bul->rect.top = 48;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = 64;
	return;
}

void actBulletSpurTrail1(bullet *bul)
{
	if (++bul->count1 > 20)
		bul->ani_no = bul->count1 - 20;
	if (bul->ani_no > 2)
		bul->cond = 0;
	if (bul->damage && bul->life != 100)
		bul->damage = 0;

	bul->rect.left = 192 + (bul->ani_no * 8);
	bul->rect.top = 32 + ((bul->direct % 2) * 8);
	bul->rect.right = bul->rect.left + 8;
	bul->rect.bottom = bul->rect.top + 8;
	return;
}

void actBulletSpurTrail2(bullet *bul)
{
	if (++bul->count1 > 20)
		bul->ani_no = bul->count1 - 20;
	if (bul->ani_no > 2)
		bul->cond = 0;
	if (bul->damage && bul->life != 100)
		bul->damage = 0;

	bul->rect.left = 216 + (bul->ani_no * 8);
	bul->rect.top = 32 + ((bul->direct % 2) * 8);
	bul->rect.right = bul->rect.left + 8;
	bul->rect.bottom = bul->rect.top + 8;
	return;
}

void actBulletSpurTrail3(bullet *bul)
{
	if (++bul->count1 > 20)
		bul->ani_no = bul->count1 - 20;
	if (bul->ani_no > 2)
		bul->cond = 0;
	if (bul->damage && bul->life != 100)
		bul->damage = 0;

	bul->rect.left = 240 + (bul->ani_no * 8);
	bul->rect.top = 32;
	bul->rect.right = bul->rect.left + 8;
	bul->rect.bottom = bul->rect.top + 8;
	return;
}