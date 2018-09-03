#include "npc040.h"

#include "player.h"
#include "sound.h"
#include "mathUtils.h"
#include "render.h"

using std::vector;

void npcAct040(npc * NPC) // Santa
{
	vector<RECT> rcLeft(7);
	vector<RECT> rcRight(7);

	rcLeft[0] = { 0, 32, 16, 48 };
	rcLeft[1] = { 16, 32, 32, 48 };
	rcLeft[2] = { 32, 32, 48, 48 };
	rcLeft[3] = rcLeft[0];
	rcLeft[4] = { 48, 32, 64, 48 };
	rcLeft[5] = rcLeft[0];
	rcLeft[6] = { 64, 32, 80, 48 };

	rcRight[0] = { 0, 48, 16, 64 };
	rcRight[1] = { 16, 48, 32, 64 };
	rcRight[2] = { 32, 48, 48, 64 };
	rcRight[3] = rcRight[0];
	rcRight[4] = { 48, 48, 64, 64 };
	rcRight[5] = rcRight[0];
	rcRight[6] = { 64, 48, 80, 64 };

	enum
	{
		init = 0,
		stand = 1,
		blinkWait = 2,
		walk = 3,
		faceAway = 5,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = stand;
		NPC->ani_no = 1;
		NPC->act_wait = 0;
		// Fallthrough
	case stand:
		if (!random(0, 120))
		{
			NPC->act_no = blinkWait;
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}
		if (NPC->isPlayerWithinDistance(0x4000, 0x4000, 0x2000))
			NPC->facePlayer();
		break;

	case blinkWait:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = stand;
			NPC->ani_no = 0;
		}
		break;

	case walk:
		NPC->animate(4, 2, 5);
		if (NPC->direct == dirLeft)
			NPC->x -= 0x200;
		else
			NPC->x += 0x200;
		break;

	default:
		break;
	}
	
	NPC->doRects(rcLeft, rcRight);
}

void npcAct041(npc * NPC) // Busted doorway
{
	if (!NPC->act_no)
	{
		++NPC->act_no;
		NPC->y -= 0x2000;
	}

	NPC->rect = { 0, 80, 48, 112 };
}

