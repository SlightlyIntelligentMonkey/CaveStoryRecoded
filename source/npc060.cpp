#include "npc060.h"

#include "mathUtils.h"
#include "player.h"
#include "sound.h"
#include "game.h"

void npcAct060(npc *NPC) //Toroko
{
	constexpr int frameMap[11] = { 0, 1, 2, 1, 3, 4, 5, 4, 6, 7, 8 };

	switch(NPC->act_no)
	{
	case 0: //Stand still and blink
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
	// Fallthrough
	case 1:
		//Blink randomly
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}

		//Face towards player if nearby
		if (NPC->x - 0x2000 < currentPlayer.x && NPC->x + 0x2000 > currentPlayer.x && NPC->y - 0x2000 < currentPlayer.y && NPC->y + 0x2000 > currentPlayer.y)
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
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;

	case 3: //Running
		NPC->act_no = 4;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
	// Fallthrough
	case 4:
		//Animate
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

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
		if (NPC->direct != dirLeft)
			NPC->xm = 0x400;
		else
			NPC->xm = -0x400;

		break;

	case 6: //Jump then run
		NPC->act_no = 7;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
		NPC->ym = -0x400;
	// Fallthrough
	case 7: //In air
		//Animate
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		//Run in facing direction
		if (NPC->direct != dirLeft)
			NPC->xm = 0x100;
		else
			NPC->xm = -0x100;

		if (++NPC->act_wait > 1 && NPC->flag & ground)
			NPC->act_no = 3;

		break;

	case 8: //Jump on the spot (I think this is used in the shack?)
		NPC->ani_no = 1;
		NPC->act_wait = 0;
		NPC->act_no = 9;
		NPC->ym = -0x200;
	// Fallthrough
	case 9:
		if (++NPC->act_wait > 1 && NPC->flag & ground)
			NPC->act_no = 0;

		break;

	case 10: //Fall down
		NPC->act_no = 11;
		NPC->ani_no = 9;
		NPC->ym = -0x400;
		playSound(SFX_EnemySqueal);

		if (NPC->direct != dirLeft)
			NPC->xm = 0x100;
		else
			NPC->xm = -0x100;

		break;

	case 11: //Fallen
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
	NPC->ym += 0x40;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	if (NPC->xm > 0x400)
		NPC->xm = 0x400;
	if (NPC->xm < -0x400)
		NPC->xm = -0x400;

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
	RECT rcLeft[11];
	RECT rcRight[11];

	rcLeft[0] = { 224, 32, 240, 48 };
	rcLeft[1] = { 240, 32, 256, 48 };
	rcLeft[2] = { 256, 32, 272, 48 };
	rcLeft[3] = { 272, 32, 288, 48 };
	rcLeft[4] = { 288, 32, 304, 48 };
	rcLeft[5] = { 224, 32, 240, 48 };
	rcLeft[6] = { 304, 32, 320, 48 };
	rcLeft[7] = { 224, 32, 240, 48 };
	rcLeft[8] = { 272, 32, 288, 48 };
	rcLeft[9] = { 0, 0, 0, 0 };
	rcLeft[10] = { 96, 32, 112, 48 };

	rcRight[0] = { 224, 48, 240, 64 };
	rcRight[1] = { 240, 48, 256, 64 };
	rcRight[2] = { 256, 48, 272, 64 };
	rcRight[3] = { 272, 48, 288, 64 };
	rcRight[4] = { 288, 48, 304, 64 };
	rcRight[5] = { 224, 48, 240, 64 };
	rcRight[6] = { 304, 48, 320, 64 };
	rcRight[7] = { 224, 48, 240, 64 };
	rcRight[8] = { 272, 48, 288, 64 };
	rcRight[9] = { 0, 0, 0, 0 };
	rcRight[10] = { 112, 32, 128, 48 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
	// Fallthrough
	case 1:
		if (random(0, 120) == 10)
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
		NPC->ym = -0x400;
	// Fallthrough
	case 7:
		NPC->ani_no = 2;

		//Move in "facing" direction
		if (NPC->direct != dirLeft)
			NPC->xm = 512;
		else
			NPC->xm = -512;

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
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 7)
			NPC->ani_no = 4;

		//Walk forward
		if (NPC->direct != dirLeft)
			NPC->xm = 0x200;
		else
			NPC->xm = -0x200;

		break;

	case 10: //Running
		NPC->act_no = 11;
		NPC->ani_no = 4;
		NPC->ani_wait = 0;
	// Fallthrough
	case 11:
		//Animate
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 7)
			NPC->ani_no = 4;

		//Run forward
		if (NPC->direct != dirLeft)
			NPC->xm = 0x400;
		else
			NPC->xm = -0x400;

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
		if (NPC->direct != dirLeft)
			NPC->xm = 0x600;
		else
			NPC->xm = -0x600;

		if (NPC->flag & leftWall)
		{
			//Bounce from the wall
			NPC->direct = dirRight;
			NPC->act_no = 7;
			NPC->act_wait = 0;
			NPC->ani_wait = 0;

			NPC->ym = -0x400;
			NPC->xm = 0x200;

			playSound(SFX_QuoteSmashIntoGround);

			createSmoke(NPC->x, NPC->y, 2048, 4);
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

		NPC->xm = 0x400;
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
		NPC->ym += 0x40;

		if (NPC->xm > 0x400)
			NPC->xm = 0x400;
		if (NPC->xm < -0x400)
			NPC->xm = -0x400;

		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;
	}

	//Move
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Set framerect
	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct062(npc *NPC) // Kazuma, facing away
{
	constexpr RECT rcNPC[3] = { {272, 192, 288, 216}, {288, 192, 304, 216}, {304, 192, 320, 216} };

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
		NPC->x -= 0x800;
		NPC->y += 0x2000;
		NPC->act_no = typing;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
	// Fallthrough
	case typing:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;

		if (random(0, 80) == 1)
		{
			NPC->act_no = slouch;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}
		if (random(0, 120) == 10)
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

	NPC->rect = rcNPC[NPC->ani_no];
}

