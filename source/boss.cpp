#include "boss.h"

#include <string>
#include <cstring>
#include <SDL_messagebox.h>
#include "valueview.h"
#include "npc.h"
#include "game.h"
#include "render.h"
#include "bossCollision.h"
#include "bulletCollision.h"

using std::string;
using std::to_string;

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
	else if (bossActs[boss[0].code_char] == nullptr && boss[0].code_char != 0)
    {
		static bool wasNotifiedAbout[_countof(bossActs)] = { 0 };

		if (wasNotifiedAbout[boss[0].code_char])
			return;

		wasNotifiedAbout[boss[0].code_char] = true;
		string msg = "Boss " + to_string(boss[0].code_char) + " is not implementated yet.";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Missing Boss", msg.c_str(), nullptr);
    }

    bossHitMap();
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
