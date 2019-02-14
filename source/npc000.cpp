#include "npc000.h"

#include <string>
#include <array>
#include <SDL_messagebox.h>
#include "render.h"
#include "mathUtils.h"
#include "player.h"
#include "sound.h"
#include "caret.h"
#include "npcAct.h"
#include "game.h"
#include "log.h"
#include "stage.h"

void npcActNone(npc *NPC)
{
	NPC->surf = 0x27;
	NPC->doRects({ 0, 0, NPC->view.left >> 8, NPC->view.top >> 8 });	// Checkered texture thing

	// Array to check for having already notified about an NPC
	static bool wasNotifiedAbout[_countof(npcActs)] = { 0 };

	if (wasNotifiedAbout[NPC->code_char])
		return;	// We already notified about this NPC

	wasNotifiedAbout[NPC->code_char] = true;
	std::string msg = "NPC " + std::to_string(NPC->code_char) + " is not implementated yet.";
	logWarning(msg);
	if (debugFlags & notifyOnNotImplemented)
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Missing NPC", msg.c_str(), nullptr);
}

void npcAct000(npc *NPC) // Null
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;

		if (NPC->direct == dirRight)
			NPC->y += tilesToUnits(1);
	}

	NPC->doRects({ 0, 0, 16, 16 });
}

void npcAct001(npc *NPC) // EXP
{
	if (gBackground.mode != 5 && gBackground.mode != 6)
	{
		// Initialisation
		if (!NPC->act_no)
		{
			NPC->act_no = 1;
			NPC->ani_no = random(0, 4);
			NPC->xm = random(pixelsToUnits(-1), pixelsToUnits(1));
			NPC->ym = random(pixelsToUnits(-2), 0);

			if (random(0, 1))
				NPC->direct = dirLeft;
			else
				NPC->direct = dirRight;
		}

		// Gravity
		if (NPC->flag & water)
			NPC->ym += pixelsToUnits(0.041015625);	// Gravity in wqter
		else
			NPC->ym += pixelsToUnits(0.08203125);	// Gravity in air

		// Bounce off walls
		if (NPC->flag & leftWall && NPC->xm < 0)
			NPC->xm = -NPC->xm;
		if (NPC->flag & rightWall && NPC->xm > 0)
			NPC->xm = -NPC->xm;
		if (NPC->flag & ceiling && NPC->ym < 0)
			NPC->ym = -NPC->ym;

		// Bounce off floor
		if (NPC->flag & ground)
		{
			playSound(SFX_EXPBounce);
			NPC->ym = pixelsToUnits(-1.25);
			NPC->xm = 2 * NPC->xm / 3;
		}

		// Clip out of floors???
		if (NPC->flag & (leftWall | rightWall | ground))
		{
			playSound(SFX_EXPBounce);
			if (++NPC->count2 > 2)
				NPC->y -= pixelsToUnits(1);
		}
		else
			NPC->count2 = 0;

		// Limit speed
		NPC->limitXVel(0x5FF);
		NPC->limitYVel(0x5FF);
	}
	else	// When in places with weird XP gravity (outside wall, etc.)
	{
		// Initialization
		if (!NPC->act_no)
		{
			NPC->act_no = 1;
			NPC->ym = random(pixelsToUnits(-0.25), pixelsToUnits(0.25));
			NPC->xm = random(pixelsToUnits(0.248046875), pixelsToUnits(0.5));
		}

		// Fly to the left
		NPC->xm -= 8;
		if (NPC->x < pixelsToUnits(5))
			NPC->cond = 0;

		// Snap to x-pos pixelsToUnits(-3) if less than that (this is useless wtf pixel)
		if (NPC->x < pixelsToUnits(-3))
			NPC->x = pixelsToUnits(-3);

		// Bounce off walls
		if (NPC->flag & leftWall)
			NPC->xm = pixelsToUnits(0.5);
		if (NPC->flag & ceiling)
			NPC->ym = pixelsToUnits(0.125);
		if (NPC->flag & ground)
			NPC->ym = pixelsToUnits(-0.125);
	}

	// Move
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	// Framerects
	constexpr std::array<RECT, 6> rcNPC
	{{
		{ 0x00, 0x10, 0x10, 0x20 },
		{ 0x10, 0x10, 0x20, 0x20 },
		{ 0x20, 0x10, 0x30, 0x20 },
		{ 0x30, 0x10, 0x40, 0x20 },
		{ 0x40, 0x10, 0x50, 0x20 },
		{ 0x50, 0x10, 0x60, 0x20 },
	}};

	// Animate
	++NPC->ani_wait;
	if (NPC->direct != dirLeft)
		NPC->animateReverse(2, 5, 0);
	else 
		NPC->animate(2, 0, 5);

	NPC->doRects(rcNPC);

	//Change size
	if (NPC->act_no)
	{
		if (NPC->exp == 5)
		{
			NPC->rect.top += 0x10;
			NPC->rect.bottom += 0x10;
		}
		else if (NPC->exp == 20)
		{
			NPC->rect.top += 0x20;
			NPC->rect.bottom += 0x20;
		}

		NPC->act_no = 1;
	}

	// Disappear after 10 seconds
	if (++NPC->count1 > secondsToFrames(10) && NPC->ani_no == 5 && NPC->ani_wait == 2)
		NPC->cond = 0;

	// Blink every odd frame after 8 seconds
	if (NPC->count1 > secondsToFrames(8) && (NPC->count1 / 2 & 1))
		NPC->rect = { 0, 0, 0, 0 };
}

