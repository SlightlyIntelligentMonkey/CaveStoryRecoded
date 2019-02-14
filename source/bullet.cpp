#include "bullet.h"

#include <string>
#include <deque>
#include <SDL.h>
#include <cstring>
#include <SDL_messagebox.h>
#include "weapons.h"
#include "bulletCollision.h"
#include "render.h"
#include "game.h"
#include "log.h"

std::deque<bullet> gBullets(0);

BULLETSTATS gBulletTable[] =
{
	//null bullet
	{ 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0, 0 }},

	//snake
	{ 4, 1, 20, 36, 4, 4, 2, 2, { 8, 8, 8, 8 }},
	{ 6, 1, 23, 36, 4, 4, 2, 2, { 8, 8, 8, 8 }},
	{ 8, 1, 30, 36, 4, 4, 2, 2, { 8, 8, 8, 8 }},

	//polarstar
	{ 1, 1, 8, 32, 6, 6, 2, 2, { 8, 8, 8, 8 }},
	{ 2, 1, 12, 32, 6, 6, 2, 2, { 8, 8, 8, 8 }},
	{ 4, 1, 16, 32, 6, 6, 2, 2, { 8, 8, 8, 8 }},

	//fireball
	{ 2, 2, 100, 8, 8, 16, 4, 2, { 8, 8, 8, 8 }},
	{ 3, 2, 100, 8, 4, 4, 4, 2, { 8, 8, 8, 8 }},
	{ 3, 2, 100, 8, 4, 4, 4, 2, { 8, 8, 8, 8 }},

	//machine gun
	{ 2, 1, 20, 32, 2, 2, 2, 2, { 8, 8, 8, 8 }},
	{ 4, 1, 20, 32, 2, 2, 2, 2, { 8, 8, 8, 8 }},
	{ 6, 1, 20, 32, 2, 2, 2, 2, { 8, 8, 8, 8 }},

	//missile
	{ 0, 10, 50, 40, 2, 2, 2, 2, { 8, 8, 8, 8 }},
	{ 0, 10, 70, 40, 4, 4, 4, 4, { 8, 8, 8, 8 }},
	{ 0, 10, 90, 40, 4, 4, 0, 0, { 8, 8, 8, 8 }},

	//missile explosion
	{ 1, 100, 100, 20, 16, 16, 0, 0, { 0, 0, 0, 0 }},
	{ 1, 100, 100, 20, 16, 16, 0, 0, { 0, 0, 0, 0 }},
	{ 1, 100, 100, 20, 16, 16, 0, 0, { 0, 0, 0, 0 }},

	//bubbler
	{ 1, 1, 20, 8, 2, 2, 2, 2, { 4, 4, 4, 4 }},
	{ 2, 1, 20, 8, 2, 2, 2, 2, { 4, 4, 4, 4 }},
	{ 2, 1, 20, 8, 4, 4, 4, 4, { 4, 4, 4, 4 }},

	//bubbler pop projectile
	{ 3, 1, 32, 32, 2, 2, 2, 2, { 4, 4, 4, 4 }},

	//blade slashes
	{ 0, 100, 0, 36, 8, 8, 8, 8, { 12, 12, 12, 12 }},

	//egg corridor spike (FUCK YOU PIXEL)
	{ 127, 1, 2, 4, 8, 4, 8, 4, { 0, 0, 0, 0 }},

	//blade
	{ 15, 1, 30, 36, 8, 8, 4, 2, { 8, 8, 8, 8 }},
	{ 6, 3, 18, 36, 10, 10, 4, 2, { 12, 12, 12, 12 }},
	{ 1, 100, 30, 36, 6, 6, 4, 4, { 12, 12, 12, 12 }},

	//super missile
	{ 0, 10, 30, 40, 2, 2, 2, 2, { 8, 8, 8, 8 }},
	{ 0, 10, 40, 40, 4, 4, 4, 4, { 8, 8, 8, 8 }},
	{ 0, 10, 40, 40, 4, 4, 0, 0, { 8, 8, 8, 8 }},

	//super missile explosion
	{ 2, 100, 100, 20, 12, 12, 0, 0, { 0, 0, 0, 0 }},
	{ 2, 100, 100, 20, 12, 12, 0, 0, { 0, 0, 0, 0 }},
	{ 2, 100, 100, 20, 12, 12, 0, 0, { 0, 0, 0, 0 }},

	//nemesis
	{ 4, 4, 20, 32, 4, 4, 3, 3, { 8, 8, 24, 8 }},
	{ 4, 2, 20, 32, 2, 2, 2, 2, { 8, 8, 24, 8 }},
	{ 1, 1, 20, 32, 2, 2, 2, 2, { 8, 8, 24, 8 }},

	//spur
	{ 4, 4, 30, 64, 6, 6, 3, 3, { 8, 8, 8, 8 }},
	{ 8, 8, 30, 64, 6, 6, 3, 3, { 8, 8, 8, 8 }},
	{ 12, 12, 30, 64, 6, 6, 3, 3, { 8, 8, 8, 8 }},

	//spur trail
	{ 3, 100, 30, 32, 6, 6, 3, 3, { 4, 4, 4, 4 }},
	{ 6, 100, 30, 32, 6, 6, 3, 3, { 4, 4, 4, 4 }},
	{ 11, 100, 30, 32, 6, 6, 3, 3, { 4, 4, 4, 4 }},

	//curly nemesis
	{ 4, 4, 20, 32, 4, 4, 3, 3, { 8, 8, 24, 8 }},

	//clear all
	{ 0, 4, 4, 4, 0, 0, 0, 0, { 0, 0, 0, 0 }},

	//whimsical star
	{ 1, 1, 1, 36, 1, 1, 1, 1, { 1, 1, 1, 1 }}
};