void npcAct063(npc *NPC) // Toroko, panicking
{
	RECT rcLeft[6];
	RECT rcRight[6];

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
			NPC->bits &= ~npc_ignoresolid;

		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 3)
			NPC->ani_no = 0;

		if (NPC->direct != dirLeft)
			NPC->xm = 0x100;
		else
			NPC->xm = -0x100;

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
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 3)
			NPC->ani_no = 0;

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

		if (NPC->direct != dirLeft)
			NPC->xm += 0x40;
		else
			NPC->xm -= 0x40;

		if (NPC->shock)
		{
			NPC->act_no = 4;
			NPC->ani_no = 4;
			NPC->ym = -0x400;
			NPC->bits &= ~npc_shootable;
			NPC->damage = 0;
		}
		break;

	case 4:
		if (NPC->direct != dirLeft)
			NPC->xm = 0x100;
		else
			NPC->xm = -0x100;

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

	NPC->ym += 0x40;
	if (NPC->xm > 0x400)
		NPC->xm = 0x400;
	if (NPC->xm < -0x400)
		NPC->xm = -0x400;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft[NPC->ani_no];
	else
		NPC->rect = rcRight[NPC->ani_no];
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
		if (NPC->act_wait < 8 || NPC->x - 0xE000 >= currentPlayer.x || NPC->x + 0xE000 <= currentPlayer.x || NPC->y - 0xA000 >= currentPlayer.y || NPC->y + 0xA000 <= currentPlayer.y)
		{
			if (NPC->act_wait <= 7)
				++NPC->act_wait;
			NPC->ani_no = 0;
		}
		else
		{
			NPC->ani_no = 1;
		}

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
		        && NPC->x - 0x8000 < currentPlayer.x
		        && NPC->x + 0x8000 > currentPlayer.x
		        && NPC->y - 0xA000 < currentPlayer.y
		        && NPC->y + 0x6000 > currentPlayer.y)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;

			NPC->ani_no = 0;
		}
	}

	//Gravity
	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

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
	case 2:
		//Face towards player
		if (currentPlayer.x >= NPC->x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;

		//Fly up and down
		if (NPC->tgt_y < NPC->y)
			NPC->ym -= 0x10;
		if (NPC->tgt_y > NPC->y)
			NPC->ym += 0x10;

		//Limit speed
		if (NPC->ym > 0x300)
			NPC->ym = 0x300;
		if (NPC->ym < -0x300)
			NPC->ym = -0x300;
	}

	if (action == 1)
	{
		if (++NPC->act_wait >= 50)
		{
			NPC->act_wait = 0;
			NPC->act_no = 2;
			NPC->ym = 0x300;
		}
	}

	//Move bat
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Animate
	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	NPC->ani_no %= 3;

	if (NPC->direct != dirLeft)
		NPC->rect = { 32 + (NPC->ani_no << 4), 48, 48 + (NPC->ani_no << 4), 64 };
	else
		NPC->rect = { 32 + (NPC->ani_no << 4), 32, 48 + (NPC->ani_no << 4), 48 };
}

