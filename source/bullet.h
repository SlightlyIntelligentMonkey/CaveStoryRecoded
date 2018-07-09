#pragma once
#include "common.h"
#include "weapons.h"

class bullet
{
public: //Variables
	int flag;
	int type;
	int flags;
	int cond;

	int x;
	int y;

	int direction;

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

public: //Functions
	void init(int id, int setX, int setY, int dir);

	void update();
};

extern std::vector<bullet> bullets;