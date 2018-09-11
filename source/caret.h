#pragma once

#include <deque>
#include "common.h"

struct CARETSTAT
{
	int offsetX;
	int offsetY;
};

class caret
{
public:
	int cond;
	int code;

	int direct;
	int x;
	int y;

	int xm;
	int ym;

	int act_no;
	int act_wait;

	int ani_no;
	int ani_wait;

	int view_left;
	int view_top;
	RECT rect;

public:
	void init(int setX, int setY, int setType, int setDir);

	void update();
	void draw();
};

using caretAct = void (*)(caret *);
extern caretAct caretActs[];

extern std::deque<caret> carets;

void updateCarets();
void drawCarets();

void createCaret(int setX, int setY, int setType, int setDir = dirLeft);

enum EffectNames
{
	effect_fountainDisk = 0x1,
	effect_RisingDisc = 0x2,
	effect_Star = 0x3,
	effect_FireballImpact = 0x4,
	effect_ZzZ = 0x5,
	effect_BoosterSmoke = 0x7,
	effect_DrownedQuote = 0x8,
	effect_ExclamationMark = 0x9,
	effect_LevelUpOrDown = 0xA,
	effect_RedDamageRings = 0xB,
	effect_BigExplosion = 0xC,
	effect_HeadbumpSparks = 0xD,
	effect_UnknownUnused = 0xE,
	effect_SmallExplosionDisc = 0xF,
	effect_Nothing = 0x10,
	effect_PushJumpKey = 0x11,
};