void npcAct066(npc *NPC) //Bubble (to catch Toroko in the shack)
{
	uint8_t deg;
	RECT rect[4];

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
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 1)
			NPC->ani_no = 0;

		//Check if hit Toroko
		if (NPC->x - 0x600 < NPC->tgt_x
		        && NPC->x + 0x600 > NPC->tgt_x
		        && NPC->y - 0x600 < NPC->tgt_y
		        && NPC->y + 0x600 > NPC->tgt_y)
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

		if (NPC->y < -4096)
			NPC->cond = 0;

		if (++NPC->ani_wait > 3)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 3)
			NPC->ani_no = 2;

		break;

	default:
		break;
	}

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->rect = rect[NPC->ani_no];
}

void npcAct067(npc *NPC) //Misery floating
{
	RECT rcLeft[8];
	RECT rcRight[8];
	
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
		NPC->x = NPC->tgt_x + (random(-1, 1) << 9);

		if (++NPC->act_wait >= 32)
			NPC->act_no = 10;
		break;

	case 10: //Floating
		NPC->act_no = 11;
		NPC->act_wait = 0;
		NPC->ani_no = 0;
		NPC->ym = 0x200;
//Fallthrough
	case 11:
		if (NPC->tgt_y < NPC->y)
			NPC->ym -= 0x10;
		if (NPC->tgt_y > NPC->y)
			NPC->ym += 0x10;
		if (NPC->ym > 0x100)
			NPC->ym = 0x100;
		if (NPC->ym < -0x100)
			NPC->ym = -0x100;
		break;

	case 13: //Fall down to the floor
		NPC->ani_no = 1;
		NPC->ym += 0x40;

		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;

		if (NPC->flag & ground)
		{
			playSound(23);
			NPC->ym = 0;
			NPC->act_no = 14;
			NPC->bits |= npc_ignoresolid;
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
			createNpc(66, NPC->x, NPC->y - 0x2000);
		}
		if (NPC->act_wait >= 50)
			NPC->act_no = 14;
		break;

	case 20: //Fly up
		NPC->act_no = 21;
		NPC->ani_no = 0;
		NPC->ym = 0;
		NPC->bits |= npc_ignoresolid;
		//Fallthrough
	case 21:
		NPC->ym -= 0x20;
		if (NPC->y < -0x1000)
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

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];

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

		// Face player
		if (NPC->x <= currentPlayer.x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;
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
				playSound(SFX_QuoteHitGround);
		}
		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		if (NPC->direct != dirLeft)
			NPC->xm += 0x10;
		else
			NPC->xm -= 0x10;

		if (NPC->act_wait < 8
			|| NPC->x - 0x1800 >= currentPlayer.x
			|| NPC->x + 0x1800 <= currentPlayer.x
			|| NPC->y - 0x1800 >= currentPlayer.y
			|| NPC->y + 0x1800 <= currentPlayer.y)
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
				NPC->ym = -0x400;
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
			&& NPC->x - 0x1800 < currentPlayer.x
			&& NPC->x + 0x1800 > currentPlayer.x
			&& NPC->y - 0x1800 < currentPlayer.y
			&& NPC->y + 0x1800 > currentPlayer.y)
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

		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 6)
			NPC->ani_no = 5;

		if (++NPC->act_wait > 100)
			NPC->act_no = throwQuote;
		break;

	case throwQuote:
		playSound(SFX_SillyExplosion);
		currentPlayer.cond &= ~player_removed;

		if (NPC->direct != dirLeft)
		{
			currentPlayer.x -= 0x800;
			currentPlayer.y -= 0x1000;
			currentPlayer.xm = -0x5FF;
			currentPlayer.ym = -0x200;
			currentPlayer.direct = dirLeft;
			NPC->direct = dirLeft;
		}
		else
		{
			currentPlayer.x += 0x800;
			currentPlayer.y -= 0x1000;
			currentPlayer.xm = 0x5FF;
			currentPlayer.ym = -0x200;
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

	NPC->ym += 0x20;
	if (NPC->xm < -0x400)
		NPC->xm = -0x400;
	if (NPC->xm > 0x400)
		NPC->xm = 0x400;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	RECT rcLeft[9];
	RECT rcRight[9];
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

	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft[NPC->ani_no];
	else
		NPC->rect = rcRight[NPC->ani_no];
}

