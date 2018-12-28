#include "npc060.h"

#include <array>
#include "mathUtils.h"
#include "player.h"
#include "sound.h"
#include "game.h"
#include "stage.h"
#include "render.h"

using std::array;

void npcAct060(npc *NPC) //Toroko
{
	constexpr int frameMap[11] = { 0, 1, 2, 1, 3, 4, 5, 4, 6, 7, 8 };

	enum
	{
	    init = 0,
	    stand = 1,
	    blink = 2,
        startRunning = 3,
        running = 4,
        jumpThenRun = 6,
        inAir = 7,
        jumpOnTheSpot = 8,
        fallDown = 10,
        fallen = 11,
	};

	switch(NPC->act_no)
	{
	case init: //Stand still and blink
		NPC->act_no = stand;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
	// Fallthrough
	case stand:
		//Blink randomly
		if (!random(0, 120))
		{
			NPC->act_no = blink;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}

		//Face towards player if nearby
		if (NPC->x - tilesToUnits(1) < currentPlayer.x && NPC->x + tilesToUnits(1) > currentPlayer.x
            && NPC->y - tilesToUnits(1) < currentPlayer.y && NPC->y + tilesToUnits(1) > currentPlayer.y)
		{
			if (NPC->x <= currentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}

		break;

	case 2: //Blinking
		//Blink for 8 frames
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = stand;
			NPC->ani_no = 0;
		}
		break;

	case startRunning: //Running
		NPC->act_no = running;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
	// Fallthrough
	case running:
		//Animate
		NPC->animate(2, 1, 4);

		//Turn when hit wall
		if (NPC->flag & leftWall)
		{
			NPC->direct = dirRight;
			NPC->xm = 0x200; //Nullified by below code
		}
		if (NPC->flag & rightWall)
		{
			NPC->direct = dirLeft;
			NPC->xm = -0x200; //Nullified by below code
		}

		//Run in facing direction
		NPC->moveInDir(pixelsToUnits(2));

		break;

	case jumpThenRun: //Jump then run
		NPC->act_no = inAir;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
		NPC->ym = -0x400;
	// Fallthrough
	case inAir: //In air
		//Animate

		//Run in facing direction
		NPC->moveInDir(pixelsToUnits(0.5));

		if (++NPC->act_wait > 1 && NPC->flag & ground)
			NPC->act_no = startRunning;

		break;

	case jumpOnTheSpot: //Jump on the spot (I think this is used in the shack?)
		NPC->ani_no = 1;
		NPC->act_wait = 0;
		NPC->act_no = jumpOnTheSpot + 1;
		NPC->ym = -0x200;
	// Fallthrough
	case jumpOnTheSpot + 1:
		if (++NPC->act_wait > 1 && NPC->flag & ground)
			NPC->act_no = init;

		break;

	case fallDown: //Fall down
		NPC->act_no = fallen;
		NPC->ani_no = 9;
		NPC->ym = pixelsToUnits(-2);
		playSound(SFX_EnemySqueal);

		NPC->moveInDir(pixelsToUnits(0.5));
		break;

	case fallen: //Fallen
		if (++NPC->act_wait > 1 && NPC->flag & ground)
		{
			NPC->act_no = 12;
			NPC->ani_no = 10;

			NPC->bits |= npc_shootable;
		}

		break;

	case 12:
		NPC->xm = 0;
		break;

	default:
		break;
	}

	//Move
	NPC->doGravity(0x40, 0x5FF);
    NPC->limitXVel(pixelsToUnits(2));

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Framerect
	int yOff = 64;

	if (NPC->direct != dirLeft)
		yOff += 16;

	NPC->rect = { frameMap[NPC->ani_no] << 4, yOff, 16 + (frameMap[NPC->ani_no] << 4), yOff + 16};
}

