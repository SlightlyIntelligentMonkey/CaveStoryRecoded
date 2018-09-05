#include "npc240.h"

#include <vector>
#include "render.h"
#include "player.h"
#include "level.h"
#include "caret.h"
#include "sound.h"

using std::vector;

void npcAct244(npc *NPC)
{
	NPC->ym += 0x40;
	bool die = false;
	if (NPC->flag & 0xFF || (NPC->act_wait > 10 && NPC->flag & 0x100))
		die = true;

	if (die)
	{
		for (int i = 0; i < 3; ++i)
			createCaret(NPC->x, NPC->y + pixelsToUnits(4), effect_fountainDisk, dirRight);
		if (NPC->x > currentPlayer.x - tilesToUnits(16)
		        && NPC->x < currentPlayer.x + tilesToUnits(16)
		        && NPC->y > currentPlayer.y - tilesToUnits(10)
		        && NPC->y < currentPlayer.y + tilesToUnits(10))
			playSound(SFX_Bubble);
        NPC->cond = 0;
	}
	else
    {
        if (NPC->ym > 0x5FF)
            NPC->ym = 0x5FF;
        NPC->y += NPC->ym;

        NPC->rect = {96, 0, 104, 16};
    }
}

void npcAct245(npc *NPC) // Generator - Lava Drop
{
	vector<RECT> rcNPC = {{0, 0, 0, 0}, {104, 0, 112, 16}, {112, 0, 120, 16}, {120, 0, 128, 16}};

	if (NPC->act_no)
	{
		if (NPC->act_no != 1)
		{
			if (NPC->act_no == 10)
			{
				if (++NPC->ani_wait > 10)
				{
					NPC->ani_wait = 0;
                    ++NPC->ani_no;
				}
				if (NPC->ani_no > 3)
				{
					NPC->ani_no = 0;
					NPC->act_no = 1;
                    NPC->act_wait = NPC->code_flag;
                    createNpc(NPC_AcidDrop, NPC->x, NPC->y);
				}
			}
			goto endOfAI;
		}
	}
	else
    {
        NPC->act_no = 1;
        NPC->tgt_x = NPC->x;
        NPC->act_wait = NPC->code_event;
    }

    NPC->ani_no = 0;
    if (NPC->act_wait)
    {
        --NPC->act_wait;
        return;
    }
    NPC->act_no = 10;
    NPC->act_wait = 0;

endOfAI:
    if (NPC->act_wait / 2 % 2)
        NPC->x = NPC->tgt_x;
    else
        NPC->x = NPC->tgt_x + 0x200;

    NPC->doRects(rcNPC);
}

void npcAct259(npc *NPC) // Sleeping mimiga
{
	NPC->rect = { 48, 32, 64, 48 };
}
