#include "player.h"

player currentPlayer;

void player::init() {
	memset(this, 0, sizeof(*this));
	cond = player_visible;
	direct = 2;
	view = { 0x1000, 0x1000, 0x1000, 0x1000 };
	hit = { 0xA00, 0x1000, 0xA00, 0x1000 };

	viewport.lookX = &tgt_x;
	viewport.lookY = &tgt_y;

	viewport.speed = 16;

	life = 3;
	max_life = 3;
	gamePhysics = 0;
}

void player::setPos(int setX, int setY) {
	x = setX;
	y = setY;

	tgt_x = x;
	tgt_y = y;
	index_x = 0;
	index_y = 0;

	xm = 0;
	ym = 0;
	cond &= ~player_removed;
}

void player::setDir(int setDirect) {
	if (setDirect == 3)
	{
		cond |= player_interact;
		xm = 0;
		animate(false);
		return;
	}
	
	cond &= ~player_interact;

	if (setDirect < 10)
	{
		direct = setDirect;
		xm = 0;
		animate(false);
		return;
	}

	for (size_t i = 0; i < npcs.size(); i++)
	{
		if (npcs[i].code_event == setDirect)
		{
			if (x <= npcs[i].x)
				direct = 2;
			else
				direct = 0;

			xm = 0;
			animate(false);
			return;
		}
	}
}

void player::damage(int damage) {
	if (!shock)
	{
		cond &= ~player_removed;
		shock = 0x80;

		if (!gamePhysics)
			ym = -0x400;

		life -= damage;

		if (equip & equip_whimsicalStar && star > 0)
			--star;

		if (life <= 0)
		{
			playSound(17);
			cond = 0;

			createSmoke(x, y, 5120, 64);
			createCaret(x, y, 12, 0);
			runScriptEvent(40);
		}
	}
}

void player::collide() {
	flag = 0;

	playerHitMap(&hit);
	playerHitNpcs(&hit);
}

