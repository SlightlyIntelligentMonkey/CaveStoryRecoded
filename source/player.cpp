#include "player.h"

player currentPlayer;

void player::init(int createX, int createY, int createDirection) {
	x = createX << 13;

	y = createY << 13;

	xsp = 0;
	ysp = 0;

	direction = createDirection;

	lastHealth = health;
	healthDrainCount = 0;

	air = 1000;

	airShow = 0;

	shock = 0;

	//Camera
	viewOffX = 0;
	viewOffY = 0;

	viewX = x - (screenWidth * 0x100);
	viewY = y - (screenHeight * 0x100);

	//Animation
	animation = 0;
	animationWait = 0;

	frame = 0;

	//State
	cond = player_visible;

	lookingUp = false;
	lookingDown = false;

	interacting = false;

	//Collision state
	flags = 0;
}

void player::hit(int damage) {
	if (shock == 0)
	{
		//PlaySoundObject(16, 1);
		if (cond & player_removed)
			cond ^= player_removed;

		shock = -128;

		//if (unk_81C8594 != 1) { unk_81C85B8 = -1024; }
		ysp = -0x400;

		health -= damage;

		//if (unk_81C8598 & 0x80 && unk_81C8616 > 0) { unk_81C8616--; }
		//if (unk_81C8598 & 4) { v1 = gArmsData[gSelectedArms].exp - damage; }
		//else { v1 = gArmsData[gSelectedArms].exp - 2 * damage; }
		//gArmsData[gSelectedArms].exp = v1;
		//while (gArmsData[gSelectedArms].exp < 0)
		//{
		//	if (gArmsData[gSelectedArms].level <= 1)
		//	{
		//		gArmsData[gSelectedArms].exp = 0;
		//	}
		//	else
		//	{
		//		gArmsData[gSelectedArms].exp += gArmsLevelTable[0].exp[gArmsData[gSelectedArms].level--
		//			- 1
		//			+ 3 * gArmsData[gSelectedArms].code];
		//		if (gMC.life > 0 && gArmsData[gSelectedArms].code != 13) { SetCaret(x, y, 10, 2); }
		//	}
		//}
		//SetValueView(&x, &y, -damage);
		//if (gMC.life <= 0)
		//{
		//	PlaySoundObject(17, 1);
		//	gMC.cond = 0;
		//	SetDestroyNpChar(x, y, 5120, 64);
		//	StartTextScript(40);
		//}
	}
}

void player::collide() {
	int width = 0xA00;
	int height = 0x1000;

	RECT hitRect = { width, height, width, height };
	
	flags = playerHitMap(&hitRect);
	playerHitNpcs(&hitRect);
}

