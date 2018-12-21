#include "npc340.h"

#include <array>
#include "player.h"
#include "mathUtils.h"
#include "sound.h"
#include "render.h"
#include "stage.h"
#include "game.h"

using std::array;

void npcAct341(npc *NPC)
{
	constexpr array<RECT, 3> rcNPC = {{{288, 32, 320, 48}, {288, 48, 320, 64}, {288, 64, 320, 80}}};

    if (NPC->pNpc)
    {
        if (NPC->pNpc->act_no == 11 && NPC->pNpc->act_wait > 50)
            NPC->animate(4, 2, 2);

        if (NPC->pNpc->ani_no != 0)
            NPC->cond = 0;
    }

    NPC->doRects(rcNPC);
}

void npcAct344(npc *NPC)
{
    NPC->doRects({272, 0, 296, 16}, {296, 0, 320, 16});

    if (NPC->pNpc)
    {
        if (NPC->direct != dirLeft)
            NPC->x = NPC->pNpc->x + tilesToUnits(1.5);
        else
            NPC->x = NPC->pNpc->x - tilesToUnits(1.5);
    }

    if (++NPC->act_wait > secondsToFrames(2))
        NPC->cond = 0;

    if (NPC->pNpc)
        NPC->y = NPC->pNpc->y - tilesToUnits(2.25);
}

void npcAct347(npc *NPC) // Hoppy (enemy)
{
    enum
    {
        init = 0,
        waitForPlayer = 1,
        startJump = 10,
        jumping = 12,
        land = 13,
    };

    switch (NPC->act_no)
    {
    case init:
        NPC->act_no = waitForPlayer;
        // Fallthrough
    case waitForPlayer:
        NPC->ani_no = 0;
        if (currentPlayer.y < NPC->y + tilesToUnits(8) && currentPlayer.y > NPC->y - tilesToUnits(8))
        {
            NPC->act_no = startJump;
            NPC->act_wait = 0;
            NPC->ani_no = 1;
        }
        break;

    case startJump:
        if (++NPC->act_wait == 4)
            NPC->ani_no = 2;
        if (NPC->act_wait > 12)
        {
            NPC->act_no = jumping;
            NPC->xm = 0x700;
            playSound(SFX_HighPitchCritterHop);
            NPC->ani_no = 3;
        }
        break;

    case jumping:
        if (currentPlayer.y >= NPC->y)
            NPC->ym = 0xAA;
        else
            NPC->ym = -0xAA;

        if (NPC->flag & leftWall)
        {
            NPC->act_no = land;
            NPC->act_wait = 0;
            NPC->ani_no = 2;
            NPC->xm = 0;
            NPC->ym = 0;
        }
        else
        {
            NPC->xm -= 42;
            if (NPC->xm < -0x5FF)
                NPC->xm = -0x5FF;

            NPC->x += NPC->xm;
            NPC->y += NPC->ym;
        }
        break;

    case land:
        if (++NPC->act_wait == 2)
            NPC->ani_no = 1;
        else if (NPC->act_wait == 6)
            NPC->ani_no = 0;
        else if (NPC->act_wait > 16)
            NPC->act_no = waitForPlayer;
        break;

    default:
        break;
    }

	constexpr array<RECT, 4> rcNPC = { {{256, 48, 272, 64}, {272, 48, 288, 64}, {288, 48, 304, 64}, {304, 48, 320, 64}} };

    NPC->doRects(rcNPC);
}

void npcAct348(npc *NPC)
{
    if (!NPC->act_no)
        NPC->act_no = 1;

    if (NPC->act_no == 1)
    {
        if (++NPC->act_wait >= 0x80)
        {
            NPC->act_no = 10;
            NPC->ani_no = 0;
            NPC->damage = 2;
        }
        else
        {
            NPC->y -= pixelsToUnits(0.25);

            if (NPC->act_wait / 2 % 2)
                NPC->ani_no = 1;
            else
                NPC->ani_no = 0;
        }
    }

    constexpr array<RECT, 2> rcNPC = {{{128, 152, 160, 176}, {160, 152, 192, 176}}};

    NPC->doRects(rcNPC);
}

void npcAct349(npc *NPC)
{
	if (NPC->act_no == 0)
	{
		NPC->act_no = 1;
		if (NPC->direct == dirLeft)
			NPC->x += pixelsToUnits(8);
		else if (NPC->direct == dirRight)
			NPC->y += tilesToUnits(1);
	}

	NPC->doRects({ 0, 0, 16, 16 });
}

void npcAct354(npc *NPC) // Invisible deathtrap wall
{
    switch (NPC->act_no)
    {
    case 0:
        NPC->hit.bottom = tilesToUnits(17.5);
        return;

    case 10:
        NPC->act_no = 11;
        NPC->act_wait = 0;
        if (NPC->direct != dirLeft)
            NPC->x -= tilesToUnits(1);
        else
            NPC->x += tilesToUnits(1);
        // Fallthrough
    case 11:
        if (++NPC->act_wait > 100)
        {
            NPC->act_wait = 0;
            viewport.quake = 20;
            playSound(SFX_LargeObjectHitGround);
            playSound(SFX_DestroyBreakableBlock);
            if (NPC->direct != dirLeft)
                NPC->x += tilesToUnits(1);
            else
                NPC->x -= tilesToUnits(1);
            for (size_t i = 0; i < 20; ++i)
                changeTile(unitsToTiles(NPC->x), i + unitsToTiles(NPC->y), 109);
        }
    }
}

void npcAct355(npc *NPC) // Balrog, crashing through wall
{
    constexpr array<RECT, 4> rcNPC = {{{80, 16, 96, 32}, {80, 96, 96, 112}, {128, 16, 144, 32}, {208, 96, 224, 112}}};

    if (!NPC->act_no && NPC->pNpc)
    {
        switch (NPC->direct)
        {
        case dirLeft:
            NPC->surf = 16;
            NPC->ani_no = 0;
            NPC->x = NPC->pNpc->x - pixelsToUnits(14);
            NPC->y = NPC->pNpc->y + pixelsToUnits(10);
            break;

        case dirUp:
            NPC->surf = 23;
            NPC->ani_no = 1;
            NPC->x = NPC->pNpc->x + pixelsToUnits(14);
            NPC->y = NPC->pNpc->y + pixelsToUnits(10);
            break;

        case dirRight:
            NPC->surf = 16;
            NPC->ani_no = 2;
            NPC->x = NPC->pNpc->x - pixelsToUnits(7);
            NPC->y = NPC->pNpc->y - pixelsToUnits(19);
            break;

        case dirDown:
            NPC->surf = 23;
            NPC->ani_no = 3;
            NPC->x = NPC->pNpc->x + pixelsToUnits(4);
            NPC->y = NPC->pNpc->y - pixelsToUnits(19);
        }
    }
    else
        NPC->ani_no = 0;

    NPC->doRects(rcNPC);
}

void npcAct359(npc *NPC)
{
	if (currentPlayer.x < NPC->x + tilesToUnits(20) && currentPlayer.x > NPC->x - tilesToUnits(20)
     && currentPlayer.y < NPC->y + tilesToUnits(20) && currentPlayer.y > NPC->y - tilesToUnits(20) && random(0, 100) == 2)
		createNpc(NPC_Waterdrop, (random(-6, 6) << 9) + NPC->x, NPC->y - pixelsToUnits(7));
}

