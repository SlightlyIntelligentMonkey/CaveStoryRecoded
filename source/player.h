#pragma once
#include "common.h"
#include "playerCollision.h"

class player
{
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

public:
	void init() noexcept;

	void setPos(int setX, int setY) noexcept;
	void setDir(int setDirect);

	void damage(int16_t damage);

	void actNormal(bool bKey);
	void actStream(bool bKey);

	void animate(bool bKey) noexcept;

	void update(bool bKey);
	void draw();
};

extern player currentPlayer;

enum player_flag
{
	player_interact = 0x1, //Inspecting
	player_removed = 0x2, //Removed
	player_walk = 0x4, //Walking (used for animation)
	player_noFriction = 0x20, //Disables friction
	player_visible = 0x80, //Quote is visible
	player_water = 0x100,
};

enum equipItems
{
	equip_booster08 = 0x01,
	equip_mapSystem = 0x02,
	equip_armsBarrier = 0x04,
	equip_turboCharge = 0x08,
	equip_airTank = 0x10,
	equip_booster20 = 0x20,
	equip_mimigaMask = 0x40,
	equip_whimsicalStar = 0x80,
	equip_nikumaruCounter = 0x100,
};