void npcAct061(npc *NPC) //King
{
	constexpr array<RECT, 11> rcLeft =
	{{
        { 224, 32, 240, 48 },
        { 240, 32, 256, 48 },
        { 256, 32, 272, 48 },
        { 272, 32, 288, 48 },
        { 288, 32, 304, 48 },
        { 224, 32, 240, 48 },
        { 304, 32, 320, 48 },
        { 224, 32, 240, 48 },
        { 272, 32, 288, 48 },
        { 0, 0, 0, 0 },
        { 96, 32, 112, 48 },
    }};

	constexpr array<RECT, 11> rcRight =
	{{
        { 224, 48, 240, 64 },
        { 240, 48, 256, 64 },
        { 256, 48, 272, 64 },
        { 272, 48, 288, 64 },
        { 288, 48, 304, 64 },
        { 224, 48, 240, 64 },
        { 304, 48, 320, 64 },
        { 224, 48, 240, 64 },
        { 272, 48, 288, 64 },
        { 0, 0, 0, 0 },
        { 112, 32, 128, 48 },
    }};

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
	// Fallthrough
	case 1:
		if (!random(0, 120))
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 2:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;

	case 5: //Laying on ground
		NPC->ani_no = 3;
		NPC->xm = 0;
		break;

	case 6: //Fall over
		NPC->act_no = 7;
		NPC->act_wait = 0;
		NPC->ani_wait = 0;
		NPC->ym = pixelsToUnits(-2);
	// Fallthrough
	case 7:
		NPC->ani_no = 2;

		//Move in "facing" direction
		NPC->moveInDir(pixelsToUnits(1));

		if (++NPC->act_wait > 1 && NPC->flag & ground)
			NPC->act_no = 5;
		break;

	case 8: //Walking
		NPC->act_no = 9;
		NPC->ani_no = 4;
		NPC->ani_wait = 0;
	// Fallthrough
	case 9:
		//Animate
		NPC->animate(4, 4, 7);

		//Walk forward
		NPC->moveInDir(pixelsToUnits(1));

		break;

	case 10: //Running
		NPC->act_no = 11;
		NPC->ani_no = 4;
		NPC->ani_wait = 0;
	// Fallthrough
	case 11:
		//Animate
		NPC->animate(2, 4, 7);

		//Run forward
		NPC->moveInDir(pixelsToUnits(2));

		break;

	case 20: //Create blade
		createNpc(NPC_KingStruckByLightning, 0, 0, 0, 0, dirRight, NPC);
		NPC->ani_no = 0;
		NPC->act_no = 0;
		break;

	case 30: //Gets bitch slapped by lightning
		NPC->act_no = 31;
		NPC->act_wait = 0;
		NPC->ani_wait = 0;
		NPC->ym = 0;
	// Fallthrough
	case 31:
		NPC->ani_no = 2;

		//Fly in current direction (knockback frame is backwards, though)
		NPC->moveInDir(pixelsToUnits(3));

		if (NPC->flag & leftWall)
		{
			//Bounce from the wall
			NPC->direct = dirRight;
			NPC->act_no = 7;
			NPC->act_wait = 0;
			NPC->ani_wait = 0;

			NPC->ym = pixelsToUnits(-2);
			NPC->xm = pixelsToUnits(1);

			playSound(SFX_QuoteSmashIntoGround);

			createSmokeLeft(NPC->x, NPC->y, pixelsToUnits(4), 4);
		}
		break;

	case 40: //Die and leave sword
		NPC->act_no = 42;
		NPC->act_wait = 0;
		NPC->ani_no = 8;
		playSound(SFX_Teleport);
	// Fallthrough
	case 42:
		//Flash away
		if (++NPC->ani_no > 9)
			NPC->ani_no = 8;

		//Die and literally turn into the blade
		if (++NPC->act_wait > 100)
		{
			//Create smoke
			for (int i = 0; i <= 3; ++i)
				createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), random(-0x155, 0x155), random(-0x600, 0));

			NPC->act_no = 50;
			NPC->surf = 20;
			NPC->ani_no = 10;
		}

		break;

	case 60: //Jump and slash
		NPC->ani_no = 6;
		NPC->act_no = 61;

		NPC->xm = pixelsToUnits(2);
		NPC->ym = -0x5FF;

		NPC->count2 = 1; //Hold his sword up
		break;

	case 61:
		NPC->ym += 0x40; //Gravity

		if (NPC->flag & ground)
		{
			NPC->act_no = 0;
			NPC->count2 = 0; //Slash
			NPC->xm = 0;
		}
		break;

	default:
		break;
	}

	//Gravity and speed limit stuff
	if (NPC->act_no <= 29 || NPC->act_no > 39)
	{
		NPC->limitXVel(pixelsToUnits(2));
		NPC->doGravity(0x40, 0x5FF);
	}

	//Move
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Set framerect
	NPC->doRects(rcLeft, rcRight);

}

void npcAct062(npc *NPC) // Kazuma, facing away
{
	constexpr array<RECT, 3> rcNPC = { { {272, 192, 288, 216}, {288, 192, 304, 216}, {304, 192, 320, 216} } };

	enum
	{
		initial = 0,
		typing = 1,
		slouch = 2,
		upright = 3,
	};

	switch (NPC->act_no)
	{
	case initial:
		NPC->x -= pixelsToUnits(4);
		NPC->y += tilesToUnits(1);
		NPC->act_no = typing;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
	// Fallthrough
	case typing:
	    NPC->animate(2, 0, 1);

		if (!random(0, 80))
		{
			NPC->act_no = slouch;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}
		if (!random(0, 120))
		{
			NPC->act_no = upright;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}
		break;

	case slouch:
		if (++NPC->act_wait > 40)
		{
			NPC->act_no = upright;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}
		break;

	case upright:
		if (++NPC->act_wait > 80)
		{
			NPC->act_no = typing;
			NPC->ani_no = 0;
		}
		break;
	default:
		break;
	}

	NPC->doRects(rcNPC);
}