void npcAct069(npc *NPC) //Pignon
{
	RECT rcLeft[6];
	RECT rcRight[6];

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
			NPC->xm = 0x200; //Overwritten right away
		}

		if (NPC->flag & rightWall)
		{
			NPC->direct = 0;
			NPC->xm = -0x200; //Overwritten right away
		}

		if (NPC->direct)
			NPC->xm = 0x100;
		else
			NPC->xm = -0x100;
		break;

	case 5:
		if (NPC->flag & 8)
			NPC->act_no = 0;
		break;

	default:
		break;
	}

	if (NPC->act_no < 5 && (1 << NPC->act_no) & 0x16 && NPC->shock)
	{
		NPC->ym = -0x200;
		NPC->ani_no = 5;
		NPC->act_no = 5;
	}

	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct070(npc * NPC) // Sparkling Item
{
	constexpr RECT rcNPC[4] = { {96, 48, 112, 64}, {112, 48, 128, 64}, {128, 48, 144, 64}, {144, 48, 160, 64} };

	if (++NPC->ani_wait > 3)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}
	if (NPC->ani_no > 3)
		NPC->ani_no = 0;

	NPC->rect = rcNPC[NPC->ani_no];
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
		if (NPC->tgt_y < NPC->y)
			NPC->ym -= 8;
		if (NPC->tgt_y > NPC->y)
			NPC->ym += 8;
		if (NPC->ym > 0x100)
			NPC->ym = 0x100;
		if (NPC->ym < -0x100)
			NPC->ym = 0x100;
	}

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	constexpr RECT rcLeft[3] = { {64, 32, 80, 48}, {80, 32, 96, 48}, {92, 32, 112, 48} };
	constexpr RECT rcRight[3] = { {64, 48, 80, 64}, {80, 48, 96, 64}, {96, 48, 112, 64} };

	if (++NPC->ani_wait > 4)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}
	if (NPC->ani_no > 1)
		NPC->ani_no = 0;
	if (NPC->shock)
		NPC->ani_no = 2;

	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft[NPC->ani_no];
	else
		NPC->rect = rcRight[NPC->ani_no];
}