int gEmpty = 0;

//Bullet functions
void createBullet(int setCode, int setX, int setY, uint8_t setDir, int weapon)
{
	bullet *repBullet = nullptr;

	if (gBullets.size())
	{
		for (size_t i = 0; i < gBullets.size(); ++i)
		{
			if (!(gBullets[i].cond & 0x80))
			{
				repBullet = &gBullets[i];
				break;
			}
		}
	}

	if (repBullet != nullptr)
		repBullet->init(setCode, setX, setY, setDir, weapon);

	else
	{
		bullet newBullet;
		newBullet.init(setCode, setX, setY, setDir, weapon);
		gBullets.push_back(newBullet);
	}
}

void updateBullets()
{
	if (gBullets.size())
	{
		for (size_t i = 0; i < gBullets.size(); i++)
		{
			if (gBullets[i].cond & 0x80)
				gBullets[i].update();
		}

		while (gBullets.size() && !(gBullets[gBullets.size() - 1].cond & 0x80))
			gBullets.erase(gBullets.begin() + gBullets.size() - 1);
	}

	bulletHitMap();
	bulletHitNpcs();
	bulletHitBoss();
}

void drawBullets()
{
	if (gBullets.size())
	{
		for (size_t i = 0; i < gBullets.size(); i++)
		{
			if (gBullets[i].cond & 0x80)
				gBullets[i].draw();
		}
	}
}

//CLASS
void bullet::init(int setCode, int setX, int setY, uint8_t setDir, int weaponId)
{
	memset(this, 0, sizeof(*this));

	code_bullet = setCode;
	cond = 0x80;

	x = setX;
	y = setY;
	direct = setDir;

	damage = gBulletTable[setCode].damage;

	life = gBulletTable[setCode].life;
	life_count = gBulletTable[setCode].life_count;

	bbits = gBulletTable[setCode].bbits;

	enemyXL = gBulletTable[setCode].enemyXL << 9;
	enemyYL = gBulletTable[setCode].enemyYL << 9;
	blockXL = gBulletTable[setCode].blockXL << 9;
	blockYL = gBulletTable[setCode].blockYL << 9;

	view.right = gBulletTable[setCode].view.right << 9;
	view.left = gBulletTable[setCode].view.left << 9;
	view.top = gBulletTable[setCode].view.top << 9;
	view.bottom = gBulletTable[setCode].view.bottom << 9;

	weapon = weaponId;
}

