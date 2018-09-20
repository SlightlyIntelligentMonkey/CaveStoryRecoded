#include "npc040.h"

#include <array>
#include "player.h"
#include "sound.h"
#include "mathUtils.h"
#include "render.h"
#include "caret.h"

using std::array;

void npcAct040(npc * NPC) // Santa
{
	array<RECT, 7> rcLeft;
	array<RECT, 7> rcRight;

	rcLeft[0] = { 0, 32, 16, 48 };
	rcLeft[1] = { 16, 32, 32, 48 };
	rcLeft[2] = { 32, 32, 48, 48 };
	rcLeft[3] = rcLeft[0];
	rcLeft[4] = { 48, 32, 0x40, 48 };
	rcLeft[5] = rcLeft[0];
	rcLeft[6] = { 0x40, 32, 80, 48 };

	rcRight[0] = { 0, 48, 16, 0x40 };
	rcRight[1] = { 16, 48, 32, 0x40 };
	rcRight[2] = { 32, 48, 48, 0x40 };
	rcRight[3] = rcRight[0];
	rcRight[4] = { 48, 48, 0x40, 0x40 };
	rcRight[5] = rcRight[0];
	rcRight[6] = { 0x40, 48, 80, 0x40 };

	enum
	{
		init = 0,
		stand = 1,
		blinkWait = 2,
		walk = 3,
		faceAway = 5,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = stand;
		NPC->ani_no = 1;
		NPC->act_wait = 0;
		// Fallthrough
	case stand:
		if (!random(0, 120))
		{
			NPC->act_no = blinkWait;
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}
		if (NPC->isPlayerWithinDistance(0x4000, 0x4000, 0x2000))
			NPC->facePlayer();
		break;

	case blinkWait:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = stand;
			NPC->ani_no = 0;
		}
		break;

	case walk:
		NPC->animate(4, 2, 5);
		if (NPC->direct == dirLeft)
			NPC->x -= 0x200;
		else
			NPC->x += 0x200;
		break;

	default:
		break;
	}

	NPC->doRects(rcLeft, rcRight);
}

void npcAct041(npc * NPC) // Busted doorway
{
	if (!NPC->act_no)
	{
		++NPC->act_no;
		NPC->y -= 0x2000;
	}

	NPC->doRects({ 0, 80, 48, 112 });
}

void npcAct042(npc *NPC) // Sue
{
	array<RECT, 13> rcLeft;
	array<RECT, 13> rcRight;

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 16, 0, 32, 16 };
	rcLeft[2] = { 32, 0, 48, 16 };
	rcLeft[3] = { 0, 0, 16, 16 };
	rcLeft[4] = { 48, 0, 0x40, 16 };
	rcLeft[5] = { 0, 0, 16, 16 };
	rcLeft[6] = { 0x40, 0, 80, 16 };
	rcLeft[7] = { 80, 32, 96, 48 };
	rcLeft[8] = { 96, 32, 112, 48 };
	rcLeft[9] = { 128, 32, 144, 48 };
	rcLeft[10] = { 0, 0, 16, 16 };
	rcLeft[11] = { 112, 32, 128, 48 };
	rcLeft[12] = { 160, 32, 176, 48 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 16, 16, 32, 32 };
	rcRight[2] = { 32, 16, 48, 32 };
	rcRight[3] = { 0, 16, 16, 32 };
	rcRight[4] = { 48, 16, 0x40, 32 };
	rcRight[5] = { 0, 16, 16, 32 };
	rcRight[6] = { 0x40, 16, 80, 32 };
	rcRight[7] = { 80, 48, 96, 0x40 };
	rcRight[8] = { 96, 48, 112, 0x40 };
	rcRight[9] = { 128, 48, 144, 0x40 };
	rcRight[10] = { 0, 16, 16, 32 };
	rcRight[11] = { 112, 48, 128, 0x40 };
	rcRight[12] = { 160, 48, 176, 0x40 };

	enum // Sue's states
	{
		standAndBlink = 0,
		walk = 3,
		punchedByIgor = 6,
		superPunchedByIgor = 8,
		collapsed = 10,
		punchAir = 11,
		carriedByIgor = 13,
		undeadCoreIntro = 15,
		undeadCoreIntroLookUp = 17,
		undeadCoreIntroRunToQuote = 20,
		postUndeadCoreRun = 30,
		jumpOffIsland = 40,
	};

	switch (NPC->act_no)
	{
	case standAndBlink:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
	// Fallthrough
	case 1:
		if (random(0, 120) == 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 2:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;

	case walk:
		NPC->act_no = 4;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
	// Fallthrough
	case 4:
		NPC->animate(4, 2, 5);
		if (NPC->direct)
			NPC->xm = 0x200;
		else
			NPC->xm = -0x200;
		break;

	case 5:
		NPC->ani_no = punchedByIgor;
		NPC->xm = 0;
		break;

	case punchedByIgor: // Flinches for 10 frames then goes back to blinking
		playSound(SFX_EnemySqueal);
		NPC->act_wait = 0;
		NPC->act_no = 7;
		NPC->ani_no = 7;
	// Fallthrough
	case 7:
		if (++NPC->act_wait > 10)
			NPC->act_no = standAndBlink;
		break;

	case superPunchedByIgor: // Flies through the air backwards and crashes
		playSound(SFX_EnemySqueal);
		NPC->act_wait = 0;
		NPC->act_no = 9;
		NPC->ani_no = 7;
		NPC->ym = -0x200;
		if (NPC->direct)
			NPC->xm = -0x400;
		else
			NPC->xm = 0x400;
	// Fallthrough
	case 9:
		if (++NPC->act_wait > 3 && NPC->flag & ground)
		{
			NPC->act_no = collapsed;
			if (NPC->direct != dirLeft)
				NPC->direct = dirLeft;
			else
				NPC->direct = dirRight;
		}
		break;

	case collapsed:
		NPC->xm = 0;
		NPC->ani_no = 8;
		break;

	case punchAir:
		NPC->act_no = 12;
		NPC->act_wait = 0;
		NPC->ani_no = 9;
		NPC->ani_wait = 0;
		NPC->xm = 0;
	// Fallthrough
	case 12:
		NPC->animate(8, 9, 10);
		break;

	case carriedByIgor:
		NPC->ani_no = 11;
		NPC->xm = 0;
		NPC->ym = 0;
		NPC->act_no = 14;
		size_t i;
		for (i = 0; i < npcs.size() && npcs[i].code_event != 501; ++i)	// Search for Igor ?
			;
		if (i == npcs.size())
		{
			NPC->act_no = standAndBlink;
			break;
		}
		NPC->pNpc = &npcs[i];
	// Fallthrough
	case 14:
		if (NPC->pNpc->direct != dirLeft)
			NPC->direct = dirLeft;
		else
			NPC->direct = dirRight;

		if (NPC->pNpc->direct != dirLeft)
			NPC->x = NPC->pNpc->x + 0xC00;
		else
			NPC->x = NPC->pNpc->x - 0XC00;
		NPC->y = NPC->pNpc->y + 0x800;
		if (NPC->pNpc->ani_no == 2 || NPC->pNpc->ani_no == 4)
			NPC->y -= 0x200;
		break;

	case undeadCoreIntro:
		NPC->act_no = 16;
		createNpc(NPC_RedCrystal, NPC->x + 0x10000, NPC->y, 0, 0, dirLeft, nullptr);
		createNpc(NPC_RedCrystal, NPC->x + 0x10000, NPC->y, 0, 0, dirRight, nullptr);
		NPC->xm = 0;
		NPC->ani_no = 0;
	// Fallthrough
	case 16:
		superXPos = NPC->x - 0x3000;
		superYPos = NPC->y - 0x1000;
		break;

	case undeadCoreIntroLookUp:
		NPC->xm = 0;
		NPC->ani_no = 12;
		superXPos = NPC->x;
		superYPos = NPC->y - 0x1000;
		break;

	case undeadCoreIntroRunToQuote:
		NPC->act_no = 21;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
	// Fallthrough
	case 21:
		NPC->animate(2, 2, 5);
		if (NPC->direct)
			NPC->xm = pixelsToUnits(2);
		else
			NPC->xm = pixelsToUnits(-2);

		if (NPC->x < currentPlayer.x - 0x1000)
		{
			NPC->direct = dirRight;
			NPC->act_no = standAndBlink;
		}
		break;

	case postUndeadCoreRun:
		NPC->act_no = 31;
		NPC->ani_no = 2;
		NPC->ani_wait = 0;
	// Fallthrough
	case 31:
		NPC->animate(2, 2, 5);
		if (NPC->direct)
			NPC->xm = pixelsToUnits(2);
		else
			NPC->xm = pixelsToUnits(-2);
		break;
	case jumpOffIsland:
		NPC->act_no = 41;
		NPC->ani_no = 9;
		NPC->ym = -0x400;
		break;
	default:
		break;
	}

	if (NPC->act_no != carriedByIgor + 1)
	{
		NPC->ym += 0x40;
		if (NPC->xm > 0x400)
			NPC->xm = 0x400;
		if (NPC->xm < -0x400)
			NPC->xm = -0x400;
		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;

		NPC->x += NPC->xm;
		NPC->y += NPC->ym;
	}

	NPC->doRects(rcLeft, rcRight);
}

