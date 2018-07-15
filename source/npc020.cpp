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

	if (++NPC->ani_wait > 2)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	NPC->ani_no %= 2;

	setRect = &rect[NPC->ani_no];

	NPC->rect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
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

	if (NPC->direct)
		NPC->ani_no = 1;
	else
		NPC->ani_no = 0;

	setRect = &rect[NPC->ani_no];

	NPC->rect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}