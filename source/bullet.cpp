#include "bullet.h"

std::vector<bullet> bullets(0);

BULLETSTATS bulletTable[] =
{
	//null bullet
	{ 0, 0, 0, 0, 0, 0, 0, 0,{ 0, 0, 0, 0 } },

	//snake
	{ 4, 1, 20, 36, 4, 4, 2, 2,{ 8, 8, 8, 8 } },
	{ 6, 1, 23, 36, 4, 4, 2, 2,{ 8, 8, 8, 8 } },
	{ 8, 1, 30, 36, 4, 4, 2, 2,{ 8, 8, 8, 8 } },

	//polarstar
	{ 1, 1, 8, 32, 6, 6, 2, 2,{ 8, 8, 8, 8 } },
	{ 2, 1, 12, 32, 6, 6, 2, 2,{ 8, 8, 8, 8 } },
	{ 4, 1, 16, 32, 6, 6, 2, 2,{ 8, 8, 8, 8 } },

	//fireball
	{ 2, 2, 100, 8, 8, 16, 4, 2,{ 8, 8, 8, 8 } },
	{ 3, 2, 100, 8, 4, 4, 4, 2,{ 8, 8, 8, 8 } },
	{ 3, 2, 100, 8, 4, 4, 4, 2,{ 8, 8, 8, 8 } },

	//machine gun
	{ 2, 1, 20, 32, 2, 2, 2, 2,{ 8, 8, 8, 8 } },
	{ 4, 1, 20, 32, 2, 2, 2, 2,{ 8, 8, 8, 8 } },
	{ 6, 1, 20, 32, 2, 2, 2, 2,{ 8, 8, 8, 8 } },

	//missile
	{ 0, 10, 50, 40, 2, 2, 2, 2,{ 8, 8, 8, 8 } },
	{ 0, 10, 70, 40, 4, 4, 4, 4,{ 8, 8, 8, 8 } },
	{ 0, 10, 90, 40, 4, 4, 0, 0,{ 8, 8, 8, 8 } },

	//missile explosion
	{ 1, 100, 100, 20, 16, 16, 0, 0,{ 0, 0, 0, 0 } },
	{ 1, 100, 100, 20, 16, 16, 0, 0,{ 0, 0, 0, 0 } },
	{ 1, 100, 100, 20, 16, 16, 0, 0,{ 0, 0, 0, 0 } },

	//bubbler
	{ 1, 1, 20, 8, 2, 2, 2, 2,{ 4, 4, 4, 4 } },
	{ 2, 1, 20, 8, 2, 2, 2, 2,{ 4, 4, 4, 4 } },
	{ 2, 1, 20, 8, 4, 4, 4, 4,{ 4, 4, 4, 4 } },

	//bubber pop projectile
	{ 3, 1, 32, 32, 2, 2, 2, 2,{ 4, 4, 4, 4 } },

	//blade slashes
	{ 0, 100, 0, 36, 8, 8, 8, 8,{ 12, 12, 12, 12 } },

	//egg corridor spike (FUCK YOU PIXEL)
	{ 127, 1, 2, 4, 8, 4, 8, 4,{ 0, 0, 0, 0 } },

	//blade
	{ 15, 1, 30, 36, 8, 8, 4, 2,{ 8, 8, 8, 8 } },
	{ 6, 3, 18, 36, 10, 10, 4, 2,{ 12, 12, 12, 12 } },
	{ 1, 100, 30, 36, 6, 6, 4, 4,{ 12, 12, 12, 12 } },

	//super missile
	{ 0, 10, 30, 40, 2, 2, 2, 2,{ 8, 8, 8, 8 } },
	{ 0, 10, 40, 40, 4, 4, 4, 4,{ 8, 8, 8, 8 } },
	{ 0, 10, 40, 40, 4, 4, 0, 0,{ 8, 8, 8, 8 } },

	//super missile explosion
	{ 2, 100, 100, 20, 12, 12, 0, 0,{ 0, 0, 0, 0 } },
	{ 2, 100, 100, 20, 12, 12, 0, 0,{ 0, 0, 0, 0 } },
	{ 2, 100, 100, 20, 12, 12, 0, 0,{ 0, 0, 0, 0 } },

	//nemesis
	{ 4, 4, 20, 32, 4, 4, 3, 3,{ 8, 8, 24, 8 } },
	{ 4, 2, 20, 32, 2, 2, 2, 2,{ 8, 8, 24, 8 } },
	{ 1, 1, 20, 32, 2, 2, 2, 2,{ 8, 8, 24, 8 } },

	//spur
	{ 4, 4, 30, 64, 6, 6, 3, 3,{ 8, 8, 8, 8 } },
	{ 8, 8, 30, 64, 6, 6, 3, 3,{ 8, 8, 8, 8 } },
	{ 12, 12, 30, 64, 6, 6, 3, 3,{ 8, 8, 8, 8 } },

	//spur trail
	{ 3, 100, 30, 32, 6, 6, 3, 3,{ 4, 4, 4, 4 } },
	{ 6, 100, 30, 32, 6, 6, 3, 3,{ 4, 4, 4, 4 } },
	{ 11, 100, 30, 32, 6, 6, 3, 3,{ 4, 4, 4, 4 } },

	//curly nemesis
	{ 4, 4, 20, 32, 4, 4, 3, 3,{ 8, 8, 24, 8 } },

	//clear all
	{ 0, 4, 4, 4, 0, 0, 0, 0,{ 0, 0, 0, 0 } },

	//whimsical star
	{ 1, 1, 1, 36, 1, 1, 1, 1,{ 1, 1, 1, 1 } }
};

//CLASS
void bullet::init(int id, int setX, int setY, int dir)
{
	memset(this, 0, sizeof(*this));

	type = id;
	cond = bullet_alive;

	x = setX;
	y = setY;

	direction = dir;

	damage = bulletTable[id].damage;

	hits = bulletTable[id].hits;
	range = bulletTable[id].range;

	flags = bulletTable[id].flags;

	enemyWidth = bulletTable[id].enemyWidth << 9;
	enemyHeight = bulletTable[id].enemyHeight << 9;

	blockWidth = bulletTable[id].blockWidth << 9;
	blockHeight = bulletTable[id].blockHeight << 9;

	frameRect.right = bulletTable[id].view.right << 9;
	frameRect.left = bulletTable[id].view.left << 9;
	frameRect.top = bulletTable[id].view.top << 9;
	frameRect.bottom = bulletTable[id].view.bottom << 9;
}

void bullet::update()
{
	//bulletActs[type](this);
}

//Global functions