void player::actNormal(bool bKey) {


	if (!(cond & player_removed))
	{
		int max_dash;
		int gravity1;
		int gravity2;
		int jump;
		int dash1;
		int dash2;
		int resist;

		if (flag & water)
		{
			max_dash = 0x196;
			gravity1 = 0x28;
			gravity2 = 0x10;
			jump = 0x280;
			dash1 = 0x2A;
			dash2 = 0x10;
			resist = 0x19;
		}
		else
		{
			max_dash = 0x32C;
			gravity1 = 0x50;
			gravity2 = 0x20;
			jump = 0x500;
			dash1 = 0x55;
			dash2 = 0x20;
			resist = 0x33;
		}

		ques = 0;

		if (!bKey)
			boost_sw = 0;

		if (flag & ground || flag & slopeRight || flag & slopeLeft)
		{
			boost_sw = 0;

			//Fuel booster
			if (equip & equip_booster08)
				boost_cnt = 50;
			else if (equip & equip_booster20)
				boost_cnt = 50;
			else
				boost_cnt = 0;

			if (bKey)
			{
				bool notDown = (isKeyDown(keyShoot) || isKeyDown(keyJump) || isKeyDown(keyUp) || isKeyDown(keyLeft) || isKeyDown(keyRight) || isKeyDown(keyMap) || isKeyDown(keyMenu) || isKeyDown(keyRotLeft) || isKeyDown(keyRotRight));
				
				if (notDown || cond & player_interact || gameFlags & 4) //Moving and interaction
				{
					//Move left and right
					if (isKeyDown(keyLeft) && xm > -max_dash)
						xm -= dash1;
					if (isKeyDown(keyRight) && xm < max_dash)
						xm += dash1;

					//Face held direction
					if (isKeyDown(keyLeft))
						direct = 0;
					if (isKeyDown(keyRight))
						direct = 2;
				}
				else if (isKeyPressed(keyDown))
				{
					cond |= player_interact;
					ques = 1;
				}
			}

			if (!(cond & player_noFriction)) //Friction
			{
				if (xm < 0)
				{
					if (xm <= -resist)
						xm += resist;
					else
						xm = 0;
				}
				if (xm > 0)
				{
					if (xm >= resist)
						xm -= resist;
					else
						xm = 0;
				}
			}
		}
		else
		{
			if (bKey)
			{
				//Boosting
				if (equip & (equip_booster08 | equip_booster20) && isKeyPressed(keyJump) && boost_cnt)
				{
					if (equip & equip_booster08)
					{
						boost_sw = 1;

						if (ym > 0x100)
							ym /= 2;
					}

					if (equip & equip_booster20)
					{
						if (isKeyDown(keyUp))
						{
							boost_sw = 2;

							xm = 0;
							ym = -0x5FF;
						}
						else if (isKeyDown(keyLeft))
						{
							boost_sw = 1;

							xm = -0x5FF;
							ym = 0;
						}
						else if (isKeyDown(keyRight))
						{
							boost_sw = 1;
							
							xm = 0x5FF;
							ym = 0;
						}
						else if (isKeyDown(keyDown))
						{
							boost_sw = 3;

							xm = 0;
							ym = 0x5FF;
						}
						else
						{
							boost_sw = 2;

							xm = 0;
							ym = -0x5FF;
						}
					}
				}

				//Move left and right
				if (isKeyDown(keyLeft) && xm > -max_dash)
					xm -= dash2;
				if (isKeyDown(keyRight) && xm < max_dash)
					xm += dash2;

				//Face held direction
				if (isKeyDown(keyLeft))
					direct = 0;
				if (isKeyDown(keyRight))
					direct = 2;
			}

			//Ending boost (2.0)
			if (equip & equip_booster20 && boost_sw && (!isKeyDown(keyJump) || !boost_cnt))
			{
				if (boost_sw == 1)
				{
					xm /= 2;
				}
				else if (boost_sw == 2)
				{
					ym /= 2;
				}
			}

			if (!boost_cnt || !isKeyDown(keyJump))
				boost_sw = 0;
		}

		if (bKey)
		{
			//Look up and down
			up = isKeyDown(keyUp);
			down = isKeyDown(keyDown) && !(flag & ground);

			//Jump
			if (isKeyPressed(keyJump)
				&& (flag & ground || flag & slopeRight || flag & slopeLeft)
				&& !(flag & windUp))
			{
				ym = -jump;
				playSound(15);
			}
		}

		//End interaction state
		if (bKey && (isKeyDown(keyShoot) || isKeyDown(keyJump) || isKeyDown(keyUp) || isKeyDown(keyLeft) || isKeyDown(keyRight)))
			cond &= ~player_interact;

		//Boost timer
		if (boost_sw && boost_cnt)
			--boost_cnt;

		//Wind
		if (flag & windLeft)
			xm -= 0x88;
		if (flag & windUp)
			ym -= 0x80;
		if (flag & windRight)
			xm += 0x88;
		if (flag & windDown)
			ym += 0x55;

		//Booster 2.0 code
		if (equip & equip_booster20 && boost_sw)
		{
			if (boost_sw == 1)
			{
				//Go up when hit wall
				if (flag & (leftWall | rightWall))
					ym = -0x100;

				//Move in facing direction
				if (!direct)
					xm -= 0x20;
				if (direct == 2)
					xm += 32;

				if (isKeyPressed(keyJump) || boost_cnt % 3 == 1)
				{
					if (!direct)
						createCaret(x + 0x400, y + 0x400, 7, 2);
					if (direct == 2)
						createCaret(x - 0x400, y + 0x400, 7, 0);

					playSound(113);
				}
			}
			else if (boost_sw == 2)
			{
				//Move up
				ym -= 0x20;

				if (isKeyPressed(keyJump) || boost_cnt % 3 == 1)
				{
					createCaret(x, y + 0xC00, 7, 3);
					playSound(113);
				}
			}
			else if (boost_sw == 3 && (isKeyPressed(keyJump) || boost_cnt % 3 == 1))
			{
				createCaret(x, y - 0xC00, 7, 1);
				playSound(113);
			}
		}
		else if (flag & windUp) //Gravity when in wind
		{
			ym += gravity1;
		}
		else if (equip & equip_booster08 && boost_sw && ym > -0x400) //Booster 8.0
		{
			ym -= 0x20;

			if (!(boost_cnt % 3))
			{
				createCaret(x, hit.bottom / 2 + y, 7, 3);
				playSound(113);
			}

			if (flag & ceiling) //Bounce of ceilings
				ym = 0x200;
		}
		else if (ym < 0 && bKey && isKeyDown(keyJump)) //Hold gravity
		{
			ym += gravity2;
		}
		else //Normal gravity
		{
			ym += gravity1;
		}

		//Keep Quote down on slopes
		if (!bKey || !isKeyPressed(keyJump))
		{
			if (flag & slopeRight && xm < 0)
				ym = -xm;
			if (flag & slopeLeft && xm > 0)
				ym = xm;

			if (flag & ground && flag & slopeH && xm < 0)
				ym = 0x400;
			if (flag & ground && flag & slopeE && xm > 0)
				ym = 0x400;
			if (flag & ground && flag & slopeF && flag & slopeG)
				ym = 0x400;
		}

		//Limit speed
		if (!(flag & water) || flag & (windLeft | windUp | windRight | windDown)) //Out of water or in wind
		{
			if (xm < -0x5FF)
				xm = -0x5FF;
			if (xm > 0x5FF)
				xm = 0x5FF;

			if (ym < -0x5FF)
				ym = -0x5FF;
			if (ym > 0x5FF)
				ym = 0x5FF;
		}
		else //Water and not in wind
		{
			if (xm < -0x2FF)
				xm = -0x2FF;
			if (xm > 0x2FF)
				xm = 0x2FF;

			if (ym < -0x2FF)
				ym = -0x2FF;
			if (ym > 0x2FF)
				ym = 0x2FF;
		}

		//Splashing
		if (!sprash && flag & water)
		{
			//Blood water stuff
			int dir = 0;

			if (flag & bloodWater)
				dir = 2;
			
			//Splash stuff
			if (flag & ground || ym <= 0x200)
			{
				if (xm > 0x200 || xm < -0x200)
				{
					for (int i = 0; i < 8; ++i)
					{
						createNpc(73, x + (random(-8, 8) << 9), y, xm + random(-512, 512), random(-0x200, 0x80), dir, nullptr);
					}

					playSound(56);
				}
			}
			else
			{
				for (int i = 0; i < 8; ++i)
				{
					createNpc(73, x + (random(-8, 8) << 9), y, xm + random(-512, 512), random(-0x200, 0x80) - ym / 2, dir, nullptr);
				}

				playSound(56);
			}

			sprash = 1;
		}

		if (!(flag & water))
			sprash = 0;

		//Spike damage
		if (flag & spike)
			damage(10);

		//Camera
		if (direct)
		{
			//Move to the right
			index_x += 0x200;

			if (index_x > 0x8000)
				index_x = 0x8000;
		}
		else
		{
			//Move to the left
			index_x -= 0x200;

			if (index_x < -0x8000)
				index_x = -0x8000;
		}

		if (bKey && isKeyDown(keyUp))
		{
			//Move up
			index_y -= 0x200;

			if (index_y < -0x8000)
				index_y = -0x8000;
		}
		else if (bKey && isKeyDown(keyDown))
		{
			//Move down
			index_y += 0x200;

			if (index_y > 0x8000)
				index_y = 0x8000;
		}
		else
		{
			//Move to neutral position
			if (index_y > 0x200)
				index_y -= 0x200;

			if (index_y < -0x200)
				index_y += 0x200;
		}

		tgt_x = x + index_x;
		tgt_y = y + index_y;

		//Move
		if (xm > resist || xm < -resist)
			x += xm;
		y += ym;
	}
}