void npcAct002(npc *NPC) // Behemoth (enemy)
{
	constexpr int damageOnCharge = 5;
	constexpr int normalDamage = 1;

	constexpr std::array<RECT, 7> rcLeft
	{{
		{ 32, 0, 64, 24 },
		{ 0, 0, 32, 24 },
		{ 32, 0, 64, 24 },
		{ 64, 0, 96, 24 },
		{ 96, 0, 128, 24 },
		{ 128, 0, 160, 24 },
		{ 160, 0, 192, 24 },
	}};

	constexpr std::array<RECT, 7> rcRight
	{{
		{ 32, 24, 64, 48 },
		{ 0, 24, 32, 48 },
		{ 32, 24, 64, 48 },
		{ 64, 24, 96, 48 },
		{ 96, 24, 128, 48 },
		{ 128, 24, 160, 48 },
		{ 160, 24, 192, 48 },
	}};

	if (NPC->flag & leftWall)
		NPC->direct = dirRight;
	else if (NPC->flag & rightWall)
		NPC->direct = dirLeft;

	switch (NPC->act_no)
	{
	case 0: // Normal act
		NPC->moveInDir(pixelsToUnits(0.5));
		NPC->animate(8, 0, 3);

		if (NPC->shock)
		{
			NPC->count1 = 0;
			NPC->act_no = 1;
			NPC->ani_no = 4;
		}

		break;

	case 1: // Hit
		NPC->xm = 7 * NPC->xm / 8;

		if (++NPC->count1 > secondsToFrames(0.8))
		{
			if (NPC->shock) // Shot multiple times in a row, charge
			{
				NPC->count1 = 0;
				NPC->act_no = 2;
				NPC->ani_no = 6;
				NPC->ani_wait = 0;
				NPC->damage = damageOnCharge;
			}
			else
			{
				NPC->act_no = 0;
				NPC->ani_wait = 0;
			}
		}

		break;

	case 2: // Charge
		NPC->moveInDir(pixelsToUnits(2));

		if (++NPC->count1 > secondsToFrames(4))
		{
			NPC->act_no = 0;
			NPC->damage = normalDamage;
		}

		NPC->animate(5, 5, 6);
	}

	// Move
	NPC->ym += pixelsToUnits(0.125);
	NPC->limitYVel(pixelsToUnits(3) - 1);

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);
}

void npcAct003(npc *NPC) // Null, spawned upon NPC death, disappears
{
	if (++NPC->count1 > secondsToFrames(2))
		NPC->cond = 0;
	NPC->doRects({ 0, 0, 0, 0 });
}

void npcAct004(npc *NPC) // Smoke
{
	constexpr std::array<RECT, 8> rcLeft
	{{
		{ 16, 0, 17, 1 },
		{ 16, 0, 32, 16 },
		{ 32, 0, 48, 16 },
		{ 48, 0, 64, 16 },
		{ 64, 0, 80, 16 },
		{ 80, 0, 96, 16 },
		{ 96, 0, 112, 16 },
		{ 112, 0, 128, 16 },
	}};

	constexpr std::array<RECT, 8> rcUp
	{{
		{ 16, 0, 17, 1 },
		{ 80, 48, 96, 64 },
		{ 0, 128, 16, 144 },
		{ 16, 128, 32, 144 },
		{ 32, 128, 48, 144 },
		{ 48, 128, 64, 144 },
		{ 64, 128, 80, 144 },
		{ 80, 128, 96, 144 },
	}};	

	if (NPC->act_no)
	{
		NPC->xm = 20 * NPC->xm / 21;
		NPC->ym = 20 * NPC->ym / 21;

		NPC->x += NPC->xm;
		NPC->y += NPC->ym;
	}
	else
	{
		if (NPC->direct == dirLeft || NPC->direct == dirUp)
		{
			const uint8_t deg = random(0, 255);

			const int degCos = getCos(deg);
			NPC->xm = 
				unitsToPixels(degCos * random(pixelsToUnits(1), pixelsToUnits(3) - 1));

			const int degSin = getSin(deg);
			NPC->ym = unitsToPixels(degSin * random(pixelsToUnits(1), pixelsToUnits(3) - 1));
		}

		NPC->ani_no = random(0, 4);
		NPC->ani_wait = random(0, 3);

		NPC->act_no = 1;
	}

	NPC->animate(4);

	if (static_cast<size_t>(NPC->ani_no) < rcLeft.size())
	{
		if (NPC->direct != dirUp)
			NPC->rect = rcLeft.at(NPC->ani_no);
		else
			NPC->rect = rcUp.at(NPC->ani_no);
	}
	else
		NPC->cond = 0;
}

