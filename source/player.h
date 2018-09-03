#pragma once
#include "common.h"
#include "playerCollision.h"

class player
{
public:
	//State things
	uint8_t cond; //Condition (alive, interactingm etc.)
	uint32_t flag; //Current collision flags
	int direct; //Facing left or right
	int up; //Looking up
	int down; //Looking down
	int unit; //Current physics
	int equip; //Items equipped

	//Positions
	int x; //X position
	int y; //Y position

	int tgt_x; //Position for camera to follow
	int tgt_y; //Position for camera to follow

	int index_x; //Offset for direction looking (used for camera to get tgt_x and y)
	int index_y; //Offset for direction looking (used for camera to get tgt_x and y)

	//Movement
	int xm; //Current velocity
	int ym; //Current velocity

	//Animation
	int ani_wait; //Used for walking
	int ani_no; //What frame is drawing

	//Rects
	RECT hit; //Collision hitbox
	RECT view; //Drawing offset
	RECT rect; //Main framerect
	RECT rect_arms; //Weapon's framerect

	//Weapons, hud, and health
	int level; //Current weapon's level (I believe this is unused)
	int exp_wait; //How many frames until the experience value appears
	int exp_count; //How much experience has been collected
	uint8_t shock; //Invulnerability frames
	uint8_t no_life; //Unused?
	uint8_t rensha; //Used for machine gun and bubbline's automatic firing
	uint8_t bubble; //Air tank's bubble animation
	int16_t life; //Health
	int16_t star; //Whimsical stars
	int16_t max_life; //Maximum health
	int lifeBr; //Health in health bar
	int lifeBr_count; //For effect where health drains when damaged

	//Air
	int air; //How much air is left
	int air_get; //Used for flashing when exiting water

	//Other things
	int8_t sprash; //Keeps track of if you were in water last frame or not
	int8_t ques; //For the ? that appears above Quote's head when down is pressed but there's no NPC nearby
	int8_t boost_sw; //Booster 2.0 direction
	int boost_cnt; //Boosters fuel

public:
	void init();

	void setPos(int setX, int setY);
	void setDir(int setDirect);

	void damage(int16_t damage);

	void actNormal(bool bKey);
	void actStream(bool bKey);

	void animate(bool bKey);

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
