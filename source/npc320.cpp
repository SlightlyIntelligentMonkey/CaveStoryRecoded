#include "npc320.h"

#include <array>
#include "player.h"
#include "stage.h"
#include "render.h"
#include "game.h"
#include "sound.h"
#include "mathUtils.h"

using std::array;

void npcAct322(npc *NPC) // Deleet (enemy)
{
    constexpr array<RECT, 2> rcNormal = {{{272, 216, 296, 240}, {296, 216, 320, 240}}};
    constexpr RECT rcExploding = {160, 216, 184, 240};

    if (NPC->act_no < 2 && NPC->life <= 968)
    {
        NPC->act_no = 2;
        NPC->act_wait = 0;
        NPC->bits &= ~npc_shootable;
        NPC->bits |= npc_invulnerable;
    }

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        if (NPC->direct != dirLeft)
            NPC->x += tilesToUnits(0.5);
        else
            NPC->y += tilesToUnits(0.5);
        // Fallthrough
    case 1:
        if (NPC->shock)
            ++NPC->count1;
        else
            NPC->count1 = 0;

        NPC->rect = rcNormal.at(NPC->count1 / 2 & 1);
        return;

    case 2:
        NPC->ani_no = 2;
        switch (NPC->act_wait)
        {
        case secondsToFrames(0):
            createNpc(NPC_BalloonCountdown, NPC->x + pixelsToUnits(4), NPC->y, 0, 0, 0);
            break;

        case secondsToFrames(1):
            createNpc(NPC_BalloonCountdown, NPC->x + pixelsToUnits(4), NPC->y, 0, 0, 1);
            break;

        case secondsToFrames(2):
            createNpc(NPC_BalloonCountdown, NPC->x + pixelsToUnits(4), NPC->y, 0, 0, 2);
            break;

        case secondsToFrames(3):
            createNpc(NPC_BalloonCountdown, NPC->x + pixelsToUnits(4), NPC->y, 0, 0, 3);
            break;

        case secondsToFrames(4):
            createNpc(NPC_BalloonCountdown, NPC->x + pixelsToUnits(4), NPC->y, 0, 0, 4);
            break;

        case secondsToFrames(5):
            NPC->hit = {tilesToUnits(3), tilesToUnits(3), tilesToUnits(3), tilesToUnits(3)};
            NPC->damage = 12;
            playSound(SFX_LargeObjectHitGround);
            createSmokeLeft(NPC->x, NPC->y, tilesToUnits(3), 40);
            viewport.quake = 10;
            if (NPC->direct != dirLeft)
            {
                deleteTile(unitsToTiles(NPC->x - tilesToUnits(0.5)), unitsToTiles(NPC->y));
                deleteTile(unitsToTiles(NPC->x + tilesToUnits(0.5)), unitsToTiles(NPC->y));
            }
            else
            {
                deleteTile(unitsToTiles(NPC->x), unitsToTiles(NPC->y - tilesToUnits(0.5)));
                deleteTile(unitsToTiles(NPC->x), unitsToTiles(NPC->y + tilesToUnits(0.5)));
            }
            NPC->cond |= 8;
            break;

        default:
            break;
        }
        ++NPC->act_wait;
        NPC->rect = rcExploding;
    }
}

void npcAct323(npc *NPC)
{
	constexpr array<RECT, 4> rcNPC = { {{216, 32, 232, 56}, {232, 32, 248, 56}, {216, 56, 232, 80}, {232, 56, 248, 80}} };

	NPC->animate(3, 0, 3);

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;

		switch (NPC->direct)
		{
		case dirLeft:
			NPC->xm = pixelsToUnits(-3);
			break;

		case dirRight:
			NPC->xm = pixelsToUnits(3);
			break;

		case dirUp:
			NPC->ym = pixelsToUnits(-3);
			break;

		case dirDown:
			NPC->ym = pixelsToUnits(3);
			break;

		default:
			break;
		}
		break;

	case 1:
		if (++NPC->act_wait == 16)
			NPC->bits &= ~npc_ignoreSolid;

		NPC->x += NPC->xm;
		NPC->y += NPC->ym;

		if (NPC->flag & 0xFF)
			NPC->act_no = 10;

		if (NPC->act_wait > 20)
		{
			switch (NPC->direct)
			{
			case dirLeft:
				if (NPC->x <= currentPlayer.x + tilesToUnits(2))
					NPC->act_no = 10;
				break;

			case dirRight:
				if (NPC->x >= currentPlayer.x - tilesToUnits(2))
					NPC->act_no = 10;
				break;

			case dirUp:
				if (NPC->y <= currentPlayer.y + tilesToUnits(2))
					NPC->act_no = 10;
				break;

			case dirDown:
				if (NPC->y >= currentPlayer.y - tilesToUnits(2))
					NPC->act_no = 10;
				break;

			default:
				break;
			}
		}
		break;

	case 10:
		NPC->code_char = NPC_EnemyBute;
		NPC->ani_no = 0;
		NPC->act_no = 11;
		NPC->bits |= npc_shootable;
		NPC->bits &= ~npc_ignoreSolid;
		NPC->damage = 5;
		NPC->view.top = pixelsToUnits(8);
		break;

	default:
		break;
	}

	NPC->doRects(rcNPC);
}

void npcAct324(npc *NPC)
{
	if (NPC->act_no == 10)
	{
		NPC->act_no = 11;
		NPC->act_wait = 0;
	}
	else if (NPC->act_no != 11)
		return;

	if (++NPC->act_wait % 50 == 1)
		createNpc(NPC_EnemyButeGenerator, NPC->x, NPC->y, 0, 0, NPC->direct);
	if (NPC->act_wait > 351)
		NPC->act_no = 0;
}

void npcAct328(npc *NPC) // Transmogrifier
{
	NPC->doRects({96, 0, 128, 48});
}

void npcAct329(npc *NPC) // Building fan
{
    if (++NPC->act_wait / 2 % 2)
        NPC->rect = {48, 0, 64, 16};
    else
        NPC->rect = {64, 0, 80, 16};
}

void npcAct334(npc *NPC) //sweat
{
	constexpr std::array<RECT, 2> rcRight =
	{{
		{176, 184, 184, 200},
		{184, 184, 192, 200}
	}};
	constexpr std::array<RECT, 2> rcLeft =
	{{
		{160, 184, 168, 200},
		{168, 184, 176, 200}
	}};

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
	NPC->doRects(rcLeft, rcRight);

	return;
}

void npcAct336(npc *NPC)    // Generator - Ikachan
{
    if (NPC->act_no == 0)
    {
        if (currentPlayer.shock)
            NPC->cond = 0;
    }
    else if (NPC->act_no == 10 && ++NPC->act_wait % 4 == 1)
        createNpc(NPC_Ikachan, NPC->x, NPC->y + tilesToUnits(random(0, 13)));
}

void npcAct339(npc *NPC) // Generator - Green Devil
{
    if (!NPC->act_no)
    {
        NPC->act_no = 1;
        NPC->act_wait = random(0, 40);
    }
    else if (NPC->act_no != 1)
        return;

    if (NPC->act_wait)
        --NPC->act_wait;
    else
    {
        NPC->act_no = 0;
        createNpc(NPC_EnemyGreenDevil, NPC->x, NPC->y + pixelsToUnits(random(-16, 16)), 0, 0, NPC->direct);
    }
}