void npcAct005(npc *NPC) // Critter, Hopping Green (enemy)
{
	enum
	{
		init = 0,
		waiting = 1,
		startJump = 2,
		jumping = 3,
	};

	switch (NPC->act_no)
	{
	case init: // Initialize
		NPC->y += pixelsToUnits(3) - 1;
		NPC->act_no = waiting;
		break;

	case waiting:		
		// Face towards player
		if (NPC->x <= gCurrentPlayer.x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;

		// TargetX being used as timer (10/10 pixel code)
		if (NPC->tgt_x < secondsToFrames(2))
			++NPC->tgt_x;

		// Timer for looking at Quote
		if (NPC->act_wait < 8 
			|| NPC->x - tilesToUnits(7) >= gCurrentPlayer.x || NPC->x + tilesToUnits(7) <= gCurrentPlayer.x 
			|| NPC->y - tilesToUnits(5) >= gCurrentPlayer.y || NPC->y + tilesToUnits(5) <= gCurrentPlayer.y)
		{
			if (NPC->act_wait < 8)
				++NPC->act_wait;
			NPC->ani_no = 0;
		}
		else
			NPC->ani_no = 1;

		// If shocked, jump
		if (NPC->shock)
		{
			NPC->act_no = startJump;
			NPC->act_wait = 0;

			NPC->ani_no = 0;
		}

		// Go into "going to jump" state
		if (NPC->act_wait >= 8
		        && NPC->tgt_x >= secondsToFrames(2)
		        && NPC->x - tilesToUnits(4) < gCurrentPlayer.x
		        && NPC->x + tilesToUnits(4) > gCurrentPlayer.x
		        && NPC->y - tilesToUnits(5) < gCurrentPlayer.y
		        && NPC->y + tilesToUnits(3) > gCurrentPlayer.y)
		{
			NPC->act_no = startJump;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
		}
		break;

	case startJump: // Going to jump
		if (++NPC->act_wait > secondsToFrames(0.16))
		{
			// Jump
			NPC->act_no = jumping;
			NPC->ani_no = 2;

			NPC->flag &= ~ground;
			NPC->ym = pixelsToUnits(-3) + 1;

			// Jump in direction facing
			NPC->moveInDir(pixelsToUnits(0.5));
			playSound(SFX_CritterHop);
		}

		break;

	case 3: // In air
		if (NPC->flag & ground) // Landed on the ground after jumping
		{
			NPC->act_no = waiting;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->xm = 0;
		}
	}

	//Gravity
	NPC->ym += pixelsToUnits(0.125);
	NPC->limitYVel(pixelsToUnits(3) - 1);

	//Move critter
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Change framerect
	NPC->doRects({ (NPC->ani_no << 4), 48, ((NPC->ani_no + 1) << 4), 64 },
              { (NPC->ani_no << 4), 64, ((NPC->ani_no + 1) << 4), 80 });
}

void npcAct006(npc *NPC) //Beetle
{
	const int act_no = NPC->act_no;

	switch (act_no)
	{
	case 0: //Init
		if (NPC->direct != dirLeft)
			NPC->act_no = 3;
		else
			NPC->act_no = 1;

		break;

	case 1: //Fly left
		NPC->xm -= 16;

		if (NPC->xm < -0x400)
			NPC->xm = -0x400;

		//Move
		if (NPC->shock)
			NPC->x += NPC->xm / 2;
		else
			NPC->x += NPC->xm;

		//Animate
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 2)
			NPC->ani_no = 1;

		//Hit wall, turn
		if (NPC->flag & leftWall)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->xm = 0;
			NPC->direct = dirRight;
		}

		break;

	case 2:
		//Wait 60 frames and then fly in facing direct
		if (++NPC->act_wait > 60)
		{
			if (NPC->direct != dirLeft)
				NPC->act_no = 3;
			else
				NPC->act_no = 1;

			NPC->ani_wait = 0;
			NPC->ani_no = 1;
		}

		break;

	case 3: //Fly right
		NPC->xm += 16;

		if (NPC->xm > 0x400)
			NPC->xm = 0x400;

		//Move
		if (NPC->shock)
			NPC->x += NPC->xm / 2;
		else
			NPC->x += NPC->xm;

		//Animate
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 2)
			NPC->ani_no = 1;

		//Hit wall, turn
		if (NPC->flag & rightWall)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->xm = 0;
			NPC->direct = dirLeft;
		}

		break;
	}

	//Set framerect
	NPC->doRects({ (NPC->ani_no << 4), 80, (NPC->ani_no << 4) + 16, 96 },
              { (NPC->ani_no << 4), 96, (NPC->ani_no << 4) + 16, 112 });
}

