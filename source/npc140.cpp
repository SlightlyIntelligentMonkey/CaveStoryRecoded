#include "npc140.h"

#include "sound.h"
#include "player.h"
#include "mathUtils.h"

void npcAct145(npc *NPC) //King's blade
{
	RECT rcLeft = { 96, 32, 112, 48 };
	RECT rcRight = { 112, 32, 128, 48 };

	if (!NPC->act_no)
	{
		//Some orientation stuff
		if (NPC->pNpc->count2)
		{
			if (NPC->pNpc->direct != dirLeft)
				NPC->direct = dirLeft;
			else
				NPC->direct = dirRight;
		}
		else if (NPC->pNpc->direct != dirLeft)
		{
			NPC->direct = dirRight;
		}
		else
		{
			NPC->direct = dirLeft;
		}

		//Set position
		if (NPC->direct != dirLeft)
			NPC->x = NPC->pNpc->x + 0x1400;
		else
			NPC->x = NPC->pNpc->x - 0x1400;
		NPC->y = NPC->pNpc->y;
	}

	//Set framerect
	if (NPC->direct != dirLeft)
		NPC->rect = rcRight;
	else
		NPC->rect = rcLeft;
}

void npcAct146(npc *NPC) //Lightning
{
	RECT rect[5];

	rect[0] = { 0, 0, 0, 0 };
	rect[1] = { 260, 0, 275, 240 };
	rect[2] = { 275, 0, 290, 240 };
	rect[3] = { 290, 0, 305, 240 };
	rect[4] = { 305, 0, 320, 240 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;

	//if (NPC->direct == 2)
	//	SetFlash(0, 0, 2);
	// Fallthrough
	case 1:
		if (++NPC->act_wait > 10)
		{
			NPC->act_no = 2;
			playSound(SFX_Lightning);
		}
		break;

	case 2:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no == 2)
			NPC->damage = 10;

		if (NPC->ani_no > 4)
		{
			createSmokeLeft(NPC->x, NPC->y, 0x1000, 8);
			NPC->cond = 0;
		}

	default:
		break;
	}

	NPC->rect = rect[NPC->ani_no];
}

