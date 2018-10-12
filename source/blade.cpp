#include "bullet.h"
#include "blade.h"
#include "caret.h"
#include "sound.h"
#include "mathUtils.h"

void actBulletBlade1(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	if (bul->count1 == 3)
		bul->bbits &= 0xFFFFFFFB;
	if (bul->count1 % 5 == 1)
		playSound(SFX_FireballBounce);
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
			bul->xm = -2048;
			break;
		case(dirUp):
			bul->ym = -2048;
			break;
		case(dirRight):
			bul->xm = 2048;
			break;
		case(dirDown):
			bul->ym = 2048;
			break;
		}
	}

	if (++bul->ani_wait > 1)
	{
		bul->ani_wait = 0;
		++bul->ani_no;
	}
	if (bul->ani_no > 3)
		bul->ani_no = 0;

	bul->rect.left = (((bul->direct + 2)/3)*64) + (bul->ani_no * 16);
	bul->rect.top = 48;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = bul->rect.top + 16;
}

void actBulletBlade2(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	if (bul->count1 == 3)
		bul->bbits &= 0xFFFFFFFB;
	if (bul->count1 % 7 == 1)
		playSound(SFX_SpinningProjectile);

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
			bul->xm = -2048;
			break;
		case(dirUp):
			bul->ym = -2048;
			break;
		case(dirRight):
			bul->xm = 2048;
			break;
		case(dirDown):
			bul->ym = 2048;
			break;
		}
	}

	if (++bul->ani_wait > 1)
	{
		bul->ani_wait = 0;
		++bul->ani_no;
	}
	if (bul->ani_no > 3)
		bul->ani_no = 0;

	bul->rect.left = 160 + (bul->ani_no * 24);
	bul->rect.top = 48 + (((bul->direct + 2) / 3) * 24);
	bul->rect.right = bul->rect.left + 24;
	bul->rect.bottom = bul->rect.top + 24;
}

void actBulletBlade3(bullet *bul)
{
	enum bladeStats
	{
		ini = 0,
		moving = 1,
		stationary = 2
	};

	switch (bul->act_no)
	{
	case(ini):
		bul->act_no = 1;
		bul->xm = 0;
		bul->ym = 0;
		break;

	case(moving):
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -2048;
			break;
		case(dirUp):
			bul->ym = -2048;
			break;
		case(dirRight):
			bul->xm = 2048;
			break;
		case(dirDown):
			bul->ym = 2048;
			break;
		}

		if (bul->life != 100)
		{
			bul->act_no = 2;
			bul->ani_no = 1;
			bul->damage = -1;
			bul->act_wait = 0;
		}
		if (++bul->act_wait % 4 == 1)
		{
			playSound(SFX_SpinningProjectile);
			++bul->count1;
			if (bul->count1 & 1)
				createBullet(bullet_BladeSlash, bul->x, bul->y, dirLeft);
			else
				createBullet(bullet_BladeSlash, bul->x, bul->y, dirRight);
		}
		if (++bul->count1 == 5)
			bul->bbits &= 0xFFFFFFFB;
		if (bul->count1 > bul->life_count)
		{
			bul->cond = 0;
			createCaret(bul->x, bul->y, 3, 0);
			return;
		}
		break;
	case(stationary):
		bul->xm = 0;
		bul->ym = 0;
		++bul->act_wait;
		if (random(-1, 1) == 0)
		{
			playSound(SFX_SpinningProjectile);
			if (random(0, 1) & 1)
				createBullet(bullet_BladeSlash, bul->x + (random(-64, 64) << 9), bul->y + (random(-64, 64) << 9), dirLeft);
			else
				createBullet(bullet_BladeSlash, bul->x + (random(-64, 64) << 9), bul->y + (random(-64, 64) << 9), dirRight);
		}
		if (bul->act_wait > 50)
			bul->cond = 0;
		break;
	}

	bul->x += bul->xm;
	bul->y += bul->ym;

	if (bul->ani_no == 0)
	{
		bul->rect.left = 272 + ((bul->direct/3)*24);
		if (bul->direct % 2 == 0)
			bul->rect.top = (bul->direct / 2) * 24;
		else
			bul->rect.top = 48;
		bul->rect.right = bul->rect.left + 24;
		bul->rect.bottom = bul->rect.top + 24;
	}
	else
	{
		bul->rect.left = 296;
		bul->rect.top = (bul->direct / 2) * 24;
		bul->rect.right = bul->rect.left + 24;
		bul->rect.bottom = bul->rect.top + 24;
	}

	if (bul->act_wait & 1)
		bul->rect.right = bul->rect.left;
}

void actBulletSlash(bullet *bul)
{
	if (bul->act_no)
	{
		if (bul->act_no != 1)
			return;
	}
	else
	{
		bul->act_no = 1;
		bul->y -= 6144;
		if (bul->direct)
			bul->x = bul->x - 0x2000;
		else
			bul->x = bul->x + 0x2000;
	}

	if (++bul->ani_wait > 2)
	{
		bul->ani_wait = 0;
		++bul->ani_no;
	}
	if (bul->direct)
		bul->x = bul->x + 1024;
	else
		bul->x = bul->x - 1024;
	bul->y += 1024;

	if (bul->ani_no == 1)
		bul->damage = 2;
	else
		bul->damage = 1;
	if (bul->ani_no > 4)
		bul->cond = 0;
	else
	{
		bul->rect.left = bul->ani_no * 24;
		bul->rect.top = 64 + ((bul->direct / 2) * 24);
		bul->rect.right = bul->rect.left + 24;
		bul->rect.bottom = bul->rect.top + 24;
	}
}