void npcAct042(npc *NPC) // Sue
{
	vector<RECT> rcLeft(13);
	vector<RECT> rcRight(13);

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 16, 0, 32, 16 };
	rcLeft[2] = { 32, 0, 48, 16 };
	rcLeft[3] = { 0, 0, 16, 16 };
	rcLeft[4] = { 48, 0, 64, 16 };
	rcLeft[5] = { 0, 0, 16, 16 };
	rcLeft[6] = { 64, 0, 80, 16 };
	rcLeft[7] = { 80, 32, 96, 48 };
	rcLeft[8] = { 96, 32, 112, 48 };
	rcLeft[9] = { 128, 32, 144, 48 };
	rcLeft[10] = { 0, 0, 16, 16 };
	rcLeft[11] = { 112, 32, 128, 48 };
	rcLeft[12] = { 160, 32, 176, 48 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 16, 16, 32, 32 };
	rcRight[2] = { 32, 16, 48, 32 };
	rcRight[3] = { 0, 16, 16, 32 };
	rcRight[4] = { 48, 16, 64, 32 };
	rcRight[5] = { 0, 16, 16, 32 };
	rcRight[6] = { 64, 16, 80, 32 };
	rcRight[7] = { 80, 48, 96, 64 };
	rcRight[8] = { 96, 48, 112, 64 };
	rcRight[9] = { 128, 48, 144, 64 };
	rcRight[10] = { 0, 16, 16, 32 };
	rcRight[11] = { 112, 48, 128, 64 };
	rcRight[12] = { 160, 48, 176, 64 };

	enum // Sue's states
	{
		standAndBlink = 0,
		walk = 3,
		punchedByIgor = 6,
		superPunchedByIgor = 8,
		collapsed = 10,
		punchAir = 11,
		carriedByIgor = 13,
		undeadCoreIntro = 15,
		undeadCoreIntroLookUp = 17,
		undeadCoreIntroRunToQuote = 20,
		postUndeadCoreRun = 30,
		jumpOffIsland = 40,
	};

	switch (NPC->act_no)
	{
	case standAndBlink:
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

	case walk:
		NPC->act_no = 4;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
	// Fallthrough
	case 4:
		NPC->animate(4, 2, 5);
		NPC->moveTowardsPlayer(pixelsToUnits(1));
		break;

	case 5:
		NPC->ani_no = punchedByIgor;
		NPC->xm = 0;
		break;

	case punchedByIgor: // Flinches for 10 frames then goes back to blinking
		playSound(SFX_EnemySqueal);
		NPC->act_wait = 0;
		NPC->act_no = 7;
		NPC->ani_no = 7;
	// Fallthrough
	case 7:
		if (++NPC->act_wait > 10)
			NPC->act_no = standAndBlink;
		break;

	case superPunchedByIgor: // Flies through the air backwards and crashes
		playSound(SFX_EnemySqueal);
		NPC->act_wait = 0;
		NPC->act_no = 9;
		NPC->ani_no = 7;
		NPC->ym = -0x200;
		NPC->moveTowardsPlayer(pixelsToUnits(2));
	// Fallthrough
	case 9:
		if (++NPC->act_wait > 3 && NPC->flag & ground)
		{
			NPC->act_no = collapsed;
			if (NPC->direct != dirLeft)
				NPC->direct = dirLeft;
			else
				NPC->direct = dirRight;
		}
		break;

	case collapsed:
		NPC->xm = 0;
		NPC->ani_no = 8;
		break;

	case punchAir:
		NPC->act_no = 12;
		NPC->act_wait = 0;
		NPC->ani_no = 9;
		NPC->ani_wait = 0;
		NPC->xm = 0;
	// Fallthrough
	case 12:
		NPC->animate(8, 9, 10);
		break;

	case carriedByIgor:
		NPC->ani_no = 11;
		NPC->xm = 0;
		NPC->ym = 0;
		NPC->act_no = 14;
		size_t i;
		for (i = 0; i < npcs.size() && npcs[i].code_event != 501; ++i)	// Search for Igor ?
			;
		if (i == npcs.size())
		{
			NPC->act_no = standAndBlink;
			break;
		}
		NPC->pNpc = &npcs[i];
	// Fallthrough
	case 14:
		if (NPC->pNpc->direct != dirLeft)
			NPC->direct = dirLeft;
		else
			NPC->direct = dirRight;

		if (NPC->pNpc->direct != dirLeft)
			NPC->x = NPC->pNpc->x + 0xC00;
		else
			NPC->x = NPC->pNpc->x - 0XC00;
		NPC->y = NPC->pNpc->y + 0x800;
		if (NPC->pNpc->ani_no == 2 || NPC->pNpc->ani_no == 4)
			NPC->y -= 0x200;
		break;

	case undeadCoreIntro:
		NPC->act_no = 16;
		createNpc(NPC_RedCrystal, NPC->x + 0x10000, NPC->y, 0, 0, dirLeft, nullptr);
		createNpc(NPC_RedCrystal, NPC->x + 0x10000, NPC->y, 0, 0, dirRight, nullptr);
		NPC->xm = 0;
		NPC->ani_no = 0;
	// Fallthrough
	case 16:
		superXPos = NPC->x - 0x3000;
		superYPos = NPC->y - 0x1000;
		break;

	case undeadCoreIntroLookUp:
		NPC->xm = 0;
		NPC->ani_no = 12;
		superXPos = NPC->x;
		superYPos = NPC->y - 0x1000;
		break;

	case undeadCoreIntroRunToQuote:
		NPC->act_no = 21;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
	// Fallthrough
	case 21:
		NPC->animate(2, 2, 5);
		NPC->moveTowardsPlayer(pixelsToUnits(2));

		if (NPC->x < currentPlayer.x - 0x1000)
		{
			NPC->direct = dirRight;
			NPC->act_no = standAndBlink;
		}
		break;

	case postUndeadCoreRun:
		NPC->act_no = 31;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
	// Fallthrough
	case 31:
		NPC->animate(2, 2, 5);
		NPC->moveTowardsPlayer(pixelsToUnits(2));
		break;
	case jumpOffIsland:
		NPC->act_no = 41;
		NPC->ani_no = 9;
		NPC->ym = -0x400;
		break;
	default:
		break;
	}

	if (NPC->act_no != carriedByIgor + 1)
	{
		NPC->ym += 0x40;
		if (NPC->xm > 0x400)
			NPC->xm = 0x400;
		if (NPC->xm < -0x400)
			NPC->xm = -0x400;
		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;

		NPC->x += NPC->xm;
		NPC->y += NPC->ym;
	}

	NPC->doRects(rcLeft, rcRight);
}

