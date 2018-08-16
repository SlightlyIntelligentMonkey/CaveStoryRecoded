#include "spur.h"

void actBulletSpurLevel2(bullet *bullet)
{
	if (++bullet->count1 <= bullet->life_count)
	{
		bullet->damage = 10000;
		bullet->enemyXL = 0xC8000;
		bullet->enemyYL = 0xC8000;
	}
	else
		bullet->cond = 0;
}
void actBulletSpurLevel3(bullet *bullet)
{
	if (++bullet->count1 > bullet->life_count)
		bullet->cond = 0;
}
