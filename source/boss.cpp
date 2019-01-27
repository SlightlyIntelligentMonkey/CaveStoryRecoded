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
#include "stage.h"

npc gBossObj[BOSSNPCS];

using bossAct = void(*)(npc *);
const bossAct gBossActs[10] =
{
	[](npc *) {},
	&actBoss_Omega,
	&actBoss_Balfrog,
	&actBoss_MonsterX,
	&actBoss_Core,
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	static_cast<bossAct>(nullptr),
	&actBoss_HeavyPress,
	&actBoss_Ballos
};

void initBoss(int code)
{
	memset(gBossObj, 0, sizeof(gBossObj));
	gBossObj[0].cond = npccond_alive;
	gBossObj[0].code_char = code;
}

void setBossAction(int a)
{
	gBossObj[0].act_no = a;
}

void updateBoss()
{
	if ((gBossObj[0].cond & npccond_alive) && gBossActs[gBossObj[0].code_char] != nullptr)
	{
		gBossActs[gBossObj[0].code_char](&gBossObj[0]);

		for (size_t bos = 0; bos < BOSSNPCS; ++bos)
		{
			if (gBossObj[bos].shock)
				--gBossObj[bos].shock;
		}
	}
	else if (gBossActs[gBossObj[0].code_char] == nullptr)
    {
		static bool wasNotifiedAbout[_countof(gBossActs)] = { 0 };

		if (wasNotifiedAbout[gBossObj[0].code_char])
			return;

		wasNotifiedAbout[gBossObj[0].code_char] = true;
		std::string msg = "Boss " + std::to_string(gBossObj[0].code_char) + " is not implementated yet.";
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

		if (gBossObj[bos].cond & npccond_alive)
		{
			int shake = 0;

			if (gBossObj[bos].shock)
			{
				shake = 2 * ((gBossObj[bos].shock >> 1) & 1) - 1;
			}
			else
			{
				if (gBossObj[bos].bits & npc_showDamage && gBossObj[bos].damage_view)
				{
					createValueView(&gBossObj[bos].x, &gBossObj[bos].y, gBossObj[bos].damage_view);
					gBossObj[bos].damage_view = 0;
				}
			}

			int side = gBossObj[bos].view.left;
			if (gBossObj[bos].direct != dirLeft)
				side = gBossObj[bos].view.right;

			drawTexture(gSprites[TEX_NPC_2], &gBossObj[bos].rect,
				(gBossObj[bos].x - side) / 0x200 - (gViewport.x / 0x200) + shake,
				(gBossObj[bos].y - gBossObj[bos].view.top) / 0x200 - (gViewport.y / 0x200));

			if (debugFlags & showHitRects)
			{
				SDL_SetRenderDrawColor(gRenderer, 0x80, 0x80, 0x80, 0xFF);
				drawRect(unitsToPixels(gBossObj[bos].x - gBossObj[bos].hit.left) - unitsToPixels(gViewport.x),
					unitsToPixels(gBossObj[bos].y - gBossObj[bos].hit.top) - unitsToPixels(gViewport.y),
					unitsToPixels(gBossObj[bos].hit.right + gBossObj[bos].hit.left),
					unitsToPixels(gBossObj[bos].hit.bottom + gBossObj[bos].hit.top));
			}
		}
	}
}
