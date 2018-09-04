#include "bullet.h"
#include "missile.h"
#include "caret.h"
#include "mathUtils.h"
#include "sound.h"
#include "npc.h"


// -- missile launcher bullets -- //

void actBulletMissileLauncher1(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	//checks if its hit anything
	bool bHit = false;
	if (bul->life != 10)
		bHit = true;
	if (bul->direct == dirLeft && (bul->flag & leftWall || bul->flag & 0x80 || bul->flag & 0x20))
		bHit = true;
	if (bul->direct == dirRight && (bul->flag & rightWall || bul->flag & 0x40 || bul->flag & 0x10))
		bHit = true;
	if (bul->direct == dirUp && bul->flag & ceiling)
		bHit = true;
	if (bul->direct == dirDown && bul->flag & ground)
		bHit = true;

	if (bHit)
	{
		createBullet(bul->code_bullet + 3, bul->x, bul->y, 0, 6);
		bul->cond = 0;
		return;
	}

	if (!bul->act_no)
	{
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -256;
			bul->ym = 0;
			break;
		case(dirUp):
			bul->ym = -256;
			bul->xm = 0;
			break;
		case(dirRight):
			bul->xm = 256;
			bul->ym = 0;
			break;
		case(dirDown):
			bul->ym = 256;
			bul->xm = 0;
			break;
		}
	}

	switch (bul->direct)
	{
	case(dirLeft):
		bul->xm -= 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x + 4096, bul->y, 7, 2);
		}
		break;
	case(dirUp):
		bul->ym -= 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y + 4096, 7, 3);
		}
		break;
	case(dirRight):
		bul->xm += 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x - 4096, bul->y, 7, 0);
		}
		break;
	case(dirDown):
		bul->ym += 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y - 4096, 7, 1);
		}
		break;
	}

	if (bul->xm < -2560)
		bul->xm = -2560;
	if (bul->xm > 2560)
		bul->xm = 2560;
	if (bul->ym < -2560)
		bul->ym = -2560;
	if (bul->ym > 2560)
		bul->ym = 2560;


	bul->x += bul->xm;
	bul->y += bul->ym;
	bul->rect.left = bul->direct*16;
	bul->rect.top = 0;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = bul->rect.top + 16;
	return;
}

void actBulletMissileLauncher2(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	//checks if its hit anything
	bool bHit = false;
	if (bul->life != 10)
		bHit = true;
	if (bul->direct == dirLeft && (bul->flag & leftWall || bul->flag & 0x80 || bul->flag & 0x20))
		bHit = true;
	if (bul->direct == dirRight && (bul->flag & rightWall || bul->flag & 0x40 || bul->flag & 0x10))
		bHit = true;
	if (bul->direct == dirUp && bul->flag & ceiling)
		bHit = true;
	if (bul->direct == dirDown && bul->flag & ground)
		bHit = true;

	if (bHit)
	{
		createBullet(bul->code_bullet + 3, bul->x, bul->y, 0, 6);
		bul->cond = 0;
		return;
	}

	if (!bul->act_no)
	{
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -256;
			bul->ym = 0;
			break;
		case(dirUp):
			bul->ym = -256;
			bul->xm = 0;
			break;
		case(dirRight):
			bul->xm = 256;
			bul->ym = 0;
			break;
		case(dirDown):
			bul->ym = 256;
			bul->xm = 0;
			break;
		}
	}

	switch (bul->direct)
	{
	case(dirLeft):
		bul->xm -= 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x + 4096, bul->y, 7, 2);
		}
		break;
	case(dirUp):
		bul->ym -= 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y + 4096, 7, 3);
		}
		break;
	case(dirRight):
		bul->xm += 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x - 4096, bul->y, 7, 0);
		}
		break;
	case(dirDown):
		bul->ym += 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y - 4096, 7, 1);
		}
		break;
	}

	if (bul->xm < -2560)
		bul->xm = -2560;
	if (bul->xm > 2560)
		bul->xm = 2560;
	if (bul->ym < -2560)
		bul->ym = -2560;
	if (bul->ym > 2560)
		bul->ym = 2560;

	bul->x += bul->xm;
	bul->y += bul->ym;
	bul->rect.left = bul->direct*16;
	bul->rect.top = 16;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = bul->rect.top + 16;
	return;
}