void npcAct063(npc *NPC) // Toroko, panicking
{
	array<RECT, 6> rcLeft;
	array<RECT, 6> rcRight;

	rcLeft[0] = { 64, 64, 80, 80 };
	rcLeft[1] = { 80, 64, 96, 80 };
	rcLeft[2] = { 64, 64, 80, 80 };
	rcLeft[3] = { 96, 64, 112, 80 };
	rcLeft[4] = { 112, 64, 128, 80 };
	rcLeft[5] = { 128, 64, 144, 80 };

	rcRight[0] = { 64, 80, 80, 96 };
	rcRight[1] = { 80, 80, 96, 96 };
	rcRight[2] = { 64, 80, 80, 96 };
	rcRight[3] = { 96, 80, 112, 96 };
	rcRight[4] = { 112, 80, 128, 96 };
	rcRight[5] = { 128, 80, 144, 96 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->act_wait = 0;
		NPC->ani_wait = 0;
		NPC->ym = -0x400;
	// Fallthrough
	case 1:
		if (NPC->ym > 0)
			NPC->bits &= ~npc_ignoreSolid;

        NPC->animate(2, 0, 3);

        NPC->moveInDir(pixelsToUnits(0.5));

		if (NPC->act_wait++ && NPC->flag & ground)
			NPC->act_no = 2;
		break;

	case 2:
		NPC->act_no = 3;
		NPC->act_wait = 0;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
	// Fallthrough
	case 3:
	    NPC->animate(2, 0, 3);

		if (++NPC->act_wait > 50)
		{
			NPC->act_wait = 40;
			NPC->xm = -NPC->xm;
			if (NPC->direct != dirLeft)
				NPC->direct = dirLeft;
			else
				NPC->direct = dirRight;
		}

		if (NPC->act_wait > 35)
			NPC->bits |= npc_shootable;

        NPC->moveInDir(0x40);

		if (NPC->shock)
		{
			NPC->act_no = 4;
			NPC->ani_no = 4;
			NPC->ym = pixelsToUnits(-2);
			NPC->bits &= ~npc_shootable;
			NPC->damage = 0;
		}
		break;

	case 4:
	    NPC->moveInDir(pixelsToUnits(0.5));

		if (NPC->act_wait++ && NPC->flag & ground)
		{
			NPC->act_no = 5;
			NPC->bits |= npc_interact;
		}
		break;

	case 5:
		NPC->xm = 0;
		NPC->ani_no = 5;
		break;
	default:
		break;
	}

	NPC->doGravity(0x40, 0x5FF);
	NPC->limitXVel(pixelsToUnits(2));

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);
}

void npcAct064(npc *NPC) //First Cave critter
{
	const int action = NPC->act_no;

	switch (action)
	{
	case 0: //Initialize
		NPC->y += 0x5FF;
		NPC->act_no = 1;

		break;

	case 2: //Going to jump
		if (++NPC->act_wait > 8)
		{
			//Jump
			NPC->act_no = 3;
			NPC->ani_no = 2;

			NPC->flag &= ~ground;
			NPC->ym = -0x5FF;

			//Jump in direction facing
			NPC->moveInDir(pixelsToUnits(0.5));
			if (NPC->direct != dirLeft)
				NPC->xm = 0x100;
			else
				NPC->xm = -0x100;

			playSound(SFX_CritterHop);
		}

		break;

	case 3: //In air
		if (NPC->flag & ground) //Landed on the ground after jumping
		{
			NPC->act_no = 1;
			NPC->act_wait = 0;

			NPC->ani_no = 0;

			NPC->xm = 0;
		}
	}

	if (action == 1)
	{
		//Face towards player
		if (NPC->x <= currentPlayer.x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;

		//TargetX being used as timer (10/10 pixel code)
		if (NPC->tgt_x < 100)
			++NPC->tgt_x;

		//Timer for looking at Quote
		if (NPC->act_wait < 8
            || NPC->x - tilesToUnits(7) >= currentPlayer.x || NPC->x + tilesToUnits(7) <= currentPlayer.x
            || NPC->y - tilesToUnits(5) >= currentPlayer.y || NPC->y + tilesToUnits(5) <= currentPlayer.y)
		{
			if (NPC->act_wait <= 7)
				++NPC->act_wait;
			NPC->ani_no = 0;
		}
		else
			NPC->ani_no = 1;

		//If shocked, jump
		if (NPC->shock)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;

			NPC->ani_no = 0;
		}

		//Go into "going to jump" state
		if (NPC->act_wait >= 8
		        && NPC->tgt_x >= 100
		        && NPC->x - tilesToUnits(4) < currentPlayer.x
		        && NPC->x + tilesToUnits(4) > currentPlayer.x
		        && NPC->y - tilesToUnits(5) < currentPlayer.y
		        && NPC->y + tilesToUnits(3) > currentPlayer.y)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;

			NPC->ani_no = 0;
		}
	}

	//Gravity
	NPC->doGravity(0x40, 0x5FF);

	//Move critter
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Change framerect
	if (NPC->direct != dirLeft)
		NPC->rect = { (NPC->ani_no << 4), 16, ((NPC->ani_no + 1) << 4), 32 };
	else
		NPC->rect = { (NPC->ani_no << 4), 0, ((NPC->ani_no + 1) << 4), 16 };
}

