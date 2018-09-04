#include "npc240.h"

#include <vector>
#include "mathUtils.h"
#include "render.h"
#include "sound.h"

using std::vector;

void npcAct243(npc *NPC) // Generator - Red Bat
{
	if (NPC->act_no)
	{
		if (NPC->act_no != 1)
			return;
	}
	else
	{
		NPC->act_no = 1;
		NPC->act_wait = random(0, 500);
	}
	if (NPC->act_wait)
		--NPC->act_wait;
	else
	{
		NPC->act_no = 0;
		createNpc(NPC_EnemyBatRed, NPC->x, NPC->y + pixelsToUnits(random(-0x20, 0x20)), 0, 0, NPC->direct);
	}
}

void npcAct253(npc *NPC) // Energy Capsule
{
	if (!NPC->act_no)
		NPC->act_no = 1;
	if (NPC->act_no == 1)
		NPC->animate(4, 0, 1);

	if (NPC->life <= 100)
	{
		dropExperience(NPC->x, NPC->y, NPC->code_flag);
		createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 8);
		playSound(SFX_SillyExplosion);
		NPC->cond = 0;
	}

	NPC->doRects({ {0, 64, 16, 80}, {16, 64, 32, 80} });
}

void npcAct259(npc *NPC) // Sleeping mimiga
{
	NPC->rect = { 48, 32, 64, 48 };
}
