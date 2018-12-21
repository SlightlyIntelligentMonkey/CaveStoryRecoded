#include "npc280.h"

#include <array>
#include <algorithm>
#include "mathUtils.h"
#include "stage.h"
#include "game.h"
#include "render.h"

using std::array;
using std::max;

void npcAct291(npc *NPC) // Mini Undead Core, stationary
{
    if (!NPC->act_no)
    {
        NPC->act_no = 20;
        if (NPC->direct == dirRight)
        {
            NPC->bits &= ~npc_solidHard;
            NPC->ani_no = 1;
        }
    }

    constexpr std::array<RECT, 2> rcNPC = {{{256, 80, 320, 120}, {256, 0, 320, 40}}};
    NPC->doRects(rcNPC);
}

void npcAct292(npc * /*NPC*/) //Quake
{
	viewport.quake = 10;
}

void npcAct293(npc *NPC) // Undead Core large energy ball (projectile)
{
    constexpr array<RECT, 2> rcNPC = {{{240, 200, 280, 240}, {280, 200, 320, 240}}};

    if (!NPC->act_no)
        NPC->act_no = 1;

    if (NPC->act_no == 1)
    {
        NPC->animate(0, 0, 1);

        createNpc(NPC_Smoke, NPC->x + pixelsToUnits(random(0, 16)), NPC->y + pixelsToUnits(random(-16, 16)));

        NPC->x -= tilesToUnits(0.5);

        if (NPC->x < tilesToUnits(-2))
            NPC->cond = 0;
    }

    NPC->doRects(rcNPC);
}

void npcAct295(npc *NPC) // Cloud
{
    constexpr array<RECT, 4> rcNPC = {{{0, 0, 208, 64}, {32, 64, 144, 96}, {32, 96, 104, 128}, {104, 96, 144, 128}}};

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->ani_no = NPC->direct % 4;
        switch (NPC->direct)
        {
        case 0:
            NPC->ym = tilesToUnits(-0.5);
            NPC->view.right = tilesToUnits(6.5);
            NPC->view.left = tilesToUnits(6.5);
            break;

        case 1:
            NPC->ym = pixelsToUnits(-4);
            NPC->view.right = tilesToUnits(3.5);
            NPC->view.left = tilesToUnits(3.5);
            break;

        case 2:
            NPC->ym = pixelsToUnits(-2);
            NPC->view.right = tilesToUnits(2);
            NPC->view.left = tilesToUnits(2);
            break;

        case 3:
            NPC->ym = pixelsToUnits(-1);
            NPC->view.right = tilesToUnits(1.25);
            NPC->view.left = tilesToUnits(1.25);
            break;

        case 4:
            NPC->xm = pixelsToUnits(-2);
            NPC->view.right = tilesToUnits(6.5);
            NPC->view.left = tilesToUnits(6.5);
            break;

        case 5:
            NPC->xm = pixelsToUnits(-1);
            NPC->view.right = tilesToUnits(3.5);
            NPC->view.left = tilesToUnits(3.5);
            break;

        case 6:
            NPC->xm = pixelsToUnits(-0.5);
            NPC->view.right = tilesToUnits(2);
            NPC->view.left = tilesToUnits(2);
            break;

        case 7:
            NPC->xm = pixelsToUnits(-0.25);
            NPC->view.right = tilesToUnits(1.25);
            NPC->view.left = tilesToUnits(1.25);
            break;
        }
        break;

    case 1:
        NPC->x += NPC->xm;
        NPC->y += NPC->ym;
        if (NPC->x < tilesToUnits(-4))
            NPC->cond = 0;
        if (NPC->y < tilesToUnits(-2))
            NPC->cond = 0;
    }

    NPC->doRects(rcNPC);
}

void npcAct296(npc *NPC) // Generator - Cloud
{
    if (++NPC->act_wait > 16)
    {
        NPC->act_wait = random(0, 16);
        int randDir = random(0, 100) % 4;
        if (NPC->direct != dirLeft)
            createNpc(NPC_Cloud, NPC->x, NPC->y + tilesToUnits(random(-7, 7)), 0, 0, randDir + 4, nullptr);
        else
            createNpc(NPC_Cloud, NPC->x + tilesToUnits(random(-7, 7)), NPC->y, 0, 0, randDir, nullptr);
    }
}

void npcAct297(npc *NPC) //Sue on sky dragon
{
	NPC->x = NPC->pNpc->x + 0x2000;
	NPC->y = NPC->pNpc->y + 0x1000;
	NPC->doRects({112, 48, 128, 64});
	NPC->rect.left = 112;
	NPC->rect.top = 48;
	NPC->rect.right = 128;
	NPC->rect.bottom = 64;
}

void npcAct298(npc *NPC) //Doctor without crown (intro)
{
	array<RECT, 8> rc;

	rc[0] = { 0x48, 0x80, 0x58, 0xA0 };
	rc[1] = { 0x58, 0x80, 0x68, 0xA0 };
	rc[2] = { 0x68, 0x80, 0x78, 0xA0 };
	rc[3] = { 0x48, 0x80, 0x58, 0xA0 };
	rc[4] = { 0x78, 0x80, 0x88, 0xA0 };
	rc[5] = { 0x48, 0x80, 0x58, 0xA0 };
	rc[6] = { 0x68, 0xA0, 0x78, 0xC0 };
	rc[7] = { 0x78, 0xA0, 0x88, 0xC0 };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->y -= 0x1000;
	}

	switch (NPC->act_no)
	{
	case 1:
		NPC->ani_no = 0;
		break;

	case 10:
		NPC->act_no = 11;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->count1 = 0;
	// Fallthrough
	case 11:
		if (++NPC->ani_wait > 6)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 1)
		{
			NPC->ani_no = 0;

			if (++NPC->count1 > 7)
			{
				NPC->ani_no = 0;
				NPC->act_no = 1;
			}
		}
		break;

	case 20:
		NPC->act_no = 21;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
	// Fallthrough
	case 21:
		if (++NPC->ani_wait > 10)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 5)
			NPC->ani_no = 2;

		NPC->x += 0x100;
		break;

	case 30:
		NPC->ani_no = 6;
		break;

	case 40:
		NPC->act_no = 41;
		NPC->ani_no = 6;
		NPC->ani_wait = 0;
		NPC->count1 = 0;
	// Fallthrough
	case 41:
		if (++NPC->ani_wait > 6)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 7)
		{
			NPC->ani_no = 6;

			if (++NPC->count1 > 7)
			{
				NPC->ani_no = 6;
				NPC->act_no = 30;
			}
		}
		break;

	default:
		break;
	}

	NPC->doRects(rc);
}

void npcAct299(npc *NPC) //Balrog / Misery in bubble (intro)
{
	array<RECT, 2> rc; // [esp+0h] [ebp-20h]

	rc[0] = { 0, 0, 48, 48 };
	rc[1] = { 48, 0, 96, 48 };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		if (NPC->direct != dirLeft)
		{
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}
		else
		{
			NPC->ani_no = 1;
			NPC->act_wait = 25;
			NPC->y -= 0x640;
		}
	}

	if (++NPC->act_wait / 50 & 1)
		NPC->y += 64;
	else
		NPC->y -= 64;

	NPC->doRects(rc);
}