//Act functions
#include "snake.h"
#include "polarStar.h"
#include "fireball.h"
#include "machineGun.h"
#include "missile.h"
#include "bubbler.h"
#include "misc.h"
#include "blade.h"
#include "nemesis.h"
#include "spur.h"
#include "stage.h"

bulletAct gBulletActs[46] =
{
	static_cast<bulletAct>(nullptr),
	actBulletSnake1,
	actBulletSnake2,
	actBulletSnake3,

	actBulletPolarStar1,
	actBulletPolarStar2,
	actBulletPolarStar3,

	actBulletFireball1,
	actBulletFireball2,
	actBulletFireball3,

	actBulletMachineGun1,
	actBulletMachineGun2,
	actBulletMachineGun3,

	actBulletMissileLauncher1,
	actBulletMissileLauncher2,
	actBulletMissileLauncher3,
	actBulletBoom1,
	actBulletBoom2,
	actBulletBoom3,

	actBulletBubbler1,
	actBulletBubbler2,
	actBulletBubbler3,
	actBulletBubblerSpur,

	actBulletSlash,
	actBulletDrop,

	actBulletBlade1,
	actBulletBlade2,
	actBulletBlade3,

	actBulletSuperMissileLauncher1,
	actBulletSuperMissileLauncher2,
	actBulletSuperMissileLauncher3,
	actBulletSuperBoom1,
	actBulletSuperBoom2,
	actBulletSuperBoom3,

	actBulletNemesis1,
	actBulletNemesis2,
	actBulletNemesis3,

	actBulletSpur1,
	actBulletSpur2,
	actBulletSpur3,
	actBulletSpurTrail1,
	actBulletSpurTrail2,
	actBulletSpurTrail3,
	actBulletNemesis3,
	actBulletEnemyClear,
	actBulletStar
};

//Update and draw
void bullet::update()
{
	if (life > 0)
	{
		if (gBulletActs[code_bullet] != nullptr)
			gBulletActs[code_bullet](this);
		else
		{
			static bool wasNotifiedAboutBullet[_countof(gBulletActs)] = { false };

			if (wasNotifiedAboutBullet[this->code_bullet])
				return;

			wasNotifiedAboutBullet[this->code_bullet] = true;
			std::string msg = "Bullet " + std::to_string(this->code_bullet) + " is not implemented yet.";
			logWarning(msg);
			if (debugFlags & notifyOnNotImplemented)
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Missing Bullet", msg.c_str(), nullptr);
		}
	}
	else
		cond = 0;
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

	drawTexture(gSprites[TEX_BULLET], &rect, drawX / 0x200 - gViewport.x / 0x200, drawY / 0x200 - gViewport.y / 0x200);

	if (debugFlags & showBULId)
	{
		size_t index = 0;

		for (size_t i = 0; i < gBullets.size(); i++)
		{
			if (&gBullets[i] == this)
			{
				index = i;
				break;
			}
		}

		drawString(drawX / 0x200 - gViewport.x / 0x200, drawY / 0x200 - gViewport.y / 0x200 - 16, std::to_string(index));
	}
	if (debugFlags & showHitRects)
	{
		SDL_SetRenderDrawColor(gRenderer, 0x80, 0x80, 0x80, 0xFF);
		drawRect(unitsToPixels(x - (blockXL / 2) - 0x200) - unitsToPixels(gViewport.x),
			unitsToPixels(y - (blockYL / 2) - 0x200) - unitsToPixels(gViewport.y),
			unitsToPixels(blockXL + 0x400), unitsToPixels(blockYL + 0x400));
	}
	if (debugFlags & showHurtRects)
	{
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
		drawRect(unitsToPixels(x - (enemyXL / 2) - 0x200) - unitsToPixels(gViewport.x),
			unitsToPixels(y - (enemyYL / 2) - 0x200) - unitsToPixels(gViewport.y),
			unitsToPixels(enemyXL + 0x400), unitsToPixels(enemyYL + 0x400));
	}
}