void npcAct043(npc * NPC) // Blackboard
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->y -= 0x2000;
	}
	NPC->doRects({128, 80, 168, 112}, {168, 80, 168, 112});
}

void npcAct044(npc *NPC) //Polish
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = { 0, 0, 32, 32 };
	rcLeft[1] = { 96, 0, 128, 32 };
	rcLeft[2] = { 128, 0, 160, 32 };

	rcRight[0] = { 0, 0, 32, 32 };
	rcRight[1] = { 32, 0, 0x40, 32 };
	rcRight[2] = { 0x40, 0, 96, 32 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
//Fallthrough
	case 1:
		NPC->ani_no = 0;
		if (NPC->direct)
			NPC->act_no = 2;
		else
			NPC->act_no = 8;
//Fallthrough
	case 2:
		NPC->ym += 0x20;

		if (NPC->ym > 0 && NPC->flag & ground)
		{
			NPC->ym = -0x100;
			NPC->xm += 0x100;
		}

		if (NPC->flag & rightWall)
			NPC->act_no = 3;
		break;

	case 3:
		NPC->xm += 0x20;

		if (NPC->xm > 0 && NPC->flag & rightWall)
		{
			NPC->xm = -0x100;
			NPC->ym -= 0x100;
		}

		if (NPC->flag & ceiling)
			NPC->act_no = 4;
		break;

	case 4:
		NPC->ym -= 0x20;

		if (NPC->ym < 0 && NPC->flag & ceiling)
		{
			NPC->ym = 0x100;
			NPC->xm -= 0x100;
		}

		if (NPC->flag & leftWall)
			NPC->act_no = 5;
		break;

	case 5:
		NPC->xm -= 0x20;

		if (NPC->xm < 0 && NPC->flag & leftWall)
		{
			NPC->xm = 0x100;
			NPC->ym += 0x100;
		}

		if (NPC->flag & ground)
			NPC->act_no = 2;
		break;

	case 6:
		NPC->ym += 0x20;

		if (NPC->ym > 0 && NPC->flag & ground)
		{
			NPC->ym = -0x100;
			NPC->xm -= 0x100;
		}

		if (NPC->flag & leftWall)
			NPC->act_no = 7;
		break;

	case 7:
		NPC->xm -= 0x20;

		if (NPC->xm < 0 && NPC->flag & leftWall)
		{
			NPC->xm = 0x100;
			NPC->ym -= 0x100;
		}

		if (NPC->flag & ceiling)
			NPC->act_no = 8;
		break;

	case 8:
		NPC->ym -= 0x20;

		if (NPC->ym < 0 && NPC->flag & ceiling)
		{
			NPC->ym = 0x100;
			NPC->xm += 0x100;
		}

		if (NPC->flag & rightWall)
			NPC->act_no = 9;
		break;

	case 9:
		NPC->xm += 0x20;

		if (NPC->xm > 0 && NPC->flag & rightWall)
		{
			NPC->xm = -0x100;
			NPC->ym += 0x100;
		}

		if (NPC->flag & ground)
			NPC->act_no = 6;
		break;

	default:
		break;
	}

	if (NPC->life <= 100)
	{
		for (int i = 0; i < 10; ++i)
			createNpc(NPC_EnemyBaby, NPC->x, NPC->y, 0, 0, 0, nullptr, true);
		createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 8);
		playSound(SFX_SillyExplosion);
		NPC->cond = 0;
	}

	if (NPC->xm > 0x200)
		NPC->xm = 0x200;
	if (NPC->xm < -0x200)
		NPC->xm = -0x200;

	if (NPC->ym > 0x200)
		NPC->ym = 0x200;
	if (NPC->ym < -0x200)
		NPC->ym = -0x200;

	if (NPC->shock)
	{
		NPC->x += NPC->xm / 2;
		NPC->y += NPC->ym / 2;
	}
	else
	{
		NPC->x += NPC->xm;
		NPC->y += NPC->ym;
	}
	
	if (NPC->act_no > 1 && NPC->act_no < 10 && ++NPC->ani_no > 2)
		NPC->ani_no = 1;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct045(npc *NPC) //Baby polish
{
	RECT rect[3];
	
	rect[0] = { 0, 32, 16, 48 };
	rect[1] = { 16, 32, 32, 48 };
	rect[2] = { 32, 32, 48, 48 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 2;

		if (random(0, 1) != 0)
			NPC->xm = random(-0x200, -0x100);
		else
			NPC->xm = random(0x100, 0x200);

		if (random(0, 1) != 0)
			NPC->ym = random(-0x200, -0x100);
		else
			NPC->ym = random(0x100, 0x200);

		NPC->xm2 = NPC->xm;
		NPC->ym2 = NPC->ym;
//Fallthrough
	case 1:
	case 2:
		if (++NPC->ani_no > 2)
			NPC->ani_no = 1;
		break;

	default:
		break;
	}
	
	if (NPC->xm2 < 0 && NPC->flag & leftWall)
		NPC->xm2 = -NPC->xm2;
	if (NPC->xm2 > 0 && NPC->flag & rightWall)
		NPC->xm2 = -NPC->xm2;
	if (NPC->ym2 < 0 && NPC->flag & ceiling)
		NPC->ym2 = -NPC->ym2;
	if (NPC->ym2 > 0 && NPC->flag & ground)
		NPC->ym2 = -NPC->ym2;

	if (NPC->xm2 > 0x200)
		NPC->xm2 = 0x200;
	if (NPC->xm2 < -0x200)
		NPC->xm2 = -0x200;
	if (NPC->ym2 > 0x200)
		NPC->ym2 = 0x200;
	if (NPC->ym2 < -0x200)
		NPC->ym2 = -0x200;

	if (NPC->shock)
	{
		NPC->x += NPC->xm2 / 2;
		NPC->y += NPC->ym2 / 2;
	}
	else
	{
		NPC->x += NPC->xm2;
		NPC->y += NPC->ym2;
	}
	
	NPC->rect = rect[NPC->ani_no];
}

