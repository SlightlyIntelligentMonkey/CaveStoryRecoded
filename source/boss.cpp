#include "boss.h"

#include <string>
#include <cstring>
#include <SDL.h>
#include "valueview.h"
#include "npc.h"
#include "game.h"
#include "render.h"
#include "bossCollision.h"
#include "bulletCollision.h"
#include "log.h"

using std::string;
using std::to_string;

npc bossObj[BOSSNPCS];

using bossAct = void(*)(npc *);
const bossAct bossActs[10] =
{
	static_cast<bossAct>(nullptr),
	&actBoss_Omega,
	&actBoss_Balfrog,
	&actBoss_MonsterX,
	&actBoss_Core,
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	&actBoss_HeavyPress,
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
	else if (bossActs[bossObj[0].code_char] == nullptr && bossObj[0].code_char != 0)
    {
		static bool wasNotifiedAbout[_countof(bossActs)] = { 0 };

		if (wasNotifiedAbout[bossObj[0].code_char])
			return;

		wasNotifiedAbout[bossObj[0].code_char] = true;
		string msg = "Boss " + to_string(bossObj[0].code_char) + " is not implementated yet.";
		logWarning(msg);
		if (debugFlags & notifyOnNotImplemented)
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Missing Boss", msg.c_str(), nullptr);
    }

    bossHitMap();
}

void drawBoss()
{
	for (size_t i = 0; i < BOSSNPCS; i++)
	{
		size_t bos = (BOSSNPCS - 1) - i;

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

			if (debugFlags & showHitRects)
			{
				SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0xFF);
				drawRect(unitsToPixels(bossObj[bos].x - bossObj[bos].hit.left) - unitsToPixels(viewport.x),
					unitsToPixels(bossObj[bos].y - bossObj[bos].hit.top) - unitsToPixels(viewport.y),
					unitsToPixels(bossObj[bos].hit.right + bossObj[bos].hit.left),
					unitsToPixels(bossObj[bos].hit.bottom + bossObj[bos].hit.top));
			}
		}
	}
}
