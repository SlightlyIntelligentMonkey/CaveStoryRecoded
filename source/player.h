#pragma once
#include "common.h"

class CLASS_player {
	public:
		//Important varialbes
		int x = 0;
		int y = 0;

		int xsp = 0;
		int ysp = 0;

		int direction = 0;

		//Animation
		int animation = 0;
		int animationWait = 0;

		int frame = 0;

		//State
		u_int cond = 0;

		bool lookingUp = false;
		bool lookingDown = false;

		bool key = true;

		//Collision state
		u_int flags = 0;

		//physics
		int maxDash = 0x32C;
		int gravity = 0x50;
		int gravityHold = 0x20;
		int jump = 0x500;
		int dashGround = 0x55;
		int dashAir = 0x20;
		int resist = 0x33;

		//Camera
		int viewOffX = 0;
		int viewOffY = 0;

	private:

	public:
		void init(int createX, int createY, int createDirection);

		void collide();

		void actNormal();

		void animate();

		void draw();

		void update();
};

extern CLASS_player currentPlayer;