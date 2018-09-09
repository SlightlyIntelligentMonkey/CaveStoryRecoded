#include "npc100.h"

#include <vector>
#include "player.h"
#include "sound.h"
#include "render.h"
#include "mathUtils.h"
#include "game.h"
#include "caret.h"

using std::vector;

void npcAct100(npc * NPC) // Grate
{
	if (NPC->act_no == 0)
	{
		NPC->act_no = 1;
		NPC->y += 0x2000;
	}

	NPC->rect = { 272, 48, 288, 64 };
}

void npcAct101(npc *NPC) // Power Controls, screen
{
    vector<RECT> rcNPC = {{240, 136, 256, 152}, {240, 136, 256, 152}, {256, 136, 272, 152}};
    NPC->animate(3, 0, 2);
    NPC->doRects(rcNPC);
}

void npcAct102(npc *NPC) // Power Controls, power flow
{
    vector<RECT> rcNPC = {{208, 120, 224, 136}, {224, 120, 240, 136}, {240, 120, 256, 136}, {256, 120, 272, 136}};

    if (!NPC->act_no)
    {
        NPC->act_no = 1;
        NPC->y += 0x1000;
    }

    NPC->animate(0, 0, 3);
    NPC->doRects(rcNPC);
}

void npcAct103(npc *NPC) // Manann red blast (projectile)
{
    vector<RECT> rcLeft = {{192, 96, 208, 120}, {208, 96, 224, 120}, {224, 96, 240, 120}};
    vector<RECT> rcRight = {{192, 120, 208, 144}, {208, 120, 224, 144}, {224, 120, 240, 144}};

    if (NPC->act_no == 1 || !NPC->act_no)
    {
        if (!NPC->act_no)
            NPC->act_no = 1;
        NPC->accelerateTowardsPlayer(0x20);
        NPC->animate(0, 0, 2);
    }

    NPC->x += NPC->xm;
    NPC->doRects(rcLeft, rcRight);

    if (++NPC->count1 > 100)
        NPC->cond = 0;
    if (NPC->count1 % 4 == 1)
        playSound(SFX_IronHeadShot);
}

void npcAct104(npc *NPC) // Frog (enemy)
{
	vector<RECT> rcLeft(3);
	vector<RECT> rcRight(3);

	rcLeft = { {0, 112, 32, 144}, {32, 112, 64, 144}, {64, 112, 96, 144} };
	rcRight = { {0, 144, 32, 176}, {32, 144, 64, 176}, {64, 144, 96, 176} };

	enum
	{
		init = 0,
		standing = 1,
		mouthTwitch = 2,
		fallingFromCeiling = 3,
		startJump = 10,
		jumping = 11,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = standing;
		NPC->act_wait = 0;
		NPC->xm = 0;
		NPC->ym = 0;
		if (NPC->direct == 4)	// Spawned by Balfrog
		{
			if (random(0, 1))
				NPC->direct = dirLeft;
			else
				NPC->direct = dirRight;
			NPC->bits |= npc_ignoreSolid;
			NPC->ani_no = 2;
			NPC->act_no = fallingFromCeiling;
			break;
		}
		NPC->bits &= ~npc_ignoreSolid;
		// Fallthrough
	case standing:
		++NPC->act_wait;
		if (!random(0, 50))
		{
			NPC->act_no = mouthTwitch;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->ani_wait = 0;
		}
		break;

	case mouthTwitch:
		NPC->animate(2, 0, 1);

		if (++NPC->act_wait > 18)
			NPC->act_no = standing;
		break;

	case fallingFromCeiling:
		if (++NPC->act_wait > 40)
			NPC->bits &= ~npc_ignoreSolid;

		if (NPC->flag & ground)
		{
			NPC->act_no = init;
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}
		break;

	case startJump:
	    NPC->act_no = jumping;
	    // Fallthrough
	case jumping:
		if (NPC->flag & leftWall && NPC->xm < 0)
		{
			NPC->xm = -NPC->xm;
			NPC->direct = dirRight;
		}
		if (NPC->flag & rightWall && NPC->xm > 0)
		{
			NPC->xm = -NPC->xm;
			NPC->direct = dirLeft;
		}

		if (NPC->flag & ground)
		{
			NPC->act_no = init;
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}
		break;

	default:
		break;
	}

	bool doJump = false;
	if (NPC->act_no < startJump && NPC->act_no != fallingFromCeiling && NPC->act_wait > 10)
	{
		if (NPC->shock)
			doJump = true;
		if (NPC->x >= currentPlayer.x - 0x14000
			&& NPC->x <= currentPlayer.x + 0x14000
			&& NPC->y >= currentPlayer.y - 0x8000
			&& NPC->y <= currentPlayer.y + 0x8000
			&& !random(0, 50))
			doJump = true;
	}
	if (doJump)
	{
		NPC->facePlayer();
		NPC->act_no = startJump;
		NPC->ani_no = 2;
		NPC->ym = -0x5FF;
		if (!(currentPlayer.cond & player_removed))
			playSound(SFX_CritterHop);

		NPC->moveInDir(0x200);
	}

	NPC->doGravity(0x80, 0x5FF);
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);
}


