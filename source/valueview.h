#pragma once
#include "common.h"

#include <vector>

class valueview
{
public: //Variables
	int flag;
	int *px;
	int *py;
	int offset_y;
	int value;
	int count;
	RECT rect;

public: //Functions
	void init(int *setPx, int *setPy, int value);
	void add(int addVal);

	void update();
	void draw();
};

void createValueView(int *px, int *py, int value);
void updateValueView();
void drawValueView();

extern std::vector<valueview> valueviews;
