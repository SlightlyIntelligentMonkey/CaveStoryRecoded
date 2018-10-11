#include "npc340.h"

#include <array>
#include "player.h"
#include "mathUtils.h"
#include "sound.h"
#include "render.h"
#include "level.h"

using std::array;

void npcAct334(npc *NPC) //sweat
{
	RECT rcRight[] =
	{
		{176, 184, 184, 200},
		{184, 184, 192, 200}
	};
	RECT rcLeft[] =
	{
		{160, 184, 168, 200},
		{168, 184, 176, 200}
	};

	if (NPC->act_no != 0)
	{
		if (NPC->act_no != 10)
			goto LABEL_12;
	}
	else
	{
		NPC->act_no = 10;
		if (NPC->direct)
		{
			NPC->x = currentPlayer.x - 5120;
			NPC->y = currentPlayer.y - 1024;
		}
		else
		{
			NPC->x += 5120;
			NPC->y -= 9216;
		}
	}
	if (++NPC->act_wait / 8 & 1)
		NPC->ani_no = 0;
	else
		NPC->ani_no = 1;
	if (NPC->act_wait > 63)
		NPC->cond = 0;
LABEL_12:
	if (NPC->direct)
	{
		NPC->rect = rcRight[NPC->ani_no];
	}
	else
	{
		NPC->rect = rcLeft[NPC->ani_no];
	}

	return;
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

	array<RECT, 4> rcNPC = { {{256, 48, 272, 64}, {272, 48, 288, 64}, {288, 48, 304, 64}, {304, 48, 320, 64}} };

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

void npcAct359(npc *NPC)
{
	if (currentPlayer.x < NPC->x + tilesToUnits(20) && currentPlayer.x > NPC->x - tilesToUnits(20)
     && currentPlayer.y < NPC->y + tilesToUnits(20) && currentPlayer.y > NPC->y - tilesToUnits(20) && random(0, 100) == 2)
		createNpc(NPC_Waterdrop, (random(-6, 6) << 9) + NPC->x, NPC->y - pixelsToUnits(7));
}

