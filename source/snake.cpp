#include "bullet.h"
#include "snake.h"
#include "caret.h"
#include "mathUtils.h"
#include "npc.h"

void actBulletSnake1(bullet *bul)
{
	//checks to see if bullet is past range
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	if (bul->act_no)
	{
		//moves bullet
		bul->x += bul->xm;
		bul->y += bul->ym;
	}
	else
	{	//sets up bullet
		bul->ani_no = random(0, 2);
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
	
	if (++bul->ani_wait > 0)
	{
		bul->ani_wait = 0;
		if (bul->direct == dirLeft)
			bul->ani_no--;
		else
			bul->ani_no++;
		bul->ani_no &= 0x3;
	}


	bul->rect.left = 120 + (16 * (bul->ani_no % 2));
	bul->rect.top = 64 + (16 * (bul->ani_no / 2));
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = bul->rect.top + 16;
	if (bul->rect.left == 136 && bul->rect.top == 80)
		bul->rect.bottom = 80;
}

uint8_t snakeNextWiggle = 0;
void actBulletSnake2(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	if (!bul->act_no)
	{
		bul->ani_no = random(0, 2);
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -512;
			break;
		case(dirUp):
			bul->ym = -512;
			break;
		case(dirRight):
			bul->xm = 512;
			break;
		case(dirDown):
			bul->ym = 512;
			break;
		}
		snakeNextWiggle++;
		snakeNextWiggle &= 1;
		if (bul->direct == dirLeft || bul->direct == dirRight)
		{
			if (snakeNextWiggle)
				bul->ym = 1024;
			else
				bul->ym = -1024;
		}
		else if (bul->direct == dirUp || bul->direct == dirDown)
		{
			if (snakeNextWiggle)
				bul->xm = 1024;
			else
				bul->xm = -1024;
		}
	}
	else
	{
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm -= 128;
			break;
		case(dirUp):
			bul->ym -= 128;
			break;
		case(dirRight):
			bul->xm += 128;
			break;
		case(dirDown):
			bul->ym += 128;
			break;
		}

		//wiggles
		bul->count2++;
		if (bul->direct == dirLeft || bul->direct == dirRight)
		{
			if (bul->count2 % 5 == 2)
			{
				if (bul->ym >= 0)
					bul->ym = -1024;
				else
					bul->ym = 1024;
			}
		}
		else if (bul->direct == dirUp || bul->direct == dirDown)
		{
			if (bul->count2 % 5 == 2)
			{
				if (bul->xm >= 0)
					bul->xm = -1024;
				else
					bul->xm = 1024;
			}
		}

		bul->x += bul->xm;
		bul->y += bul->ym;
	}

	if (bul->ani_wait++ % 2 == 1)
	{
		bul->ani_no++;
	}
	if (bul->ani_no > 2)
		bul->ani_no = 0;
	bul->rect.left = 192 + (16*bul->ani_no);
	bul->rect.top = 16;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = bul->rect.top + 16;

	createNpc(NPC_ProjectileFireballSnakeTrail, bul->x, bul->y, 0, -512, bul->ani_no);
}

void actBulletSnake3(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
		return;
	}

	if (!bul->act_no)
	{
		bul->ani_no = random(0, 2);
		bul->act_no = 1;
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm = -512;
			break;
		case(dirUp):
			bul->ym = -512;
			break;
		case(dirRight):
			bul->xm = 512;
			break;
		case(dirDown):
			bul->ym = 512;
			break;
		}
		snakeNextWiggle++;
		snakeNextWiggle &= 1;
		if (bul->direct == dirLeft || bul->direct == dirRight)
		{
			if (snakeNextWiggle)
				bul->ym = 1024;
			else
				bul->ym = -1024;
		}
		else if (bul->direct == dirUp || bul->direct == dirDown)
		{
			if (snakeNextWiggle)
				bul->xm = 1024;
			else
				bul->xm = -1024;
		}
	}
	else
	{
		switch (bul->direct)
		{
		case(dirLeft):
			bul->xm -= 128;
			break;
		case(dirUp):
			bul->ym -= 128;
			break;
		case(dirRight):
			bul->xm += 128;
			break;
		case(dirDown):
			bul->ym += 128;
			break;
		}

		//wiggles
		bul->count2++;
		if (bul->direct == dirLeft || bul->direct == dirRight)
		{
			if (bul->count2 % 5 == 2)
			{
				if (bul->ym >= 0)
					bul->ym = -1024;
				else
					bul->ym = 1024;
			}
		}
		else if (bul->direct == dirUp || bul->direct == dirDown)
		{
			if (bul->count2 % 5 == 2)
			{
				if (bul->xm >= 0)
					bul->xm = -1024;
				else
					bul->xm = 1024;
			}
		}

		bul->x += bul->xm;
		bul->y += bul->ym;
	}

	if (bul->ani_wait++ % 2 == 1)
	{
		bul->ani_no++;
	}
	if (bul->ani_no > 2)
		bul->ani_no = 0;
	bul->rect.left = 192 + (16 * bul->ani_no);
	bul->rect.top = 16;
	bul->rect.right = bul->rect.left + 16;
	bul->rect.bottom = bul->rect.top + 16;

	createNpc(NPC_ProjectileFireballSnakeTrail, bul->x, bul->y, 0, -512, bul->ani_no + 3);
}