void actBulletMissileLauncher3(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	//checks if its hit anything
	bool bHit = false;
	if (bul->life != 10)
		bHit = true;
	if (bul->direct == dirLeft && (bul->flag & leftWall || bul->flag & 0x80 || bul->flag & 0x20))
		bHit = true;
	if (bul->direct == dirRight && (bul->flag & rightWall || bul->flag & 0x40 || bul->flag & 0x10))
		bHit = true;
	if (bul->direct == dirUp && bul->flag & ceiling)
		bHit = true;
	if (bul->direct == dirDown && bul->flag & ground)
		bHit = true;

	if (bHit)
	{
		createBullet(bul->code_bullet + 3, bul->x, bul->y, 0, 6);
		bul->cond = 0;
		return;
	}

	if (!bul->act_no)
	{
		bul->act_no = 1;

		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -256;
			bul->ym = random(-512, 512);
			bul->tgt_y = bul->y;
			break;
		case(dirUp):
			bul->ym = -256;
			bul->xm = random(-512, 512);
			bul->tgt_x = bul->x;
			break;
		case(dirRight):
			bul->xm = 256;
			bul->ym = random(-512, 512);
			bul->tgt_y = bul->y;
			break;
		case(dirDown):
			bul->ym = 256;
			bul->xm = random(-512, 512);
			bul->tgt_x = bul->x;
			break;
		}
	}


	if (bul->ani_wait++ % 3 == 1) 
	{
		bul->ani_no = 64;
	}
	else if (bul->ani_wait < 1)
	{
		bul->ani_no = 128;
	}
	else if (bul->ani_wait == 2)
	{
		bul->ani_no = 51;
	}

	switch (bul->direct)
	{
	case(dirLeft):
		bul->xm -= bul->ani_no;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x + 4096, bul->y, 7, 2);
		}
		if (bul->y >= bul->tgt_y)
			bul->ym -= 32;
		else
			bul->ym += 32;
		break;
	case(dirUp):
		bul->ym -= bul->ani_no;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y + 4096, 7, 3);
		}
		if (bul->x >= bul->tgt_x)
			bul->xm -= 32;
		else
			bul->xm += 32;
		break;
	case(dirRight):
		bul->xm += bul->ani_no;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x - 4096, bul->y, 7, 0);
		}
		if (bul->y >= bul->tgt_y)
			bul->ym -= 32;
		else
			bul->ym += 32;
		break;
	case(dirDown):
		bul->ym += bul->ani_no;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y - 4096, 7, 1);
		}
		if (bul->x >= bul->tgt_x)
			bul->xm -= 32;
		else
			bul->xm += 32;
		break;
	}

	if (bul->xm < -2560)
		bul->xm = -2560;
	if (bul->xm > 2560)
		bul->xm = 2560;
	if (bul->ym < -2560)
		bul->ym = -2560;
	if (bul->ym > 2560)
		bul->ym = 2560;


	bul->x += bul->xm;
	bul->y += bul->ym;
	bul->rect.left = bul->direct*16;
	bul->rect.top = 32;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = bul->rect.top + 16;
	return;
}

void actBulletBoom1(bullet *bul)
{
	if (bul->act_no)
	{
		if (bul->act_no != 1)
			return;
	}
	else
	{
		bul->act_wait = 10;
		bul->act_no = 1;
		playSound(SFX_MissileImpact);
	}


	if (!(bul->act_wait % 3))
	{
		createSmokeLeft(bul->x + (random(-16, 16) << 9), bul->y + (random(-16, 16) << 9), bul->enemyXL, 2);
	}

	if (--bul->act_wait < 0)
		bul->cond = 0;
	return;
}

void actBulletBoom2(bullet *bul)
{
	if (bul->act_no)
	{
		if (bul->act_no != 1)
			return;
	}
	else
	{
		bul->act_wait = 15;
		bul->act_no = 1;
		playSound(SFX_MissileImpact);
	}

	if (!(bul->act_wait % 3))
	{
		createSmokeLeft(bul->x + (random(-32, 32) << 9), bul->y + (random(-32, 32) << 9), bul->enemyXL, 2);
	}

	if (--bul->act_wait < 0)
		bul->cond = 0;
	return;
}

void actBulletBoom3(bullet *bul)
{
	if (bul->act_no)
	{
		if (bul->act_no != 1)
			return;
	}
	else
	{
		bul->act_wait = 5;
		bul->act_no = 1;
		playSound(SFX_MissileImpact);
	}

	if (!(bul->act_wait % 3))
	{
		createSmokeLeft(bul->x + (random(-40, 40) << 9), bul->y + (random(-40, 40) << 9), bul->enemyXL, 2);
	}

	if (--bul->act_wait < 0)
		bul->cond = 0;
	return;
}


