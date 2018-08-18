#include "spur.h"

void actBulletSpurLevel2(bullet *bul)
{
	if (++bul->count1 <= bul->life_count)
	{
		bul->damage = 10000;
		bul->enemyXL = 0xC8000;
		bul->enemyYL = 0xC8000;
	}
	else
		bul->cond = 0;
}
void actBulletSpurLevel3(bullet *bul)
{
	if (++bul->count1 > bul->life_count)
		bul->cond = 0;
}