void npcAct007(npc *NPC) //Basil
{
	std::array<RECT, 3> rcRight;
	std::array<RECT, 3> rcLeft;

	rcLeft[0] = { 256, 64, 288, 80 };
	rcLeft[1] = { 256, 80, 288, 96 };
	rcLeft[2] = { 256, 96, 288, 112 };

	rcRight[0] = { 288, 64, 320, 80 };
	rcRight[1] = { 288, 80, 320, 96 };
	rcRight[2] = { 288, 96, 320, 112 };

	const int act_no = NPC->act_no;
	switch (act_no)
	{
	case 1:
		//Move left
		NPC->xm -= 64;

		//Turn when far from player
		if (NPC->x < gCurrentPlayer.x - 98304)
			NPC->act_no = 2;

		//Turn when hit wall
		if (NPC->flag & leftWall)
		{
			NPC->xm = 0;
			NPC->act_no = 2;
		}

		break;

	case 2:
		//Move right
		NPC->xm += 64;

		//Turn when far from player
		if (NPC->x > gCurrentPlayer.x + 98304)
			NPC->act_no = 1;

		//Turn when hit wall
		if (NPC->flag & rightWall)
		{
			NPC->xm = 0;
			NPC->act_no = 1;
		}

		break;

	case 0:
		//Start at player's x
		NPC->x = gCurrentPlayer.x;

		//Move in facing direct
		if (NPC->direct != dirLeft)
			NPC->act_no = 2;
		else
			NPC->act_no = 1;

		break;
	}

	//Face in moving direct
	if (NPC->xm >= 0)
		NPC->direct = dirRight;
	else
		NPC->direct = dirLeft;

	//Limit speed
	if (NPC->xm > 0x5FF)
		NPC->xm = 0x5FF;

	if (NPC->xm < -0x5FF)
		NPC->xm = -0x5FF;

	//Move
	NPC->x += NPC->xm;

	//Animate
	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 2)
		NPC->ani_no = 0;

	//Set framerect
	NPC->doRects(rcLeft, rcRight);
}

void npcAct008(npc *NPC) //Follow beetle (egg corridor)
{
	std::array<RECT, 2> rcRight;
	std::array<RECT, 2> rcLeft;

	rcLeft[0] = { 80, 80, 96, 96 };
	rcLeft[1] = { 96, 80, 112, 96 };

	rcRight[0] = { 80, 96, 96, 112 };
	rcRight[1] = { 96, 96, 112, 112 };

	const int act_no = NPC->act_no;
	if (act_no)
	{
		if (act_no == 1)
		{
			if (NPC->x <= gCurrentPlayer.x)
			{
				NPC->direct = dirRight;
				NPC->xm += 0x10;
			}
			else
			{
				NPC->direct = dirLeft;
				NPC->xm -= 0x10;
			}

			if (NPC->xm > 0x2FF)
				NPC->xm = 0x2FF;
			if (NPC->xm < -0x2FF)
				NPC->xm = -0x2FF;

			if (NPC->y >= NPC->tgt_y)
				NPC->ym -= 0x8;
			else
				NPC->ym += 0x8;

			if (NPC->ym > 0x100)
				NPC->ym = 0x100;
			if (NPC->ym < -0x100)
				NPC->ym = -0x100;

			if (NPC->shock)
			{
				NPC->x += NPC->xm / 2;
				NPC->y += NPC->ym / 2;
			}
			else
			{
				NPC->x += NPC->xm;
				NPC->y += NPC->ym;
			}
		}
	}
	else
	{
		if (gCurrentPlayer.x >= NPC->x + 0x2000 || gCurrentPlayer.x <= NPC->x - 0x2000)
		{
			NPC->bits &= ~npc_shootable;
			NPC->rect.right = 0;
			NPC->damage = 0;
			NPC->xm = 0;
			NPC->ym = 0;

			return;
		}

		NPC->bits |= npc_shootable;
		NPC->ym = -256;
		NPC->tgt_y = NPC->y;
		NPC->act_no = 1;
		NPC->damage = 2;

		if (NPC->direct != dirLeft)
		{
			NPC->x = gCurrentPlayer.x - 0x20000;
			NPC->xm = 767;
		}
		else
		{
			NPC->x = gCurrentPlayer.x + 0x20000;
			NPC->xm = -767;
		}
	}

	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 1)
		NPC->ani_no = 0;

	NPC->doRects(rcLeft, rcRight);
}