void npcAct105(npc *NPC) // Speech balloon 'Hey' low
{
    vector<RECT> rcNPC = {{128, 32, 144, 48}, {128, 38, 128, 32}};

    if (++NPC->act_wait > 30)
        NPC->cond = 0;
    if (NPC->act_wait < 5)
        NPC->y -= pixelsToUnits(1);

    NPC->doRects(rcNPC);
}

void npcAct106(npc *NPC) // Speech balloon 'Hey' high
{
	if (NPC->act_no == 0)
	{
		createNpc(NPC_BalloonHeyLow, NPC->x, NPC->y - 0x1000);
		NPC->act_no = 1;
	}
}

<<<<<<< HEAD
void npcAct107(npc *NPC) //malco
{
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		if (NPC->direct == 2)
			NPC->ani_no = 5;
		break;
	case 10:
		NPC->act_no = 11;
		NPC->act_wait = 0;
		NPC->ani_wait = 0;
		for (int i = 0; i <= 3; ++i)
			createNpc(4, NPC->x, NPC->y, random(-341, 341), random(-1536, 0));
	case 11:
		if (++NPC->ani_wait > 1)
		{
			playSound(43);
=======
void npcAct107(npc *NPC) // Malco
{
	enum
	{
		init = 0,
		flickerAndBeep = 10,
		shaking = 15,
		standUp = 16,
		bopUpDown = 18,
		getSmashed = 21,
		babyMalco = 100,
		babyBlowsUp = 110,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = init + 1;
		if (NPC->direct == dirRight)	// After fight when not pulled out
			NPC->ani_no = 5;
		break;

	case flickerAndBeep:
		NPC->act_no = flickerAndBeep + 1;
		NPC->act_wait = 0;
		NPC->ani_wait = 0;
		for (size_t i = 0; i < 4; ++i)
		{
			auto xVel = random(-0x155, 0x155);
			auto yVel = random(-0x600, 0x600);
			createNpc(NPC_Smoke, NPC->x, NPC->y, xVel, yVel);
		}
		// Fallthrough
	case flickerAndBeep + 1:
		if (++NPC->ani_wait > 1)
		{
			playSound(SFX_ComputerScreenOn);
>>>>>>> upstream/master
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;
		if (++NPC->act_wait > 100)
<<<<<<< HEAD
			NPC->act_no = 12;
		break;
	case 12:
		NPC->act_no = 13;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
	case 13:
		if (++NPC->act_wait > 50)
			NPC->act_no = 14;
		break;
	case 14:
		NPC->act_no = 15;
		NPC->act_wait = 0;
	case 15:
		if (NPC->act_wait / 2 & 1)
		{
			NPC->x += 512;
			playSound(11);
		}
		else
		{
			NPC->x -= 512;
		}
		if (++NPC->act_wait > 50)
			NPC->act_no = 16;
		break;
	case 16:
		NPC->act_no = 17;
		NPC->act_wait = 0;
		NPC->ani_no = 2;
		playSound(12);
		for (int i = 0; i <= 7; ++i)
			createNpc(4, NPC->x, NPC->y, random(-341, 341), random(-1536, 0));
	case 17:
		if (++NPC->act_wait > 150)
			NPC->act_no = 18;
		break;
	case 18:
		NPC->act_no = 19;
		NPC->act_wait = 0;
		NPC->ani_no = 3;
		NPC->ani_wait = 0;
	case 19:
		if (++NPC->ani_wait > 3)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 4)
		{
			playSound(11, 1);
			NPC->ani_no = 3;
		}
		if (++NPC->act_wait > 100)
		{
			NPC->act_no = 20;
			playSound(12);
			for (int i = 0; i <= 3; ++i)
				createNpc(4, NPC->x, NPC->y, random(-341, 341), random(-1536, 0));
		}
		break;
	case 20:
		NPC->ani_no = 4;
		break;
	case 21:
		NPC->act_no = 22;
		NPC->ani_no = 5;
		playSound(51);
		break;
	case 100:
		NPC->act_no = 101;
		NPC->ani_no = 6;
		NPC->ani_wait = 0;
	case 101:
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 9)
			NPC->ani_no = 6;
		break;
	case 110:
		createSmokeLeft(NPC->x, NPC->y, 0x2000, 16);
		NPC->cond = 0;
		break;
	default:
		break;
	}

	NPC->rect.left = 144 + (NPC->ani_no*16);
	NPC->rect.top = (NPC->direct/2)*24;
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 24;
}

void npcAct108(npc *NPC) //balfrog projectile
{

	if (NPC->flag & 0xFF)
	{
		createCaret(NPC->x, NPC->y, 2, 0);
		NPC->cond = 0;
	}

	NPC->y += NPC->ym;
	NPC->x += NPC->xm;

	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}
	if (NPC->ani_no > 2)
		NPC->ani_no = 0;

	NPC->rect.left = 96 + (NPC->ani_no * 16);
	NPC->rect.top = 48;
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 16;

	if (++NPC->count1 > 300)
	{
		createCaret(NPC->x, NPC->y, 2, 0);
		NPC->cond = 0;
	}
}

