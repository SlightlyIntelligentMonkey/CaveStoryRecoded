#pragma once
#include "common.h"

#include <vector>

enum bulletFlags
{
	bullet_ignoreSolid = 0x4,
	bullet_goThroughWalls = 0x8,
	bullet_breakBlocks = 0x20,
	bullet_pierceBlocks = 0x40,
	bullet_alive = 0x80,
};

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

public:
	void init(int setCode, int setX, int setY, uint8_t setDir);

	void update();
	void draw();
};

void createBullet(int setCode, int setX, int setY, int setDir);
void updateBullets();
void drawBullets();

using bulletAct = void(*)(bullet *);
extern std::vector<bullet> bullets;
