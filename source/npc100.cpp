#include "npc100.h"

#include <vector>
#include "player.h"
#include "sound.h"
#include "render.h"
#include "mathUtils.h"
#include "game.h"
#include "caret.h"
#include "level.h"

using std::vector;

void npcAct100(npc * NPC) // Grate
{
	if (NPC->act_no == 0)
	{
		NPC->act_no = 1;
		NPC->y += 0x2000;
	}

	NPC->doRects({ 272, 48, 288, 64 });
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
		if (NPC->direct)
			NPC->xm += 0x20;
		else
			NPC->xm -= 0x20;
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

	rcLeft = { { 0, 112, 32, 144 },{ 32, 112, 64, 144 },{ 64, 112, 96, 144 } };
	rcRight = { { 0, 144, 32, 176 },{ 32, 144, 64, 176 },{ 64, 144, 96, 176 } };

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

		NPC->moveInDir(pixelsToUnits(1));
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
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
            NPC->ani_no = 0;
        if (++NPC->act_wait > 100)
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
		if (NPC->act_wait / 2 & 1)
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

void npcAct109(npc *NPC) // Malco, damaged
{
    vector<RECT> rcLeft = {{240, 0, 256, 24}, {256, 0, 272, 24}};
    vector<RECT> rcRight = {{240, 24, 256, 48}, {256, 24, 272, 48}};

    enum
    {
        init = 0,
        stand = 1,
        blinking = 2,
        pulledOutOfGround = 10,
    };

    switch (NPC->act_no)
    {
    case init:
        if (--NPC->act_wait)
            NPC->act_no = stand;
        NPC->ani_no = 0;
        NPC->ani_wait = 0;
        // Fallthrough

    case stand:
        if (!random(0, 120))
        {
            NPC->act_no = blinking;
            NPC->act_wait = 0;
            NPC->ani_no = 0;
        }

        if (NPC->isPlayerWithinDistance(tilesToUnits(2), tilesToUnits(2), tilesToUnits(1)))
            NPC->facePlayer();

        break;

    case blinking:
        if (++NPC->act_wait > 8)
        {
            NPC->act_no = 1;
            NPC->ani_no = 0;
        }
        break;

    case pulledOutOfGround:
        NPC->act_no = 0;
        playSound(SFX_DestroyBreakableBlock);
        for (size_t i = 0; i < 8; ++i)
            createNpc(NPC_Smoke, NPC->x, NPC->y, random(-0x155, 0x155), random(pixelsToUnits(-3), 0));
        break;

    default:
        break;
    }

    NPC->doGravity(0x40, 0x5FF);
    NPC->y += NPC->ym;

    NPC->doRects(rcLeft, rcRight);
}

void npcAct110(npc *NPC)
{
    vector<RECT> rcLeft = {{96, 128, 112, 144}, {112, 128, 128, 144}, {128, 128, 144, 144}};
    vector<RECT> rcRight = {{96, 144, 112, 160}, {112, 144, 128, 160}, {128, 144, 144, 160}};

    enum
    {
        init = 0,
        stand = 1,
        mouthFlitter = 2,
        fallingFromCeiling = 3,
        jumping = 10,
    };

    switch (NPC->act_no)
    {
    case init:
        NPC->act_no = stand;
        NPC->act_wait = 0,
        NPC->xm = 0;
        NPC->ym = 0;
        if (NPC->direct == dirAuto) // Spawned by Balfrog
        {
            NPC->direct = random(0, 1) ? dirLeft : dirRight;
            NPC->bits |= npc_ignoreSolid;
            NPC->ani_no = 2;
            NPC->act_no = fallingFromCeiling;
            break;
        }
        NPC->bits &= ~npc_ignoreSolid;
        // Fallthrough
    case stand:
        if (!random(0, 50))
        {
            NPC->act_no = mouthFlitter;
            NPC->act_wait = 0;
            NPC->ani_no = 0;
            NPC->ani_wait = 0;
        }
        break;

    case mouthFlitter:
        NPC->animate(2, 0, 1);

        if (++NPC->act_wait > 18)
            NPC->act_no = stand;
        break;

    case fallingFromCeiling:
        if (++NPC->act_wait > 40)
            NPC->bits &= ~npc_ignoreSolid;

        if (NPC->flag & ground)
        {
            NPC->act_no = init;
            NPC->ani_no = 0;
            NPC->ani_wait = 0;
        }
        break;

    case jumping:
    case jumping + 1:
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

    if (NPC->act_no < jumping && NPC->act_no != fallingFromCeiling && NPC->act_wait > 10)
    {
        if (NPC->shock)
            doJump = true;
        else if (NPC->x >= currentPlayer.x - tilesToUnits(10)
                 && NPC->x <= currentPlayer.x + tilesToUnits(10)
                 && NPC->y >= currentPlayer.y - tilesToUnits(4)
                 && NPC->y <= currentPlayer.y + tilesToUnits(4)
                 && !random(0, 50))
            doJump = true;
    }

    if (doJump)
    {
        NPC->facePlayer();

        NPC->act_no = jumping;
        NPC->ani_no = 2;

        NPC->ym = -0x2FF;
        playSound(SFX_HighPitchCritterHop);
        NPC->moveInDir(pixelsToUnits(0.5));
    }

    NPC->doGravity(0x80, 0x5FF);

    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    NPC->doRects(rcLeft, rcRight);
}

void npcAct111(npc *NPC) //Quote teleport out
{
	vector<RECT> rcLeft(2);
	vector<RECT> rcRight(2);

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

	NPC->doRects(rcLeft, rcRight);

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
	vector<RECT> rcLeft(2);
	vector<RECT> rcRight(2);

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

	NPC->doRects(rcLeft, rcRight);

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

void npcAct113(npc *NPC) // Professor Booster
{
    vector<RECT> rcLeft(7);
    vector<RECT> rcRight(7);

    rcLeft[0] = {224, 0, 240, 16};
    rcLeft[1] = {240, 0, 256, 16};
    rcLeft[2] = {256, 0, 272, 16};
    rcLeft[3] = rcLeft[0];
    rcLeft[4] = {272, 0, 288, 16};
    rcLeft[5] = {224, 0, 240, 16};
    rcLeft[6] = {288, 0, 304, 16};

    rcRight[0] = {224, 16, 240, 32};
    rcRight[1] = {240, 16, 256, 32};
    rcRight[2] = {256, 16, 272, 32};
    rcRight[3] = rcRight[0];
    rcRight[4] = {272, 16, 288, 32};
    rcRight[5] = {224, 16, 240, 32};
    rcRight[6] = {288, 16, 304, 32};

    enum
    {
        init = 0,
        stand = 1,
        blinking = 2,
        startWalk = 3,
        walking = 4,
        faceAway = 5,
        shelterTeleportIn = 30,
        teleportingIn = 31,
        waitThenHopOut = 32,
        hoppingOut = 33,
    };

    switch (NPC->act_no)
    {
    case init:
        NPC->act_no = stand;
        NPC->ani_no = 0;
        NPC->ani_wait = 0;
        // Fallthrough
    case stand:
        if (!random(0, 120))
        {
            NPC->act_no = blinking;
            NPC->act_wait = 0;
            NPC->ani_no = 1;
        }
        break;

    case blinking:
        if (++NPC->act_wait > 8)
        {
            NPC->act_no = stand;
            NPC->ani_no = 0;
        }
        break;

    case startWalk:
        NPC->act_no = walking;
        NPC->ani_no = 2;
        NPC->ani_wait = 0;
        // Fallthrough
    case walking:
        NPC->animate(4, 2, 5);
        if (NPC->direct != dirLeft)
            NPC->x += pixelsToUnits(1);
        else
            NPC->x -= pixelsToUnits(1);
        break;

    case faceAway:
        NPC->ani_no = 6;
        break;

    case shelterTeleportIn:
        NPC->act_no = teleportingIn;
        NPC->ani_no = 0;
        NPC->ani_wait = 0;
        NPC->hit.bottom = tilesToUnits(1);
        NPC->x -= tilesToUnits(1);
        NPC->y += tilesToUnits(0.5);
        playSound(SFX_Teleport);
        // Fallthrough
    case teleportingIn:
        if (++NPC->act_wait == 0x40)
        {
            NPC->act_no = waitThenHopOut;
            NPC->act_wait = 0;
        }
        break;

    case waitThenHopOut:
        if (++NPC->act_wait > 20)
        {
            NPC->act_no = hoppingOut;
            NPC->ani_no = 1;
            NPC->hit.bottom = tilesToUnits(0.5);
        }
        break;

    case hoppingOut:
        if (NPC->flag & ground)
        {
            NPC->act_no = hoppingOut + 1;
            NPC->act_wait = 0;
            NPC->ani_no = 0;
        }
        break;

    default:
        break;
    }

    NPC->ym += 0x40;
    NPC->y += NPC->ym;

    NPC->doRects(rcLeft, rcRight);

    if (NPC->act_no == teleportingIn)
    {
        NPC->rect.bottom = NPC->rect.top + NPC->act_wait / 4;
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

void npcAct115(npc *NPC) // Ravil (enemy)
{
    vector<RECT> rcLeft(6);
    vector<RECT> rcRight(6);

    rcLeft[0] = {0, 120, 24, 144};
    rcLeft[1] = {24, 120, 48, 144};
    rcLeft[2] = {48, 120, 72, 144};
    rcLeft[3] = {72, 120, 96, 144};
    rcLeft[4] = {96, 120, 120, 144};
    rcLeft[5] = {120, 120, 144, 144};

    rcRight[0] = {0, 144, 24, 168};
    rcRight[1] = {24, 144, 48, 168};
    rcRight[2] = {48, 144, 72, 168};
    rcRight[3] = {72, 144, 96, 168};
    rcRight[4]= {96, 144, 120, 168};
    rcRight[5] = {120, 144, 144, 168};

    enum
    {
        init = 0,
        wait = 1,
        readyAttack = 10,
        hopHopLunge = 20,
        inAir = 21,
        jumpingFromFireplace = 30,
        killedAsMiniBoss = 50,
        deathJumping = 51,
    };

    switch (NPC->act_no)
    {
    case init:
        NPC->xm = 0;
        NPC->act_no = wait;
        NPC->act_wait = 0;
        NPC->count1 = 0;
        // Fallthrough
    case wait:
        if (NPC->isPlayerWithinDistance(tilesToUnits(6), tilesToUnits(6), tilesToUnits(2)) || NPC->shock)
            NPC->act_no = readyAttack;
        break;

    case readyAttack:
        NPC->facePlayer();
        NPC->ani_no = 1;

        if (++NPC->act_wait > 20)
        {
            NPC->act_wait = 0;
            NPC->act_no = hopHopLunge;
        }
        break;

    case hopHopLunge:
        NPC->damage = 0;
        NPC->xm = 0;

        NPC->animate(2);
        if (NPC->ani_no > 2)
        {
            NPC->facePlayer();
            NPC->moveInDir(pixelsToUnits(1));

            if (++NPC->count1 <= 2)
            {
                NPC->act_no = inAir;
                NPC->ym = pixelsToUnits(-2);
                playSound(SFX_CritterHop);
            }
            else
            {
                NPC->count1 = 0;
                NPC->ani_no = 4;
                NPC->act_no = inAir;
                NPC->ym = pixelsToUnits(-2);
                NPC->xm *= 2;
                NPC->damage = 5;
                playSound(SFX_SandCroc);
            }
        }
        break;

    case inAir:
        if (NPC->flag & ground)
        {
            playSound(SFX_HitGround);
            NPC->act_no = hopHopLunge;
            NPC->ani_no = 1;
            NPC->ani_wait = 0;
            NPC->damage = 0;

            if (currentPlayer.x > NPC->x + tilesToUnits(6)
                || currentPlayer.x < NPC->x - tilesToUnits(6)
                || currentPlayer.y > NPC->y + tilesToUnits(3)
                || currentPlayer.y < NPC->y - tilesToUnits(6))
                NPC->act_no = init;
        }
        break;

    case jumpingFromFireplace:
        for (size_t i = 0; i < 8; ++i)
        {
            auto xPos = NPC->x + pixelsToUnits(random(-12, 12));
            auto yPos = NPC->y + pixelsToUnits(random(-12, 12));
            auto xVel = random(-0x155, 0x155);
            auto yVel = random(pixelsToUnits(-3), 0);
            createNpc(NPC_Smoke, xPos, yPos, xVel, yVel);
        }
        NPC->ani_no = 0;
        NPC->act_no = init;
        break;

    case killedAsMiniBoss:
        NPC->act_no = deathJumping;
        NPC->ani_no = 4;
        NPC->damage = 0;
        NPC->ym = pixelsToUnits(-1);
        NPC->bits &= ~(npc_shootable | npc_solidSoft);
        playSound(SFX_CritterSmallHurt);
        // Fallthrough
    case deathJumping:
        if (NPC->flag & ground)
        {
            playSound(SFX_HitGround);
            NPC->act_no = deathJumping + 1;
            NPC->ani_no = 5;
            NPC->xm = 0;
        }
        break;

    default:
        break;
    }

    if (NPC->act_no <= killedAsMiniBoss)
        NPC->ym += 0x40;
    else
        NPC->ym += 0x20;
    NPC->doGravity(0, 0x5FF);
    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    NPC->doRects(rcLeft, rcRight);
}

void npcAct116(npc *NPC) // Red flowers petals
{
	NPC->doRects({ 272, 184, 320, 200 });
}

void npcAct117(npc *NPC)
{
	vector<RECT> rcLeft(10);
	vector<RECT> rcRight(10);

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
		if (NPC->direct == dirAuto)
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

	NPC->doRects(rcLeft, rcRight);
}

void npcAct119(npc *NPC) // Table & Chair
{
	NPC->doRects({ 248, 184, 272, 200 });
}