void npcAct046(npc *NPC) //H/V trigger
{
	NPC->bits |= npc_eventTouch;

	if (NPC->direct != dirLeft)
	{
		if (NPC->y >= currentPlayer.y)
			NPC->y -= 0x5FF;
		else
			NPC->y += 0x5FF;
	}
	else
	{
		if (NPC->x >= currentPlayer.x)
			NPC->x -= 0x5FF;
		else
			NPC->x += 0x5FF;
	}

	NPC->doRects({ 0, 0, 16, 16 });
}

void npcAct047(npc *NPC) //Sandcroc
{
	RECT rect[5];

	rect[0] = { 0x00, 0x30, 0x30, 0x50};
	rect[1] = { 0x30, 0x30, 0x60, 0x50 };
	rect[2] = { 0x60, 0x30, 0x90, 0x50 };
	rect[3] = { 0x90, 0x30, 0xC0, 0x50 };
	rect[4] = { 0xC0, 0x30, 0xF0, 0x50 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->ani_no = 0;
		NPC->act_no = 1;
		NPC->act_wait = 0;
		NPC->tgt_y = NPC->y;
		NPC->bits &= ~(npc_shootable | npc_invulnerable | npc_solidSoft | npc_ignoreSolid);
//Fallthrough
	case 1:
		if (currentPlayer.x > NPC->x - 0x1000 && currentPlayer.x < NPC->x + 0x1000 && currentPlayer.y > NPC->y && currentPlayer.y < NPC->y + 0x1000)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			playSound(SFX_SandCroc, 1);
		}

		if (NPC->x < currentPlayer.x)
			NPC->x += 0x400;
		if (NPC->x > currentPlayer.x)
			NPC->x -= 0x400;
		break;

	case 2:
		if (++NPC->ani_wait > 3)
		{
			++NPC->ani_no;
			NPC->ani_wait = 0;
		}

		if (NPC->ani_no == 3)
			NPC->damage = 10;

		if (NPC->ani_no == 4)
		{
			NPC->bits |= npc_shootable;
			NPC->act_no = 3;
			NPC->act_wait = 0;
		}
		break;

	case 3:
		NPC->bits |= npc_solidSoft;
		NPC->damage = 0;

		++NPC->act_wait;
		if (NPC->shock)
		{
			NPC->act_no = 4;
			NPC->act_wait = 0;
		}
		break;

	case 4:
		NPC->bits |= npc_ignoreSolid;
		NPC->y += 0x200;

		if (++NPC->act_wait == 32)
		{
			NPC->bits &= ~(npc_ignoreSolid | npc_shootable);
			NPC->act_no = 5;
			NPC->act_wait = 0;
		}
		break;

	case 5:
		if (NPC->act_wait >= 100)
		{
			NPC->y = NPC->tgt_y;
			NPC->ani_no = 0;
			NPC->act_no = 0;
		}
		else
		{
			++NPC->act_wait;
		}
		break;

	default:
		break;
	}

	NPC->rect = rect[NPC->ani_no];
}

