#include "polarStar.h"

void actBulletPolarStar1(bullet *bul)
{
	if (++bul->count1 <= bul->life_count)
	{
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
			case dirLeft:
				bul->enemyYL = 0x400;
				bul->xm = -0x1000;
				break;
			case dirUp:
				bul->enemyXL = 0x400;
				bul->ym = -0x1000;
				break;
			case dirRight:
				bul->enemyYL = 0x400;
				bul->xm = 0x1000;
				break;
			case dirDown:
				bul->enemyXL = 0x400;
				bul->ym = 0x1000;
				break;
			default:
				break;
			}
		}

		if (bul->direct != dirUp && bul->direct != dirDown)
			bul->rect = { 128, 32, 144, 48 };
		else
			bul->rect = { 144, 32, 160, 48 };
	}
	else
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, effect_Star, 0);
	}
}

void actBulletPolarStar2(bullet *bul)
{
	if (++bul->count1 <= bul->life_count)
	{
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
			case dirLeft:
				bul->enemyYL = 0x800;
				bul->xm = -0x1000;
				break;
			case dirUp:
				bul->enemyXL = 0x800;
				bul->ym = -0x1000;
				break;
			case dirRight:
				bul->enemyYL = 0x800;
				bul->xm = 0x1000;
				break;
			case dirDown:
				bul->enemyXL = 0x800;
				bul->ym = 0x1000;
				break;
			default:
				break;
			}
		}

		if (bul->direct != dirUp && bul->direct != dirDown)
			bul->rect = { 160, 32, 176, 48 };
		else
			bul->rect = { 176, 32, 192, 48 };
	}
	else
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, effect_Star, 0);
	}
}

void actBulletPolarStar3(bullet *bul)
{
	if (++bul->count1 <= bul->life_count)
	{
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
			case dirLeft:
				bul->xm = -0x1000;
				break;
			case dirUp:
				bul->ym = -0x1000;
				break;
			case dirRight:
				bul->xm = 0x1000;
				break;
			case dirDown:
				bul->ym = 0x1000;
				break;
			default:
				break;
			}
		}

		if (bul->direct != dirUp && bul->direct != dirDown)
			bul->rect = { 128, 48, 144, 64 };
		else
			bul->rect = { 144, 48, 160, 64 };
	}
	else
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, effect_Star, 0);
	}
}
