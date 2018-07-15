#pragma once
#include "common.h"

struct CARETSTAT
{
	int offsetX;
	int offsetY;
};

class caret
{
public: //variables
	int cond;
	int type;

	int x;
	int y;

	int direction;

	int xsp;
	int ysp;

	int action;
	int actionWait;

	int animation;
	int animationWait;

	POINT offset;
	RECT frameRect;

public: //functions
	void init(int x, int y, int type, int dir);

	void update();
	void draw();
};

typedef void(*caretAct)(caret *);
extern caretAct caretActs[];

extern std::vector<caret> carets;

void updateCarets();
void drawCarets();

void createCaret(int setX, int setY, int setType, int setDir);