void npcAct065(npc *NPC) //First Cave Bat
{
	const int action = NPC->act_no;

	switch (action)
	{
	case 0:
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
		NPC->act_no = 1;
		NPC->act_wait = random(0, 50);
	// Fallthrough
	case 1:
		if (++NPC->act_wait >= 50)
		{
			NPC->act_wait = 0;
			NPC->act_no = 2;
			NPC->ym = pixelsToUnits(1.5);
		}
		break;
	case 2:
		//Face towards player
		if (currentPlayer.x >= NPC->x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;

		//Fly up and down
		NPC->accelerateTowardsYTarget(0x10);
		NPC->limitYVel(pixelsToUnits(1.5));
		break;
	}



	//Move bat
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Animate
	NPC->animate(1, 0, 2);

	NPC->doRects({ 32 + (NPC->ani_no << 4), 32, 48 + (NPC->ani_no << 4), 48 },
              { 32 + (NPC->ani_no << 4), 48, 48 + (NPC->ani_no << 4), 64 });
}

void npcAct066(npc *NPC) //Bubble (to catch Toroko in the shack)
{
	uint8_t deg;
	array<RECT, 4> rect;

	rect[0] = { 32, 192, 56, 216 };
	rect[1] = { 56, 192, 80, 216 };
	rect[2] = { 32, 216, 56, 240 };
	rect[3] = { 56, 216, 80, 240 };

	switch (NPC->act_no)
	{
	case 0:
		//Shoot towards Toroko
		for (size_t i = 0; i < npcs.size(); i++)
		{
			if (npcs[i].code_event == 1000)
			{
				NPC->tgt_x = npcs[i].x;
				NPC->tgt_y = npcs[i].y;
				NPC->count1 = i;
				deg = getAtan(NPC->x - NPC->tgt_x, NPC->y - NPC->tgt_y);
				NPC->xm = 2 * getCos(deg);
				NPC->ym = 2 * getSin(deg);
				NPC->act_no = 1;
				break;
			}
		}
	// Fallthrough
	case 1:
		//Animate
		NPC->animate(1, 0, 1);

		//Check if hit Toroko
		if (NPC->x - pixelsToUnits(3) < NPC->tgt_x
		        && NPC->x + pixelsToUnits(3) > NPC->tgt_x
		        && NPC->y - pixelsToUnits(3) < NPC->tgt_y
		        && NPC->y + pixelsToUnits(3) > NPC->tgt_y)
		{
			NPC->act_no = 2;
			NPC->ani_no = 2;
			npcs[NPC->count1].cond = 0;
			playSound(SFX_Bubble);
		}

		break;

	case 2:
		//Move a bit towards the top left
		NPC->xm -= 0x20;
		NPC->ym -= 0x20;

		if (NPC->xm < -0x5FF)
			NPC->xm = -0x5FF;
		if (NPC->ym < -0x5FF)
			NPC->ym = -0x5FF;

		if (NPC->y < -0x1000)
			NPC->cond = 0;

        NPC->animate(3, 2, 3);
		break;

	default:
		break;
	}

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rect);
}