void player::actNormal() {
	if (!(cond & 2))
	{
		if (flags & water)
		{
			maxDash = 406;
			gravity = 40;
			gravityHold = 16;
			jump = 640;
			dashGround = 42;
			dashAir = 16;
			resist = 25;
		}
		else
		{
			maxDash = 0x32C;
			gravity = 0x50;
			gravityHold = 0x20;
			jump = 0x500;
			dashGround = 0x55;
			dashAir = 0x20;
			resist = 0x33;
		}

		interacting = false;

		if (flags & ground || flags & slopeLeft || flags & slopeRight)
		{
			if (key)
			{
				if (!keyPressed(SDL_SCANCODE_DOWN) || (cond & player_inspect))
				{
					if (keyDown(SDL_SCANCODE_LEFT))
					{
						direction = 0;

						if (xsp > -maxDash)
							xsp -= dashGround;
					}

					if (keyDown(SDL_SCANCODE_RIGHT))
					{
						direction = 2;

						if (xsp < maxDash)
							xsp += dashGround;
					}
				}
				else
				{
					cond |= player_inspect;
					interacting = true;
				}
			}

			if (!(cond & player_noFriction))
			{
				if (xsp < 0)
				{
					if (xsp <= -resist)
						xsp += resist;
					else
						xsp = 0;
				}
				if (xsp > 0)
				{
					if (xsp >= resist)
						xsp -= resist;
					else
						xsp = 0;
				}
			}
		}
		else
		{
			if (key)
			{
				if (keyDown(SDL_SCANCODE_LEFT))
				{
					direction = 0;

					if (xsp > -maxDash)
						xsp -= dashAir;
				}

				if (keyDown(SDL_SCANCODE_RIGHT))
				{
					direction = 2;

					if (xsp < maxDash)
						xsp += dashAir;
				}
			}
		}
		
		if (key)
		{
			//Looking up and down
			lookingUp = (keyDown(SDL_SCANCODE_UP)) != 0;
			lookingDown = keyDown(SDL_SCANCODE_DOWN) && !(flags & ground);

			//Jump
			if (keyPressed(SDL_SCANCODE_Z)
				&& (flags & ground || flags & slopeLeft || flags & slopeRight)
				&& !(flags & windUp))
			{
				ysp = -jump;
				//PlaySoundObject(15, 1);
			}
		}

		//If pressed anything, stop inspecting
		if (key == false || keyDown(SDL_SCANCODE_X) || keyDown(SDL_SCANCODE_Z) || keyDown(SDL_SCANCODE_UP) || keyDown(SDL_SCANCODE_RIGHT) || keyDown(SDL_SCANCODE_LEFT))
		{
			if (cond & player_inspect)
				cond ^= player_inspect;
		}

		//Wind
		if (flags & windLeft)
			xsp -= 0x88;
		if (flags & windUp)
			ysp -= 0x100;
		if (flags & windRight)
			xsp += 0x88;
		if (flags & windDown)
			ysp += 0x55;

		//Gravity
		if (ysp < 0 && keyDown(SDL_SCANCODE_Z))
		{
			ysp += gravityHold;
		}
		else
		{
			ysp += gravity;
		}

		//Keep Quote on slopes
		if (!keyPressed(SDL_SCANCODE_Z))
		{
			if (flags & slopeRight && xsp < 0)
				ysp = -xsp;

			if (flags & slopeLeft && xsp > 0)
				ysp = xsp;

			if (flags & ground && flags & slopeH && xsp < 0)
				ysp = 0x400;

			if (flags & ground && flags & slopeE && xsp > 0)
				ysp = 0x400;

			if (flags & ground && flags & slopeF && flags & slopeG)
				ysp = 0x400;
		}

		//Cap Quote's speed
		if (!(flags & water) || flags & (windLeft | windUp | windRight | windDown))
		{
			if (xsp < -0x5FF)
				xsp = -0x5FF;
			if (xsp > 0x5FF)
				xsp = 0x5FF;
			if (ysp < -0x5FF)
				ysp = -0x5FF;
			if (ysp > 0x5FF)
				ysp = 0x5FF;
		}
		else
		{
			if (xsp < -0x2FF)
				xsp = -0x2FF;
			if (xsp > 0x2FF)
				xsp = 0x2FF;
			if (ysp < -0x2FF)
				ysp = -0x2FF;
			if (ysp > 0x2FF)
				ysp = 0x2FF;
		}

		//Entering water
		if (!(cond & player_wasWater) && flags & water)
		{
			int waterDir = 0;

			if (flags & bloodWater)
				waterDir = 2;

			if (flags & ground || ysp <= 0x200)
			{
				if (xsp > 0x200 || xsp < -0x200)
				{
					for (int waterit = 0; waterit <= 7; ++waterit)
					{
						int spawnX = (random(-8, 8) << 9) + x;
						int waterYsp = random(-0x200, 0x80);
						int xspDiff = random(-0x200, 0x200);

						createNpc(spawnX, y, waterDir, xsp + xspDiff, waterYsp, 0, 0, 73, 0);
					}
					//PlaySoundObject(56, 1);
				}
			}
			else
			{
				for (int waterit = 0; waterit <= 7; ++waterit)
				{
					int spawnX = (random(-8, 8) << 9) + x;
					int waterYsp = random(-0x200, 0x80) - (ysp / 2);
					int xspDiff = random(-0x200, 0x200);

					createNpc(spawnX, y, waterDir, xsp + xspDiff, waterYsp, 0, 0, 73, 0);
				}
				//PlaySoundObject(56, 1);
			}

			cond |= player_wasWater;
		}

		if (!(flags & 0x100))
		{
			if (cond & player_wasWater)
				cond ^= player_wasWater;
		}

		//Get hurt by spikes
		if (flags & spike)
			hit(10);

		//Move camera
		if (direction)
			viewOffX = std::min(viewOffX + 0x200, 0x8000);
		else
			viewOffX = std::max(viewOffX - 0x200, -0x8000);

		if (keyDown(SDL_SCANCODE_UP))
			viewOffY = std::max(viewOffY - 0x200, -0x8000);
		else if (keyDown(SDL_SCANCODE_DOWN))
			viewOffY = std::min(viewOffY + 0x200, 0x8000);
		else
			viewOffY = std::max(std::abs(viewOffY) - 0x200, 0) * sign(viewOffY);

		viewGoalX = x - (screenWidth << 8) + viewOffX;
		viewGoalY = y - (screenHeight << 8) + viewOffY;

		//Move Quote
		if (xsp > resist || xsp < -resist) //If moving above friction speed
			x += xsp;

		y += ysp;
	}
}