void npcAct048(npc *NPC) //Omega projectile things
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = { 288, 88, 304, 104 };
	rcLeft[1] = { 304, 88, 320, 104 };
	rcRight[0] = { 288, 104, 304, 120 };
	rcRight[1] = { 304, 104, 320, 120 };

	if (NPC->flag & leftWall && NPC->xm < 0)
	{
		NPC->xm = -NPC->xm;
	}
	else if (NPC->flag & rightWall && NPC->xm > 0)
	{
		NPC->xm = -NPC->xm;
	}
	else if (NPC->flag & ground)
	{
		if (++NPC->count1 <= 2 && NPC->direct != 2)
		{
			NPC->ym = -0x100;
		}
		else
		{
			NPC->cond = 0;
			createCaret(NPC->x, NPC->y, 2, 0);
		}
	}

	if (NPC->direct == 2)
	{
		NPC->bits &= ~npc_shootable;
		NPC->bits |= npc_invulnerable;
	}

	NPC->ym += 5;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;
	
	if (++NPC->ani_wait > 2)
	{
		NPC->ani_wait = 0;
		if (++NPC->ani_no > 1)
			NPC->ani_no = 0;
	}

	if (++NPC->act_wait > 750)
	{
		createCaret(NPC->x, NPC->y, 2, 0);
		NPC->cond = 0;
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct049(npc *NPC) //Skull heads
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0].left = 0;
	rcLeft[0].top = 80;
	rcLeft[0].right = 32;
	rcLeft[0].bottom = 104;
	rcLeft[1].left = 32;
	rcLeft[1].top = 80;
	rcLeft[1].right = 0x40;
	rcLeft[1].bottom = 104;
	rcLeft[2].left = 0x40;
	rcLeft[2].top = 80;
	rcLeft[2].right = 96;
	rcLeft[2].bottom = 104;
	rcRight[0].left = 0;
	rcRight[0].top = 104;
	rcRight[0].right = 32;
	rcRight[0].bottom = 128;
	rcRight[1].left = 32;
	rcRight[1].top = 104;
	rcRight[1].right = 0x40;
	rcRight[1].bottom = 128;
	rcRight[2].left = 0x40;
	rcRight[2].top = 104;
	rcRight[2].right = 96;
	rcRight[2].bottom = 128;
	
	uint8_t deg;
	int xm;
	int ym;

	if (NPC->act_no > 9 && NPC->pNpc->code_char == 3)
	{
		NPC->act_no = 3;
		NPC->xm = 0;
		NPC->ym = 0;
		NPC->count2 = 1;
	}

	if (NPC->flag & leftWall)
	{
		NPC->direct = dirRight;
		NPC->xm = 0x100;
	}
	if (NPC->flag & rightWall)
	{
		NPC->direct = dirLeft;
		NPC->xm = -0x100;
	}

	switch (NPC->act_no)
	{
	case 0:
		if (NPC->pNpc)
			NPC->act_no = 10;
		else
			NPC->act_no = 1;
//Fallthrough
	case 1:
		if (++NPC->act_wait > 3)
		{
			NPC->ym = -0x400;
			NPC->act_no = 3;
			NPC->ani_no = 2;

			if (NPC->count2)
			{
				if (NPC->direct)
					NPC->xm = 0x200;
				else
					NPC->xm = -0x200;
			}
			else if (NPC->direct)
			{
				NPC->xm = 0x100;
			}
			else
			{
				NPC->xm = -0x100;
			}
		}

		NPC->ani_no = 1;
		break;

	case 3:
		if (NPC->flag & ground)
		{
			NPC->act_no = 1;
			NPC->act_wait = 0;
			NPC->xm = 0;
		}

		if (NPC->flag & ground || NPC->ym > 0)
			NPC->ani_no = 1;
		else
			NPC->ani_no = 2;
		break;

	case 10:
		if (NPC->count1 > 49)
		{
			if (NPC->x - 0x10000 < currentPlayer.x && NPC->x + 0x10000 > currentPlayer.x && NPC->y - 0xC000 < currentPlayer.y && NPC->y + 0xC000 > currentPlayer.y)
			{
				NPC->act_no = 11;
				NPC->act_wait = 0;
				NPC->ani_no = 2;
			}
		}
		else
		{
			++NPC->count1;
		}
		break;

	case 11:
		if (++NPC->act_wait == 30 || NPC->act_wait == 35)
		{
			deg = getAtan(NPC->x - currentPlayer.x, NPC->y + 0x800 - currentPlayer.y);
			xm = 2 * getCos(deg);
			ym = 2 * getSin(deg);

			createNpc(50, NPC->x, NPC->y, xm, ym, 0, nullptr, true);
			playSound(SFX_EnemyShootProjectile);
		}

		if (NPC->act_wait > 50)
		{
			NPC->count1 = 0;
			NPC->act_no = 10;
			NPC->ani_no = 1;
		}
		break;

	default:
		break;
	}

	if (NPC->act_no >= 10)
	{
		NPC->x = NPC->pNpc->x;
		NPC->y = NPC->pNpc->y + 0x2000;
		NPC->direct = NPC->pNpc->direct;
		--NPC->pNpc->count1;
	}

	NPC->ym += 0x40;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct050(npc *NPC) //Skull head projectile
{
	RECT rect[4];

	switch (NPC->act_no)
	{
	case 0:
		if (NPC->direct == 2)
			NPC->act_no = 2;
//Fallthrough
	case 1:
		NPC->x += NPC->xm;
		NPC->y += NPC->ym;

		if (NPC->flag & leftWall)
		{
			NPC->act_no = 2;
			NPC->xm = 0x200;
			++NPC->count1;
		}

		if (NPC->flag & rightWall)
		{
			NPC->act_no = 2;
			NPC->xm = -0x200;
			++NPC->count1;
		}

		if (NPC->flag & ceiling)
		{
			NPC->act_no = 2;
			NPC->ym = 0x200;
			++NPC->count1;
		}

		if (NPC->flag & ground)
		{
			NPC->act_no = 2;
			NPC->ym = -0x200;
			++NPC->count1;
		}
		break;

	case 2:
		NPC->ym += 0x40;
		NPC->x += NPC->xm;
		NPC->y += NPC->ym;

		if (NPC->flag & ground)
		{
			if (++NPC->count1 > 1)
			{
				createCaret(NPC->x, NPC->y, 2, 0);
				NPC->cond = 0;
			}
		}
		break;

	default:
		break;
	}

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;
	if (NPC->ym < -0x5FF)
		NPC->ym = -0x5FF;

	rect[0].left = 48;
	rect[0].top = 32;
	rect[0].right = 64;
	rect[0].bottom = 48;
	rect[1].left = 64;
	rect[1].top = 32;
	rect[1].right = 80;
	rect[1].bottom = 48;
	rect[2].left = 80;
	rect[2].top = 32;
	rect[2].right = 96;
	rect[2].bottom = 48;
	rect[3].left = 96;
	rect[3].top = 32;
	rect[3].right = 112;
	rect[3].bottom = 48;

	if (NPC->direct)
	{
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			--NPC->ani_no;
		}
		if (NPC->ani_no < 0)
			NPC->ani_no = 3;
	}
	else
	{
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 3)
			NPC->ani_no = 0;
	}

	NPC->rect = rect[NPC->ani_no];
}