void npcAct067(npc *NPC) //Misery floating
{
	array<RECT, 8> rcLeft;
	array<RECT, 8> rcRight;

	rcLeft[0] = { 0x50, 0x00, 0x60, 0x10 };
	rcLeft[1] = { 0x60, 0x00, 0x70, 0x10 };
	rcLeft[2] = { 0x70, 0x00, 0x80, 0x10 };
	rcLeft[3] = { 0x80, 0x00, 0x90, 0x10 };
	rcLeft[4] = { 0x90, 0x00, 0xA0, 0x10 };
	rcLeft[5] = { 0xA0, 0x00, 0xB0, 0x10 };
	rcLeft[6] = { 0xB0, 0x00, 0xC0, 0x10 };
	rcLeft[7] = { 0x90, 0x00, 0xA0, 0x10 };

	rcRight[0] = { 0x50, 0x10, 0x60, 0x20 };
	rcRight[1] = { 0x60, 0x10, 0x70, 0x20 };
	rcRight[2] = { 0x70, 0x10, 0x80, 0x20 };
	rcRight[3] = { 0x80, 0x10, 0x90, 0x20 };
	rcRight[4] = { 0x90, 0x10, 0xA0, 0x20 };
	rcRight[5] = { 0xA0, 0x10, 0xB0, 0x20 };
	rcRight[6] = { 0xB0, 0x10, 0xC0, 0x20 };
	rcRight[7] = { 0x90, 0x10, 0xA0, 0x20 };

	switch (NPC->act_no)
	{
	case 0: //Teleport in
		NPC->act_no = 1;
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
		NPC->ani_no = 0;
		playSound(29);
//Fallthrough
	case 1:
		NPC->x = NPC->tgt_x + pixelsToUnits(random(-1, 1));

		if (++NPC->act_wait >= 32)
			NPC->act_no = 10;
		break;

	case 10: //Floating
		NPC->act_no = 11;
		NPC->act_wait = 0;
		NPC->ani_no = 0;
		NPC->ym = pixelsToUnits(1);
//Fallthrough
	case 11:
	    NPC->accelerateTowardsYTarget(0x10);
	    NPC->limitYVel(pixelsToUnits(0.5));
		break;

	case 13: //Fall down to the floor
		NPC->ani_no = 1;
		NPC->doGravity(0x40, 0x5FF);

		if (NPC->flag & ground)
		{
			playSound(23);
			NPC->ym = 0;
			NPC->act_no = 14;
			NPC->bits |= npc_ignoreSolid;
			NPC->ani_no = 2;
		}
		break;

	case 15: //Shoot bubble towards Toroko?
		NPC->act_no = 16;
		NPC->act_wait = 0;
		NPC->ani_no = 4;
		//Fallthrough
	case 16:
		if (++NPC->act_wait == 30)
		{
			playSound(21);
			createNpc(NPC_ProjectileMiseryBubble, NPC->x, NPC->y - tilesToUnits(1));
		}
		if (NPC->act_wait >= 50)
			NPC->act_no = 14;
		break;

	case 20: //Fly up
		NPC->act_no = 21;
		NPC->ani_no = 0;
		NPC->ym = 0;
		NPC->bits |= npc_ignoreSolid;
		//Fallthrough
	case 21:
		NPC->ym -= 0x20;
		if (NPC->y < tilesToUnits(-0.5))
			NPC->cond = 0;
		break;

	case 25:
		NPC->act_no = 26;
		NPC->act_wait = 0;
		NPC->ani_no = 5;
		NPC->ani_wait = 0;
//Fallthrough
	case 26:
		if (++NPC->ani_no > 7)
			NPC->ani_no = 5;

		if (++NPC->act_wait >= 30)
		{
			playSound(101);
			//SetFlash(0, 0, 2);
			NPC->act_no = 27;
			NPC->ani_no = 7;
		}
		break;

	case 27:
		if (++NPC->act_wait == 50)
			NPC->act_no = 14;
		break;

	default:
		break;
	}

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->act_no == 11)
	{
		if (NPC->ani_wait)
		{
			--NPC->ani_wait;
			NPC->ani_no = 1;
		}
		else
		{
			if (random(0, 100) == 1)
				NPC->ani_wait = 30;

			NPC->ani_no = 0;
		}
	}

	if (NPC->act_no == 14)
	{
		if (NPC->ani_wait)
		{
			--NPC->ani_wait;
			NPC->ani_no = 3;
		}
		else
		{
			if (random(0, 100) == 1)
				NPC->ani_wait = 30;

			NPC->ani_no = 2;
		}
	}

	NPC->doRects(rcLeft, rcRight);

	if (NPC->act_no == 1 && NPC->ani_wait < 32)
		NPC->rect.bottom += ++NPC->ani_wait / 2 - 16;
}