void npcAct009(npc *NPC) //Balrog drop in
{
	std::array<RECT, 3> rcLeft;
	std::array<RECT, 3> rcRight;

	rcLeft[0] = { 0, 0, 40, 24 };
	rcLeft[1] = { 80, 0, 120, 24 };
	rcLeft[2] = { 120, 0, 160, 24 };

	rcRight[0] = { 0, 24, 40, 48 };
	rcRight[1] = { 80, 24, 120, 48 };
	rcRight[2] = { 120, 24, 160, 48 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 2;
	// Fallthrough
	case 1: //Falling
		NPC->ym += 0x20;

		//Go through ceiling for the first 40 frames of existing, then become solid
		if (NPC->count1 >= 40)
		{
			NPC->bits &= ~npc_ignoreSolid;
			NPC->bits |= npc_solidSoft;
		}
		else
			++NPC->count1;

		//Landing
		if (NPC->flag & ground)
		{
			//Create smoke
			for (int i = 0; i < 4; ++i)
				createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0));

			//Go into landed state
			NPC->act_no = 2;
			NPC->ani_no = 1;
			NPC->act_wait = 0;
			playSound(SFX_LargeObjectHitGround);
			gViewport.quake = 30;
		}

		break;

	case 2: //Landed
		if (++NPC->act_wait > 16)
		{
			NPC->act_no = 3;
			NPC->ani_no = 0;
			NPC->ani_wait = 0;
		}

		break;

	default:
		break;
	}

	//Speed limit and move
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;
	if (NPC->ym < -0x5FF)
		NPC->ym = -0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Set framerect
	NPC->doRects(rcLeft, rcRight);
}

void npcAct010(npc *NPC) // Balrog, Shooting (boss)
{
	enum
	{
		init = 0,
		start = 1,
		shooting = 2,
		startJump = 3,
		jumping = 4,
		land = 5,
	};

	constexpr int jumpingDamage = 5;
	constexpr int orbsShotPerPhase = 3;

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = start;
		// Fallthrough
	case start:
		if (++NPC->act_wait > secondsToFrames(0.24))
		{
			NPC->act_no = shooting;
			NPC->act_wait = 0;
			NPC->count1 = orbsShotPerPhase;	// Shoot 3 orbs
			NPC->ani_no = 1;
		}
		break;

	case shooting:
		if (++NPC->act_wait >= secondsToFrames(0.32))
		{
			--NPC->count1;
			NPC->act_wait = 0;

			auto deg = getAtan(NPC->x - gCurrentPlayer.x, NPC->y - gCurrentPlayer.y + tilesToUnits(0.25));
			deg += random(-0x10, 0x10);
			createNpc(NPC_ProjectileBalrogEnergyBallInvincible, NPC->x, NPC->y + tilesToUnits(0.25)
				, getSin(deg), getCos(deg));
			playSound(SFX_EnemyShootProjectile);

			if (!NPC->count1)
			{
				NPC->act_no = startJump;
				NPC->act_wait = 0;
			}
		}
		break;

	case startJump:	// Jump towards Quote
		if (++NPC->act_wait > 3)
		{
			NPC->act_no = jumping;
			NPC->act_wait = 0;
			NPC->xm = (gCurrentPlayer.x - NPC->x) / 100;
			NPC->ym = pixelsToUnits(-3);
			NPC->ani_no = 3;
		}
		break;

	case jumping:
		if (NPC->flag & (leftWall | rightWall))	// Stop when touching a wall
			NPC->xm = 0;

		if (NPC->y + tilesToUnits(1) >= gCurrentPlayer.y)
			NPC->damage = 0;	// No damage when touching ground
		else
			NPC->damage = jumpingDamage;

		if (NPC->flag & ground)
		{
			NPC->act_no = land;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
			playSound(SFX_LargeObjectHitGround);
			gViewport.quake = 30;
			NPC->damage = 0;	// No damage when landed
		}
		break;

	case land:
		NPC->xm = 0;
		if (++NPC->act_wait > 3)
		{
			NPC->act_no = start;	// Loop through states shooting-jumping
			NPC->act_wait = 0;
		}

	default:
		break;
	}

	NPC->ym += pixelsToUnits(0.1);
	if (NPC->ym >= pixelsToUnits(3))
		NPC->ym = pixelsToUnits(3) - 1;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	constexpr std::array<RECT, 4> rcLeft
	{{
		{0, 0, 40, 24},
		{40, 0, 80, 24},
		{80, 0, 120, 24},
		{120, 0, 160, 24},
	}};

	constexpr std::array<RECT, 4> rcRight
	{{
		{0, 24, 40, 48},
		{40, 24, 80, 48},
		{80, 24, 120, 48},
		{120, 24, 160, 48},
	}};

	NPC->facePlayer();
	NPC->doRects(rcLeft, rcRight);
}

void npcAct011(npc *NPC) //Bubble
{
	std::array<RECT, 3> rect;

	rect[0] = { 208, 104, 224, 120 };
	rect[1] = { 224, 104, 240, 120 };
	rect[2] = { 240, 104, 256, 120 };

	if (NPC->flag & 0xFF)
	{
		NPC->cond = 0;
		createCaret(NPC->x, NPC->y, effect_RisingDisc);
	}

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;

		if (++NPC->ani_no > 2)
			NPC->ani_no = 0;
	}

	NPC->doRects(rect);

	if (++NPC->count1 > 150)
	{
		createCaret(NPC->x, NPC->y, effect_RisingDisc);
		NPC->cond = 0;
	}
}