void npcAct051(npc *NPC) //Skull and crow
{
	RECT rcLeft[5];
	RECT rcRight[5];

	rcLeft[0].left = 96;
	rcLeft[0].top = 80;
	rcLeft[0].right = 128;
	rcLeft[0].bottom = 112;
	rcLeft[1].left = 128;
	rcLeft[1].top = 80;
	rcLeft[1].right = 160;
	rcLeft[1].bottom = 112;
	rcLeft[2].left = 160;
	rcLeft[2].top = 80;
	rcLeft[2].right = 192;
	rcLeft[2].bottom = 112;
	rcLeft[3].left = 192;
	rcLeft[3].top = 80;
	rcLeft[3].right = 224;
	rcLeft[3].bottom = 112;
	rcLeft[4].left = 224;
	rcLeft[4].top = 80;
	rcLeft[4].right = 256;
	rcLeft[4].bottom = 112;
	rcRight[0].left = 96;
	rcRight[0].top = 112;
	rcRight[0].right = 128;
	rcRight[0].bottom = 144;
	rcRight[1].left = 128;
	rcRight[1].top = 112;
	rcRight[1].right = 160;
	rcRight[1].bottom = 144;
	rcRight[2].left = 160;
	rcRight[2].top = 112;
	rcRight[2].right = 192;
	rcRight[2].bottom = 144;
	rcRight[3].left = 192;
	rcRight[3].top = 112;
	rcRight[3].right = 224;
	rcRight[3].bottom = 144;
	rcRight[4].left = 224;
	rcRight[4].top = 112;
	rcRight[4].right = 256;
	rcRight[4].bottom = 144;

	switch (NPC->act_no)
	{
	case 0:
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
		NPC->ym = 0x400;
		NPC->act_no = 1;
		createNpc(49, 0, 0, 0, 0, 0, NPC, false);
//Fallthrough
	case 1:
		if (currentPlayer.x >= NPC->x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;

		if (NPC->tgt_y < NPC->y)
			NPC->ym -= 10;
		if (NPC->tgt_y > NPC->y)
			NPC->ym += 10;

		if (NPC->ym > 0x200)
			NPC->ym = 0x200;

		if (NPC->ym < -0x200)
			NPC->ym = -0x200;

		if (NPC->count1 >= 10)
			NPC->act_no = 2;
		else
			++NPC->count1;
		break;

	case 2:
		if (!(NPC->x - 0x28000 >= currentPlayer.x || NPC->x + 0x28000 <= currentPlayer.x || NPC->y - 0x28000 >= currentPlayer.y || NPC->y + 0x28000 <= currentPlayer.y))
		{
			if (currentPlayer.x >= NPC->x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;

			if (NPC->y <= currentPlayer.y + 0x4000)
			{
				if (currentPlayer.x < NPC->x)
					NPC->xm -= 0x10;
				if (currentPlayer.x > NPC->x)
					NPC->xm += 0x10;
			}
			else
			{
				if (currentPlayer.x < NPC->x)
					NPC->xm += 0x10;
				if (currentPlayer.x > NPC->x)
					NPC->xm -= 0x10;
			}

			if (currentPlayer.y < NPC->y)
				NPC->ym -= 0x10;
			if (currentPlayer.y > NPC->y)
				NPC->ym += 0x10;

			if (NPC->shock)
			{
				NPC->ym += 0x20;
				NPC->xm = 0;
			}
		}
		break;

	default:
		break;
	}
	
	if (NPC->xm < 0 && NPC->flag & leftWall)
		NPC->xm = 0x100;
	if (NPC->xm > 0 && NPC->flag & rightWall)
		NPC->xm = -0x100;
	if (NPC->ym < 0 && NPC->flag & ceiling)
		NPC->ym = 0x100;
	if (NPC->ym > 0 && NPC->flag & ground)
		NPC->ym = -0x100;

	if (NPC->xm > 0x400)
		NPC->xm = 0x400;
	if (NPC->xm < -0x400)
		NPC->xm = -0x400;

	if (NPC->ym > 0x200)
		NPC->ym = 0x200;
	if (NPC->ym < -0x200)
		NPC->ym = -0x200;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;
	
	if (NPC->shock)
	{
		NPC->ani_no = 4;
	}
	else if (NPC->act_no != 2 || NPC->y >= currentPlayer.y - 0x4000)
	{
		if (NPC->act_no)
		{
			if (++NPC->ani_wait > 1)
			{
				NPC->ani_wait = 0;
				++NPC->ani_no;
			}

			if (NPC->ani_no > 1)
				NPC->ani_no = 0;
		}
	}
	else
	{
		NPC->ani_no = 0;
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct052(npc *NPC) // Blue robots
{
	NPC->doRects({ 240, 96, 0x100, 112 });
}

void npcAct053(npc *NPC) //Skullstep (leg)
{
	RECT rcLeft[2];
	RECT rcRight[2];
	uint8_t deg;

	rcLeft[0].left = 0;
	rcLeft[0].top = 128;
	rcLeft[0].right = 24;
	rcLeft[0].bottom = 144;
	rcLeft[1].left = 24;
	rcLeft[1].top = 128;
	rcLeft[1].right = 48;
	rcLeft[1].bottom = 144;
	rcRight[0].left = 48;
	rcRight[0].top = 128;
	rcRight[0].right = 72;
	rcRight[0].bottom = 144;
	rcRight[1].left = 72;
	rcRight[1].top = 128;
	rcRight[1].right = 96;
	rcRight[1].bottom = 144;

	if (NPC->pNpc->code_char == 3)
	{
		NPC->init(3, NPC->x, NPC->y, 0, 0, 0, nullptr);
		createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 8);
		return;
	}

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->count1 = 10;
//Fallthrough
	case 1:
		if (NPC->direct == dirLeft && NPC->flag & slopeLeft)
		{
			NPC->pNpc->y -= 0x400;
			NPC->pNpc->ym -= 0x100;
		}

		if (NPC->direct == dirRight && NPC->flag & slopeRight)
		{
			NPC->pNpc->y -= 0x400;
			NPC->pNpc->ym -= 0x100;
		}

		if (NPC->flag & ground)
		{
			NPC->pNpc->y -= 0x400;
			NPC->pNpc->ym -= 0x100;

			if (NPC->pNpc->direct)
				NPC->pNpc->xm += 0x80;
			else
				NPC->pNpc->xm -= 0x80;
		}

		deg = NPC->xm + NPC->pNpc->count2;
		NPC->x = NPC->pNpc->x + NPC->count1 * getCos(deg);
		NPC->y = NPC->pNpc->y + NPC->count1 * getSin(deg);
		NPC->direct = NPC->pNpc->direct;
		break;
	}
	
	NPC->direct = NPC->pNpc->direct;
	NPC->ani_no = deg < 0x14u || deg > 0x6Cu;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct054(npc *NPC) //Skullstep (body)
{
	RECT rcLeft[3];
	RECT rcRight[3];
	uint8_t deg;

	rcLeft[0].left = 0;
	rcLeft[0].top = 80;
	rcLeft[0].right = 32;
	rcLeft[0].bottom = 104;
	rcLeft[1].left = 32;
	rcLeft[1].top = 80;
	rcLeft[1].right = 64;
	rcLeft[1].bottom = 104;
	rcLeft[2].left = 64;
	rcLeft[2].top = 80;
	rcLeft[2].right = 96;
	rcLeft[2].bottom = 104;
	rcRight[0].left = 0;
	rcRight[0].top = 104;
	rcRight[0].right = 32;
	rcRight[0].bottom = 128;
	rcRight[1].left = 32;
	rcRight[1].top = 104;
	rcRight[1].right = 64;
	rcRight[1].bottom = 128;
	rcRight[2].left = 64;
	rcRight[2].top = 104;
	rcRight[2].right = 96;
	rcRight[2].bottom = 128;
	
	switch (NPC->act_no)
	{
	case 0:
		createNpc(53, 0, 0, 0, 0, NPC->direct, NPC, true);
		createNpc(53, 0, 0, 128, 0, NPC->direct, NPC, false);
		NPC->act_no = 1;
		NPC->ani_no = 1;
//Fallthrough
	case 1:
		deg = NPC->count2;

		if (NPC->direct)
			deg += 6;
		else
			deg -= 6;

		NPC->count2 = deg;

		if (NPC->flag & ground)
		{
			NPC->xm = 3 * NPC->xm / 4;

			if (++NPC->act_wait > 60)
			{
				NPC->act_no = 2;
				NPC->act_wait = 0;
			}
		}
		else
		{
			NPC->act_wait = 0;
		}

		if (NPC->direct || !(NPC->flag & leftWall))
		{
			if (NPC->direct == 2 && NPC->flag & rightWall)
			{
				if (++NPC->count1 > 8)
				{
					NPC->direct = dirLeft;
					NPC->xm = -NPC->xm;
				}
			}
			else
			{
				NPC->count1 = 0;
			}
		}
		else if (++NPC->count1 > 8)
		{
			NPC->direct = dirRight;
			NPC->xm = -NPC->xm;
		}
		break;

	case 2:
		++NPC->act_wait;
		NPC->shock += NPC->act_wait & 0xFF;

		if (NPC->act_wait > 50)
		{
			NPC->init(3, NPC->x, NPC->y, 0, 0, 0, nullptr);
			createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 8);
			playSound(SFX_SillyExplosion);
		}
		break;
	}
	
	NPC->ym += 0x80;

	if (NPC->xm > 0x2FF)
		NPC->xm = 0x2FF;
	if (NPC->xm < -0x2FF)
		NPC->xm = -0x2FF;

	if (NPC->ym > 0x2FF)
		NPC->ym = 0x2FF;
	if (NPC->ym < -0x2FF)
		NPC->ym = -0x2FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct055(npc *NPC) //Kazuma
{
	array<RECT, 6> rcLeft;
	array<RECT, 6> rcRight;

	rcLeft[0] = { 0xC0, 0xC0, 0xD0, 0xD8 };
	rcLeft[1] = { 0xD0, 0xC0, 0xE0, 0xD8 };
	rcLeft[2] = { 0xC0, 0xC0, 0xD0, 0xD8 };
	rcLeft[3] = { 0xE0, 0xC0, 0xF0, 0xD8 };
	rcLeft[4] = { 0xC0, 0xC0, 0xD0, 0xD8 };
	rcLeft[5] = { 0xF0, 0xC0, 0x100, 0xD8 };

	rcRight[0] = { 0xC0, 0xD8, 0xD0, 0xF0 };
	rcRight[1] = { 0xD0, 0xD8, 0xE0, 0xF0 };
	rcRight[2] = { 0xC0, 0xD8, 0xD0, 0xF0 };
	rcRight[3] = { 0xE0, 0xD8, 0xF0, 0xF0 };
	rcRight[4] = { 0xC0, 0xD8, 0xD0, 0xF0 };
	rcRight[5] = { 0xF0, 0xD8, 0x100, 0xF0 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		break;

	case 3:
		NPC->act_no = 4;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
//Fallthrough
	case 4:
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		if (NPC->direct)
			NPC->x += 0x200;
		else
			NPC->x -= 0x200;
		break;

	case 5:
		NPC->ani_no = 5;
		break;
	default:
		break;
	}

	NPC->ym += 32;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);
}

void npcAct056(npc *NPC) //Sand zone beetles
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = { 0, 144, 16, 160 };
	rcLeft[1] = { 16, 144, 32, 160 };
	rcLeft[2] = { 32, 144, 48, 160 };
	rcRight[0] = { 0, 160, 16, 176 };
	rcRight[1] = { 16, 160, 32, 176 };
	rcRight[2] = { 32, 160, 48, 176 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		if (NPC->direct)
			NPC->act_no = 3;
		else
			NPC->act_no = 1;
		break;

	case 1:
		NPC->xm -= 0x10;

		if (NPC->xm < -0x400)
			NPC->xm = -0x400;

		if (NPC->shock)
			NPC->x += NPC->xm / 2;
		else
			NPC->x += NPC->xm;

		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 2)
			NPC->ani_no = 1;

		if (NPC->flag & leftWall)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->xm = 0;
			NPC->direct = 2;
		}
		break;

	case 2:
		if (NPC->x < currentPlayer.x && NPC->x > currentPlayer.x - 0x20000 && NPC->y < currentPlayer.y + 0x1000 && NPC->y > currentPlayer.y - 0x1000)
		{
			NPC->act_no = 3;
			NPC->ani_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 3:
		NPC->xm += 0x10;

		if (NPC->xm > 0x400)
			NPC->xm = 0x400;

		if (NPC->shock)
			NPC->x += NPC->xm / 2;
		else
			NPC->x += NPC->xm;
		
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 2)
			NPC->ani_no = 1;

		if (NPC->flag & rightWall)
		{
			NPC->act_no = 4;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->xm = 0;
			NPC->direct = 0;
		}
		break;

	case 4:
		if (NPC->x < currentPlayer.x + 0x20000 && NPC->x > currentPlayer.x && NPC->y < currentPlayer.y + 0x1000 && NPC->y > currentPlayer.y - 0x1000)
		{
			NPC->act_no = 1;
			NPC->ani_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	default:
		break;
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct057(npc *NPC) //Crow
{
	RECT rcLeft[5];
	RECT rcRight[5];
	uint8_t deg;

	rcLeft[0].left = 96;
	rcLeft[0].top = 80;
	rcLeft[0].right = 128;
	rcLeft[0].bottom = 112;
	rcLeft[1].left = 128;
	rcLeft[1].top = 80;
	rcLeft[1].right = 160;
	rcLeft[1].bottom = 112;
	rcLeft[2].left = 160;
	rcLeft[2].top = 80;
	rcLeft[2].right = 192;
	rcLeft[2].bottom = 112;
	rcLeft[3].left = 192;
	rcLeft[3].top = 80;
	rcLeft[3].right = 224;
	rcLeft[3].bottom = 112;
	rcLeft[4].left = 224;
	rcLeft[4].top = 80;
	rcLeft[4].right = 256;
	rcLeft[4].bottom = 112;
	rcRight[0].left = 96;
	rcRight[0].top = 112;
	rcRight[0].right = 128;
	rcRight[0].bottom = 144;
	rcRight[1].left = 128;
	rcRight[1].top = 112;
	rcRight[1].right = 160;
	rcRight[1].bottom = 144;
	rcRight[2].left = 160;
	rcRight[2].top = 112;
	rcRight[2].right = 192;
	rcRight[2].bottom = 144;
	rcRight[3].left = 192;
	rcRight[3].top = 112;
	rcRight[3].right = 224;
	rcRight[3].bottom = 144;
	rcRight[4].left = 224;
	rcRight[4].top = 112;
	rcRight[4].right = 256;
	rcRight[4].bottom = 144;

	switch (NPC->act_no)
	{
	case 0:
		deg = random(0, 0xFF);
		NPC->xm = getCos(deg);
		deg += 0x40;
		NPC->tgt_x = NPC->x + 8 * getCos(deg);

		deg = random(0, 0xFF);
		NPC->ym = getSin(deg);
		deg += 0x40;
		NPC->tgt_y = NPC->y + 8 * getSin(deg);

		NPC->act_no = 1;

		NPC->count1 = 120;

		NPC->ani_no = random(0, 1);
		NPC->ani_wait = random(0, 4);
//Fallthrough
	case 1:
		if (currentPlayer.x >= NPC->x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;

		if (NPC->tgt_x < NPC->x)
			NPC->xm -= 0x10;
		if (NPC->tgt_x > NPC->x)
			NPC->xm += 0x10;
		if (NPC->tgt_y < NPC->y)
			NPC->ym -= 0x10;
		if (NPC->tgt_y > NPC->y)
			NPC->ym += 0x10;

		if (NPC->xm > 0x200)
			NPC->xm = 0x200;
		if (NPC->xm < -0x200)
			NPC->xm = -0x200;
		if (NPC->ym > 0x200)
			NPC->ym = 0x200;
		if (NPC->ym < -0x200)
			NPC->ym = -0x200;

		if (NPC->shock)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;

			if (NPC->direct == dirRight)
				NPC->xm = -0x200;
			else
				NPC->xm = 0x200;

			NPC->ym = 0;
		}
		break;

	case 2:
		if (currentPlayer.x >= NPC->x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;

		if (NPC->y <= currentPlayer.y + 0x6000)
		{
			if (currentPlayer.x < NPC->x)
				NPC->xm -= 0x10;
			if (currentPlayer.x > NPC->x)
				NPC->xm += 0x10;
		}
		else
		{
			if (currentPlayer.x < NPC->x)
				NPC->xm += 0x10;
			if (currentPlayer.x > NPC->x)
				NPC->xm -= 0x10;
		}

		if (currentPlayer.y < NPC->y)
			NPC->ym -= 0x10;
		if (currentPlayer.y > NPC->y)
			NPC->ym += 0x10;

		if (NPC->shock)
		{
			NPC->ym += 0x20;
			NPC->xm = 0;
		}

		if (NPC->xm < 0 && NPC->flag & leftWall)
			NPC->xm = 0x200;
		if (NPC->xm > 0 && NPC->flag & rightWall)
			NPC->xm = -0x200;
		if (NPC->ym < 0 && NPC->flag & ceiling)
			NPC->ym = 0x200;
		if (NPC->ym > 0 && NPC->flag & ground)
			NPC->ym = -0x200;

		if (NPC->xm > 0x5FF)
			NPC->xm = 0x5FF;
		if (NPC->xm < -0x5FF)
			NPC->xm = -0x5FF;
		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;
		if (NPC->ym < -0x5FF)
			NPC->ym = -0x5FF;
		break;

	default:
		break;
	}
	
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->shock)
	{
		NPC->ani_no = 4;
	}
	else
	{
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 1)
			NPC->ani_no = 0;
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct058(npc *NPC) //Basu 1
{
	const int action = NPC->act_no;

	if (!action)
	{
		//Don't appear until Quote walks into spawn area
		if (currentPlayer.x >= NPC->x + 0x2000 || currentPlayer.x <= NPC->x - 0x2000)
		{
			NPC->rect = { 0, 0, 0, 0 };
			NPC->damage = 0;
			NPC->xm = 0;
			NPC->ym = 0;
			NPC->bits &= ~npc_shootable;
		}
		else
		{
			NPC->bits |= npc_shootable;
			NPC->ym = -0x100;

			NPC->tgt_x = NPC->x;
			NPC->tgt_y = NPC->y;

			NPC->act_no = 1;
			NPC->act_wait = 0;

			NPC->count1 = NPC->direct; //a
			NPC->count2 = 0;

			NPC->damage = 6;

			if (NPC->direct != dirLeft)
			{
				NPC->x = currentPlayer.x - 0x20000;
				NPC->xm = 0x2FF;
			}
			else
			{
				NPC->x = currentPlayer.x + 0x20000;
				NPC->xm = -0x2FF;
			}
		}
	}
	else
	{
		if (action == 1)
		{
			//Face towards player
			if (NPC->x <= currentPlayer.x)
			{
				NPC->direct = dirRight;
				NPC->xm += 16;
			}
			else
			{
				NPC->direct = dirLeft;
				NPC->xm -= 16;
			}

			//Bounce off of wall
			if (NPC->flag & leftWall)
				NPC->xm = 0x200;
			if (NPC->flag & rightWall)
				NPC->xm = -0x200;

			//Fly up and down
			if (NPC->y >= NPC->tgt_y)
				NPC->ym -= 8;
			else
				NPC->ym += 8;

			//Limit speed
			if (NPC->xm > 0x2FF)
				NPC->xm = 0x2FF;
			if (NPC->xm < -0x2FF)
				NPC->xm = -0x2FF;

			if (NPC->ym > 0x100)
				NPC->ym = 0x100;
			if (NPC->ym < -0x100)
				NPC->ym = -0x100;

			//Move
			if (NPC->shock)
			{
				NPC->x += NPC->xm / 2;
				NPC->y += NPC->ym / 2;
			}
			else
			{
				NPC->x += NPC->xm;
				NPC->y += NPC->ym;
			}
		}

		if ((currentPlayer.x <= NPC->x + 0x32000 && currentPlayer.x >= NPC->x - 0x32000) || action != 1) //Do stuff if nearby
		{
			if (NPC->act_no)
			{
				if (NPC->act_wait < 150)
					++NPC->act_wait;
				else
				{
					if (!(++NPC->count2 & 7) && NPC->x < currentPlayer.x + 0x14000 && NPC->x > currentPlayer.x - 0x14000)
					{
						uint8_t deg = getAtan(NPC->x - currentPlayer.x, NPC->y - currentPlayer.y);
						deg += random(-6, 6);

						const int fireXm = 2 * getCos(deg);
						const int fireYm = 2 * getSin(deg);

						createNpc(NPC_ProjectileBasu1, NPC->x, NPC->y, fireXm, fireYm);
						playSound(SFX_EnemyShootProjectile);
					}

					if (NPC->count2 > 8)
					{
						NPC->act_wait = 0;
						NPC->count2 = 0;
					}
				}
			}

			//Animate
			if (++NPC->ani_wait > 1)
			{
				NPC->ani_wait = 0;
				++NPC->ani_no;
			}

			if (NPC->ani_no > 1)
				NPC->ani_no = 0;

			if (NPC->act_wait > 120 && NPC->act_wait / 2 % 2 == 1 && NPC->ani_no == 1)
				NPC->ani_no = 2;

            NPC->doRects({ 192 + (NPC->ani_no * 24), 0, 216 + (NPC->ani_no * 24), 24 },
                         { 192 + (NPC->ani_no * 24), 24, 216 + (NPC->ani_no * 24), 48 });
		}
		else
		{
			NPC->x = NPC->tgt_x;
			NPC->xm = 0;

			//Stop existing
			NPC->act_no = 0;
			NPC->direct = NPC->count1;
			NPC->rect = { 0, 0, 0, 0 };
			NPC->damage = 0;
		}
	}
}

void npcAct059(npc *NPC) //Eye door
{
	array<RECT, 4> rect;

	rect[0] = { 224, 16, 240, 40 };
	rect[1] = { 208, 80, 224, 104 };
	rect[2] = { 224, 80, 240, 104 };
	rect[3] = { 240, 80, 0x100, 104 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
	// Fallthrough
	case 1:
		if (NPC->x - 0x8000 < currentPlayer.x && NPC->x + 0x8000 > currentPlayer.x && NPC->y - 0x8000 < currentPlayer.y && NPC->y + 0x8000 > currentPlayer.y)
		{
			NPC->act_no = 2;
			NPC->ani_wait = 0;
		}
		break;

	case 2:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no == 2)
			NPC->act_no = 3;

		break;

	case 3:
		if (NPC->x - 0x8000 >= currentPlayer.x || NPC->x + 0x8000 <= currentPlayer.x || NPC->y - 0x8000 >= currentPlayer.y || NPC->y + 0x8000 <= currentPlayer.y)
		{
			NPC->act_no = 4;
			NPC->ani_wait = 0;
		}
		break;

	case 4:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			--NPC->ani_no;
		}

		if (!NPC->ani_no)
			NPC->act_no = 1;

		break;

	default:
		break;
	}

	if (NPC->shock)
		NPC->rect = rect[3];
	else
		NPC->rect = rect[NPC->ani_no];
}