void npcAct068(npc * NPC) // Balrog, Running (boss)
{
	enum
	{
		initial = 0,
		runTowardsQuote = 2,
		jump = 4,
		slowDown = 9,
		caughtQuote = 10,
		throwQuote = 20,
	};

	switch (NPC->act_no)
	{
	case initial:
		NPC->act_no = initial + 1;
		NPC->ani_no = 0;
		NPC->act_wait = 20;
		NPC->facePlayer();
		// Fallthrough
	case initial + 1:
		if (!--NPC->act_wait)
		{
			NPC->act_no = 2;
			++NPC->count1;
		}
		break;

	case runTowardsQuote:
		NPC->act_no = runTowardsQuote + 1;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
		// Fallthrough
	case runTowardsQuote + 1:
		if (++NPC->ani_wait > 3)
		{
			NPC->ani_wait = 0;
			if (++NPC->ani_no == 2 || NPC->ani_no == 4)
				playSound(SFX_HitGround);
		}
		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		if (NPC->direct != dirLeft)
			NPC->xm += 0x10;
		else
			NPC->xm -= 0x10;

		if (NPC->act_wait < 8
			|| NPC->x - pixelsToUnits(12) >= currentPlayer.x
			|| NPC->x + pixelsToUnits(12) <= currentPlayer.x
			|| NPC->y - pixelsToUnits(12) >= currentPlayer.y
			|| NPC->y + pixelsToUnits(12) <= currentPlayer.y)
		{
			++NPC->act_wait;
			if (NPC->flag & (rightWall | leftWall) || NPC->act_wait > 75)
			{
				NPC->act_no = slowDown;
				NPC->ani_no = 0;
			}
			else if (!(NPC->count1 % 3) && NPC->act_wait > 25)
			{
				NPC->act_no = jump;
				NPC->ani_no = 7;
				NPC->ym = pixelsToUnits(-2);
			}
		}
		else
		{
			NPC->act_no = caughtQuote;
			NPC->ani_no = 5;
			currentPlayer.cond |= player_removed;
			currentPlayer.damage(2);
		}
		break;

	case jump:
		if (NPC->flag & ground)
		{
			NPC->act_no = slowDown;
			NPC->ani_no = 8;
			viewport.quake = 30;
			playSound(SFX_LargeObjectHitGround);
		}

		if (NPC->act_wait >= 8
			&& NPC->x - pixelsToUnits(12) < currentPlayer.x
			&& NPC->x + pixelsToUnits(12) > currentPlayer.x
			&& NPC->y - pixelsToUnits(12) < currentPlayer.y
			&& NPC->y + pixelsToUnits(12) > currentPlayer.y)
		{
			NPC->act_no = caughtQuote;
			NPC->ani_no = 5;
			currentPlayer.cond |= player_removed;
			currentPlayer.damage(2);
		}
		break;

	case slowDown:
		NPC->xm = (4 * NPC->xm) / 5;
		if (!NPC->xm)
			NPC->act_no = initial;
		break;

	case caughtQuote:
		currentPlayer.x = NPC->x;
		currentPlayer.y = NPC->y;
		NPC->xm = (4 * NPC->xm) / 5;

		if (!NPC->xm)
		{
			NPC->act_no = caughtQuote + 1;
			NPC->act_wait = 0;
			NPC->ani_no = 5;
			NPC->ani_wait = 0;
		}
		break;

	case caughtQuote + 1:
		currentPlayer.x = NPC->x;
		currentPlayer.y = NPC->y;

		NPC->animate(2, 5, 6);

		if (++NPC->act_wait > 100)
			NPC->act_no = throwQuote;
		break;

	case throwQuote:
		playSound(SFX_SillyExplosion);
		currentPlayer.cond &= ~player_removed;

		if (NPC->direct != dirLeft)
		{
			currentPlayer.x -= pixelsToUnits(4);
			currentPlayer.y -= pixelsToUnits(8);
			currentPlayer.xm = -0x5FF;
			currentPlayer.ym = pixelsToUnits(-1);
			currentPlayer.direct = dirLeft;
			NPC->direct = dirLeft;
		}
		else
		{
			currentPlayer.x += pixelsToUnits(4);
			currentPlayer.y -= pixelsToUnits(8);
			currentPlayer.xm = 0x5FF;
			currentPlayer.ym = pixelsToUnits(-1);
			currentPlayer.direct = dirLeft;
			NPC->direct = dirLeft;
		}

		NPC->act_no = throwQuote + 1;
		NPC->act_wait = 0;
		NPC->ani_no = 7;
		// Fallthrough

	case throwQuote + 1:
		if (++NPC->act_wait >= 50)
			NPC->act_no = initial;
		break;

	default:
		break;
	}

	NPC->doGravity(0x20, 0x5FF);
	NPC->limitXVel(pixelsToUnits(2));
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	array<RECT, 9> rcLeft;
	array<RECT, 9> rcRight;
	rcLeft[0] = { 0, 0, 40, 24 };
	rcLeft[1] = { 0, 48, 40, 72 };
	rcLeft[2] = rcLeft[0];
	rcLeft[3] = { 40, 48, 80, 72 };
	rcLeft[4] = rcLeft[0];
	rcLeft[5] = { 80, 48, 120, 72 };
	rcLeft[6] = { 120, 48, 160, 72 };
	rcLeft[7] = { 120, 0, 160, 24 };
	rcLeft[8] = { 80, 0, 120, 24 };
	rcRight[0] = { 0, 24, 40, 48 };
	rcRight[1] = { 0, 72, 40, 96 };
	rcRight[2] = rcRight[0];
	rcRight[3] = { 40, 72, 80, 96 };
	rcRight[4] = rcRight[0];
	rcRight[5] = { 80, 72, 120, 96 };
	rcRight[6] = { 120, 72, 160, 96 };
	rcRight[7] = { 120, 24, 160, 48 };
	rcRight[8] = { 80, 24, 120, 48 };

	NPC->doRects(rcLeft, rcRight);
}

