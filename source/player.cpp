#include "player.h"

#include <cstring>
#include "weapons.h"
#include "render.h"
#include "input.h"
#include "caret.h"
#include "flags.h"
#include "script.h"
#include "sound.h"
#include "mathUtils.h"
#include "valueview.h"
#include "game.h"
#include <SDL.h>
#include "stage.h"
#include "npc.h"

player gCurrentPlayer;

bool gDisableDamage = false;

void player::init()
{
	memset(this, 0, sizeof(*this));
	cond = player_visible;
	direct = dirRight;
	view = { 0x1000, 0x1000, 0x1000, 0x1000 };
	hit = { 0xA00, 0x1000, 0xA00, 0x1000 };

	gViewport.lookX = &tgt_x;
	gViewport.lookY = &tgt_y;

	gViewport.speed = 16;

	air = 1000;

	life = 3;
	max_life = 3;
}

void player::setPos(int setX, int setY)
{
	x = setX;
	y = setY;

	tgt_x = x;
	tgt_y = y;
	index_x = 0;
	index_y = 0;

	xm = 0;
	ym = 0;
	cond &= ~player_interact;
}

void player::setDir(int setDirect)
{
	if (setDirect == 3)
	{
		cond |= player_interact;
		xm = 0;
		this->animate(false);
		return;
	}

	cond &= ~player_interact;

	if (setDirect < 10)
	{
		direct = setDirect;
		xm = 0;
		this->animate(false);
		return;
	}

	for (size_t i = 0; i < gNPC.size(); i++)
	{
		if (gNPC[i].code_event == setDirect)
		{
			if (x <= gNPC[i].x)
				direct = dirRight;
			else
				direct = dirLeft;

			xm = 0;
			this->animate(false);
			return;
		}
	}
}

void player::backStep(int entityEventNum)
{
	this->cond &= ~player_interact;
	this->ym = -0x200;

	switch (entityEventNum)
	{
	case 0:
		this->direct = dirLeft;
		this->xm = 0x200;
		break;

	case 2:
		this->direct = dirRight;
		this->xm = -0x200;
		break;

	default:
		for (size_t i = 0; i < gNPC.size(); i++)
		{
			if (gNPC[i].cond & npccond_alive && gNPC[i].code_event == entityEventNum)
			{
				if (gNPC[i].x >= this->x)
				{
					this->direct = dirRight;
					this->xm = -0x200;
				}
				else
				{
					this->direct = dirLeft;
					this->xm = 0x200;
				}
			}
		}
		break;
	}
}

void player::damage(int16_t damage)
{
    if (gDisableDamage)
        return;

    if (!shock)
	{
		playSound(SFX_QuoteHurt);

		cond &= ~player_interact;
		shock = 0x80;

		if (!unit)
			ym = -0x400;

		life -= damage;

		if (equip & equip_whimsicalStar && star > 0)
			--star;

		//Lose experience
		if (equip & equip_armsBarrier)
			gWeapons[gSelectedWeapon].exp -= damage;
		else
			gWeapons[gSelectedWeapon].exp -= damage * 2;

		//Level down
		while (gWeapons[gSelectedWeapon].exp < 0)
		{
			if (gWeapons[gSelectedWeapon].level <= 1)
				gWeapons[gSelectedWeapon].exp = 0;
			else
			{
				gWeapons[gSelectedWeapon].exp += gWeaponLevels[gWeapons[gSelectedWeapon].code].exp[--gWeapons[gSelectedWeapon].level - 1];

				//Level down caret
				if (life > 0 && gWeapons[gSelectedWeapon].code != 13)
					createCaret(x, y, effect_LevelUpOrDown, dirRight);
			}
		}

		//Show damage taken
		createValueView(&x, &y, -damage);

		if (life <= 0)
		{
			playSound(SFX_QuoteDie);
			cond = 0;

			createSmokeLeft(x, y, 5120, 64);
			startTscEvent(gTsc, 40);
		}
	}
}

