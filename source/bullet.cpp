#include "bullet.h"
#include "weapons.h"
#include "bulletCollision.h"
#include "render.h"
#include "game.h"

#include <string>
#include <deque>
#include <SDL_messagebox.h>
#include <cstring>

using std::string;
using std::to_string;
using std::deque;
using std::memset;

deque<bullet> bullets(0);

BULLETSTATS bulletTable[] =
{
	//null bullet
	{ 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0, 0 }, 0 },

	//snake
	{ 4, 1, 20, 36, 4, 4, 2, 2, { 8, 8, 8, 8 }, 1 },
	{ 6, 1, 23, 36, 4, 4, 2, 2, { 8, 8, 8, 8 }, 1 },
	{ 8, 1, 30, 36, 4, 4, 2, 2, { 8, 8, 8, 8 }, 1 },

	//polarstar
	{ 1, 1, 8, 32, 6, 6, 2, 2, { 8, 8, 8, 8 }, 2 },
	{ 2, 1, 12, 32, 6, 6, 2, 2, { 8, 8, 8, 8 }, 2 },
	{ 4, 1, 16, 32, 6, 6, 2, 2, { 8, 8, 8, 8 }, 2 },

	//fireball
	{ 2, 2, 100, 8, 8, 16, 4, 2, { 8, 8, 8, 8 }, 3 },
	{ 3, 2, 100, 8, 4, 4, 4, 2, { 8, 8, 8, 8 }, 3 },
	{ 3, 2, 100, 8, 4, 4, 4, 2, { 8, 8, 8, 8 }, 3 },

	//machine gun
	{ 2, 1, 20, 32, 2, 2, 2, 2, { 8, 8, 8, 8 }, 4 },
	{ 4, 1, 20, 32, 2, 2, 2, 2, { 8, 8, 8, 8 }, 4 },
	{ 6, 1, 20, 32, 2, 2, 2, 2, { 8, 8, 8, 8 }, 4 },

	//missile
	{ 0, 10, 50, 40, 2, 2, 2, 2, { 8, 8, 8, 8 }, 5 },
	{ 0, 10, 70, 40, 4, 4, 4, 4, { 8, 8, 8, 8 }, 5 },
	{ 0, 10, 90, 40, 4, 4, 0, 0, { 8, 8, 8, 8 }, 5 },

	//missile explosion
	{ 1, 100, 100, 20, 16, 16, 0, 0, { 0, 0, 0, 0 }, 6 },
	{ 1, 100, 100, 20, 16, 16, 0, 0, { 0, 0, 0, 0 }, 6 },
	{ 1, 100, 100, 20, 16, 16, 0, 0, { 0, 0, 0, 0 }, 6 },

	//bubbler
	{ 1, 1, 20, 8, 2, 2, 2, 2, { 4, 4, 4, 4 }, 7 },
	{ 2, 1, 20, 8, 2, 2, 2, 2, { 4, 4, 4, 4 }, 7 },
	{ 2, 1, 20, 8, 4, 4, 4, 4, { 4, 4, 4, 4 }, 7 },

	//bubber pop projectile
	{ 3, 1, 32, 32, 2, 2, 2, 2, { 4, 4, 4, 4 }, 8 },

	//blade slashes
	{ 0, 100, 0, 36, 8, 8, 8, 8, { 12, 12, 12, 12 }, 8 },

	//egg corridor spike (FUCK YOU PIXEL)
	{ 127, 1, 2, 4, 8, 4, 8, 4, { 0, 0, 0, 0 }, 8 },

	//blade
	{ 15, 1, 30, 36, 8, 8, 4, 2, { 8, 8, 8, 8 }, 9 },
	{ 6, 3, 18, 36, 10, 10, 4, 2, { 12, 12, 12, 12 }, 9 },
	{ 1, 100, 30, 36, 6, 6, 4, 4, { 12, 12, 12, 12 }, 9 },

	//super missile
	{ 0, 10, 30, 40, 2, 2, 2, 2, { 8, 8, 8, 8 }, 10 },
	{ 0, 10, 40, 40, 4, 4, 4, 4, { 8, 8, 8, 8 }, 10 },
	{ 0, 10, 40, 40, 4, 4, 0, 0, { 8, 8, 8, 8 }, 10 },

	//super missile explosion
	{ 2, 100, 100, 20, 12, 12, 0, 0, { 0, 0, 0, 0 }, 11 },
	{ 2, 100, 100, 20, 12, 12, 0, 0, { 0, 0, 0, 0 }, 11 },
	{ 2, 100, 100, 20, 12, 12, 0, 0, { 0, 0, 0, 0 }, 11 },

	//nemesis
	{ 4, 4, 20, 32, 4, 4, 3, 3, { 8, 8, 24, 8 }, 12 },
	{ 4, 2, 20, 32, 2, 2, 2, 2, { 8, 8, 24, 8 }, 12 },
	{ 1, 1, 20, 32, 2, 2, 2, 2, { 8, 8, 24, 8 }, 12 },

	//spur
	{ 4, 4, 30, 64, 6, 6, 3, 3, { 8, 8, 8, 8 }, 13 },
	{ 8, 8, 30, 64, 6, 6, 3, 3, { 8, 8, 8, 8 }, 13 },
	{ 12, 12, 30, 64, 6, 6, 3, 3, { 8, 8, 8, 8 }, 13 },

	//spur trail
	{ 3, 100, 30, 32, 6, 6, 3, 3, { 4, 4, 4, 4 }, 14 },
	{ 6, 100, 30, 32, 6, 6, 3, 3, { 4, 4, 4, 4 }, 14 },
	{ 11, 100, 30, 32, 6, 6, 3, 3, { 4, 4, 4, 4 }, 14 },

	//curly nemesis
	{ 4, 4, 20, 32, 4, 4, 3, 3, { 8, 8, 24, 8 }, 15 },

	//clear all
	{ 0, 4, 4, 4, 0, 0, 0, 0, { 0, 0, 0, 0 }, 15 },

	//whimsical star
	{ 1, 1, 1, 36, 1, 1, 1, 1, { 1, 1, 1, 1 }, 15 }
};