void player::actStream(bool bKey)
{
	up = 0;
	down = 0;

	if (bKey)
	{
		//Move left and right
		if (isKeyDown(keyLeft) || isKeyDown(keyRight))
		{
			if (isKeyDown(keyLeft))
				xm -= 0x100;
			if (isKeyDown(keyRight))
				xm += 0x100;
		}
		else if (xm >= 0x80 || xm <= -0x80)
		{
			if (xm <= 0)
				xm += 0x80;
			else
				xm -= 0x80;
		}
		else
			xm = 0;

		//Move up and down
		if (isKeyDown(keyUp) || isKeyDown(keyDown))
		{
			if (isKeyDown(keyUp))
				ym -= 0x100;
			if (isKeyDown(keyDown))
				ym += 0x100;
		}
		else if (ym >= 0x80 || ym <= -0x80)
		{
			if (ym <= 0)
				ym += 0x80;
			else
				ym -= 0x80;
		}
		else
			ym = 0;
	}
	else
	{
		//Stop moving
		if (xm >= 0x80 || xm <= -0x40)
		{
			if (xm <= 0)
				xm += 0x80;
			else
				xm -= 0x80;
		}
		else
			xm = 0;

		if (ym >= 0x80 || ym <= -0x40)
		{
			if (ym <= 0)
				ym += 0x80;
			else
				ym -= 0x80;
		}
		else
			ym = 0;
	}

	//Bump effect (this code doesn't work because pixel forgot about how ym is set to 0)
	if (ym < -0x200 && flag & ceiling)
		createCaret(x, y - hit.top, 13, 5);
	if (ym > 0x200 && flag & ground)
		createCaret(x, y + hit.bottom, 13, 5);

	//Limit speed
	if (xm > 0x400)
		xm = 0x400;
	if (xm < -0x400)
		xm = -0x400;
	if (ym > 0x400)
		ym = 0x400;
	if (ym < -0x400)
		ym = -0x400;

	//Diagonal speed
	if (isKeyDown(keyLeft) && isKeyDown(keyUp))
	{
		if (xm < -0x30C)
			xm = -0x30C;
		if (ym < -0x30C)
			ym = -0x30C;
	}
	if (isKeyDown(keyRight) && isKeyDown(keyUp))
	{
		if (xm > 0x30C)
			xm = 0x30C;
		if (ym < -0x30C)
			ym = -0x30C;
	}
	if (isKeyDown(keyLeft) && isKeyDown(keyDown))
	{
		if (xm < -0x30C)
			xm = -0x30C;
		if (ym > 0x30C)
			ym = 0x30C;
	}
	if (isKeyDown(keyRight) && isKeyDown(keyDown))
	{
		if (xm > 0x30C)
			xm = 0x30C;
		if (ym > 0x30C)
			ym = 0x30C;
	}

	x += xm;
	y += ym;
}