void npcAct109(npc *NPC) //broken malco
{
	switch (NPC->act_no)
	{
	case(0):
		if (--NPC->act_wait)
			NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
	case(1):
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		if (NPC->x - 0x4000 < currentPlayer.x && NPC->x + 0x4000 > currentPlayer.x &&
			NPC->y - 0x4000 < currentPlayer.y && NPC->y + 0x2000 > currentPlayer.y)
		{
			NPC->facePlayer();
		}
		break;
	case(2):
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;
	case(10):
		NPC->act_no = 0;
		playSound(12);
		for (int i = 0; i <= 7; ++i)
			createNpc(4, NPC->x, NPC->y, random(-341, 341), random(-1536, 0));
		break;
	}


	NPC->doGravity(64, 1535);

	NPC->rect.left = 240 + (NPC->ani_no * 16);
	NPC->rect.top = (NPC->direct / 2) * 24;
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 24;
}

void npcAct110(npc *NPC) //mini frog
{
	bool bJump;

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->act_wait = 0;
		NPC->xm = 0;
		NPC->ym = 0;
		if (NPC->direct != 4)
		{
			NPC->bits &= 0xFFF7;
			goto CASEONE;
		}
		if (random(0, 1) != 0)
			NPC->direct = 0;
		else
			NPC->direct = 2;
		NPC->bits |= 8;
		NPC->ani_no = 2;
		NPC->act_no = 3;
		break;
	case 1:
		CASEONE:
		++NPC->act_wait;
		if (random(0, 50) == 1)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->ani_wait = 0;
		}
		break;
	case 2:
		++NPC->act_wait;
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;
		if (NPC->act_wait > 18)
		{
			NPC->act_no = 1;
			NPC->act_no = 1;
		}
		break;
	case 3:
		if (++NPC->act_wait > 40)
			NPC->bits &= 0xFFF7u;
		if (NPC->flag & 8)
		{
			NPC->act_no = 0;
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}
		break;
	case 10:
		NPC->act_no = 11;
	case 11:
		if (NPC->flag & 1 && NPC->xm < 0)
		{
			NPC->xm = -NPC->xm;
			NPC->direct = 2;
		}
		if (NPC->flag & 4 && NPC->xm > 0)
		{
			NPC->xm = -NPC->xm;
			NPC->direct = 0;
		}
		if (NPC->flag & 8)
		{
			NPC->act_no = 0;
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}
		break;
	default:
		break;
	}
	bJump = 0;
	if (NPC->act_no <= 9 && NPC->act_no != 3 && NPC->act_wait > 10)
	{
		if (NPC->shock)
			bJump = 1;
		if (NPC->x >= currentPlayer.x - 81920
			&& NPC->x <= currentPlayer.x + 81920
			&& NPC->y >= currentPlayer.y - 0x8000
			&& NPC->y <= currentPlayer.y + 0x8000
			&& random(0, 50) == 2)
		{
			bJump = 1;
		}
	}
	if (bJump)
	{
		if (NPC->x >= currentPlayer.x)
			NPC->direct = 0;
		else
			NPC->direct = 2;
		NPC->act_no = 10;
		NPC->ani_no = 2;
		NPC->ym = -767;
		playSound(6, 1);
		if (NPC->direct)
			NPC->xm = 256;
		else
			NPC->xm = -256;
	}

	NPC->doGravity(128, 1535);
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->rect.left = 96 + (NPC->ani_no * 16);
	NPC->rect.top = 128 + ((NPC->direct / 2) * 16);
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 16;
=======
			NPC->act_no = flickerAndBeep + 2;
		break;

	case flickerAndBeep + 2:
		NPC->act_no = flickerAndBeep + 3;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
		// Fallthrough
	case flickerAndBeep + 3:
		if (++NPC->act_wait > 50)
			NPC->act_no = flickerAndBeep + 4;
		break;
	
	case flickerAndBeep + 4:
		NPC->act_no = shaking;
		NPC->act_wait = 0;
		// Fallthrough
	case shaking:
		if (NPC->act_wait / 2 % 2)
		{
			NPC->x += pixelsToUnits(1);
			playSound(SFX_DoorOpen);
		}
		else
			NPC->x -= pixelsToUnits(1);

		if (++NPC->act_wait > 50)
			NPC->act_no = standUp;
		break;

	case standUp:
		NPC->act_no = standUp + 1;
		NPC->act_wait = 0;
		NPC->ani_no = 2;
		playSound(SFX_DestroyBreakableBlock);
		for (size_t i = 0; i < 8; ++i)
		{
			auto yVel = random(pixelsToUnits(-3), 0);
			auto xVel = random(-0x155, 0x155);
			createNpc(NPC_Smoke, NPC->x, NPC->y, xVel, yVel);
		}
		// Fallthrough
	case standUp + 1:
		if (++NPC->act_wait > 150)
			NPC->act_no = 18;
		break;

	case bopUpDown:
		NPC->act_no = bopUpDown + 1;
		NPC->act_wait = 0;
		NPC->ani_no = 3;
		NPC->ani_wait = 0;
		// Fallthrough
	case bopUpDown + 1:
		NPC->animate(3);

		if (NPC->ani_no > 4)
		{
			playSound(SFX_DoorOpen);
			NPC->ani_no = 3;
		}

		if (++NPC->act_wait > 100)
		{
			NPC->act_no = bopUpDown + 2;
			playSound(SFX_DestroyBreakableBlock);
			NPC->createSmokeWithVel(4, random(pixelsToUnits(-3), 0), random(-0x155, 0x155));
		}
		break;

	case bopUpDown + 2:
		NPC->ani_no = 4;
		break;

	case getSmashed:
		NPC->act_no = getSmashed + 1;
		NPC->ani_no = 5;
		playSound(SFX_CritterSmallHurt);
		break;

	case babyMalco:
		NPC->animate(4, 6, 9);
		break;

	case babyBlowsUp:
		createSmokeLeft(NPC->x, NPC->y, tilesToUnits(1), 16);
		NPC->cond = 0;
		break;

	default:
		break;
	}

	vector<RECT> rcNPC(10);

	rcNPC[0] = { 144, 0, 160, 24 };
	rcNPC[1] = { 160, 0, 176, 24 };
	rcNPC[2] = { 176, 0, 192, 24 };
	rcNPC[3] = { 192, 0, 208, 24 };
	rcNPC[4] = { 208, 0, 224, 24 };
	rcNPC[5] = { 224, 0, 240, 24 };
	rcNPC[6] = { 176, 0, 192, 24 };
	rcNPC[7] = { 192, 0, 208, 24 };
	rcNPC[8] = { 208, 0, 224, 24 };
	rcNPC[9] = { 192, 0, 208, 24 };

	NPC->doRects(rcNPC);
