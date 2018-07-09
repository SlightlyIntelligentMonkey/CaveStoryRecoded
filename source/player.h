#pragma once
#include "common.h"
#include "playerCollision.h"

class player {
	public:
		//Important varialbes
		int x = 0;
		int y = 0;

		int xsp = 0;
		int ysp = 0;

		int direction = 0;

		int health = 3;
		int maxHealth = 3;

		int lastHealth = 3;
		int healthDrainCount = 0;

		int air = 1000;

		int airShow = 0;

		int shock = 0;

		//Animation
		int animation = 0;
		int animationWait = 0;

		int frame = 0;

		//State
		UINT cond = 0;

		bool lookingUp = false;
		bool lookingDown = false;

		bool key = true;

		bool interacting = false;

		//Collision state
		UINT flags = 0;

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

		void explode(int x, int y, int w, int num);
		void hit(int damage);

		void collide();

		void actNormal();

		void animate();

		void draw();
		void update();
};

extern player currentPlayer;