#pragma once
#include "common.h"

struct WEAPON
{
	int id;

	int level;
	int exp;

	int ammo;
	int maxAmmo;
};

enum bulletFlags {
	bullet_ignoreSolid = 0x4,
	bullet_goThroughWalls = 0x8,
	bullet_breakBlocks = 0x20,
	bullet_pierceBlocks = 0x40,
};

struct BULLET
{
	int flag;
	int type;
	int flags;
	int cond;

	int x;
	int y;

	int xsp;
	int ysp;

	int targetX;
	int targetY;

	int action;
	int actionWait;

	int animation;
	int animationWait;

	RECT frameRect;
	POINT offset;

	int count1;
	int count2;

	int hits;
	int range;

	int damage;
	
	int enemyWidth;
	int enemyHeight;

	int blockWidth;
	int blockHeight;
};