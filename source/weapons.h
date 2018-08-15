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

extern WEAPON weapons[8];
extern RECT weaponRect;
extern int selectedWeapon;
extern int weaponShiftX;
extern int weaponExpFlash;

void initWeapons();

void actWeapon();
int useWeaponAmmo(int num);
int weaponBullets(int arms_code);

int giveWeapon(int code, int max_num);
int removeWeapon(int code);
int tradeWeapons(int code1, int code2, int max_num);
void clearWeaponExperience();
void maxWeaponAmmo();
bool checkWeapon(int code);

int rotateWeaponRight();
int rotateWeaponLeft();