void npcAct150(npc *NPC) // Quote
{
	RECT rcLeft[9];
	RECT rcRight[9];

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 48, 0, 64, 16 };
	rcLeft[2] = { 144, 0, 160, 16 };
	rcLeft[3] = { 16, 0, 32, 16 };
	rcLeft[4] = { 0, 0, 16, 16 };
	rcLeft[5] = { 32, 0, 48, 16 };
	rcLeft[6] = { 0, 0, 16, 16 };
	rcLeft[7] = { 160, 0, 176, 16 };
	rcLeft[8] = { 112, 0, 128, 16 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 48, 16, 64, 32 };
	rcRight[2] = { 144, 16, 160, 32 };
	rcRight[3] = { 16, 16, 32, 32 };
	rcRight[4] = { 0, 16, 16, 32 };
	rcRight[5] = { 32, 16, 48, 32 };
	rcRight[6] = { 0, 16, 16, 32 };
	rcRight[7] = { 160, 16, 176, 32 };
	rcRight[8] = { 112, 16, 128, 32 };

	enum
	{
		init = 0,
		lookUp = 2,
		getFlatenned = 10,
		teleportAway = 20,
		walking = 50,
		fallingUpsideDown = 60,
		lookAway = 80,
		walkingInPlace = 100,
	};

	switch(NPC->act_no)
	{
	case init:
		NPC->act_no = 1;
		NPC->ani_no = 0;

		if (NPC->direct > 10) // Hermit Gunsmith related ?
		{
			NPC->x = currentPlayer.x;
			NPC->y = currentPlayer.y;
			NPC->direct = dirLeft;
		}
		break;

	case lookUp:
		NPC->ani_no = 1;
		break;

	case getFlatenned:
		NPC->act_no = 11;

		for (size_t i = 0; i < 4; i++)
			createNpc(NPC_Smoke, NPC->x, NPC->y, random(-0x155, 0x155), random(-0x600, 0));

		playSound(SFX_QuoteSmashIntoGround);
	// Fallthrough
	case 11:
		NPC->ani_no = 2;
		break;

	case teleportAway:
		NPC->act_no = 21;
		NPC->act_wait = 0x40;
		playSound(SFX_Teleport);
	// Fallthrough
	case 21:
		if (!--NPC->act_wait)
			NPC->cond = 0;
		break;

	case walking:
		NPC->act_no = 51;
		NPC->ani_no = 3;
		NPC->ani_wait = 0;
	// Fallthrough
	case 51:
		if (++NPC->act_wait > 4)
		{
			NPC->act_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 6)
			NPC->ani_no = 3;

		if (NPC->direct != dirLeft)
			NPC->x += 0x200;
		else
			NPC->x -= 0x200;
		break;

	case fallingUpsideDown:
		NPC->act_no = 61;
		NPC->ani_no = 7;
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
	// Fallthrough
	case 61:
		NPC->tgt_y += 0x100;
		NPC->x = NPC->tgt_x + (random(-1, 1) << 9);
		NPC->y = NPC->tgt_y + (random(-1, 1) << 9);
		break;

	case 70:
		NPC->act_no = 71;
		NPC->act_wait = 0;
		NPC->ani_no = 3;
		NPC->ani_wait = 0;
	// Fallthrough
	case 71:
		if (NPC->direct != dirLeft)
			NPC->x -= 0x100;
		else
			NPC->x += 0x100;

		if (++NPC->ani_wait > 8)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 6)
			NPC->ani_no = 3;
		break;

	case lookAway:
		NPC->ani_no = 8;
		break;

	case 99:
	case walkingInPlace:
		NPC->act_no = 101;
		NPC->ani_no = 3;
		NPC->ani_wait = 0;
	// Fallthrough
	case 101:	// Fall a short distance THEN walk in place
		NPC->ym += 0x40;
		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;

		if (NPC->flag & ground)
		{
			NPC->ym = 0;
			NPC->act_no = 102;
		}
		NPC->y += NPC->ym;
		break;

	case 102:
		if (++NPC->act_wait > 8)
		{
			NPC->act_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 6)
			NPC->ani_no = 3;
		break;

	default:
		break;
	}

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];

	if (NPC->act_no == teleportAway + 1)
	{
		NPC->rect.bottom = NPC->rect.top + NPC->act_wait / 4;
		if (NPC->act_wait / 2 % 2)
			++NPC->rect.left;	// For the shaking effect
	}

	if (currentPlayer.equip & equip_mimigaMask)
	{
		NPC->rect.top += 32;
		NPC->rect.bottom += 32;
	}
}

