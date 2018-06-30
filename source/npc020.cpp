#include "npc020.h"

void npcAct032(npc *NPC) //Life Capsule
{
	RECT *setRect;
	RECT rect[2];

	rect[0].left = 32;
	rect[0].top = 96;
	rect[0].right = 48;
	rect[0].bottom = 112;
	rect[1].left = 48;
	rect[1].top = 96;
	rect[1].right = 64;
	rect[1].bottom = 112;

	if (++NPC->animationWait > 2)
	{
		NPC->animationWait = 0;
		++NPC->animation;
	}

	NPC->animation %= 2;

	setRect = &rect[NPC->animation];

	NPC->frameRect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}

void npcAct039(npc *NPC) //Save Sign
{
	RECT *setRect;
	RECT rect[2];

	rect[0].left = 224;
	rect[0].top = 64;
	rect[0].right = 240;
	rect[0].bottom = 80;
	rect[1].left = 240;
	rect[1].top = 64;
	rect[1].right = 256;
	rect[1].bottom = 80;

	if (NPC->direction)
		NPC->animation = 1;
	else
		NPC->animation = 0;

	setRect = &rect[NPC->animation];

	NPC->frameRect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}