>>>>>>> upstream/master
}

void npcAct111(npc *NPC) //Quote teleport out
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 16, 0, 32, 16 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 16, 16, 32, 32 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->y -= 0x2000;
		break;

	case 1:
		if (++NPC->act_wait > 20)
		{
			NPC->act_wait = 0;
			NPC->act_no = 2;
			NPC->ani_no = 1;
			NPC->ym = -0x2FF;
		}
		break;

	case 2:
		if (NPC->ym > 0)
			NPC->hit.bottom = 0x2000;

		if (NPC->flag & ground)
		{
			NPC->act_no = 3;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
		}

		break;
	case 3:
		if (++NPC->act_wait > 40)
		{
			NPC->act_no = 4;
			NPC->act_wait = 64;
			playSound(SFX_Teleport);
		}
		break;
	case 4:
		--NPC->act_wait;
		NPC->ani_no = 0;

		if (!NPC->act_wait)
			NPC->cond = 0;
		break;
	default:
		break;
	}

	NPC->ym += 0x40;
	NPC->y += NPC->ym;

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];

	if (currentPlayer.equip & equip_mimigaMask)
	{
		NPC->rect.top += 32;
		NPC->rect.bottom += 32;
	}

	if (NPC->act_no == 4)
	{
		NPC->rect.bottom = NPC->act_wait / 4 + NPC->rect.top;

		if (NPC->act_wait / 2 & 1)
			++NPC->rect.left;
	}
}