void player::animate() //Basically the decomp code (thanks 20kdc for the help)
{
	if (flags & ground)
	{
		if (cond & player_inspect)
		{
			frame = 11;
		}
		else if (key && (keyDown(SDL_SCANCODE_RIGHT) || keyDown(SDL_SCANCODE_LEFT)))
		{
			cond |= player_walk;

			if (key && keyDown(SDL_SCANCODE_UP))
			{
				if (++animationWait > 4)
				{
					animationWait = 0;
					++frame;

					//if (unk_81C85C0 == 8 || unk_81C85C0 == 9)
					//	PlaySoundObject(24, 1);
				}

				if (frame > 9 || frame <= 5)
					frame = 6;
			}
			else
			{
				if (++animationWait > 4)
				{
					animationWait = 0;
					++frame;

					//if (unk_81C85C0 == 3 || unk_81C85C0 == 4)
					//	PlaySoundObject(24, 1);
				}

				if (frame > 4 || frame <= 0)
					frame = 1;
			}
		}
		else
		{
			if (cond & player_walk)
				cond ^= player_walk;

			if (key && keyDown(SDL_SCANCODE_UP))
				frame = 5;
			else
				frame = 0;
		}
	}
	else
	{
		if (lookingUp)
			frame = 6;
		else if (lookingDown)
			frame = 10;
		else if (ysp <= 0)
			frame = 3;
		else
			frame = 1;
	}
}

void player::draw() {
	if (cond & player_visible)
	{
		//Set framerect
		int frameMap[12] = { 0,1,0,2,0,3,4,3,5,3,6,7 };

		ImageRect.x = frameMap[frame] * 16;
		ImageRect.y = 0;

		//Offset for directional and mimiga mask purposes
		if (direction)
			ImageRect.y += 16;

		ImageRect.w = 16;
		ImageRect.h = 16;

		//Draw
		bool shockFlash = ((shock >> 1) & 1);

		if (!shockFlash)
			drawTexture(sprites[0x10], x - 0x1000, y - 0x1000, false);

		/*/ COLLISION DEBUG
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

		drawRect((x - 0xA00 - viewX) >> 9, (y - 0x1000 - viewY) >> 9, 0xA00 >> 8, 0x1000 >> 8);
		*/
	}
}

void player::update() {
	if (cond & player_visible)
	{
		if (shock)
			++shock;

		if (keyDown(SDL_SCANCODE_LCTRL))
		{
			if (keyPressed(SDL_SCANCODE_MINUS))
				--maxHealth;

			if (keyPressed(SDL_SCANCODE_EQUALS))
				++maxHealth;

			if (keyDown(SDL_SCANCODE_LEFT))
				x -= 0x1000;
			if (keyDown(SDL_SCANCODE_RIGHT))
				x += 0x1000;
			if (keyDown(SDL_SCANCODE_UP))
				y -= 0x1000;
			if (keyDown(SDL_SCANCODE_DOWN))
				y += 0x1000;

			viewGoalX = x - (screenWidth * 0x100);
			viewGoalY = y - (screenHeight * 0x100);
		}
		else
		{
			if (keyPressed(SDL_SCANCODE_MINUS))
				--health;

			if (keyPressed(SDL_SCANCODE_EQUALS))
				++health;

			actNormal();

			collide();
		}

		//Air
		if (flags & water)
		{
			airShow = 60;

			//Drowning
			if (--air <= 0)
			{
				//Drowning code
				air = 0;
			}
		}
		else if (airShow)
		{
			//Reset air stuff
			air = 1000;
			--airShow;
		}

		if (flags & player_noFriction)
			flags ^= player_noFriction;

		animate();
	}
}