//Bullet functions
void createBullet(int setCode, int setX, int setY, uint8_t setDir)
{
	bullet *repBullet = nullptr;

	if (bullets.size())
	{
		for (size_t i = 0; i < bullets.size(); ++i)
		{
			if (!(bullets[i].cond & 0x80))
			{
				repBullet = &bullets[i];
				break;
			}
		}
	}

	if (repBullet != nullptr)
		repBullet->init(setCode, setX, setY, setDir);
	else
	{
		bullet newBullet;
		newBullet.init(setCode, setX, setY, setDir);
		bullets.push_back(newBullet);
	}
}

void updateBullets()
{
	if (bullets.size())
	{
		for (size_t i = 0; i < bullets.size(); i++)
		{
			if (bullets[i].cond & 0x80)
				bullets[i].update();
		}

		while (bullets.size() && !(bullets[bullets.size() - 1].cond & 0x80))
			bullets.erase(bullets.begin() + bullets.size() - 1);

		bulletHitMap();
		bulletHitNpcs();
	}
}

void drawBullets()
{
	if (bullets.size())
	{
		for (size_t i = 0; i < bullets.size(); i++)
		{
			if (bullets[i].cond & 0x80)
				bullets[i].draw();
		}
	}
}

//CLASS
void bullet::init(int setCode, int setX, int setY, uint8_t setDir) noexcept
{
	memset(this, 0, sizeof(*this));

	code_bullet = setCode;
	cond = 0x80;

	x = setX;
	y = setY;
	direct = setDir;

	damage = bulletTable[setCode].damage;

	life = bulletTable[setCode].life;
	life_count = bulletTable[setCode].life_count;

	bbits = bulletTable[setCode].bbits;

	enemyXL = bulletTable[setCode].enemyXL << 9;
	enemyYL = bulletTable[setCode].enemyYL << 9;
	blockXL = bulletTable[setCode].blockXL << 9;
	blockYL = bulletTable[setCode].blockYL << 9;

	view.right = bulletTable[setCode].view.right << 9;
	view.left = bulletTable[setCode].view.left << 9;
	view.top = bulletTable[setCode].view.top << 9;
	view.bottom = bulletTable[setCode].view.bottom << 9;

	weapon = bulletTable[setCode].weapon;
}

//Act functions

#include "polarStar.h"
#include "fireball.h"

#include "missile.h"

#include "spur.h"

bulletAct bulletActs[46] =
{
	static_cast<bulletAct>(nullptr),	// There isn't a 0th bullet
	static_cast<bulletAct>(nullptr),	// Snake
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	&actBulletPolarStar1,				// Polar Star
	&actBulletPolarStar2,
	&actBulletPolarStar3,
	&actBulletFireball1,				// Fireball
	&actBulletFireball2,
	&actBulletFireball3,
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	&actBulletMissileLauncher1,
	&actBulletMissileLauncher2,
	&actBulletMissileLauncher3,
	&actBulletBoom1,
	&actBulletBoom2,
	&actBulletBoom3,
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
	static_cast<bulletAct>(nullptr),
};

//Update and draw
void bullet::update()
{
	if (life > 0)
	{
		if (bulletActs[code_bullet] != nullptr)
			bulletActs[code_bullet](this);
		else if (debugFlags & notifyOnNotImplemented)
		{
			static bool wasNotifiedAboutBullet[_countof(bulletActs)] = { false };

			if (wasNotifiedAboutBullet[this->code_bullet])
				return;

			wasNotifiedAboutBullet[this->code_bullet] = true;
			string msg = "Bullet " + to_string(this->code_bullet) + " is not implemented yet.";
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Missing Bullet", msg.c_str(), nullptr);
		}
	}
	else
	{
		cond = 0;
	}
}

void bullet::draw()
{
	int drawX;
	int drawY;

	switch (direct)
	{
	case dirLeft:
		drawX = x - view.left;
		drawY = y - view.top;
		break;
	case dirUp:
		drawX = x - view.top;
		drawY = y - view.left;
		break;
	case dirRight:
		drawX = x - view.right;
		drawY = y - view.top;
		break;
	case dirDown:
		drawX = x - view.top;
		drawY = y - view.right;
		break;
	default:
		drawX = x;
		drawY = y;
		break;
	}

	drawTexture(sprites[TEX_BULLET], &rect, drawX / 0x200 - viewport.x / 0x200, drawY / 0x200 - viewport.y / 0x200);

	if (debugFlags & showBULId)
	{
		size_t index = 0;

		for (size_t i = 0; i < bullets.size(); i++)
		{
			if (&bullets[i] == this)
			{
				index = i;
				break;
			}
		}

		drawString(drawX / 0x200 - viewport.x / 0x200, drawY / 0x200 - viewport.y / 0x200 - 16, std::to_string(index).c_str());
	}
}
