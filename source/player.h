#pragma once
#include "common.h"
#include "playerCollision.h"

class player {
	public:
		//State things
		uint8_t cond;
		uint32_t flag;
		int direct;
		int up;
		int down;
		int unit;
		int equip;

		//Positions
		int x;
		int y;

		int tgt_x;
		int tgt_y;

		int index_x;
		int index_y;

		//Movement
		int xm;
		int ym;

		//Animation
		int ani_wait;
		int ani_no;

		//Rects
		RECT hit;
		RECT view;
		RECT rect;
		RECT rect_arms;

		//Weapons, hud, and health
		int level;
		int exp_wait;
		int exp_count;
		uint8_t shock;
		uint8_t no_life;
		uint8_t rensha;
		uint8_t bubble;
		int16_t life;
		int16_t star;
		int16_t max_life;
		int lifeBr;
		int lifeBr_count;

		//Air
		int air;
		int air_get;

		//Other things
		int8_t sprash;
		int8_t ques;
		int8_t boost_sw;
		int boost_cnt;

	private:

	public:
		void init();

		void setPos(int setX, int setY);

		void damage(int damage);

		void actNormal(bool bKey);
		void actStream(bool bKey);

		void animate(bool bKey);

		void collide();

		void update(bool bKey);
		void draw();
};

extern player currentPlayer;