void player::animate(bool bKey)
{
	RECT rcLeft[12];
	RECT rcRight[12];

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 16, 0, 32, 16 };
	rcLeft[2] = { 0, 0, 16, 16 };
	rcLeft[3] = { 32, 0, 48, 16 };
	rcLeft[4] = { 0, 0, 16, 16 };
	rcLeft[5] = { 48, 0, 64, 16 };
	rcLeft[6] = { 64, 0, 80, 16 };
	rcLeft[7] = { 48, 0, 64, 16 };
	rcLeft[8] = { 80, 0, 96, 16 };
	rcLeft[9] = { 48, 0, 64, 16 };
	rcLeft[10] = { 96, 0, 112, 16 };
	rcLeft[11] = { 112, 0, 128, 16 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 16, 16, 32, 32 };
	rcRight[2] = { 0, 16, 16, 32 };
	rcRight[3] = { 32, 16, 48, 32 };
	rcRight[4] = { 0, 16, 16, 32 };
	rcRight[5] = { 48, 16, 64, 32 };
	rcRight[6] = { 64, 16, 80, 32 };
	rcRight[7] = { 48, 16, 64, 32 };
	rcRight[8] = { 80, 16, 96, 32 };
	rcRight[9] = { 48, 16, 64, 32 };
	rcRight[10] = { 96, 16, 112, 32 };
	rcRight[11] = { 112, 16, 128, 32 };

	if (!(cond & player_removed))
	{
		if (flag & ground)
		{
			if (cond & player_interact) //Look away
			{
				ani_no = 11;
			}
			else if (bKey && isKeyDown(keyUp) && (isKeyDown(keyLeft) || isKeyDown(keyRight))) //Look up while walking
			{
				cond |= player_walk;

				if (++ani_wait > 4)
				{
					ani_wait = 0;

					if (++ani_no == 7 || ani_no == 9)
						playSound(24);
				}

				if (ani_no > 9 || ani_no <= 5)
					ani_no = 6;
			}
			else if (bKey && (isKeyDown(keyLeft) || isKeyDown(keyRight))) //Walk
			{
				cond |= player_walk;

				if (++ani_wait > 4)
				{
					ani_wait = 0;

					if (++ani_no == 2 || ani_no == 4)
						playSound(24);
				}

				if (ani_no > 4 || ani_no <= 0)
					ani_no = 1;
			}
			else if (bKey && isKeyDown(keyUp)) //Look up
			{
				if (cond & player_walk)
					playSound(24);

				cond &= ~player_walk;
				ani_no = 5;
			}
			else //Idle
			{
				if (cond & player_walk)
					playSound(24);

				cond &= ~player_walk;
				ani_no = 0;
			}
		}
		else if (up) //Look up in air
			ani_no = 6;
		else if (down) //Look down
			ani_no = 10;
		else if (ym <= 0) //Jumping
			ani_no = 3;
		else //Fall
			ani_no = 1;
	}

	if (direct)
		rect = rcRight[ani_no];
	else
		rect = rcLeft[ani_no];

	if (equip & equip_mimigaMask)
	{
		rect.top += 32;
		rect.bottom += 32;
	}
}

