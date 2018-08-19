#pragma once
#include "common.h"

#include <vector>

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

extern std::vector<caret> carets;

void updateCarets();
void drawCarets();

void createCaret(int setX, int setY, int setType, int setDir);
