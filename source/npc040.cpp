#include "npc040.h"

void npcAct059(npc *NPC) //Eye door
{
	RECT *setRect;
	RECT rect[4];

	rect[0].left = 224;
	rect[0].top = 16;
	rect[0].right = 240;
	rect[0].bottom = 40;
	rect[1].left = 208;
	rect[1].top = 80;
	rect[1].right = 224;
	rect[1].bottom = 104;
	rect[2].left = 224;
	rect[2].top = 80;
	rect[2].right = 240;
	rect[2].bottom = 104;
	rect[3].left = 240;
	rect[3].top = 80;
	rect[3].right = 256;
	rect[3].bottom = 104;

	switch (NPC->action)
	{
	case 0:
		NPC->action = 1;
		goto npc059start;

	case 1:
	npc059start:
		if (NPC->x - 0x8000 < currentPlayer.x && NPC->x + 0x8000 > currentPlayer.x && NPC->y - 0x8000 < currentPlayer.y && NPC->y + 0x8000 > currentPlayer.y)
		{
			NPC->action = 2;
			NPC->animationWait = 0;
		}

		break;

	case 2:
		if (++NPC->animationWait > 2)
		{
			NPC->animationWait = 0;
			++NPC->animation;
		}
		if (NPC->animation == 2)
			NPC->action = 3;

		break;

	case 3:
		if (NPC->x - 0x8000 >= currentPlayer.x || NPC->x + 0x8000 <= currentPlayer.x || NPC->y - 0x8000 >= currentPlayer.y || NPC->y + 0x8000 <= currentPlayer.y)
		{
			NPC->action = 4;
			NPC->animationWait = 0;
		}

		break;

	case 4:
		if (++NPC->animationWait > 2)
		{
			NPC->animationWait = 0;
			--NPC->animation;
		}

		if (!NPC->animation)
			NPC->action = 1;

		break;

	default:
		break;
	}

	if (NPC->shock)
	{
		NPC->frameRect = { rect[3].left, rect[3].top, rect[3].right, rect[3].bottom };
	}
	else
	{
		setRect = &rect[NPC->animation];

		NPC->frameRect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
	}
}