void player::update(bool bKey) {

	if (cond & player_visible)
	{
		if (shock)
			--shock;
		
		if (gamePhysics == 1)
		{
			actStream(bKey);
		}
		else
		{
			actNormal(bKey);

			if (!(gameFlags & 4) && bKey)
			{
				//Air
				if (equip & equip_airTank)
				{
					air = 1000;
					air_get = 0;
				}
				else
				{
					if (flag & water)
					{
						air_get = 60;

						if (--air <= 0)
						{
							if (getFlag(4000))
							{
								runScriptEvent(1100);
							}
							else
							{
								runScriptEvent(41);

								if (direct)
									createCaret(x, y, 8, 2);
								else
									createCaret(x, y, 8, 0);

								cond &= ~player_visible;
							}
						}
					}
					else
					{
						if (air_get)
							--air_get;

						air = 1000;
					}
				}
			}
		}

		cond &= ~player_noFriction;

		collide();

		animate(bKey);
	}
}

void player::draw() {
	RECT rcBubble[2];
	rcBubble[0] = { 56, 96, 80, 120 };
	rcBubble[1] = { 80, 96, 104, 120 };
	
	if ((cond & player_visible) != 0 && !(cond & player_removed))
	{
		if (!((shock >> 1) & 1))
		{
			//Draw quote
			drawTexture(sprites[0x10], &rect, ((x - view.left) / 0x200) - (viewport.x / 0x200), ((y - view.top) / 0x200) - (viewport.y / 0x200));
			
			//Draw bubble
			bubble++;
			if (equip & equip_airTank && flag & water)
				drawTexture(sprites[0x13], &rcBubble[(bubble >> 1) & 1], (x / 0x200) - 12 - (viewport.x / 0x200), (y / 0x200) - 12 - (viewport.y / 0x200));
		}
	}
}