// -- super missile launcher bullets -- //

void actBulletSuperMissileLauncher1(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	//checks if its hit anything
	bool bHit = false;
	if (bul->life != 10)
		bHit = true;
	if (bul->direct == dirLeft && (bul->flag & leftWall || bul->flag & 0x80 || bul->flag & 0x20))
		bHit = true;
	if (bul->direct == dirRight && (bul->flag & rightWall || bul->flag & 0x40 || bul->flag & 0x10))
		bHit = true;
	if (bul->direct == dirUp && bul->flag & ceiling)
		bHit = true;
	if (bul->direct == dirDown && bul->flag & ground)
		bHit = true;

	if (bHit)
	{
		createBullet(bul->code_bullet + 3, bul->x, bul->y, 0, 11);
		bul->cond = 0;
		return;
	}

	if (!bul->act_no)
	{
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -512;
			bul->ym = 0;
			break;
		case(dirUp):
			bul->ym = -512;
			bul->xm = 0;
			break;
		case(dirRight):
			bul->xm = 512;
			bul->ym = 0;
			break;
		case(dirDown):
			bul->ym = 512;
			bul->xm = 0;
			break;
		}
	}

	switch (bul->direct)
	{
	case(dirLeft):
		bul->xm -= 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x + 4096, bul->y, 7, 2);
		}
		break;
	case(dirUp):
		bul->ym -= 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y + 4096, 7, 3);
		}
		break;
	case(dirRight):
		bul->xm += 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x - 4096, bul->y, 7, 0);
		}
		break;
	case(dirDown):
		bul->ym += 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y - 4096, 7, 1);
		}
		break;
	}

	if (bul->xm < -5120)
		bul->xm = -5120;
	if (bul->xm > 5120)
		bul->xm = 5120;
	if (bul->ym < -5120)
		bul->ym = -5120;
	if (bul->ym > 5120)
		bul->ym = 5120;


	bul->x += bul->xm;
	bul->y += bul->ym;
	bul->rect.left = 120 + (bul->direct * 16);
	bul->rect.top = 96;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = bul->rect.top + 16;
	return;
}

void actBulletSuperMissileLauncher2(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	//checks if its hit anything
	bool bHit = false;
	if (bul->life != 10)
		bHit = true;
	if (bul->direct == dirLeft && (bul->flag & leftWall || bul->flag & 0x80 || bul->flag & 0x20))
		bHit = true;
	if (bul->direct == dirRight && (bul->flag & rightWall || bul->flag & 0x40 || bul->flag & 0x10))
		bHit = true;
	if (bul->direct == dirUp && bul->flag & ceiling)
		bHit = true;
	if (bul->direct == dirDown && bul->flag & ground)
		bHit = true;

	if (bHit)
	{
		createBullet(bul->code_bullet + 3, bul->x, bul->y, 0, 11);
		bul->cond = 0;
		return;
	}

	if (!bul->act_no)
	{
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -512;
			bul->ym = 0;
			break;
		case(dirUp):
			bul->ym = -512;
			bul->xm = 0;
			break;
		case(dirRight):
			bul->xm = 512;
			bul->ym = 0;
			break;
		case(dirDown):
			bul->ym = 512;
			bul->xm = 0;
			break;
		}
	}

	switch (bul->direct)
	{
	case(dirLeft):
		bul->xm -= 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x + 4096, bul->y, 7, 2);
		}
		break;
	case(dirUp):
		bul->ym -= 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y + 4096, 7, 3);
		}
		break;
	case(dirRight):
		bul->xm += 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x - 4096, bul->y, 7, 0);
		}
		break;
	case(dirDown):
		bul->ym += 128;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y - 4096, 7, 1);
		}
		break;
	}

	if (bul->xm < -5120)
		bul->xm = -5120;
	if (bul->xm > 5120)
		bul->xm = 5120;
	if (bul->ym < -5120)
		bul->ym = -5120;
	if (bul->ym > 5120)
		bul->ym = 5120;

	bul->x += bul->xm;
	bul->y += bul->ym;
	bul->rect.left = 184 + (bul->direct * 16);
	bul->rect.top = 96;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = bul->rect.top + 16;
	return;
}

