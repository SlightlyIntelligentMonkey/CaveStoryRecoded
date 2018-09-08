#include "npc320.h"

#include <vector>
#include "player.h"
#include "level.h"
#include "render.h"

using std::vector;

void npcAct323(npc *NPC)
{
	vector<RECT> rcNPC = {{216, 32, 232, 56}, {232, 32, 248, 56}, {216, 56, 232, 80}, {232, 56, 248, 80}};

	NPC->animate(3, 0, 3);

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;

		switch (NPC->direct)
		{
		case dirLeft:
			NPC->xm = pixelsToUnits(-3);
			break;

		case dirRight:
			NPC->xm = pixelsToUnits(3);
			break;

		case dirUp:
			NPC->ym = pixelsToUnits(-3);
			break;

		case dirDown:
			NPC->ym = pixelsToUnits(3);
			break;

		default:
			break;
		}
		break;

	case 1:
		if (++NPC->act_wait == 16)
			NPC->bits &= ~npc_ignoreSolid;

		NPC->x += NPC->xm;
		NPC->y += NPC->ym;

		if (NPC->flag & 0xFF)
			NPC->act_no = 10;

		if (NPC->act_wait > 20)
		{
			switch (NPC->direct)
			{
			case dirLeft:
				if (NPC->x <= currentPlayer.x + tilesToUnits(2))
					NPC->act_no = 10;
				break;

			case dirRight:
				if (NPC->x >= currentPlayer.x - tilesToUnits(2))
					NPC->act_no = 10;
				break;

			case dirUp:
				if (NPC->y <= currentPlayer.y + tilesToUnits(2))
					NPC->act_no = 10;
				break;

			case dirDown:
				if (NPC->y >= currentPlayer.y - tilesToUnits(2))
					NPC->act_no = 10;
				break;

			default:
				break;
			}
		}
		break;

	case 10:
		NPC->code_char = NPC_EnemyBute;
		NPC->ani_no = 0;
		NPC->act_no = 11;
		NPC->bits |= npc_shootable;
		NPC->bits &= ~npc_ignoreSolid;
		NPC->damage = 5;
		NPC->view.top = pixelsToUnits(8);
		break;

	default:
		break;
	}

	NPC->doRects(rcNPC);
}

void npcAct324(npc *NPC)
{
	if (NPC->act_no == 10)
	{
		NPC->act_no = 11;
		NPC->act_wait = 0;
	}
	else if (NPC->act_no != 11)
		return;

	if (++NPC->act_wait % 50 == 1)
		createNpc(NPC_EnemyButeGenerator, NPC->x, NPC->y, 0, 0, NPC->direct);
	if (NPC->act_wait > 351)
		NPC->act_no = 0;
}

void npcAct328(npc *NPC) // Transmogrifier
{
	NPC->doRects({{96, 0, 128, 48}});
}
