#pragma once
#include "common.h"

//Weapon stuff
struct WEAPONEXP
{
	int exp[3];
};

struct WEAPON
{
	int id;

	int level;
	int exp;

	int ammo;
	int maxAmmo;
};

//Bullet things
enum bulletFlags {
	bullet_ignoreSolid = 0x4,
	bullet_goThroughWalls = 0x8,
	bullet_breakBlocks = 0x20,
	bullet_pierceBlocks = 0x40,
	bullet_alive = 0x80,
};
