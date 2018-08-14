#pragma once
#include "common.h"

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
	void init(int *px, int *py, int value);

	void update();
	void draw();
};

extern std::vector<valueview> valueviews;