void npcAct072(npc *NPC) // Sprinkler
{
	if (NPC->direct == dirLeft)
	{
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 1)
		{
			NPC->ani_no = 0;
			return;
		}

		if (currentPlayer.x < NPC->x + 0x28000
		        && currentPlayer.x > NPC->x - 0x28000
		        && currentPlayer.y < NPC->y + 0x1E000
		        && currentPlayer.y > NPC->y - 0x1E000)
		{
			if (++NPC->act_no % 2)
				createNpc(NPC_Waterdrop, NPC->x, NPC->y, 2 * random(-0x200, 0x200), 3 * random(-0x200, 0x80));
			createNpc(NPC_Waterdrop, NPC->x, NPC->y, 2 * random(-0x200, 0x200), 3 * random(-0x200, 0x80));
		}
	}

	constexpr RECT rcNPC[2] = { {224, 48, 240, 64}, {240, 48, 256, 64} };
	NPC->rect = rcNPC[NPC->ani_no];
}

void npcAct073(npc *NPC) //Water drop
{
	NPC->ym += 0x20;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->ani_no = random(0, 4);

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Set frameRect
	NPC->rect = { 72 + (NPC->ani_no << 1), 16, 74 + (NPC->ani_no << 1), 18 };

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

	if (NPC->y > tileToCoord(levelHeight))
		NPC->cond = 0;
}

void npcAct074(npc *NPC) //Jack
{
	RECT rcLeft[6];
	RECT rcRight[6];

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
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 5)
			NPC->ani_no = 2;

		//Move in facing direction
		if (NPC->direct != dirLeft)
			NPC->xm = 0x200;
		else
			NPC->xm = -0x200;

		break;

	default:
		break;
	}

	//Gravity and speed limit
	NPC->ym += 64;

	if (NPC->xm > 0x400)
		NPC->xm = 0x400;
	if (NPC->xm < -0x400)
		NPC->xm = -0x400;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Set framerect
	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct075(npc * NPC) // Kanpachi
{
	constexpr RECT rcNPC[2] = { {272, 32, 296, 56}, {296, 32, 320, 56} };

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
	NPC->ani_no = NPC->x - 0x6000 < currentPlayer.x
	              && NPC->x + 0x6000 > currentPlayer.x
	              && NPC->y - 0x6000 < currentPlayer.y
	              && NPC->y + 0x2000 > currentPlayer.y;
doRects:
	NPC->rect = rcNPC[NPC->ani_no];

}

void npcAct076(npc *NPC) //Flower
{
	NPC->rect.left = 16 * NPC->code_event; // Rects depend on the event number
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
	if (!NPC->direct)
		NPC->rect = rcSandaimePresent[NPC->ani_no];
	else
		NPC->rect = rcSandaimeGone;
}

void npcAct078(npc *NPC) //Pot
{
	if (NPC->direct != dirLeft)
		NPC->rect = { 176, 48, 192, 64 };
	else
		NPC->rect = { 160, 48, 176, 64 };
}

void npcAct079(npc *NPC) // Mahin
{
	constexpr RECT rcLeft[3] = { { 0, 0, 16, 16 },{ 16, 0, 32, 16 },{ 32, 0, 48, 16 } };
	constexpr RECT rcRight[3] = { { 0, 16, 16, 32 },{ 16, 16, 32, 32 },{ 32, 16, 48, 32 } };

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
		if (NPC->x - 0x4000 < currentPlayer.x
		        && NPC->x + 0x4000 > currentPlayer.x
		        && NPC->y - 0x4000 < currentPlayer.y
		        && NPC->y + 0x2000 > currentPlayer.x)
		{
			if (NPC->x <= currentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}
	}
	else if (NPC->act_no == 3 && ++NPC->act_wait > 8)
	{
		NPC->act_no = 2;
		NPC->ani_no = 0;
	}
	NPC->ym += 64;
	if (NPC->ym > 1535)
		NPC->ym = 1535;
	NPC->y += NPC->ym;

	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft[NPC->ani_no];
	else
		NPC->rect = rcRight[NPC->ani_no];
}