void npcAct112(npc *NPC) //Quote teleport in
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 16, 0, 32, 16 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 16, 16, 32, 32 };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->x += 0x2000;
		NPC->y += 4096;
		playSound(SFX_Teleport);
	}

	switch (NPC->act_no)
	{
	case 1:
		if (++NPC->act_wait >= 64)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
		}
		break;

	case 2:
		if (++NPC->act_wait > 20)
		{
			NPC->act_no = 3;
			NPC->ani_no = 1;
			NPC->hit.bottom = 0x1000;
		}
		break;

	case 3:
		if (NPC->flag & ground)
		{
			NPC->act_no = 4;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
		}
		break;
	}

	NPC->ym += 64;
	NPC->y += NPC->ym;

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];

	if (currentPlayer.equip & equip_mimigaMask)
	{
		NPC->rect.top += 32;
		NPC->rect.bottom += 32;
	}

	if (NPC->act_no == 1)
	{
		NPC->rect.bottom = NPC->act_wait / 4 + NPC->rect.top;

		if (NPC->act_wait / 2 & 1)
			++NPC->rect.left;
	}
}

void npcAct114(npc *NPC) // Press (enemy)
{
    vector<RECT> rcNPC = {{144, 112, 160, 136}, {160, 112, 176, 136}, {176, 112, 192, 136}};

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->y -= pixelsToUnits(4);
        // Fallthrough
    case 1:
        if (!(NPC->flag & ground))
        {
            NPC->act_no = 10;
            NPC->act_wait = 8;
            NPC->ani_no = 1;
        }
        break;

    case 10:
        NPC->animate(2, 2, 2);

        if (currentPlayer.y <= NPC->y)
        {
            NPC->bits |= npc_solidHard;
            NPC->damage = 0;
        }
        else
        {
            NPC->bits &= ~npc_solidHard;
            NPC->damage = 127;
        }
        if (NPC->flag & ground)
        {
            if (NPC->ani_no > 1)
            {
                for (int i = 0; i < 4; ++i)
                {
                    auto xVel = random(-0x600, 0);
                    auto yVel = random(-0x155, 0x155);
                    createNpc(NPC_Smoke, NPC->x, NPC->y, xVel, yVel);
                }
                playSound(SFX_LargeObjectHitGround);
                viewport.quake = 10;
            }
            NPC->act_no = 1;
            NPC->ani_no = 0;
            NPC->damage = 0;
            NPC->bits |= npc_solidHard;
        }

    default:
        break;
    }

    NPC->doGravity(0x20, 0x5FF);
    NPC->y += NPC->ym;

    NPC->doRects(rcNPC);
}