void npcAct069(npc *NPC) //Pignon
{
	array<RECT, 6> rcLeft;
	array<RECT, 6> rcRight;

	rcLeft[0] = { 48, 0, 64, 16 };
	rcLeft[1] = { 64, 0, 80, 16 };
	rcLeft[2] = { 80, 0, 96, 16 };
	rcLeft[3] = { 96, 0, 112, 16 };
	rcLeft[4] = { 48, 0, 64, 16 };
	rcLeft[5] = { 112, 0, 128, 16 };
	rcRight[0] = { 48, 16, 64, 32 };
	rcRight[1] = { 64, 16, 80, 32 };
	rcRight[2] = { 80, 16, 96, 32 };
	rcRight[3] = { 96, 16, 112, 32 };
	rcRight[4] = { 48, 16, 64, 32 };
	rcRight[5] = { 112, 16, 128, 32 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
//Fallthrough
	case 1:
		if (random(0, 100) == 1)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		else
		{
			if (random(0, 150) == 1)
			{
				if (NPC->direct)
					NPC->direct = 0;
				else
					NPC->direct = 2;
			}
			if (random(0, 150) == 1)
			{
				NPC->act_no = 3;
				NPC->act_wait = 50;
				NPC->ani_no = 0;
			}
		}
		break;

	case 2:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;

	case 3:
		NPC->act_no = 4;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
//Fallthrough
	case 4:
		if (!--NPC->act_wait)
			NPC->act_no = 0;

        NPC->animate(2, 2, 4);
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 2;

		if (NPC->flag & leftWall)
		{
			NPC->direct = 2;
			NPC->xm = pixelsToUnits(1); //Overwritten right away
		}

		if (NPC->flag & rightWall)
		{
			NPC->direct = 0;
			NPC->xm = pixelsToUnits(-1); //Overwritten right away
		}

		NPC->moveInDir(pixelsToUnits(1));
		break;

	case 5:
		if (NPC->flag & ground)
			NPC->act_no = 0;
		break;

	default:
		break;
	}

	if (NPC->act_no < 5 && (1 << NPC->act_no) & 0x16 && NPC->shock)
	{
		NPC->ym = pixelsToUnits(-1);
		NPC->ani_no = 5;
		NPC->act_no = 5;
	}

	NPC->doGravity(0x40, 0x5FF);

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);
}

void npcAct070(npc * NPC) // Sparkling Item
{
	constexpr array<RECT, 4> rcNPC = { { {96, 48, 112, 64}, {112, 48, 128, 64}, {128, 48, 144, 64}, {144, 48, 160, 64} } };

	NPC->animate(3, 0, 3);
	NPC->doRects(rcNPC);
}

void npcAct071(npc * NPC) // Chinfish (enemy)
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
		NPC->ym = 0x80;
	}
	else if (NPC->act_no == 1)
	{
	    NPC->accelerateTowardsYTarget(8);
	    NPC->limitYVel(pixelsToUnits(0.5));
	}

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	constexpr array<RECT, 3> rcLeft = { { {64, 32, 80, 48}, {80, 32, 96, 48}, {92, 32, 112, 48} } };
	constexpr array<RECT, 3> rcRight = { { {64, 48, 80, 64}, {80, 48, 96, 64}, {96, 48, 112, 64} } };

	NPC->animate(4, 0, 1);
	if (NPC->shock)
		NPC->ani_no = 2;

	NPC->doRects(rcLeft, rcRight);
}

void npcAct072(npc *NPC) // Sprinkler
{
	if (NPC->direct == dirLeft)
	{
		NPC->animate(1);

		if (NPC->ani_no > 1)
		{
			NPC->ani_no = 0;
			return;
		}

		if (currentPlayer.x < NPC->x + tilesToUnits(20)
		        && currentPlayer.x > NPC->x - tilesToUnits(20)
		        && currentPlayer.y < NPC->y + tilesToUnits(15)
		        && currentPlayer.y > NPC->y - tilesToUnits(15))
		{
			if (++NPC->act_no % 2)
				createNpc(NPC_Waterdrop, NPC->x, NPC->y,
                        2 * random(pixelsToUnits(-1), pixelsToUnits(1)), 3 * random(pixelsToUnits(-1), 0x80));
			createNpc(NPC_Waterdrop, NPC->x, NPC->y,
                        2 * random(pixelsToUnits(-1), pixelsToUnits(1)), 3 * random(pixelsToUnits(-1), 0x80));
		}
	}

	array<RECT, 2> rcNPC = { { {224, 48, 240, 64}, {240, 48, 256, 64} } };

	NPC->doRects(rcNPC);
}

