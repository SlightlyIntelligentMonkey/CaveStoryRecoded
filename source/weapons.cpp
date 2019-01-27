#include "weapons.h"
#include "shootFunctions.h"

#include <string>
#include <cstring>
#include <SDL_messagebox.h>
#include "player.h"
#include "sound.h"
#include "caret.h"
#include "bullet.h"
#include "game.h"

//Shoot functions
weaponShoot gShootFunctions[14] =
{
	static_cast<weaponShoot>(nullptr),
	&shootSnake,
	&shootPolarStar,
	&shootFireball,
	&shootMachineGun,
	&shootMissile,
	static_cast<weaponShoot>(nullptr),
	&pewBubble,
	static_cast<weaponShoot>(nullptr),
	&throwBlade,
	&launchSuperMissile,
	static_cast<weaponShoot>(nullptr),
	&shootNemesis,
	&shootSpur,
};

//Weapon Levels
WEAPONEXP gWeaponLevels[14] =
{
	{{ 0,	0,	100 }},
	{{ 30,	40,	16 }},
	{{ 10,	20,	10 }},
	{{ 10,	20,	20 }},
	{{ 30,	40,	10 }},
	{{ 10,	20,	10 }},
	{{ 10,	20,	30 }},
	{{ 10,	20,	5 }},
	{{ 10,	20,	100 }},
	{{ 30,	60,	0 }},
	{{ 30,	60,	10 }},
	{{ 10,	20,	100 }},
	{{ 1,	1,	1 }},
	{{ 40,	60,	200 }},
};

WEAPON gWeapons[WEAPONS];

RECT gWeaponRect;
int gSelectedWeapon;
int gWeaponShiftX;
int gWeaponExpFlash;

void initWeapons()
{
	memset(gWeapons, 0, sizeof(gWeapons));
	gSelectedWeapon = 0;
	gWeaponShiftX = 32;
}

//Functions for shooting
void actWeapon()
{
	if (_empty)
		--_empty;

	if (!(currentPlayer.cond & player_removed) && gWeapons[gSelectedWeapon].code)
	{
		if (gShootFunctions[gWeapons[gSelectedWeapon].code] != nullptr)
			gShootFunctions[gWeapons[gSelectedWeapon].code](gWeapons[gSelectedWeapon].level);
		else if (debugFlags & notifyOnNotImplemented)
		{
			static bool wasNotifiedAbout[_countof(gShootFunctions)] = { false };

			if (wasNotifiedAbout[gWeapons[gSelectedWeapon].code])
				return;

			wasNotifiedAbout[gWeapons[gSelectedWeapon].code] = true;
			std::string msg = "Weapon " + std::to_string(gWeapons[gSelectedWeapon].code) + " is not implemented.";
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Missing Weapon", msg.c_str(), nullptr);
		}
	}
}

void giveWeaponAmmo(int num)
{
	gWeapons[gSelectedWeapon].num += num;
	if (gWeapons[gSelectedWeapon].num > gWeapons[gSelectedWeapon].max_num)
		gWeapons[gSelectedWeapon].num = gWeapons[gSelectedWeapon].max_num;
}

int useWeaponAmmo(int num)
{
	//Some checks
	if (!gWeapons[gSelectedWeapon].max_num)
		return 1;
	if (!gWeapons[gSelectedWeapon].num)
		return 0;

	//Use ammo
	gWeapons[gSelectedWeapon].num -= num;
	if (gWeapons[gSelectedWeapon].num < 0)
		gWeapons[gSelectedWeapon].num = 0;
	return 1;
}

void giveAmmo(int ammoToRefill)
{
	int i;
	for (i = 0; i < 8 && gWeapons[i].code != weapon_MissileLauncher && gWeapons[i].code != weapon_SuperMissileLauncher; ++i);
	if (i != 8)
	{
		gWeapons[i].num += ammoToRefill;
		if (gWeapons[i].num > gWeapons[i].max_num)
			gWeapons[i].num = gWeapons[i].max_num;
	}
}

bool weaponMaxExp()
{
	return gWeapons[gSelectedWeapon].level == 3
	       && gWeapons[gSelectedWeapon].exp >= gWeaponLevels[gWeapons[gSelectedWeapon].code].exp[2];
}

int weaponBullets(int arms_code)
{
	int count = 0;
	for (size_t i = 0; i < bullets.size(); ++i)
	{
		if (bullets[i].cond & 0x80 && bullets[i].weapon == arms_code)
			++count;
	}
	return count;
}

//TSC functions
int tradeWeapons(int code1, int code2, int max_num)
{
	int i;
	for (i = 0; i < WEAPONS && gWeapons[i].code != code1; ++i);
	if (i == WEAPONS)
		return 0;

	gWeapons[i].level = 1;
	gWeapons[i].code = code2;
	gWeapons[i].max_num += max_num;
	gWeapons[i].num += max_num;
	gWeapons[i].exp = 0;
	return 1;
}

