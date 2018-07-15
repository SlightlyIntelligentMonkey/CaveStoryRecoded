#include "npc200.h"

void npcAct211(npc *NPC) //Spikes
{
	RECT *setRect;
	RECT rect[4];

	rect[0].left = 256;
	rect[0].top = 200;
	rect[0].right = 272;
	rect[0].bottom = 216;
	rect[1].left = 272;
	rect[1].top = 200;
	rect[1].right = 288;
	rect[1].bottom = 216;
	rect[2].left = 288;
	rect[2].top = 200;
	rect[2].right = 304;
	rect[2].bottom = 216;
	rect[3].left = 304;
	rect[3].top = 200;
	rect[3].right = 320;
	rect[3].bottom = 216;

	setRect = &rect[NPC->code_event];

	NPC->rect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}