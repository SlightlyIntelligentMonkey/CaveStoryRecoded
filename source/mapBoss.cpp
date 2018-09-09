#include "mapBoss.h"
#include "valueview.h"
#include "game.h"
#include "render.h"



typedef void(*BOSSFUNCTION)(BOSS*);
BOSSFUNCTION bossFuncTbl[10] =
{
  &actBoss_0//,
  //&actBoss_Omega,
  //&actBoss_Frog,
  //&actBoss_MonstX,
  //&actBoss_Core,
  //&actBoss_Ironhead,
  //&actBoss_Twin,
  //&actBoss_Undead,
  //&actBoss_Press,
  //&actBoss_Ballos
}; // idb

void actBoss_0(BOSS *boss)
{
	return; 
}

void updateBoss()
{
	if (bossObj[0].cond & 0x80)
	{
		bossFuncTbl[bossObj[0].code_char]();
		for (int i = 0; i < _countof(bossObj); ++i)
		{
			if (bossObj[i].shock)
				--bossObj[i].shock;
		}
	}
}

void drawBoss(int fx, int fy)
{
	int shake = 0;
	int side = 0;
	RECT rect;
	for (int b = 19; b >= 0; --b)
	{
		if (bossObj[b].cond & 0x80)
		{
			//showing damage and shaking
			if (bossObj[b].shock)
			{
				shake = 2 * ((bossObj[b].shock >> 1) % 2) - 1;
			}
			else
			{
				shake = 0;
				if (bossObj[b].code_flag & 0x8000 && bossObj[b].damage_view)
				{
					createValueView(&bossObj[b].x, &bossObj[b].y, bossObj[b].damage_view);
					bossObj[b].damage_view = 0;
				}
			}
			if (bossObj[b].direct)
				side = bossObj[b].view.right;
			else
				side = bossObj[b].view.left;

			drawTexture(sprites[TEX_NPC_2], &rect,
				((((bossObj[b].x - side) & 0x1FF) + bossObj[b].x - side) >> 9) - (fx / 512) + shake,
				((((bossObj[b].y - bossObj[b].view.top) & 0x1FF) + bossObj[b].y - bossObj[b].view.top) >> 9) - (fy / 512));
		}
	}
}

void setMapBossActNo(int a)
{
	bossObj[0].act_no = a;
}