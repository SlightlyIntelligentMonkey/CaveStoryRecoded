#include "boss.h"
#include "valueview.h"
#include "npc.h"
#include "game.h"
#include "render.h"

#include <cstring>

npc boss[BOSSNPCS];

using bossAct = void(*)(npc *);
const bossAct bossActs[10] =
{
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
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
	memset(boss, 0, sizeof(boss));
	boss[0].cond = npccond_alive;
	boss[0].code_char = code;
}

void setBossAction(int a)
{
	boss[0].act_no = a;
}

void updateBoss()
{
	if ((boss[0].cond & npccond_alive) && bossActs[boss[0].code_char] != nullptr)
	{
		bossActs[boss[0].code_char](&boss[0]);

		for (size_t bos = 0; bos < BOSSNPCS; ++bos)
		{
			if (boss[bos].shock)
				--boss[bos].shock;
		}
	}
}

void drawBoss()
{
	for (size_t bos = BOSSNPCS - 1; bos > 0; bos--)
	{
		if (boss[bos].cond & npccond_alive)
		{
			int xOffset = 0;

			if (boss[bos].shock)
			{
				xOffset = 2 * ((boss[bos].shock >> 1) & 1) - 1;
			}
			else
			{
				if (boss[bos].bits & npc_showDamage && boss[bos].damage_view)
				{
					createValueView(&boss[bos].x, &boss[bos].y, boss[bos].damage_view);
					boss[bos].damage_view = 0;
				}
			}

			int side = boss[bos].view.left;
			if (boss[bos].direct != dirLeft)
				side = boss[bos].view.right;

			drawTexture(sprites[TEX_NPC_2], &boss[bos].rect, (boss[bos].x - side) / 0x200 - viewport.x / 0x200 + xOffset, (boss[bos].y - boss[bos].view.top) / 0x200 - viewport.y / 0x200);
		}
	}
}
