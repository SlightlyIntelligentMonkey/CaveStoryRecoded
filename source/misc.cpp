#include "bullet.h"
#include "misc.h"

//egg corridor spike
void actBulletDrop(bullet *bul)
{
	if (++bul->act_wait > 2)
		bul->cond = 0;
	bul->rect.left = 0;
	bul->rect.top = 0;
	bul->rect.right = 0;
	bul->rect.bottom = 0;
	return;
}

//debug weapon
void actBulletEnemyClear(bullet *bul)
{
	if (++bul->count1 <= bul->life_count)
	{
		bul->damage = 10000;
		bul->enemyXL = 819200;
		bul->enemyYL = 819200;
	}
	else
	{
		bul->cond = 0;
	}
	return;
}

//whimsical stars
void actBulletStar(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
		bul->cond = 0;
	return;
}