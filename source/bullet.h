#pragma once
#include "common.h"

#include <deque>

struct BULLETSTATS
{
	int8_t damage;
	int8_t life;
	int life_count;
	int bbits;

	int enemyXL;
	int enemyYL;
	int blockXL;
	int blockYL;

	RECT view;

	int weapon;
};

class bullet
{
public:
	int flag;
	int code_bullet;
	int bbits;
	int cond;

	int x;
	int y;

	int xm;
	int ym;

	int tgt_x;
	int tgt_y;

	int act_no;
	int act_wait;

	int ani_wait;
	int ani_no;
	uint8_t direct;

	RECT rect;

	int count1;
	int count2;

	int life_count;
	int damage;
	int life;

	int enemyXL;
	int enemyYL;
	int blockXL;
	int blockYL;

	RECT view;

	int weapon;
  
public:
	void init(int setCode, int setX, int setY, uint8_t setDir) ;

	void update();
	void draw();
};

extern int _empty;

void createBullet(int setCode, int setX, int setY, uint8_t setDir, int weapon = -1);
void updateBullets();
void drawBullets();

using bulletAct = void(*)(bullet *);
extern std::deque<bullet> bullets;

enum bulletFlags
{
	bullet_ignoreSolid = 0x4,
	bullet_goThroughWalls = 0x8,
	bullet_breakBlocks = 0x20,
	bullet_pierceBlocks = 0x40,
	bullet_alive = 0x80,
};

enum bulletNames
{
	bullet_SnakeLevel1 = 0x1,
	bullet_SnakeLevel2 = 0x2,
	bullet_SnakeLevel3 = 0x3,
	bullet_PolarStarLevel1 = 0x4,
	bullet_PolarStarLevel2 = 0x5,
	bullet_PolarStarLevel3 = 0x6,
	bullet_FireballLevel1 = 0x7,
	bullet_FireballLevel2 = 0x8,
	bullet_FireballLevel3 = 0x9,
	bullet_MachineGunLevel1 = 0xA,
	bullet_MachineGunLevel2 = 0xB,
	bullet_MachineGunLevel3 = 0xC,
	bullet_MissileLauncherLevel1 = 0xD,
	bullet_MissileLauncherLevel2 = 0xE,
	bullet_MissileLauncherLevel3 = 0xF,
	bullet_MissileExplosionLevel1 = 0x10,
	bullet_MissileExplosionLevel2 = 0x11,
	bullet_MissileExplosionLevel3 = 0x12,
	bullet_BubblerLevel1 = 0x13,
	bullet_BubblerLevel2 = 0x14,
	bullet_BubblerLevel3 = 0x15,
	bullet_BubblerSpurs = 0x16,
	bullet_BladeLevel1 = 0x19,
	bullet_BladeLevel2 = 0x1A,
	bullet_BladeLevel3 = 0x1B,
	bullet_BladeSlash = 0x17,
	bullet_Unused = 0x18,
	bullet_SuperMissileLauncherLevel1 = 0x1C,
	bullet_SuperMissileLauncherLevel2 = 0x1D,
	bullet_SuperMissileLauncherLevel3 = 0x1E,
	bullet_NemesisLevel1 = 0x22,
	bullet_NemesisLevel2 = 0x23,
	bullet_NemesisLevel3 = 0x24,
	bullet_NemesisRelated = 0x2B,
	bullet_ChargelessSpurLevel1 = 0x25,
	bullet_ChargelessSpurLevel2 = 0x26,
	bullet_ChargelessSpurLevel3 = 0x27,
	bullet_NonMovingSpurLevel1 = 0x28,
	bullet_NonMovingSpurLevel2 = 0x29,
	bullet_NonMovingSpurLevel3 = 0x2A,
	bullet_SpurLevel2 = 0x2C,
	bullet_SpurLevel3 = 0x2D,
};
