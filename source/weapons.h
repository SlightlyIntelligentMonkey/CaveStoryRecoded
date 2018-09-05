#pragma once
#include "common.h"

struct WEAPONEXP
{
	int exp[3];
};

extern WEAPONEXP weaponLevels[14];

struct WEAPON
{
	int code;
	int level;
	int exp;
	int max_num;
	int num;
};

using weaponShoot = void(*)(int);

constexpr auto WEAPONS = 8;

extern WEAPON weapons[WEAPONS];
extern RECT weaponRect;
extern int selectedWeapon;
extern int weaponShiftX;
extern int weaponExpFlash;

void initWeapons() noexcept;

void actWeapon();

int useWeaponAmmo(int num) noexcept;
void giveAmmo(int amount, int ammoToRefill);
void giveWeaponAmmo(int amount);
bool weaponMaxExp() noexcept attrPure;
int weaponBullets(int arms_code) attrPure;

int giveWeapon(int code, int max_num) noexcept;
int removeWeapon(int code) noexcept;
int tradeWeapons(int code1, int code2, int max_num) noexcept;
void clearWeaponExperience() noexcept;
void maxWeaponAmmo() noexcept;
bool checkWeapon(int code) noexcept attrPure;

void giveWeaponExperience(int x);

int rotateWeaponRight() noexcept;
int rotateWeaponLeft() noexcept;