void npcAct012(npc *NPC) //Balrog cutscene
{
	int x;
	int y;

	std::array<RECT, 14> rcLeft;
	std::array<RECT, 14> rcRight;

	rcLeft[0] = { 0x000, 0x000, 0x028, 0x018 };
	rcLeft[1] = { 0x0A0, 0x000, 0x0C8, 0x018 };
	rcLeft[2] = { 0x050, 0x000, 0x078, 0x018 };
	rcLeft[3] = { 0x078, 0x000, 0x0A0, 0x018 };
	rcLeft[4] = { 0x0F0, 0x000, 0x118, 0x018 };
	rcLeft[5] = { 0x0C8, 0x000, 0x0F0, 0x018 };
	rcLeft[6] = { 0x118, 0x000, 0x140, 0x018 };
	rcLeft[7] = { 0x000, 0x000, 0x000, 0x000 };
	rcLeft[8] = { 0x050, 0x030, 0x078, 0x048 };
	rcLeft[9] = { 0x000, 0x030, 0x028, 0x048 };
	rcLeft[10] = { 0x000, 0x000, 0x028, 0x018 };
	rcLeft[11] = { 0x028, 0x030, 0x050, 0x048 };
	rcLeft[12] = { 0x000, 0x000, 0x028, 0x018 };
	rcLeft[13] = { 0x118, 0x000, 0x140, 0x018 };

	rcRight[0] = { 0x000, 0x018, 0x028, 0x030 };
	rcRight[1] = { 0x0A0, 0x018, 0x0C8, 0x030 };
	rcRight[2] = { 0x050, 0x018, 0x078, 0x030 };
	rcRight[3] = { 0x078, 0x018, 0x0A0, 0x030 };
	rcRight[4] = { 0x0F0, 0x018, 0x118, 0x030 };
	rcRight[5] = { 0x0C8, 0x018, 0x0F0, 0x030 };
	rcRight[6] = { 0x118, 0x018, 0x140, 0x030 };
	rcRight[7] = { 0x000, 0x000, 0x000, 0x000 };
	rcRight[8] = { 0x050, 0x048, 0x078, 0x060 };
	rcRight[9] = { 0x000, 0x048, 0x028, 0x060 };
	rcRight[10] = { 0x000, 0x018, 0x028, 0x030 };
	rcRight[11] = { 0x028, 0x048, 0x050, 0x060 };
	rcRight[12] = { 0x000, 0x018, 0x028, 0x030 };
	rcRight[13] = { 0x118, 0x018, 0x140, 0x030 };

	switch (NPC->act_no)
	{
	case 0: //Stand and blink
		if (NPC->direct == dirAuto)
		{
			if (NPC->x <= gCurrentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}

		NPC->act_no = 1;
		NPC->ani_no = 0;
	// Fallthrough
	case 1: //Stand
		if (random(0, 100) == 0)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 2: //Blinking
		if (++NPC->act_wait > 16)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;

	case 10: //Jump up
		if (NPC->direct == dirAuto)
		{
			if (NPC->x <= gCurrentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}

		NPC->act_no = 11;
		NPC->ani_no = 2;
		NPC->act_wait = 0;
		NPC->tgt_x = 0;
	// Fallthrough
	case 11: //About to jump
		if (++NPC->act_wait > 30)
		{
			NPC->act_no = 12;
			NPC->act_wait = 0;
			NPC->ani_no = 3;
			NPC->ym = -0x800;
			NPC->bits |= npc_ignoreSolid;
		}
		break;

	case 12: //Jumping up
		if (NPC->flag & (leftWall | rightWall)) //Yeah let's ignore the fact that ignoresolid's been turned on
			NPC->xm = 0;

		if (NPC->y < 0)
		{
			NPC->code_char = 0;
			playSound(SFX_LargeObjectHitGround);
			gViewport.quake = 30;
		}
		break;

	case 20: //Defeated?
		if (NPC->direct == dirAuto)
		{
			if (NPC->x <= gCurrentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}

		NPC->act_no = 21;
		NPC->ani_no = 5;
		NPC->act_wait = 0;
		NPC->count1 = 0;

		for (int i = 0; i < 4; ++i)
			createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0));

		playSound(SFX_Explosion);
	// Fallthrough
	case 21:
		NPC->tgt_x = 1;

		if (NPC->flag & ground)
			++NPC->act_wait;

		if (++NPC->count1 / 2 & 1)
			NPC->x += 512;
		else
			NPC->x -= 512;

		if (NPC->act_wait > 100)
		{
			NPC->act_no = 11;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}

		NPC->ym += 32;

		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;

		break;

	case 30: //Smile
		NPC->ani_no = 4;

		if (++NPC->act_wait > 100)
		{
			NPC->act_no = 0;
			NPC->ani_no = 0;
		}
		break;

	case 40: //"Super panic"
		if (NPC->direct == dirAuto)
		{
			if (NPC->x <= gCurrentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}

		NPC->act_no = 41;
		NPC->act_wait = 0;
		NPC->ani_no = 5;
	// Fallthrough
	case 41:
		if (++NPC->ani_wait / 2 & 1)
			NPC->ani_no = 5;
		else
			NPC->ani_no = 6;

		break;

	case 42: //"Uh oh! Image blinks"
		if (NPC->direct == dirAuto)
		{
			if (NPC->x <= gCurrentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}

		NPC->act_no = 43;
		NPC->act_wait = 0;
		NPC->ani_no = 6;
	// Fallthrough
	case 43:
		if (++NPC->ani_wait / 2 & 1)
			NPC->ani_no = 7;
		else
			NPC->ani_no = 6;

		break;

	case 50: //Back turned
		NPC->ani_no = 8;
		NPC->xm = 0;
		break;

	case 60: //Walk forward no gravity
		NPC->act_no = 61;
		NPC->ani_no = 9;
		NPC->ani_wait = 0;
	// Fallthrough
	case 0x3D:
		if (++NPC->ani_wait > 3)
		{
			NPC->ani_wait = 0;

			if (++NPC->ani_no == 10 || NPC->ani_no == 11)
				playSound(SFX_HitGround);
		}

		if (NPC->ani_no > 12)
			NPC->ani_no = 9;

		if (NPC->direct != dirLeft)
			NPC->xm = 0x200;
		else
			NPC->xm = -0x200;
		break;

	case 70: //"Uh oh! Vanish"
		NPC->act_no = 71;
		NPC->act_wait = 64;
		playSound(SFX_Teleport);
		NPC->ani_no = 13;
	// Fallthrough
	case 71:
		if (!--NPC->act_wait)
			NPC->cond = 0;
		break;

	case 80: //"Panic"
		NPC->count1 = 0;
		NPC->act_no = 81;
	// Fallthrough
	case 81:
		if (++NPC->count1 / 2 & 1)
			NPC->x += 512;
		else
			NPC->x -= 512;

		NPC->ani_no = 5;
		NPC->xm = 0;
		NPC->ym += 32;
		break;

	case 100: //Jump up and destroy ceiling (ending)
		NPC->act_no = 101;
		NPC->act_wait = 0;
		NPC->ani_no = 2;
	// Fallthrough
	case 101: //Going to jump
		if (++NPC->act_wait > 20)
		{
			NPC->act_no = 102;
			NPC->act_wait = 0;
			NPC->ani_no = 3;
			NPC->ym = -2048;
			NPC->bits |= npc_ignoreSolid;

			createNpc(NPC_BalrogCrashingThroughWall, 0, 0, 0, 0, dirLeft, NPC);
			createNpc(NPC_BalrogCrashingThroughWall, 0, 0, 0, 0, dirUp, NPC);
		}
		break;

	case 102:
		x = NPC->x / 0x2000;
		y = NPC->y / 0x2000;

		if (y >= 0 && y <= 34 && changeTile(x, y, 0))
		{
			changeTile(x - 1, y, 0);
			changeTile(x + 1, y, 0);
			playSound(SFX_MissileImpact);
			gViewport.quake2 = 10;
		}

		if (NPC->y < -0x4000)
		{
			NPC->code_char = 0;
			gViewport.quake = 30;
		}

		break;

	default:
		break;
	}

	const bool createSmoke = NPC->tgt_x && !random(0, 10);
	if (createSmoke)
		createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0));

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);

	if (NPC->act_no == 71)
	{
		NPC->rect.bottom = NPC->act_wait / 2 + NPC->rect.top;
		if (NPC->act_wait & 1)
			++NPC->rect.left;
	}
}

