#include "npc040.h"

void npcAct059(npc *NPC) //Eye door
{
	RECT *setRect;
	RECT rect[4];

	rect[0] = { 224, 16, 240, 40 };
	rect[1] = { 208, 80, 224, 104 };
	rect[2] = { 224, 80, 240, 104 };
	rect[3] = { 240, 80, 256, 104 };

	switch (NPC->action)
	{
	case 0:
		NPC->action = 1;

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

	if (NPC->action == 1) //replaces some dumb label and goto ewww acts the same anyways.
	{
		if (NPC->x - 0x8000 < currentPlayer.x && NPC->x + 0x8000 > currentPlayer.x && NPC->y - 0x8000 < currentPlayer.y && NPC->y + 0x8000 > currentPlayer.y)
		{
			NPC->action = 2;
			NPC->animationWait = 0;
		}
	}

	if (NPC->shock)
		setRect = &rect[3];
	else
		setRect = &rect[NPC->animation];

	NPC->frameRect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}