void npcAct151(npc *NPC) //Blue robot standing
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = { 192, 0, 208, 16 };
	rcLeft[1] = { 208, 0, 224, 16 };
	rcRight[0] = { 192, 16, 208, 32 };
	rcRight[1] = { 208, 16, 224, 32 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
//Fallthrough
	case 1:
		if (random(0, 100) == 0)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 2:
		if (++NPC->act_wait > 16)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct153(npc *NPC)
{
	RECT rcLeft[21] =
	{
		{ 0, 0, 24, 24 },
		{ 24, 0, 48, 24 },
		{ 48, 0, 72, 24 },
		{ 0, 0, 24, 24 },
		{ 72, 0, 96, 24 },
		{ 0, 0, 24, 24 },
		{ 96, 0, 120, 24 },
		{ 120, 0, 144, 24 },
		{ 144, 0, 168, 24 },
		{ 168, 0, 192, 24 },
		{ 192, 0, 216, 24 },
		{ 216, 0, 240, 24 },
		{ 240, 0, 264, 24 },
		{ 264, 0, 288, 24 },
		{ 0, 48, 24, 72 },
		{ 24, 48, 48, 72 },
		{ 48, 48, 72, 72 },
		{ 72, 48, 96, 72 },
		{ 288, 0, 312, 24 },
		{ 24, 48, 48, 72 },
		{ 96, 48, 120, 72 }
	};

	RECT rcRight[21] =
	{
		{ 0, 24, 24, 48 },
		{ 24, 24, 48, 48 },
		{ 48, 24, 72, 48 },
		{ 0, 24, 24, 48 },
		{ 72, 24, 96, 48 },
		{ 0, 24, 24, 48 },
		{ 96, 24, 120, 48 },
		{ 120, 24, 144, 48 },
		{ 144, 24, 168, 48 },
		{ 168, 24, 192, 48 },
		{ 192, 24, 216, 48 },
		{ 216, 24, 240, 48 },
		{ 240, 24, 264, 48 },
		{ 264, 24, 288, 48 },
		{ 0, 72, 24, 96 },
		{ 24, 72, 48, 96 },
		{ 48, 72, 72, 96 },
		{ 72, 72, 96, 96 },
		{ 288, 24, 312, 48 },
		{ 24, 72, 48, 96 },
		{ 96, 72, 120, 96 }
	};

	if (NPC->x <= currentPlayer.x + 0x28000 && NPC->x >= currentPlayer.x - 0x28000 && NPC->y <= currentPlayer.y + 0x1E000 && NPC->y >= currentPlayer.y - 0x1E000)
	{
		switch (NPC->act_no)
		{
		case 0:
			NPC->act_no = 1;
			NPC->xm = 0;
			NPC->ani_no = 0;
			NPC->y += 1536;
			//Fallthrough
		case 1:
			if (random(0, 100) == 1)
			{
				NPC->act_no = 2;
				NPC->ani_no = 1;
				NPC->act_wait = 0;
			}

			if (random(0, 100) == 1)
			{
				if (NPC->direct)
					NPC->direct = 0;
				else
					NPC->direct = 2;
			}

			if (random(0, 100) == 1)
				NPC->act_no = 10;

			break;

		case 2:
			if (++NPC->act_wait > 20)
			{
				NPC->act_no = 1;
				NPC->ani_no = 0;
			}
			break;

		case 10:
			NPC->act_no = 11;
			NPC->act_wait = random(25, 100);
			NPC->ani_no = 2;
			NPC->ani_wait = 0;
			//Fallthrough
		case 11:
			if (++NPC->ani_wait > 3)
			{
				NPC->ani_wait = 0;
				++NPC->ani_no;
			}

			if (NPC->ani_no > 5)
				NPC->ani_no = 2;

			if (NPC->direct)
				NPC->xm = 0x200;
			else
				NPC->xm = -0x200;

			if (NPC->act_wait)
			{
				--NPC->act_wait;
			}
			else
			{
				NPC->act_no = 1;
				NPC->ani_no = 0;
				NPC->xm = 0;
			}

			if (NPC->direct || !(NPC->flag & leftWall))
			{
				if (NPC->direct == 2)
				{
					if (NPC->flag & rightWall)
					{
						NPC->ani_no = 2;
						NPC->ym = -0x5FF;
						NPC->act_no = 20;

						if (!(currentPlayer.cond & player_removed))
							playSound(30);
					}
				}
			}
			else
			{
				NPC->ani_no = 2;
				NPC->ym = -0x5FF;
				NPC->act_no = 20;

				if (!(currentPlayer.cond & player_removed))
					playSound(30);
			}
			break;

		case 20:
			if (NPC->direct || !(NPC->flag & leftWall))
			{
				if (NPC->direct == 2 && NPC->flag & rightWall)
					++NPC->count1;
				else
					NPC->count1 = 0;
			}
			else
			{
				++NPC->count1;
			}

			if (NPC->count1 > 10)
			{
				if (NPC->direct)
					NPC->direct = 0;
				else
					NPC->direct = 2;
			}

			if (NPC->direct)
				NPC->xm = 0x100;
			else
				NPC->xm = -0x100;

			if (NPC->flag & ground)
			{
				NPC->act_no = 21;
				NPC->ani_no = 20;
				NPC->act_wait = 0;
				NPC->xm = 0;

				if (!(currentPlayer.cond & player_removed))
					playSound(23);
			}
			break;

		case 21:
			if (++NPC->act_wait > 10)
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
		NPC->x += NPC->xm;
		NPC->y += NPC->ym;

		//Set framerect
		if (NPC->direct)
			NPC->rect = rcRight[NPC->ani_no];
		else
			NPC->rect = rcLeft[NPC->ani_no];

		if (NPC->life <= 985)
		{
			NPC->code_char = 154;
			NPC->act_no = 0;
		}
	}
}

void npcAct154(npc *NPC)
{
	RECT rcLeft[21] =
	{
		{ 0, 0, 24, 24 },
		{ 24, 0, 48, 24 },
		{ 48, 0, 72, 24 },
		{ 0, 0, 24, 24 },
		{ 72, 0, 96, 24 },
		{ 0, 0, 24, 24 },
		{ 96, 0, 120, 24 },
		{ 120, 0, 144, 24 },
		{ 144, 0, 168, 24 },
		{ 168, 0, 192, 24 },
		{ 192, 0, 216, 24 },
		{ 216, 0, 240, 24 },
		{ 240, 0, 264, 24 },
		{ 264, 0, 288, 24 },
		{ 0, 48, 24, 72 },
		{ 24, 48, 48, 72 },
		{ 48, 48, 72, 72 },
		{ 72, 48, 96, 72 },
		{ 288, 0, 312, 24 },
		{ 24, 48, 48, 72 },
		{ 96, 48, 120, 72 }
	};

	RECT rcRight[21] =
	{
		{ 0, 24, 24, 48 },
		{ 24, 24, 48, 48 },
		{ 48, 24, 72, 48 },
		{ 0, 24, 24, 48 },
		{ 72, 24, 96, 48 },
		{ 0, 24, 24, 48 },
		{ 96, 24, 120, 48 },
		{ 120, 24, 144, 48 },
		{ 144, 24, 168, 48 },
		{ 168, 24, 192, 48 },
		{ 192, 24, 216, 48 },
		{ 216, 24, 240, 48 },
		{ 240, 24, 264, 48 },
		{ 264, 24, 288, 48 },
		{ 0, 72, 24, 96 },
		{ 24, 72, 48, 96 },
		{ 48, 72, 72, 96 },
		{ 72, 72, 96, 96 },
		{ 288, 24, 312, 48 },
		{ 24, 72, 48, 96 },
		{ 96, 72, 120, 96 }
	};

	switch (NPC->act_no)
	{
	case 0:
		NPC->bits &= ~npc_shootable;
		NPC->bits &= ~npc_ignoresolid;
		NPC->damage = 0;
		NPC->act_no = 1;
		NPC->ani_no = 9;
		NPC->ym = -0x200;

		if (NPC->direct)
			NPC->xm = -0x100;
		else
			NPC->xm = 0x100;

		playSound(53);
		break;

	case 1:
		if (NPC->flag & ground)
		{
			NPC->ani_no = 10;
			NPC->ani_wait = 0;
			NPC->act_no = 2;
			NPC->act_wait = 0;
		}
		break;

	case 2:
		NPC->xm = 8 * NPC->xm / 9;

		if (++NPC->ani_wait > 3)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 11)
			NPC->ani_no = 10;

		if (++NPC->act_wait > 50)
			NPC->cond |= 8;

		break;
	}

	NPC->ym += 0x20;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Set framerect
	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}
