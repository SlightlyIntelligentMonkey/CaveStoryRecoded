#include "boss.h"
#include "valueview.h"
#include "npc.h"
#include "game.h"
#include "render.h"

npc bossObj[BOSSNPCS];

using bossAct = void(*)(npc *);
const bossAct bossActs[10] =
{
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	&actBoss_Frog,
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
};

void initBoss(int code)
{
	memset(bossObj, 0, sizeof(bossObj));
	bossObj[0].cond = npccond_alive;
	bossObj[0].code_char = code;
}

void setBossAction(int a)
{
	bossObj[0].act_no = a;
}

void updateBoss()
{
	if ((bossObj[0].cond & npccond_alive) && bossActs[bossObj[0].code_char] != nullptr)
	{
		bossActs[bossObj[0].code_char](&bossObj[0]);

		for (size_t bos = 0; bos < BOSSNPCS; ++bos)
		{
			if (bossObj[bos].shock)
				--bossObj[bos].shock;
		}
	}
}

void drawBoss()
{
	for (size_t bos = BOSSNPCS - 1; bos > 0; bos--)
	{
		if (bossObj[bos].cond & npccond_alive)
		{
			int shake = 0;

			if (bossObj[bos].shock)
			{
				shake = 2 * ((bossObj[bos].shock >> 1) & 1) - 1;
			}
			else
			{
				if (bossObj[bos].bits & npc_showDamage && bossObj[bos].damage_view)
				{
					createValueView(&bossObj[bos].x, &bossObj[bos].y, bossObj[bos].damage_view);
					bossObj[bos].damage_view = 0;
				}
			}

			int side = bossObj[bos].view.left;
			if (bossObj[bos].direct != dirLeft)
				side = bossObj[bos].view.right;

			drawTexture(sprites[TEX_NPC_2], &bossObj[bos].rect, 
				(bossObj[bos].x - side) / 0x200 - (viewport.x / 0x200) + shake, 
				(bossObj[bos].y - bossObj[bos].view.top) / 0x200 - (viewport.y / 0x200));
		}
	}
}
