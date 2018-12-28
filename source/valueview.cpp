#include "valueview.h"

#include <vector>
#include <cstring>
#include <cmath>
#include "render.h"
#include "game.h"

using std::memset;
using std::abs;
using std::vector;

vector<valueview> valueviews(0);

//Functions
void createValueView(int *px, int *py, int value)
{
	valueview *repVV = nullptr;

	if (valueviews.size())
	{
		for (size_t i = 0;
		        i < valueviews.size();
		        ++i)
		{
			if (!(!valueviews[i].flag || valueviews[i].px != px || ((valueviews[i].value >= 0 || value >= 0) && (valueviews[i].value <= 0 || value <= 0))))
			{
				repVV = &valueviews[i];
				break;
			}
		}
	}

	if (repVV != nullptr)
	{
		if (repVV->flag)
			repVV->add(value);
		else
			repVV->init(px, py, value);
	}
	else
	{
		valueview newVV;
		newVV.init(px, py, value);
		valueviews.push_back(newVV);
	}
}

void updateValueView()
{
	if (valueviews.size())
	{
		for (size_t i = 0; i < valueviews.size(); i++)
		{
			if (valueviews[i].flag)
				valueviews[i].update();
		}

		while (valueviews.size() && !(valueviews[valueviews.size() - 1].flag))
			valueviews.erase(valueviews.begin() + valueviews.size() - 1);
	}
}

void drawValueView()
{
	if (valueviews.size())
	{
		for (size_t i = 0; i < valueviews.size(); i++)
		{
			if (valueviews[i].flag)
				valueviews[i].draw();
		}
	}
}

//Class
void valueview::init(int *setPx, int *setPy, int setVal) 
{
	memset(this, 0, sizeof(*this));
	this->px = setPx;
	this->py = setPy;
	this->flag = 1;
	this->value = setVal;
}

void valueview::add(int addVal) 
{
	this->count = 0x20;
	this->value += addVal;
}

void valueview::update() 
{
	if (++count < 32)
		offset_y -= 0x100;
	if (count > 80)
		flag = 0;
}

void valueview::draw()
{
	RECT rcPlus;
	RECT rcMinus;

	int width;

	int hide = 0;

	if (count >= 72)
		hide = count - 72;

	rcPlus = { 32, 48 + hide, 40, 56 };
	rcMinus = { 40, 48 + hide, 48, 56 };

	if (abs(value) < 1000)
	{
		if (abs(value) < 100)
		{
			if (abs(value) < 10)
				width = 16;
			else
				width = 24;
		}
		else
			width = 32;
	}
	else
		width = 40;

	//Draw + or -
	int x = *px / 0x200 - width / 2 - viewport.x / 0x200;
	const int y = *py / 0x200 + offset_y / 0x200 - 4 - viewport.y / 0x200;

	if (value >= 0)
		drawTexture(sprites[TEX_TEXTBOX], &rcPlus, x, y);
	else
		drawTexture(sprites[TEX_TEXTBOX], &rcMinus, x, y);

	//Draw numbers
	int offset = 0;
	int pos = 1000;
	int numCount = 0;
	int printValue = abs(value);

	x = *px / 0x200 + width / 2 - 32 - viewport.x / 0x200;

	while (offset < 4)
	{
		int drawValue = 0;

		while (pos <= printValue)
		{
			printValue -= pos;

			++drawValue;
			++numCount;
		}

		if (numCount || offset == 3)
		{
			if (value >= 0)
				rect = { drawValue << 3, 56 + hide, (drawValue + 1) << 3, 64 };
			else
				rect = { drawValue << 3, 64 + hide, (drawValue + 1) << 3, 72 };

			drawTexture(sprites[TEX_TEXTBOX], &rect, x + (offset << 3), y);
		}

		offset++;
		pos /= 10;
	}
}