void npcAct013(npc *NPC) // Forcefield
{
	constexpr std::array<RECT, 4> rcNPC = { {{128, 0, 144, 16 }, {144, 0, 160, 16 }, {160, 0, 176, 16}, {176, 0, 192, 16 } } };

	if (++NPC->ani_wait > 0)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 3)
		NPC->ani_no = 0;

	NPC->doRects(rcNPC);
}

void npcAct014(npc * NPC) // Santa's Key
{
	constexpr std::array<RECT, 3> rcNPC = { { {192, 0, 208, 16}, {208, 0, 224, 16}, {224, 0, 240, 16} } };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		if (NPC->direct == dirRight)
		{
			NPC->ym = -0x200;

			for (int i = 0; i < 4; ++i)
			{
				const int childYVel = random(-0x600, 0);
				const int childXVel = random(-0x155, 0x155);
				const int childYPos = NPC->y + pixelsToUnits(random(-12, 12));
				const int childXPos = NPC->x + pixelsToUnits(random(-12, 12));
				createNpc(NPC_Smoke, childXPos, childYPos, childXVel, childYVel);
			}
		}
	}
	NPC->animate(1, 0, 2);

	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->y += NPC->ym;

	NPC->doRects(rcNPC);
}

void npcAct015(npc *NPC) //Closed chest
{
	const int act_no = NPC->act_no;

	switch (act_no)
	{
	case 0:
		NPC->act_no = 1;

		NPC->bits |= npc_interact;

		//Spawn with smoke and stuff
		if (NPC->direct == dirRight)
		{
			NPC->ym = -0x200;

			for (int i = 0; i < 4; ++i)
				createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0));
		}

		if (random(0, 30) == 0)
			NPC->act_no = 2;

		break;

	case 1:
		NPC->ani_no = 0;

		if (random(0, 30) == 0)
			NPC->act_no = 2;

		break;

	case 2:
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 2)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}

		break;

	default:
		break;
	}

	//Gravity
	NPC->ym += 0x40;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	//Move
	NPC->y += NPC->ym;

	//Framerect
	NPC->doRects({ 240 + (NPC->ani_no << 4), 0, 256 + (NPC->ani_no << 4), 16 });
}

