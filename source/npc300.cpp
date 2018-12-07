#include "npc300.h"

#include <array>
#include "player.h"
#include "mathUtils.h"
#include "caret.h"
#include "render.h"
#include "stage.h"

using std::array;

void npcAct300(npc *NPC) //Demon crown
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->y += 0xC00;
	}

	if (++NPC->ani_wait % 8 == 1)
		createCaret(NPC->x + (random(-8, 8) << 9), NPC->y + 4096, effect_HeadbumpSparks, dirUp);

	NPC->rect = { 192, 80, 208, 96 };
}

void npcAct302(npc *NPC) //Camera Helper NPC
{
	if (NPC->act_no != 20)
	{
		if (NPC->act_no <= 20)
		{
			if (NPC->act_no == 10)
			{
				NPC->x = currentPlayer.x;
				NPC->y = currentPlayer.y - 0x4000;
			}

			return;
		}

		if (NPC->act_no == 100)
		{
			NPC->act_no = 101;

			if (NPC->direct != dirLeft)
			{
				int n;

				for (n = 170; n < 512; ++n)
				{
					if ((npcs[n].cond & npccond_alive) != 0 && npcs[n].code_event == NPC->direct)
					{
						NPC->pNpc = &npcs[n];
						break;
					}
				}

				if (n == 512)
				{
					NPC->cond = 0;
					return;
				}
			}
			else
			{
				//NPC->pNpc = gBoss;
			}
		}
		else if (NPC->act_no != 101)
		{
			if (NPC->act_no == 30)
			{
				NPC->x = currentPlayer.x;
				NPC->y = currentPlayer.y + 0xA000;
			}

			return;
		}

		NPC->x = (currentPlayer.x + NPC->pNpc->x) / 2;
		NPC->y = (currentPlayer.y + NPC->pNpc->y) / 2;
		return;
	}

	if (NPC->direct == dirUp)
		NPC->y -= 0x400;
	else if (NPC->direct > dirUp)
	{
		if (NPC->direct == dirRight)
			NPC->x += 0x400;
		else if (NPC->direct == dirDown)
			NPC->y += 0x400;
	}
	else if (NPC->direct == dirLeft)
		NPC->x -= 1024;

	currentPlayer.x = NPC->x;
	currentPlayer.y = NPC->y;
}

void npcAct304(npc *NPC)    // Gaudi (sitting)
{
    constexpr array<RECT, 4> rcNPC =
    {{
        {0, 176, 24, 192},
        {24, 176, 48, 192},
        {48, 176, 72, 192},
        {72, 176, 96, 192},
    }};

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->y += pixelsToUnits(10);
        // Fallthrough
    case 1:
        NPC->ani_no = 0;
        break;

    case 10:
        NPC->ani_no = 1;
        break;

    case 20:
        NPC->act_no = 21;
        NPC->ani_no = 2;
        // Fallthrough
    case 21:
        NPC->animate(10, 2, 3);
    }

    NPC->doRects(rcNPC);
}

void npcAct305(npc *NPC)
{
    constexpr array<RECT, 2> rcLeft = {{{160, 144, 176, 160}, {176, 144, 192, 160}}};
    constexpr array<RECT, 2> rcRight = {{{160, 160, 176, 176}, {176, 160, 192, 176}}};

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->y -= tilesToUnits(1);
        NPC->ani_wait = random(0, 6);
        // Fallthrough
    case 1:
        NPC->animate(6, 0, 1);
    }

    NPC->doRects(rcLeft, rcRight);
}

void npcAct306(npc *NPC) // Balrog, Nurse
{
    constexpr array<RECT, 2> rcLeft = {{{240, 96, 280, 128}, {280, 96, 320, 128}}};
    constexpr array<RECT, 2> rcRight = {{{160, 152, 200, 184}, {200, 152, 240, 184}}};

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->ani_no = 0;
        NPC->ani_wait = 0;
        NPC->y += pixelsToUnits(4);
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
    }

    NPC->doRects(rcLeft, rcRight);
}

void npcAct309(npc *NPC)
{
    constexpr array<RECT, 2> rcLeft = {{{0, 0, 16, 16}, {16, 0, 32, 16}}};
    constexpr array<RECT, 2> rcRight = {{{0, 16, 16, 32}, {16, 16, 32, 32}}};

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        // Fallthrough
    case 1:
        if ((NPC->direct != dirLeft &&
             (currentPlayer.x >= NPC->x + tilesToUnits(18) || currentPlayer.x <= NPC->x + tilesToPixels(17)))
            || (NPC->direct == dirLeft &&
                (currentPlayer.x <= NPC->x - tilesToPixels(18) || currentPlayer.x >= NPC->x - tilesToPixels(17))))
            return;
        NPC->act_no = 10;
        break;

    case 10:
        NPC->act_no = 11;
        NPC->bits |= npc_shootable;
        NPC->damage = 5;
        // Fallthrough
    case 11:
        NPC->facePlayer();

        if (NPC->direct != dirLeft)
            NPC->xm2 += 0x10;
        else
            NPC->xm2 -= 0x10;

        if (NPC->y <= currentPlayer.y)
            NPC->ym2 += 0x10;
        else
            NPC->ym2 -= 0x10;

        if ((NPC->xm2 < 0 && NPC->flag & leftWall) || (NPC->xm2 > 0 && NPC->flag & rightWall))
            NPC->xm2 = -NPC->xm2;

        if ((NPC->ym2 < 0 && NPC->flag & ceiling) || (NPC->ym2 > 0 && NPC->flag & ground))
            NPC->ym2 = -NPC->ym2;

        NPC->limitXVel2(0x5FF);
        NPC->limitYVel2(0x5FF);

        NPC->x += NPC->xm2;
        NPC->y += NPC->ym2;

        NPC->animate(1, 0, 1);
        break;
    }

    NPC->doRects(rcLeft, rcRight);

    if (NPC->life <= 996)
    {
        NPC->code_char = NPC_EnemyButeDefeated;
        NPC->act_no = 0;
    }
}