void player::actNormal(bool bKey)
{
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
			max_dash = 406;
			gravity1 = 40;
			gravity2 = 16;
			jump = 640;
			dash1 = 42;
			dash2 = 16;
			resist = 25;
		}
		else
		{
			max_dash = 812;
			gravity1 = 80;
			gravity2 = 32;
			jump = 1280;
			dash1 = 85;
			dash2 = 32;
			resist = 51;
		}

		ques = 0;

		if (!bKey)
			boost_sw = 0;

		if (flag & ground || flag & slopeRight || flag & slopeLeft)
		{
			boost_sw = 0;

			//Fuel booster
			this->boost_cnt = this->getBoostCount();

			if (bKey)
			{
				const bool notDown = (isKeyDown(gKeyShoot) || isKeyDown(gKeyJump) || isKeyDown(gKeyUp) || isKeyDown(gKeyLeft) || isKeyDown(gKeyRight) || isKeyDown(gKeyMap) || isKeyDown(gKeyMenu) || isKeyDown(gKeyRotateLeft) || isKeyDown(gKeyRotateRight));

				if (notDown || cond & player_interact || gGameFlags & 4) //Moving and interaction
				{
					//Move left and right
					if (isKeyDown(gKeyLeft) && xm > -max_dash)
						xm -= dash1;
					if (isKeyDown(gKeyRight) && xm < max_dash)
						xm += dash1;

					//Face held direction
					if (isKeyDown(gKeyLeft))
						direct = dirLeft;
					if (isKeyDown(gKeyRight))
						direct = dirRight;
				}
				else if (isKeyPressed(gKeyDown))
				{
					cond |= player_interact;
					ques = 1;
				}
			}

            this->doFriction(resist);
		}
		else
		{
			if (bKey)
			{
				this->doBooster();

				//Move left and right
				if (isKeyDown(gKeyLeft) && xm > -max_dash)
					xm -= dash2;
				if (isKeyDown(gKeyRight) && xm < max_dash)
					xm += dash2;

				//Face held direction
				if (isKeyDown(gKeyLeft))
					direct = dirLeft;
				if (isKeyDown(gKeyRight))
					direct = dirRight;
			}

			//Ending boost (2.0)
			if (equip & equip_booster20 && boost_sw && (!isKeyDown(gKeyJump) || !boost_cnt))
            {
				if (boost_sw == 1)
					xm /= 2;
				else if (boost_sw == 2)
					ym /= 2;
            }

			if (!boost_cnt || !isKeyDown(gKeyJump))
				boost_sw = 0;
		}

		if (bKey)
		{
			//Look up and down
			up = isKeyDown(gKeyUp);
			down = isKeyDown(gKeyDown) && !(flag & ground);

			//Jump
			if (isKeyPressed(gKeyJump)
			        && (flag & ground || flag & slopeRight || flag & slopeLeft)
			        && !(flag & windUp))
			{
				ym = -jump;
				playSound(SFX_QuoteJump);
			}
		}

		//End interaction state
		if (bKey && (isKeyDown(gKeyShoot) || isKeyDown(gKeyJump) || isKeyDown(gKeyUp) || isKeyDown(gKeyLeft) || isKeyDown(gKeyRight)))
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
				if (direct == dirLeft)
					xm -= 0x20;
				if (direct == dirRight)
					xm += 32;

				if (isKeyPressed(gKeyJump) || boost_cnt % 3 == 1)
				{
					if (direct == dirLeft)
						createCaret(x + pixelsToUnits(2), y + pixelsToUnits(2), effect_BoosterSmoke, dirRight);
					if (direct == dirRight)
						createCaret(x - pixelsToUnits(2), y + pixelsToUnits(2), effect_BoosterSmoke, dirLeft);

					playSound(SFX_Booster);
				}
			}
			else if (boost_sw == 2)
			{
				//Move up
				ym -= 0x20;

				if (isKeyPressed(gKeyJump) || boost_cnt % 3 == 1)
				{
					createCaret(x, y + pixelsToUnits(6), effect_BoosterSmoke, dirDown);
					playSound(SFX_Booster);
				}
			}
			else if (boost_sw == 3 && (isKeyPressed(gKeyJump) || boost_cnt % 3 == 1))
			{
				createCaret(x, y - pixelsToUnits(6), effect_BoosterSmoke, dirUp);
				playSound(SFX_Booster);
			}
		}
		else if (flag & windUp) //Gravity when in wind
			ym += gravity1;
		else if (equip & equip_booster08 && boost_sw && ym > pixelsToUnits(-2)) //Booster 8.0
		{
			ym -= 0x20;

			if (!(boost_cnt % 3))
			{
				createCaret(x, hit.bottom / 2 + y, effect_BoosterSmoke, dirDown);
				playSound(SFX_Booster);
			}

			if (flag & ceiling) //Bounce of ceilings
				ym = pixelsToUnits(1);
		}
		else if (ym < 0 && bKey && isKeyDown(gKeyJump)) //Hold gravity
			ym += gravity2;
		else //Normal gravity
			ym += gravity1;

		//Keep Quote down on slopes
		if (!bKey || !isKeyPressed(gKeyJump))
		{
			if (flag & slopeRight && xm < 0)
				ym = -xm;
			if (flag & slopeLeft && xm > 0)
				ym = xm;

			if (flag & ground && flag & slopeH && xm < 0)
				ym = pixelsToUnits(2);
			if (flag & ground && flag & slopeE && xm > 0)
				ym = pixelsToUnits(2);
			if (flag & ground && flag & slopeF && flag & slopeG)
				ym = pixelsToUnits(2);
		}

		//Limit speed
		this->limitSpeed();

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
						createNpc(NPC_WaterDrop1, x + pixelsToUnits(random(-8, 8)), y,
						          xm + random(pixelsToUnits(-1), pixelsToUnits(1)), random(pixelsToUnits(-1), 0x80), dir);

					playSound(SFX_WaterSplash);
				}
			}
			else
			{
				for (int i = 0; i < 8; ++i)
					createNpc(NPC_WaterDrop1, x + pixelsToUnits(random(-8, 8)), y, xm + random(pixelsToUnits(-1),
                              pixelsToUnits(1)), random(pixelsToUnits(-1), 0x80) - ym / 2, dir);

				playSound(SFX_WaterSplash);
			}

			sprash = 1;
		}

		if (!(flag & water))
			sprash = 0;

		//Spike damage
		if (flag & spike)
			this->damage(10);

		//Camera
		if (direct != dirLeft)
		{
			//Move to the right
			index_x += pixelsToUnits(1);

			if (index_x > tilesToUnits(4))
				index_x = tilesToUnits(4);
		}
		else
		{
			//Move to the left
			index_x -= pixelsToUnits(1);

			if (index_x < tilesToUnits(-4))
				index_x = tilesToUnits(-4);
		}

		if (bKey && isKeyDown(gKeyUp))
		{
			//Move up
			index_y -= pixelsToUnits(1);

			if (index_y < tilesToUnits(-4))
				index_y = tilesToUnits(-4);
		}
		else if (bKey && isKeyDown(gKeyDown))
		{
			//Move down
			index_y += pixelsToUnits(1);

			if (index_y > tilesToUnits(4))
				index_y = tilesToUnits(4);
		}
		else
		{
			//Move to neutral position
			if (index_y > pixelsToUnits(1))
				index_y -= pixelsToUnits(1);

			if (index_y < pixelsToUnits(-1))
				index_y += pixelsToUnits(1);
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
		if (isKeyDown(gKeyLeft) || isKeyDown(gKeyRight))
		{
			if (isKeyDown(gKeyLeft))
				xm -= 0x100;
			if (isKeyDown(gKeyRight))
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
		if (isKeyDown(gKeyUp) || isKeyDown(gKeyDown))
		{
			if (isKeyDown(gKeyUp))
				ym -= 0x100;
			if (isKeyDown(gKeyDown))
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
	if (ym < pixelsToUnits(-1) && flag & ceiling)
		createCaret(x, y - hit.top, effect_HeadbumpSparks, 5);
	if (ym > pixelsToUnits(1) && flag & ground)
		createCaret(x, y + hit.bottom, effect_HeadbumpSparks, 5);

	//Limit speed
    if (xm > pixelsToUnits(2))
		xm = pixelsToUnits(2);
	if (xm < pixelsToUnits(-2))
		xm = pixelsToUnits(-2);
	if (ym > pixelsToUnits(2))
		ym = pixelsToUnits(2);
	if (ym < pixelsToUnits(-2))
		ym = pixelsToUnits(-2);

	//Diagonal speed
	if (isKeyDown(gKeyLeft) && isKeyDown(gKeyUp))
	{
		if (xm < -0x30C)
			xm = -0x30C;
		if (ym < -0x30C)
			ym = -0x30C;
	}
	if (isKeyDown(gKeyRight) && isKeyDown(gKeyUp))
	{
		if (xm > 0x30C)
			xm = 0x30C;
		if (ym < -0x30C)
			ym = -0x30C;
	}
	if (isKeyDown(gKeyLeft) && isKeyDown(gKeyDown))
	{
		if (xm < -0x30C)
			xm = -0x30C;
		if (ym > 0x30C)
			ym = 0x30C;
	}
	if (isKeyDown(gKeyRight) && isKeyDown(gKeyDown))
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
			else if (bKey && isKeyDown(gKeyUp) && (isKeyDown(gKeyLeft) || isKeyDown(gKeyRight))) //Look up while walking
			{
				cond |= player_walk;

				if (++ani_wait > 4)
				{
					ani_wait = 0;

					if (++ani_no == 7 || ani_no == 9)
						playSound(SFX_QuoteWalk);
				}

				if (ani_no > 9 || ani_no < 6)
					ani_no = 6;
			}
			else if (bKey && (isKeyDown(gKeyLeft) || isKeyDown(gKeyRight))) //Walk
			{
				cond |= player_walk;

				if (++ani_wait > 4)
				{
					ani_wait = 0;

					if (++ani_no == 2 || ani_no == 4)
						playSound(SFX_QuoteWalk);
				}

				if (ani_no > 4 || ani_no < 1)
					ani_no = 1;
			}
			else if (bKey && isKeyDown(gKeyUp)) //Look up
			{
				if (cond & player_walk)
					playSound(SFX_QuoteWalk);

				cond &= ~player_walk;
				ani_no = 5;
			}
			else //Idle
			{
				if (cond & player_walk)
					playSound(SFX_QuoteWalk);

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

	if (direct != dirLeft)
		rect = rcRight[ani_no];
	else
		rect = rcLeft[ani_no];

	if (equip & equip_mimigaMask)
	{
		rect.top += 32;
		rect.bottom += 32;
	}
}

void player::update(bool bKey)
{

	if (cond & player_visible)
	{
		if (exp_wait)
			--exp_wait;

		if (shock)
		{
			--shock;
		}
		else if (exp_count)
		{
			createValueView(&x, &y, exp_count);
			exp_count = 0;
		}

		if (!unit)
		{
			actNormal(bKey);

			if (!(gGameFlags & 4) && bKey)
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
								startTscEvent(gTsc, 1100);
							}
							else
							{
								startTscEvent(gTsc, 41);

								if (direct != dirLeft)
									createCaret(x, y, effect_DrownedQuote, dirRight);
								else
									createCaret(x, y, effect_DrownedQuote, dirLeft);

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
		else if (unit == 1)
		{
			actStream(bKey);
		}

		cond &= ~player_noFriction;
	}
}

void player::draw()
{
	RECT rcBubble[2];
	rcBubble[0] = { 56, 96, 80, 120 };
	rcBubble[1] = { 80, 96, 104, 120 };

	if ((cond & player_visible) != 0 && !(cond & player_removed))
	{
		//Set held weapon's framerect
		int weaponOffsetY = 0;

		gWeaponRect.left = 24 * (gWeapons[gSelectedWeapon].code % 13);
		gWeaponRect.right = gWeaponRect.left + 24;
		gWeaponRect.top = 96 * (gWeapons[gSelectedWeapon].code / 13);
		gWeaponRect.bottom = gWeaponRect.top + 16;

		if (direct == dirRight)
		{
			gWeaponRect.top += 16;
			gWeaponRect.bottom += 16;
		}

		if (up)
		{
			weaponOffsetY = -4;
			gWeaponRect.top += 32;
			gWeaponRect.bottom += 32;
		}
		else if (down)
		{
			weaponOffsetY = 4;
			gWeaponRect.top += 64;
			gWeaponRect.bottom += 64;
		}

		//Shift up when Quote does
		if (ani_no == 1 || ani_no == 3 || ani_no == 6 || ani_no == 8)
			++gWeaponRect.top;

		const int weaponOffsetX = (direct != dirLeft ? 0 : 8); //Make the weapon shift to the left if facing left
		drawTexture(gSprites[TEX_ARMS], &gWeaponRect, (x - view.left) / 0x200 - gViewport.x / 0x200 - weaponOffsetX, (y - view.top) / 0x200 - gViewport.y / 0x200 + weaponOffsetY);

		if (!((shock >> 1) & 1)) //Invulnerability BLinking
		{
			//Draw Quote
			drawTexture(gSprites[TEX_MYCHAR], &rect, (x - view.left) / 0x200 - gViewport.x / 0x200, (y - view.top) / 0x200 - gViewport.y / 0x200);

			//Draw bubble
			bubble++;
			if (equip & equip_airTank && flag & water)
				drawTexture(gSprites[TEX_CARET], &rcBubble[(bubble >> 1) & 1], x / 0x200 - 12 - gViewport.x / 0x200, y / 0x200 - 12 - gViewport.y / 0x200);
		}
		if (debugFlags & showHitRects)
		{
			SDL_SetRenderDrawColor(gRenderer, 0x80, 0x80, 0x80, 0x00);
			drawRect(unitsToPixels(x - hit.left) - unitsToPixels(gViewport.x),
				unitsToPixels(y - hit.top) - unitsToPixels(gViewport.y),
				unitsToPixels(hit.right + hit.left), unitsToPixels(hit.bottom + hit.top));
		}
		if (debugFlags & showHurtRects)
		{
			SDL_SetRenderDrawColor(gRenderer, 0x0FF, 0x00, 0x00, 0xFF);
			drawRect(unitsToPixels(x - 0x400) - unitsToPixels(gViewport.x),
				unitsToPixels(y - 0x400) - unitsToPixels(gViewport.y),
				unitsToPixels(0x800), unitsToPixels(0x800));
		}
	}
}

void player::doBooster()
{
	//Boosting
	if (this->equip & (equip_booster08 | equip_booster20) && isKeyPressed(gKeyJump) && this->boost_cnt)
	{
		if (this->equip & equip_booster08)
		{
			this->boost_sw = 1;

			if (this->ym > 0x100)
				this->ym /= 2;
		}

		if (this->equip & equip_booster20)
		{
			if (isKeyDown(gKeyUp))
			{
				this->boost_sw = 2;

				this->xm = 0;
				this->ym = -0x5FF;
			}
			else if (isKeyDown(gKeyLeft))
			{
				this->boost_sw = 1;

				this->xm = -0x5FF;
				this->ym = 0;
			}
			else if (isKeyDown(gKeyRight))
			{
				this->boost_sw = 1;

				this->xm = 0x5FF;
				this->ym = 0;
			}
			else if (isKeyDown(gKeyDown))
			{
				this->boost_sw = 3;

				this->xm = 0;
				this->ym = 0x5FF;
			}
			else
			{
				this->boost_sw = 2;	// same as with keyUp

				this->xm = 0;
				this->ym = -0x5FF;
			}
		}
	}
}

void player::doFriction(int resist)
{
	if (!(this->cond & player_noFriction)) //Friction
	{
		if (this->xm < 0)
		{
			if (this->xm <= -resist)
				this->xm += resist;
			else
				this->xm = 0;
		}
		if (this->xm > 0)
		{
			if (this->xm >= resist)
				this->xm -= resist;
			else
				this->xm = 0;
		}
	}
}

int player::getBoostCount()
{
	if (this->equip & (equip_booster08 | equip_booster20))
		return 50;
    return 0;
}

void player::limitSpeed()
{
	if (!(this->flag & water) || this->flag & (windLeft | windUp | windRight | windDown)) //Out of water or in wind
	{
		if (this->xm < -0x5FF)
			this->xm = -0x5FF;
		if (this->xm > 0x5FF)
			this->xm = 0x5FF;

		if (this->ym < -0x5FF)
			this->ym = -0x5FF;
		if (this->ym > 0x5FF)
			this->ym = 0x5FF;
	}
	else // Water and not in wind
	{
		if (this->xm < -0x2FF)
			this->xm = -0x2FF;
		if (this->xm > 0x2FF)
			this->xm = 0x2FF;

		if (this->ym < -0x2FF)
			this->ym = -0x2FF;
		if (this->ym > 0x2FF)
			this->ym = 0x2FF;
	}
}