void npcAct016(npc *NPC) //Save point
{
	//First created
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->bits |= npc_interact;

		//Spawn with smoke and stuff
		if (NPC->direct != dirLeft)
		{
			NPC->ym = -0x200;
			NPC->bits &= ~npc_interact;

			for (int i = 0; i < 4; ++i)
				createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0));
		}
	}

	if (NPC->flag & ground)
		NPC->bits |= npc_interact;

	//Animate
	if (++NPC->ani_wait > 2)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 7)
		NPC->ani_no = 0;

	//Set framerect
	NPC->doRects({ 96 + (NPC->ani_no << 4), 16, 112 + (NPC->ani_no << 4), 32 });

	//Fall down
	NPC->ym += 0x40;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->y += NPC->ym;
}

void npcAct017(npc *NPC) //Health refill
{
	int rand;
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;

		//Spawn with smoke and stuff
		if (NPC->direct != dirLeft)
		{
			NPC->ym = -0x200;

			for (int i = 0; i < 4; ++i)
				createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0)
				);
		}
	// Fallthrough
	case 1:
		rand = random(0, 30);

		if (rand > 9)
		{
			if (rand > 24)
				NPC->act_no = 4;
			else
				NPC->act_no = 3;
		}
		else
		{
			NPC->act_no = 2;
		}

		NPC->act_wait = random(16, 64);
		NPC->ani_wait = 0;

		break;

	case 2:
		NPC->rect = { 288, 0, 304, 16 };

		if (!--NPC->act_wait)
			NPC->act_no = 1;

		break;

	case 3:
		++NPC->ani_wait;

		if (NPC->ani_wait & 1)
		{
			NPC->rect = { 288, 0, 304, 16 };
		}
		else
		{
			NPC->rect = { 304, 0, 320, 16 };
		}

		if (!--NPC->act_wait)
			NPC->act_no = 1;

		break;

	case 4:
		NPC->rect = { 304, 0, 320, 16 };

		if (!--NPC->act_wait)
			NPC->act_no = 1;

		break;

	default:
		break;
	}

	NPC->ym += 0x40;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->y += NPC->ym;
}

void npcAct018(npc *NPC) //Door
{
	const int act_no = NPC->act_no;
	if (act_no)
	{
		if (act_no == 1)
		{
			NPC->act_no = 0;
			NPC->rect = { 224, 16, 240, 40 };
		}
	}
	else
        NPC->doRects({224, 16, 240, 40}, {192, 112, 208, 136});
}

void npcAct019(npc *NPC) //Balrog burst
{
	std::array<RECT, 4> rcLeft;
	std::array<RECT, 4> rcRight;

	rcLeft[0] = { 0, 0, 40, 24 };
	rcLeft[1] = { 160, 0, 200, 24 };
	rcLeft[2] = { 80, 0, 120, 24 };
	rcLeft[3] = { 120, 0, 160, 24 };

	rcRight[0] = { 0, 24, 40, 48 };
	rcRight[1] = { 160, 24, 200, 48 };
	rcRight[2] = { 80, 24, 120, 48 };
	rcRight[3] = { 120, 24, 160, 48 };

	switch (NPC->act_no)
	{
	case 0:
		for (int i = 0; i <= 15; ++i)
			createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0));

		NPC->y += 0x1400;
		NPC->act_no = 1;
		NPC->ani_no = 3;
		NPC->ym = -0x100;
		playSound(12);
		playSound(26);
		gViewport.quake = 30;
//Fallthrough
	case 1:
		NPC->ym += 16;

		if (NPC->ym > 0 && NPC->flag & ground)
		{
			NPC->act_no = 2;
			NPC->ani_no = 2;
			NPC->act_wait = 0;
			playSound(26);
			gViewport.quake = 30;
		}
		break;

	case 2:
		if (++NPC->act_wait > 16)
		{
			NPC->act_no = 3;
			NPC->ani_no = 0;
			NPC->ani_wait = 0;
		}
		break;

	case 3:
		if (random(0, 100) == 0)
		{
			NPC->act_no = 4;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 4:
		if (++NPC->act_wait > 16)
		{
			NPC->act_no = 3;
			NPC->ani_no = 0;
		}
		break;

	default:
		break;
	}

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;
	if (NPC->ym < -0x5FF)
		NPC->ym = -0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);
}

