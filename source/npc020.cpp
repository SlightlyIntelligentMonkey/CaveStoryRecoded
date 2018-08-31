#include "npc020.h"

#include "mathUtils.h"
#include "caret.h"

void npcAct020(npc *NPC) // Computer
{
	constexpr RECT rcLeft = { 288, 16, 320, 40 };
	constexpr RECT rcRight[3] = { {288, 40, 320, 64}, {288, 40, 320, 64}, {288, 64, 320, 88} };

	// Animate from animation No 0 to 2 with a 3-frame delay
	if (++NPC->ani_wait > 3)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}
	if (NPC->ani_no > 2)
		NPC->ani_no = 0;

	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft;
	else
		NPC->rect = rcRight[NPC->ani_no];
}

void npcAct021(npc *NPC) //Open chest
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;

		if (NPC->direct == dirRight)
			NPC->y += 0x2000;
	}

	NPC->rect = { 224, 40, 240, 48 };
}

void npcAct022(npc *NPC) //Teleporter
{
	RECT rect[2];

	rect[0].left = 240;
	rect[0].top = 16;
	rect[0].right = 264;
	rect[0].bottom = 48;
	rect[1].left = 248;
	rect[1].top = 152;
	rect[1].right = 272;
	rect[1].bottom = 184;

	if (NPC->act_no)
	{
		if (NPC->act_no == 1 && ++NPC->ani_no > 1)
			NPC->ani_no = 0;
	}
	else
	{
		NPC->ani_no = 0;
	}

	NPC->rect = rect[NPC->ani_no];
}

void npcAct023(npc *NPC) //Teleporter lights
{
	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 7)
		NPC->ani_no = 0;

	NPC->rect = { 264, 16 + (NPC->ani_no * 4), 288, 20 + (NPC->ani_no * 4) };
}

void npcAct027(npc *NPC) // Death Spikes
{
	NPC->rect = { 96, 64, 128, 88 };
}

void npcAct030(npc *NPC) // Hermit Gunsmith
{
	constexpr RECT rcNPC[3] = { { 48, 0, 64, 16 },{ 48, 16, 64, 32 },{ 0, 32, 16, 48 } };

	if (NPC->direct == dirLeft)	// Wherever he's awoken depends on his direction, it would seem
	{
		if (NPC->act_no == 0)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
			NPC->ani_wait = 0;
		}
		else if (NPC->act_no != 1)
		{
			if (NPC->act_no == 2 && ++NPC->act_wait)
			{
				NPC->act_no = 1;
				NPC->ani_no = 0;
			}
			goto doRects;
		}
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
	}
	else
	{
		if (!NPC->act_no)
		{
			NPC->act_no = 1;
			NPC->y += 0x2000;
			NPC->ani_no = 2;
		}
		if (++NPC->act_wait > 100)
		{
			NPC->act_wait = 0;
			createCaret(NPC->x, NPC->y - 1024, effect_ZzZ, 0);
		}
	}
doRects:
	NPC->rect = rcNPC[NPC->ani_no];
}

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

void npcAct037(npc *NPC) //Sign
{
	RECT rect[2];

	rect[0] = { 192, 64, 208, 80 };
	rect[1] = { 208, 64, 224, 80 };

	//Animate (pixel is dumb)
	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 1)
		NPC->ani_no = 0;

	NPC->rect = rect[NPC->ani_no];
}

void npcAct038(npc * NPC)
{
	constexpr RECT rcNPC[4] = { {128, 64, 144, 80}, {144, 64, 160, 80}, {160, 64, 176, 80}, {176, 64, 192, 80} };

	if (NPC->act_no != 0)
	{
		if (NPC->act_no == 10)
		{
			NPC->act_no = 11;
			createSmoke(NPC->x, NPC->y, NPC->view.bottom, 8);
		}
		else if (NPC->act_no != 11)
			return;
		NPC->rect.left = 0;
		NPC->rect.right = 0;
		return;
	}

	if (++NPC->ani_wait > 3)
	{
		NPC->ani_wait = 0;
		NPC->ani_no++;
	}
	if (NPC->ani_no > 3)
		NPC->ani_no = 0;
	
	NPC->rect = rcNPC[NPC->ani_no];
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

	if (NPC->direct != dirLeft)
		NPC->ani_no = 1;
	else
		NPC->ani_no = 0;

	setRect = &rect[NPC->ani_no];

	NPC->rect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}