int giveWeapon(int code, int maxAmmo)
{
	int i;
	for (i = 0; i < WEAPONS && gWeapons[i].code != code && gWeapons[i].code; ++i)
        ;
	if (i == WEAPONS)
		return 0;

	//Ensure past data is erased
	if (!gWeapons[i].code)
	{
		memset(&gWeapons[i], 0, sizeof(WEAPON));
		gWeapons[i].level = 1;
	}

	//Set weapon and ammo
	gWeapons[i].code = code;
	gWeapons[i].max_num += maxAmmo;
	gWeapons[i].num += maxAmmo;

	if (gWeapons[i].num > gWeapons[i].max_num)
		gWeapons[i].num = gWeapons[i].max_num;
	return 1;
}

int removeWeapon(int code)
{
	int i;
	for (i = 0; i < WEAPONS && gWeapons[i].code != code; ++i);
	if (i == WEAPONS)
		return 0;

	//Push back all weapons ahead
	int ia;
	for (ia = i + 1; ia < 8; ++ia)
		memcpy(&gWeapons[ia - 1], &gWeapons[ia], sizeof(WEAPON));

	//Clear farthest weapon and select first weapon
	gWeapons[ia - 1].code = 0;
	gSelectedWeapon = 0;
	return 1;
}

void clearWeaponExperience()
{
	for (auto& i : gWeapons)
	{
		i.level = 1;
		i.exp = 0;
	}
}

void maxWeaponAmmo()
{
	for (auto& i : gWeapons)
		i.num = i.max_num;
}

bool checkWeapon(int code)
{
	for (const auto& i : gWeapons)
	{
		if (i.code == code)
			return true;
	}
	return false;
}

void giveWeaponExperience(int x)
{
	int lv = gWeapons[gSelectedWeapon].level - 1; // [esp+18h] [ebp-10h]
	const int arms_code = gWeapons[gSelectedWeapon].code; // [esp+1Ch] [ebp-Ch]
	gWeapons[gSelectedWeapon].exp += x;

	if (lv == 2)
	{
		if (gWeapons[gSelectedWeapon].exp >= gWeaponLevels[arms_code].exp[2])
		{
			gWeapons[gSelectedWeapon].exp = gWeaponLevels[arms_code].exp[2];

			//Give a whimsical star
			if (currentPlayer.equip & equip_whimsicalStar && currentPlayer.star < 3)
				currentPlayer.star++;
		}
	}
	else
	{
		while (lv <= 1)
		{
			if (gWeapons[gSelectedWeapon].exp >= gWeaponLevels[arms_code].exp[lv])
			{
				++gWeapons[gSelectedWeapon].level;
				gWeapons[gSelectedWeapon].exp = 0;

				if (gWeapons[gSelectedWeapon].code != 13)
				{
					playSound(SFX_LevelUp);
					createCaret(currentPlayer.x, currentPlayer.y, effect_LevelUpOrDown);
				}
			}

			++lv;
		}
	}

	if (gWeapons[gSelectedWeapon].code == 13)
	{
		currentPlayer.exp_wait = 10;
	}
	else
	{
		currentPlayer.exp_count += x;
		currentPlayer.exp_wait = 30;
	}
}

void resetSelectedWeaponLevel()
{
	gWeapons[gSelectedWeapon].level = 1;
	gWeapons[gSelectedWeapon].exp = 0;
}

void resetSpurCharge()
{
	spurCharge = 0;
	if (gWeapons[gSelectedWeapon].code == weapon_Spur)
		resetSelectedWeaponLevel();
}

//Rotate weapon functions
int rotateWeaponRight()
{
	int weaponNo;

	for (weaponNo = 0; weaponNo < WEAPONS && gWeapons[weaponNo].code != 0; ++weaponNo);
	if (!weaponNo)
		return 0;

	resetSpurCharge();

	//Rotate to the right, wrap around
	++gSelectedWeapon;
	while (gSelectedWeapon < weaponNo && !gWeapons[gSelectedWeapon].code)
		++gSelectedWeapon;
	if (gSelectedWeapon == weaponNo)
		gSelectedWeapon = 0;

	//Do effects
	gWeaponShiftX = 32;
	playSound(SFX_SwitchWeapon);

	return gWeapons[weaponNo].code;
}

int rotateWeaponLeft()
{
	int weaponNo;

	for (weaponNo = 0; weaponNo < WEAPONS && gWeapons[weaponNo].code != 0; ++weaponNo);
	if (!weaponNo)
		return 0;

	resetSpurCharge();

	//Rotate to the left, wrap around
	--gSelectedWeapon;
	if (gSelectedWeapon < 0)
		gSelectedWeapon = weaponNo - 1;
	while (gSelectedWeapon < weaponNo && !gWeapons[gSelectedWeapon].code)
		--gSelectedWeapon;

	//Do effects
	gWeaponShiftX = 0;
	playSound(SFX_SwitchWeapon);

	return gWeapons[weaponNo].code;
}

