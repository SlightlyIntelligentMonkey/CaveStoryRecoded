#include "bullet.h"
#include "bubbler.h"
#include "caret.h"
#include "sound.h"
#include "player.h"
#include "mathUtils.h"
#include "input.h"

void actBulletBubbler1(bullet *bul)
{
	if (bul->flag & 0x2FF)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 2, 0);
		return;
	}
	if (!bul->act_no)
	{
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -1536;
			break;
		case(dirUp):
			bul->ym = -1536;
			break;
		case(dirRight):
			bul->xm = 1536;
			break;
		case(dirDown):
			bul->ym = 1536;
			break;
		}
	}
	switch (bul->direct)
	{
	case(dirLeft):
		bul->xm += 42;
		break;
	case(dirUp):
		bul->ym += 42;
		break;
	case(dirRight):
		bul->xm -= 42;
		break;
	case(dirDown):
		bul->ym -= 42;
		break;
	}

	bul->x += bul->xm;
	bul->y += bul->ym;

	if (++bul->act_wait > 40)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 15, 0);
	}

	if (++bul->ani_wait > 3)
	{
		bul->ani_wait = 0;
		++bul->ani_no;
	}
	if (bul->ani_no > 3)
		bul->ani_no = 3;

	bul->rect.left = 192 + (bul->ani_no*8);
	bul->rect.top = 0;
	bul->rect.right = bul->rect.left + 8;
	bul->rect.bottom = bul->rect.top + 8;
}

void actBulletBubbler2(bullet *bul)
{
	bool bDelete = false;
	if (!bul->direct && bul->flag & 1)
		bDelete = true;
	else if (bul->direct == 2 && bul->flag & 4)
		bDelete = true;
	else if (bul->direct == 1 && bul->flag & 2)
		bDelete = true;
	else if (bul->direct == 3 && bul->flag & 8)
		bDelete = true;
	if (bDelete)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 2, 0);
		return;
	}

	if (!bul->act_no)
	{
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -1536;
			bul->ym = random(-256, 256);
			break;
		case(dirUp):
			bul->ym = -1536;
			bul->xm = random(-256, 256);
			break;
		case(dirRight):
			bul->xm = 1536;
			bul->ym = random(-256, 256);
			break;
		case(dirDown):
			bul->ym = 1536;
			bul->xm = random(-256, 256);
			break;
		}
	}
	switch (bul->direct)
	{
	case(dirLeft):
		bul->xm += 16;
		break;
	case(dirUp):
		bul->ym += 16;
		break;
	case(dirRight):
		bul->xm -= 16;
		break;
	case(dirDown):
		bul->ym -= 16;
		break;
	}

	bul->x += bul->xm;
	bul->y += bul->ym;
	if (++bul->act_wait > 60)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 15, 0);
	}

	if (++bul->ani_wait > 3)
	{
		bul->ani_wait = 0;
		++bul->ani_no;
	}
	if (bul->ani_no > 3)
		bul->ani_no = 3;

	bul->rect.left = 192 + (bul->ani_no * 8);
	bul->rect.top = 8;
	bul->rect.right = bul->rect.left + 8;
	bul->rect.bottom = bul->rect.top + 8;
}

void actBulletBubbler3(bullet *bul)
{
	if (++bul->act_wait <= 100 && isKeyDown(keyShoot))
	{
		if (!bul->act_no)
		{
			switch (bul->direct)
			{
			case(dirLeft):
				bul->xm = random(-1024, -512);
				bul->ym = (random(-4, 4) << 9) / 2;
				break;
			case(dirUp):
				bul->ym = random(-1024, -512);
				bul->xm = (random(-4, 4) << 9) / 2;
				break;
			case(dirRight):
				bul->xm = random(512, 1024);
				bul->ym = (random(-4, 4) << 9) / 2;
				break;
			case(dirDown):
				bul->ym = random(128, 256);
				bul->xm = (random(-4, 4) << 9) / 2;
				break;
			}
			bul->act_no = 1;
		}

		if (bul->x < currentPlayer.x)
			bul->xm += 32;
		if (bul->x > currentPlayer.x)
			bul->xm -= 32;
		if (bul->y < currentPlayer.y)
			bul->ym += 32;
		if (bul->y > currentPlayer.y)
			bul->ym -= 32;
		if (bul->xm < 0 && bul->flag & 1)
			bul->xm = 1024;
		if (bul->xm > 0 && bul->flag & 4)
			bul->xm = -1024;
		if (bul->ym < 0 && bul->flag & 2)
			bul->ym = 1024;
		if (bul->ym > 0 && bul->flag & 8)
			bul->ym = -1024;

		bul->x += bul->xm;
		bul->y += bul->ym;

		if (++bul->ani_wait > 3)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}
		if (bul->ani_no > 3)
			bul->ani_no = 3;

		bul->rect.left = 240 + ((bul->ani_no % 2) * 8);
		bul->rect.top = 16 + ((bul->ani_no / 2) * 8);
		bul->rect.right = bul->rect.left + 8;
		bul->rect.bottom = bul->rect.top + 8;
	}
	else
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 2, 0);
		playSound(SFX_BubblerShootLevel3, 1);
		if (currentPlayer.up)
		{
			createBullet(bullet_BubblerSpurs, bul->x, bul->y, 1);
		}
		else if (currentPlayer.down)
		{
			createBullet(bullet_BubblerSpurs, bul->x, bul->y, 3);
		}
		else
		{
			createBullet(bullet_BubblerSpurs, bul->x, bul->y, currentPlayer.direct);
		}
	}
}

void actBulletBubblerSpur(bullet *bul)
{
	int temp = 0;
	if (++bul->count1 > bul->life_count || bul->flag & 8)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

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
			temp = random(10, 16);
			bul->xm = (((-512 * temp) >> 31) - (temp << 9)) >> 1;
			break;
		case(dirUp):
			temp = random(10, 16);
			bul->ym = (((-512 * temp) >> 31) - (temp << 9)) >> 1;
			break;
		case(dirRight):
			bul->xm = (random(10, 16) << 9) / 2;
			break;
		case(dirDown):
			bul->ym = (random(10, 16) << 9) / 2;
			break;
		}
	}
	if (++bul->ani_wait > 1)
	{
		bul->ani_wait = 0;
		++bul->ani_no;
	}
	if (bul->ani_no > 1)
		bul->ani_no = 0;


	bul->rect.left = 224 + (bul->ani_no * 8);
	bul->rect.top = (bul->ani_no % 2) * 8;
	bul->rect.right = bul->rect.left + 8;
	bul->rect.bottom = bul->rect.top + 8;
}