void npcAct043(npc * NPC) // Blackboard
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->y -= 0x2000;
	}
	if (NPC->direct != dirLeft)
		NPC->rect = { 168, 80, 208, 112 };
	else
		NPC->rect = { 128, 80, 168, 112 };
}

void npcAct046(npc *NPC) //H/V trigger
{
	NPC->bits |= npc_eventtouch;

	if (NPC->direct != dirLeft)
	{
		if (NPC->y >= currentPlayer.y)
			NPC->y -= 0x5FF;
		else
			NPC->y += 0x5FF;
	}
	else
	{
		if (NPC->x >= currentPlayer.x)
			NPC->x -= 0x5FF;
		else
			NPC->x += 0x5FF;
	}

	NPC->rect = { 0, 0, 16, 16 };
}

void npcAct052(npc *NPC) // Blue robots
{
	NPC->rect = { 240, 96, 256, 112 };
}

void npcAct055(npc *NPC) //Kazuma
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0] = { 0xC0, 0xC0, 0xD0, 0xD8 };
	rcLeft[1] = { 0xD0, 0xC0, 0xE0, 0xD8 };
	rcLeft[2] = { 0xC0, 0xC0, 0xD0, 0xD8 };
	rcLeft[3] = { 0xE0, 0xC0, 0xF0, 0xD8 };
	rcLeft[4] = { 0xC0, 0xC0, 0xD0, 0xD8 };
	rcLeft[5] = { 0xF0, 0xC0, 0x100, 0xD8 };

	rcRight[0] = { 0xC0, 0xD8, 0xD0, 0xF0 };
	rcRight[1] = { 0xD0, 0xD8, 0xE0, 0xF0 };
	rcRight[2] = { 0xC0, 0xD8, 0xD0, 0xF0 };
	rcRight[3] = { 0xE0, 0xD8, 0xF0, 0xF0 };
	rcRight[4] = { 0xC0, 0xD8, 0xD0, 0xF0 };
	rcRight[5] = { 0xF0, 0xD8, 0x100, 0xF0 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		break;

	case 3:
		NPC->act_no = 4;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
//Fallthrough
	case 4:
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		if (NPC->direct)
			NPC->x += 0x200;
		else
			NPC->x -= 0x200;
		break;

	case 5:
		NPC->ani_no = 5;
		break;
	default:
		break;
	}

	NPC->ym += 32;
	if (NPC->ym > 1535)
		NPC->ym = 1535;

	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct058(npc *NPC) //Basu 1
{
	const int action = NPC->act_no;

	if (!action)
	{
		//Don't appear until Quote walks into spawn area
		if (currentPlayer.x >= NPC->x + 0x2000 || currentPlayer.x <= NPC->x - 0x2000)
		{
			NPC->rect = { 0, 0, 0, 0 };
			NPC->damage = 0;
			NPC->xm = 0;
			NPC->ym = 0;
			NPC->bits &= ~npc_shootable;
		}
		else
		{
			NPC->bits |= npc_shootable;
			NPC->ym = -256;

			NPC->tgt_x = NPC->x;
			NPC->tgt_y = NPC->y;

			NPC->act_no = 1;
			NPC->act_wait = 0;

			NPC->count1 = NPC->direct; //a
			NPC->count2 = 0;

			NPC->damage = 6;

			if (NPC->direct != dirLeft)
			{
				NPC->x = currentPlayer.x - 0x20000;
				NPC->xm = 767;
			}
			else
			{
				NPC->x = currentPlayer.x + 0x20000;
				NPC->xm = -767;
			}
		}
	}
	else if (action)
	{
		if (action == 1)
		{
			//Face towards player
			if (NPC->x <= currentPlayer.x)
			{
				NPC->direct = dirRight;
				NPC->xm += 16;
			}
			else
			{
				NPC->direct = dirLeft;
				NPC->xm -= 16;
			}

			//Bounce off of wall
			if (NPC->flag & leftWall)
				NPC->xm = 0x200;
			if (NPC->flag & rightWall)
				NPC->xm = -0x200;

			//Fly up and down
			if (NPC->y >= NPC->tgt_y)
				NPC->ym -= 8;
			else
				NPC->ym += 8;

			//Limit speed
			if (NPC->xm > 0x2FF)
				NPC->xm = 0x2FF;
			if (NPC->xm < -0x2FF)
				NPC->xm = -0x2FF;

			if (NPC->ym > 0x100)
				NPC->ym = 0x100;
			if (NPC->ym < -0x100)
				NPC->ym = -0x100;

			//Move
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

		if ((currentPlayer.x <= NPC->x + 0x32000 && currentPlayer.x >= NPC->x - 0x32000) || action != 1) //Do stuff if nearby
		{
			if (NPC->act_no)
			{
				if (NPC->act_wait < 150)
					++NPC->act_wait;
				else
				{
					if (!(++NPC->count2 & 7) && NPC->x < currentPlayer.x + 0x14000 && NPC->x > currentPlayer.x - 0x14000)
					{
						uint8_t deg = getAtan(NPC->x - currentPlayer.x, NPC->y - currentPlayer.y);
						deg += random(-6, 6);

						const int fireXm = 2 * getCos(deg);
						const int fireYm = 2 * getSin(deg);

						createNpc(NPC_ProjectileBasu1, NPC->x, NPC->y, fireXm, fireYm);
						playSound(SFX_EnemyShootProjectile);
					}

					if (NPC->count2 > 8)
					{
						NPC->act_wait = 0;
						NPC->count2 = 0;
					}
				}
			}

			//Animate
			if (++NPC->ani_wait > 1)
			{
				NPC->ani_wait = 0;
				++NPC->ani_no;
			}

			if (NPC->ani_no > 1)
				NPC->ani_no = 0;

			if (NPC->act_wait > 120 && NPC->act_wait / 2 % 2 == 1 && NPC->ani_no == 1)
				NPC->ani_no = 2;

			if (NPC->direct != dirLeft)
				NPC->rect = { 192 + (NPC->ani_no * 24), 24, 216 + (NPC->ani_no * 24), 48 };
			else
				NPC->rect = { 192 + (NPC->ani_no * 24), 0, 216 + (NPC->ani_no * 24), 24 };
		}
		else
		{
			NPC->x = NPC->tgt_x;
			NPC->xm = 0;

			//Stop existing
			NPC->act_no = 0;
			NPC->direct = NPC->count1;
			NPC->rect = { 0, 0, 0, 0 };
			NPC->damage = 0;
		}
	}
}

void npcAct059(npc *NPC) //Eye door
{
	RECT *setRect;
	RECT rect[4];

	rect[0] = { 224, 16, 240, 40 };
	rect[1] = { 208, 80, 224, 104 };
	rect[2] = { 224, 80, 240, 104 };
	rect[3] = { 240, 80, 256, 104 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
	// Fallthrough
	case 2:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no == 2)
			NPC->act_no = 3;

		break;

	case 3:
		if (NPC->x - 0x8000 >= currentPlayer.x || NPC->x + 0x8000 <= currentPlayer.x || NPC->y - 0x8000 >= currentPlayer.y || NPC->y + 0x8000 <= currentPlayer.y)
		{
			NPC->act_no = 4;
			NPC->ani_wait = 0;
		}

		break;

	case 4:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			--NPC->ani_no;
		}

		if (!NPC->ani_no)
			NPC->act_no = 1;

		break;

	default:
		break;
	}

	if (NPC->act_no == 1) //replaces some dumb label and goto ewww acts the same anyways.
	{
		if (NPC->x - 0x8000 < currentPlayer.x && NPC->x + 0x8000 > currentPlayer.x && NPC->y - 0x8000 < currentPlayer.y && NPC->y + 0x8000 > currentPlayer.y)
		{
			NPC->act_no = 2;
			NPC->ani_wait = 0;
		}
	}

	if (NPC->shock)
		setRect = &rect[3];
	else
		setRect = &rect[NPC->ani_no];

	NPC->rect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}


