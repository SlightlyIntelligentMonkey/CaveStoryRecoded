#include "bullet.h"
#include "nemesis.h"
#include "caret.h"
#include "sound.h"
#include "mathUtils.h"
#include "npc.h"

void actBulletNemesis1(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	if (bul->act_no)
	{
		if (bul->count1 % 4 == 1)
		{
			switch (bul->direct)
			{
			case(dirLeft):
				createNpc(4, bul->x, bul->y, -512, random(-512, 512), 2);
				break;
			case(dirUp):
				createNpc(4, bul->x, bul->y, random(-512, 512), -512, 2);
				break;
			case(dirRight):
				createNpc(4, bul->x, bul->y, 512, random(-512, 512), 2);
				break;
			case(dirDown):
				createNpc(4, bul->x, bul->y, random(-512, 512), 512, 2);
				break;
			}
		}

		bul->x += bul->xm;
		bul->y += bul->ym;
	}
	else
	{
		bul->act_no = 1;
		bul->count1 = 0;

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
	if (++bul->ani_no > 1)
		bul->ani_no = 0;

	if (bul->direct == dirLeft || bul->direct == dirRight)
	{
		bul->rect.left = bul->direct * 32;
		bul->rect.top = 112 + (bul->ani_no * 16);
		bul->rect.right = bul->rect.left + 32;
		bul->rect.bottom = bul->rect.top + 16;
	}
	else
	{
		bul->rect.left = (bul->direct * 32) + (bul->ani_no * 16);
		bul->rect.top = 112;
		bul->rect.right = bul->rect.left + 16;
		bul->rect.bottom = bul->rect.top + 32;
	}

	return;
}

void actBulletNemesis2(bullet *bul)
{
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
	}
	else
	{
		bul->act_no = 1;
		bul->count1 = 0;

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
	if (++bul->ani_no > 1)
		bul->ani_no = 0;

	if (bul->direct == dirLeft || bul->direct == dirRight)
	{
		bul->rect.left = 128 + bul->direct * 32;
		bul->rect.top = 112 + (bul->ani_no * 16);
		bul->rect.right = bul->rect.left + 32;
		bul->rect.bottom = bul->rect.top + 16;
	}
	else
	{
		bul->rect.left = 128 + (bul->direct * 32) + (bul->ani_no * 16);
		bul->rect.top = 112;
		bul->rect.right = bul->rect.left + 16;
		bul->rect.bottom = bul->rect.top + 32;
	}

	return;
}

void actBulletNemesis3(bullet *bul)
{
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
	}
	else
	{
		bul->act_no = 1;
		bul->count1 = 0;

		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -4096 / 3;
			break;
		case(dirUp):
			bul->ym = -4096 / 3;
			break;
		case(dirRight):
			bul->xm = 4096 / 3;
			break;
		case(dirDown):
			bul->ym = 4096 / 3;
			break;
		}
	}
	if (++bul->ani_no > 1)
		bul->ani_no = 0;

	if (bul->direct == dirLeft || bul->direct == dirRight)
	{
		bul->rect.left = bul->direct * 32;
		bul->rect.top = 144 + (bul->ani_no * 16);
		bul->rect.right = bul->rect.left + 32;
		bul->rect.bottom = bul->rect.top + 16;
	}
	else
	{
		bul->rect.left = (bul->direct * 32) + (bul->ani_no * 16);
		bul->rect.top = 144;
		bul->rect.right = bul->rect.left + 16;
		bul->rect.bottom = bul->rect.top + 32;
	}

	return;
}