void npcAct073(npc *NPC) //Water drop
{
    NPC->doGravity(0x20, 0x5FF);

	NPC->ani_no = random(0, 4);

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Set frameRect
	NPC->doRects({ 72 + (NPC->ani_no << 1), 16, 74 + (NPC->ani_no << 1), 18 });

	if (NPC->direct == dirRight) //Blood
	{
		NPC->rect.top += 2;
		NPC->rect.bottom += 2;
	}

	if (++NPC->act_wait > 10)
	{
		if (NPC->flag & (leftWall | rightWall | ground | water))
			NPC->cond = 0;
	}

	if (NPC->y > tilesToUnits(map.height))
		NPC->cond = 0;
}

void npcAct074(npc *NPC) //Jack
{
	array<RECT, 6> rcLeft;
	array<RECT, 6> rcRight;

	rcLeft[0] = { 64, 0, 80, 16 };
	rcLeft[1] = { 80, 0, 96, 16 };
	rcLeft[2] = { 96, 0, 112, 16 };
	rcLeft[3] = { 64, 0, 80, 16 };
	rcLeft[4] = { 112, 0, 128, 16 };
	rcLeft[5] = { 64, 0, 80, 16 };

	rcRight[0] = { 64, 16, 80, 32 };
	rcRight[1] = { 80, 16, 96, 32 };
	rcRight[2] = { 96, 16, 112, 32 };
	rcRight[3] = { 64, 16, 80, 32 };
	rcRight[4] = { 112, 16, 128, 32 };
	rcRight[5] = { 64, 16, 80, 32 };

	switch (NPC->act_no)
	{
	case 0: //Stand
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
	// Fallthrough
	case 1:
		//Glasses flash randomly
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 2: //Glasses flash
		//Stop after 8 frames
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}

		break;

	case 8:
		NPC->act_no = 9;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
	// Fallthrough
	case 9:
		//Animate
		NPC->animate(4, 2, 5);

		//Move in facing direction
		NPC->moveInDir(pixelsToUnits(1));

		break;

	default:
		break;
	}

	//Gravity and speed limit
	NPC->doGravity(0x40, 0x5FF);
	NPC->limitXVel(pixelsToUnits(2));

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Set framerect
	NPC->doRects(rcLeft, rcRight);
}

void npcAct075(npc * NPC) // Kanpachi
{
	constexpr array<RECT, 2> rcNPC = { { {272, 32, 296, 56}, {296, 32, 320, 56} } };

	if (NPC->act_no)
	{
		if (NPC->act_no != 1)
			goto doRects;
	}
	else
	{
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
	}
	NPC->ani_no = NPC->x - tilesToUnits(3) < currentPlayer.x
	              && NPC->x + tilesToUnits(3) > currentPlayer.x
	              && NPC->y - tilesToUnits(3) < currentPlayer.y
	              && NPC->y + tilesToUnits(1) > currentPlayer.y;
doRects:
	NPC->doRects(rcNPC);

}

void npcAct076(npc *NPC) //Flower
{
	NPC->rect.left = 16 * NPC->code_event;
	NPC->rect.top = 0;
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = 16;
}

void npcAct077(npc * NPC) // Sandaime's Pavillion
{
	constexpr RECT rcSandaimePresent[2] = { {0, 16, 48, 48}, {48, 16, 96, 48} };
	constexpr RECT rcSandaimeGone = { 96, 16, 144, 48 };

	if (NPC->act_no == 0)
	{
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
	}
	else if (NPC->act_no != 1)
	{
		if (NPC->act_no == 2 && (++NPC->act_wait > 8))
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		goto doRects;
	}
	if (random(0, 120) == 10)
	{
		NPC->act_no = 2;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
	}
doRects:
	if (NPC->direct == dirLeft)
        NPC->rect = rcSandaimePresent[NPC->ani_no];
    else
        NPC->rect = rcSandaimeGone;
}

void npcAct078(npc *NPC) //Pot
{
    NPC->doRects({ 160, 48, 176, 64 }, { 176, 48, 192, 64 });
}

void npcAct079(npc *NPC) // Mahin
{
	constexpr array<RECT, 3> rcLeft = { { { 0, 0, 16, 16 }, { 16, 0, 32, 16 }, { 32, 0, 48, 16 } } };
	constexpr array<RECT, 3> rcRight = { { { 0, 16, 16, 32 }, { 16, 16, 32, 32 }, { 32, 16, 48, 32 } } };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
	}
	else if (NPC->act_no == 2)
	{
		NPC->ani_no = 0;
		if (random(0, 120) == 10)
		{
			NPC->act_no = 3;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		if (NPC->x - tilesToUnits(2) < currentPlayer.x
		        && NPC->x + tilesToUnits(2) > currentPlayer.x
		        && NPC->y - tilesToUnits(2) < currentPlayer.y
		        && NPC->y + tilesToUnits(1) > currentPlayer.x)
			NPC->facePlayer();
	}
	else if (NPC->act_no == 3 && ++NPC->act_wait > 8)
	{
		NPC->act_no = 2;
		NPC->ani_no = 0;
	}

	NPC->doGravity(0x40, 0x5FF);
	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);
}
