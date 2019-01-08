#include "npc360.h"

#include "stage.h"

void npcAct360(npc *NPC)    // Text, 'Thank You!'
{
    if (!NPC->act_no)
    {
        NPC->act_no = 1;
        NPC->x -= pixelsToUnits(8);
        NPC->y -= pixelsToUnits(8);
    }

    NPC->doRects({0, 176, 48, 184});
}
