#include "player.h"

player currentPlayer;

void player::init(int createX, int createY, int createDirection) {
	memset(this, 0, sizeof(*this));

	x = createX << 13;
	y = createY << 13;

	health = 3;
	maxHealth = 3;

	direction = createDirection;

	air = 1000;

	cond = player_visible;

	key = true;
}

void player::explode(int x, int y, int w, int num)
{
	int offset_x;
	int offset_y;
	int wa;

	wa = w / 512;

	for (int i = 0; i < num; ++i)
	{
		offset_x = random(-wa, wa) << 9;
		offset_y = random(-wa, wa) << 9;

		createNpc(4, x + offset_x, offset_y + y, 0, 0, 0, nullptr);
	}

	createCaret(x, y, 12, 0);
}

void player::hit(int damage) {
	if (shock == 0)
	{
		//PlaySoundObject(16, 1);
		cond &= ~player_removed;

		shock = -128;

		//if (unk_81C8594 != 1) { unk_81C85B8 = -1024; }
		ysp = -0x400;

		health -= damage;

		createCaret(x, y, 10, 2);

		if (health <= 0)
		{
			cond = 0;

			explode(x, y, 0x1400, 0x40);
		}

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
			maxDash = 0x196;
			gravity = 0x28;
			gravityHold = 0x10;
			jump = 0x280;
			dashGround = 0x2A;
			dashAir = 0x10;
			resist = 0x19;
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
				if (!isKeyPressed(keyDown) || (cond & player_inspect))
				{
					if (isKeyDown(keyLeft))
					{
						direction = 0;

						if (xsp > -maxDash)
							xsp -= dashGround;
					}

					if (isKeyDown(keyRight))
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
				if (isKeyDown(keyLeft))
				{
					direction = 0;

					if (xsp > -maxDash)
						xsp -= dashAir;
				}

				if (isKeyDown(keyRight))
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
			lookingUp = (isKeyDown(keyUp)) != 0;
			lookingDown = isKeyDown(keyDown) && !(flags & ground);

			//Jump
			if (isKeyPressed(keyJump)
				&& (flags & ground || flags & slopeLeft || flags & slopeRight)
				&& !(flags & windUp))
			{
				ysp = -jump;
				//PlaySoundObject(15, 1);
			}
		}

		//If pressed anything, stop inspecting
		if (key == false || isKeyDown(keyShoot) || isKeyDown(keyJump) || isKeyDown(keyUp) || isKeyDown(keyRight) || isKeyDown(keyLeft))
		{
			cond &= ~player_inspect;
			interacting = false;
		}

		//Wind
		if (flags & windLeft)
			xsp -= 0x88;
		if (flags & windUp)
			ysp -= 0x80;
		if (flags & windRight)
			xsp += 0x88;
		if (flags & windDown)
			ysp += 0x55;

		//Gravity
		if (ysp < 0 && isKeyDown(keyJump))
		{
			ysp += gravityHold;
		}
		else
		{
			ysp += gravity;
		}

		//Keep Quote on slopes
		if (!isKeyPressed(keyJump))
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
					for (int waterit = 0; waterit < 8; ++waterit)
					{
						int spawnX = (random(-8, 8) << 9) + x;
						int waterYsp = random(-0x200, 0x80);
						int xspDiff = random(-0x200, 0x200);

						createNpc(73, spawnX, y, xsp + xspDiff, waterYsp, waterDir, nullptr);
					}

					//PlaySoundObject(56, 1);
				}
			}
			else
			{
				for (int waterit = 0; waterit < 8; ++waterit)
				{
					int spawnX = (random(-8, 8) << 9) + x;
					int waterYsp = random(-0x200, 0x80) - (ysp / 2);
					int xspDiff = random(-0x200, 0x200);

					createNpc(73, spawnX, y, xsp + xspDiff, waterYsp, waterDir, nullptr);
				}

				//PlaySoundObject(56, 1);
			}

			cond |= player_wasWater;
		}

		if (!(flags & 0x100))
		{
			cond &= ~player_wasWater;
		}

		//Get hurt by spikes
		if (flags & spike)
			hit(10);

		//Move camera
		if (direction)
			viewOffX = std::min(viewOffX + 0x200, 0x8000);
		else
			viewOffX = std::max(viewOffX - 0x200, -0x8000);

		if (isKeyDown(keyUp))
			viewOffY = std::max(viewOffY - 0x200, -0x8000);
		else if (isKeyDown(keyDown))
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
			//Inspecting
			frame = 11;
		}
		else if (key && (isKeyDown(keyRight) || isKeyDown(keyLeft)))
		{
			//Walking
			cond |= player_walk;

			int base = 0;

			if (key && isKeyDown(keyUp))
				base = 6;
			else
				base = 1;

			if (++animationWait > 4)
			{
				animationWait = 0;
				++frame;

				//if (frame == (base + 1) || frame == (base + 3))
				//	PlaySoundObject(24, 1);
			}

			if (frame > base + 3 || frame < base)
				frame = base;
		}
		else
		{
			//Idle
			cond &= ~player_walk;

			if (key && isKeyDown(keyUp))
				frame = 5;
			else
				frame = 0;
		}
	}
	else
	{
		//Air animations
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

		if (isKeyDown(SDL_SCANCODE_LCTRL))
		{
			if (isKeyPressed(SDL_SCANCODE_MINUS))
				--maxHealth;

			if (isKeyPressed(SDL_SCANCODE_EQUALS))
				++maxHealth;

			if (isKeyDown(keyLeft))
				x -= 0x1000;
			if (isKeyDown(keyRight))
				x += 0x1000;
			if (isKeyDown(keyUp))
				y -= 0x1000;
			if (isKeyDown(keyDown))
				y += 0x1000;

			viewGoalX = x - (screenWidth * 0x100);
			viewGoalY = y - (screenHeight * 0x100);
		}
		else
		{
			if (isKeyPressed(SDL_SCANCODE_MINUS))
				--health;

			if (isKeyPressed(SDL_SCANCODE_EQUALS))
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
				//Drowning
				if (direction)
					createCaret(x, y, 8, 2);
				else
					createCaret(x, y, 8, 0);

				cond &= ~player_visible;
			}
		}
		else if (airShow)
		{
			//Reset air stuff
			air = 1000;
			--airShow;
		}

		cond &= ~player_noFriction;

		animate();
	}
}