void npcAct117(npc *NPC)
{
	RECT rcLeft[10];
	RECT rcRight[10];

	rcLeft[0] = { 0x00, 0x60, 0x10, 0x70 };
	rcLeft[1] = { 0x10, 0x60, 0x20, 0x70 };
	rcLeft[2] = { 0x00, 0x60, 0x10, 0x70 };
	rcLeft[3] = { 0x20, 0x60, 0x30, 0x70 };
	rcLeft[4] = { 0x00, 0x60, 0x10, 0x70 };
	rcLeft[5] = { 0xB0, 0x60, 0xC0, 0x70 };
	rcLeft[6] = { 0x70, 0x60, 0x80, 0x70 };
	rcLeft[7] = { 0xA0, 0x60, 0xB0, 0x70 };
	rcLeft[8] = { 0x90, 0x60, 0xA0, 0x70 };
	rcLeft[9] = { 0x30, 0x60, 0x40, 0x70 };

	rcRight[0] = { 0x00, 0x70, 0x10, 0x80 };
	rcRight[1] = { 0x10, 0x70, 0x20, 0x80 };
	rcRight[2] = { 0x00, 0x70, 0x10, 0x80 };
	rcRight[3] = { 0x20, 0x70, 0x30, 0x80 };
	rcRight[4] = { 0x00, 0x70, 0x10, 0x80 };
	rcRight[5] = { 0xB0, 0x70, 0xC0, 0x80 };
	rcRight[6] = { 0x70, 0x70, 0x80, 0x80 };
	rcRight[7] = { 0xA0, 0x70, 0xB0, 0x80 };
	rcRight[8] = { 0x90, 0x70, 0xA0, 0x80 };
	rcRight[9] = { 0x30, 0x70, 0x40, 0x80 };

	switch (NPC->act_no)
	{
	case 0: //Stand
		//Look towards player if direction is 4
		if (NPC->direct == dirCenter)
		{
			if (NPC->x <= currentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}

		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
	// Fallthrough
	case 1:
		NPC->xm = 0;
		NPC->ym += 0x40;
		break;

	case 3: //Walking
		NPC->act_no = 4;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
	// Fallthrough
	case 4:
		//Animate
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		//Gravity
		NPC->ym += 0x40;

		//Walk in facing direction
		if (NPC->direct != dirLeft)
			NPC->xm = 0x200;
		else
			NPC->xm = -0x200;

		break;

	case 5: //Hit ground with poof
		NPC->act_no = 6;
		NPC->ani_no = 5;
		createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 8);
		break;

	case 6: //Lay on ground
		NPC->ani_no = 5;
		break;

	case 10: //Walk towards player
		NPC->act_no = 11;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;

		//Face towards player
		if (NPC->x <= currentPlayer.x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;
	// Fallthrough
	case 11:
		//Animate
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		//Move forward
		if (NPC->direct != dirLeft)
			NPC->x += 512;
		else
			NPC->x -= 512;

		//Stop when close to the player
		if (NPC->x + 0x2800 > currentPlayer.x && NPC->x - 0x2800 < currentPlayer.x)
			NPC->act_no = 0;

		break;

	case 20: //Looking away
		NPC->xm = 0;
		NPC->ani_no = 6;
		break;

	case 21: //Looking up
		NPC->xm = 0;
		NPC->ani_no = 9;
		break;

	case 30: //Falling over
		NPC->act_no = 31;
		NPC->act_wait = 0;
		NPC->ym = -1024;
	// Fallthrough
	case 31:
		NPC->ani_no = 7;

		//Move opposite to facing direction
		if (NPC->direct != dirLeft)
			NPC->xm = -0x200;
		else
			NPC->xm = 0x200;

		//Gravity
		NPC->ym += 0x40;

		//Check if Curly's on the ground
		if (++NPC->act_wait > 1 && NPC->flag & ground)
			NPC->act_no = 32;

		break;

	case 32: //Fallen
		NPC->ym += 0x40;
		NPC->ani_no = 8;
		NPC->xm = 0;
		break;

	case 70: //Walk backwards slowly (Seal Chamber)
		NPC->act_no = 71;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
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

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;
		break;

	default:
		break;
	}

	//Move and speed limit
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct116(npc *NPC) // Red flowers petals
{
	NPC->rect = { 272, 184, 320, 200 };
}

void npcAct119(npc *NPC) // Table & Chair
{
	NPC->rect = { 248, 184, 272, 200 };
}

