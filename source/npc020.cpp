#include "npc020.h"

#include "mathUtils.h"
#include "caret.h"
#include "player.h"
#include "sound.h"

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

void npcAct025(npc *NPC) //egg corridor lift thing
{
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->x += 4096;
	case 1:
		if (++NPC->act_wait > 150)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		break;
	case 2:
		if (++NPC->act_wait > 64)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		else
		{
			NPC->y -= 512;
		}
		break;
	case 3:
		if (++NPC->act_wait > 150)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		break;
	case 4:
		if (++NPC->act_wait > 64)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		else
		{
			NPC->y -= 512;
		}
		break;
	case 5:
		if (++NPC->act_wait > 150)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		break;
	case 6:
		if (++NPC->act_wait > 64)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		else
		{
			NPC->y += 512;
		}
		break;
	case 7:
		if (++NPC->act_wait > 150)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		break;
	case 8:
		if (++NPC->act_wait > 64)
		{
			NPC->act_wait = 0;
			NPC->act_no = 1;
		}
		else
		{
			NPC->y += 512;
		}
		break;
	default:
		break;
	}
	if (NPC->act_no <= 8 && (1 << NPC->act_no) & 0x154)
	{
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;
	}

	NPC->rect.left = 256;
	NPC->rect.top = 64 + (16 * NPC->ani_no);
	NPC->rect.right = 288;
	NPC->rect.bottom = 80 + (16 * NPC->ani_no);
}

void npcAct027(npc *NPC) // Death Spikes
{
	NPC->rect = { 96, 64, 128, 88 };
}

void npcAct029(npc *NPC) // Cthulhu
{
	constexpr RECT rcLeft[2] = { {0, 192, 16, 216}, {16, 192, 32, 216} };
	constexpr RECT rcRight[2] = { {0, 216, 16, 240}, {16, 216, 32, 240} };

	if (NPC->act_no != 0)
	{
		if (NPC->act_no != 1)
			goto doRects;
	}
	else
	{
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
	}

	NPC->ani_no = NPC->x - 0x6000 < currentPlayer.x
		&& NPC->x + 0x6000 > currentPlayer.x
		&& NPC->y - 0x6000 < currentPlayer.y
		&& NPC->y + 0x2000 > currentPlayer.y;

doRects:
	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft[NPC->ani_no];
	else
		NPC->rect = rcRight[NPC->ani_no];
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
			createCaret(NPC->x, NPC->y - 1024, effect_ZzZ);
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

void npcAct034(npc * NPC) // Bed
{
	if (NPC->direct == dirLeft)
		NPC->rect = { 192, 48, 224, 64 };
	else
		NPC->rect = { 192, 184, 224, 200 };
}

void npcAct035(npc * NPC) // Manann (enemy)
{
	enum
	{
		init = 0,
		wait = 1,
		shooting = 2,
		dead = 3,
	};

	if (NPC->act_no < dead && NPC->life < 90)
	{
		playSound(SFX_QuoteSmashIntoGround);
		createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 8);
		dropExperience(NPC->x, NPC->y, NPC->exp);
		NPC->act_no = dead;
		NPC->act_wait = 0;
		NPC->ani_no = 2;
		NPC->bits &= ~npc_shootable;
		NPC->damage = 0;
	}

	switch (NPC->act_no)
	{
	case init:
	case wait:
		if (NPC->shock)
		{
			const int xDist = (NPC->direct != dirLeft) ? 0x1000 : -0x1000;

			createNpc(NPC_ProjectileMannan, NPC->x + xDist, NPC->y + 0x1000, 0, 0, NPC->direct);
			NPC->ani_no = 1;
			NPC->act_no = shooting;
			NPC->act_wait = 0;
		}
		break;

	case shooting:
		if (++NPC->act_wait > 20)
		{
			NPC->act_wait = 0;
			NPC->act_no = wait;
			NPC->ani_no = 0;
		}
		break;

	case dead:
		switch (++NPC->act_wait)
		{
		case 50:
		case 60:
			NPC->ani_no = 3;
			break;
		case 53:
		case 63:
			NPC->ani_no = 2;
			break;
		default:
			break;
		}
		if (NPC->act_wait > 100)
			NPC->act_no = dead + 1;
		break;

	default:
		break;
	}

	constexpr RECT rcLeft[4] = { {96, 64, 120, 96}, {120, 64, 144, 96}, {144, 64, 168, 98}, {168, 64, 192, 96} };
	constexpr RECT rcRight[4] = { {96, 96, 120, 128}, {120, 96, 144, 128}, {144, 96, 168, 128}, {168, 96, 192, 128} };

	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft[NPC->ani_no];
	else
		NPC->rect = rcRight[NPC->ani_no];
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
			createSmokeLeft(NPC->x, NPC->y, NPC->view.bottom, 8);
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