void actBulletSuperMissileLauncher3(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	//checks if its hit anything
	bool bHit = false;
	if (bul->life != 10)
		bHit = true;
	if (bul->direct == dirLeft && (bul->flag & leftWall || bul->flag & 0x80 || bul->flag & 0x20))
		bHit = true;
	if (bul->direct == dirRight && (bul->flag & rightWall || bul->flag & 0x40 || bul->flag & 0x10))
		bHit = true;
	if (bul->direct == dirUp && bul->flag & ceiling)
		bHit = true;
	if (bul->direct == dirDown && bul->flag & ground)
		bHit = true;

	if (bHit)
	{
		createBullet(bul->code_bullet + 3, bul->x, bul->y, 0, 11);
		bul->cond = 0;
		return;
	}

	if (!bul->act_no)
	{
		bul->act_no = 1;

		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -256;
			bul->ym = random(-512, 512);
			bul->tgt_y = bul->y;
			break;
		case(dirUp):
			bul->ym = -256;
			bul->xm = random(-512, 512);
			bul->tgt_x = bul->x;
			break;
		case(dirRight):
			bul->xm = 256;
			bul->ym = random(-512, 512);
			bul->tgt_y = bul->y;
			break;
		case(dirDown):
			bul->ym = 256;
			bul->xm = random(-512, 512);
			bul->tgt_x = bul->x;
			break;
		}
	}


	if (bul->ani_wait++ % 3 == 1)
	{
		bul->ani_no = 256;
	}
	else if (bul->ani_wait < 1)
	{
		bul->ani_no = 512;
	}
	else if (bul->ani_wait == 2)
	{
		bul->ani_no = 170;
	}

	switch (bul->direct)
	{
	case(dirLeft):
		bul->xm -= bul->ani_no;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x + 4096, bul->y, 7, 2);
		}
		if (bul->y >= bul->tgt_y)
			bul->ym -= 64;
		else
			bul->ym += 64;
		break;
	case(dirUp):
		bul->ym -= bul->ani_no;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y + 4096, 7, 3);
		}
		if (bul->x >= bul->tgt_x)
			bul->xm -= 64;
		else
			bul->xm += 64;
		break;
	case(dirRight):
		bul->xm += bul->ani_no;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x - 4096, bul->y, 7, 0);
		}
		if (bul->y >= bul->tgt_y)
			bul->ym -= 64;
		else
			bul->ym += 64;
		break;
	case(dirDown):
		bul->ym += bul->ani_no;
		if (bul->count2++ > 2)
		{
			bul->count2 = 0;
			createCaret(bul->x, bul->y - 4096, 7, 1);
		}
		if (bul->x >= bul->tgt_x)
			bul->xm -= 64;
		else
			bul->xm += 64;
		break;
	}

	if (bul->xm < -5120)
		bul->xm = -5120;
	if (bul->xm > 5120)
		bul->xm = 5120;
	if (bul->ym < -5120)
		bul->ym = -5120;
	if (bul->ym > 5120)
		bul->ym = 5120;


	bul->x += bul->xm;
	bul->y += bul->ym;
	bul->rect.left = 120 + (bul->direct * 16);
	bul->rect.top = 96;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = bul->rect.top + 16;
	return;
}

void actBulletSuperBoom1(bullet *bul)
{
	if (bul->act_no)
	{
		if (bul->act_no != 1)
			return;
	}
	else
	{
		bul->act_wait = 10;
		bul->act_no = 1;
		playSound(SFX_MissileImpact);
	}


	if (!(bul->act_wait % 3))
	{
		createSmokeUp(bul->x + (random(-16, 16) << 9), bul->y + (random(-16, 16) << 9), bul->enemyXL, 2);
	}

	if (--bul->act_wait < 0)
		bul->cond = 0;
	return;
}

void actBulletSuperBoom2(bullet *bul)
{
	if (bul->act_no)
	{
		if (bul->act_no != 1)
			return;
	}
	else
	{
		bul->act_wait = 14;
		bul->act_no = 1;
		playSound(SFX_MissileImpact);
	}

	if (!(bul->act_wait % 3))
	{
		createSmokeUp(bul->x + (random(-32, 32) << 9), bul->y + (random(-32, 32) << 9), bul->enemyXL, 2);
	}

	if (--bul->act_wait < 0)
		bul->cond = 0;
	return;
}

void actBulletSuperBoom3(bullet *bul)
{
	if (bul->act_no)
	{
		if (bul->act_no != 1)
			return;
	}
	else
	{
		bul->act_wait = 6;
		bul->act_no = 1;
		playSound(SFX_MissileImpact);
	}

	if (!(bul->act_wait % 3))
	{
		createSmokeUp(bul->x + (random(-40, 40) << 9), bul->y + (random(-40, 40) << 9), bul->enemyXL, 2);
	}

	if (--bul->act_wait < 0)
		bul->cond = 0;
	return;
}