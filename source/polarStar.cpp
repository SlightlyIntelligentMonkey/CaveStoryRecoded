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
			case 0:
				bul->enemyYL = 0x400;
				bul->xm = -0x1000;
				break;
			case 1:
				bul->enemyXL = 0x400;
				bul->ym = -0x1000;
				break;
			case 2:
				bul->enemyYL = 0x400;
				bul->xm = 0x1000;
				break;
			case 3:
				bul->enemyXL = 0x400;
				bul->ym = 0x1000;
				break;
			default:
				break;
			}
		}

		if (bul->direct != 1 && bul->direct != 3)
			bul->rect = { 128, 32, 144, 48 };
		else
			bul->rect = { 144, 32, 160, 48 };
	}
	else
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
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
			case 0:
				bul->enemyYL = 0x800;
				bul->xm = -0x1000;
				break;
			case 1:
				bul->enemyXL = 0x800;
				bul->ym = -0x1000;
				break;
			case 2:
				bul->enemyYL = 0x800;
				bul->xm = 0x1000;
				break;
			case 3:
				bul->enemyXL = 0x800;
				bul->ym = 0x1000;
				break;
			default:
				break;
			}
		}

		if (bul->direct != 1 && bul->direct != 3)
			bul->rect = { 160, 32, 176, 48 };
		else
			bul->rect = { 176, 32, 192, 48 };
	}
	else
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
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
			case 0:
				bul->xm = -0x1000;
				break;
			case 1:
				bul->ym = -0x1000;
				break;
			case 2:
				bul->xm = 0x1000;
				break;
			case 3:
				bul->ym = 0x1000;
				break;
			default:
				break;
			}
		}

		if (bul->direct != 1 && bul->direct != 3)
			bul->rect = { 128, 48, 144, 64 };
		else
			bul->rect = { 144, 48, 160, 64 };
	}
	else
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
	}
}
