#include "npc160.h"
#include "sound.h"
#include "player.h"
#include "caret.h"
#include "mathUtils.h"

void npcAct171(npc *NPC)
{
	RECT rcRight[2];
	RECT rcLeft[2];
	
	rcLeft[0].left = 120;
	rcLeft[0].top = 48;
	rcLeft[0].right = 152;
	rcLeft[0].bottom = 80;
	rcLeft[1].left = 152;
	rcLeft[1].top = 48;
	rcLeft[1].right = 184;
	rcLeft[1].bottom = 80;
	rcRight[0].left = 184;
	rcRight[0].top = 48;
	rcRight[0].right = 216;
	rcRight[0].bottom = 80;
	rcRight[1].left = 216;
	rcRight[1].top = 48;
	rcRight[1].right = 248;
	rcRight[1].bottom = 80;

	if (NPC->x <= currentPlayer.x)
		NPC->direct = 2;
	else
		NPC->direct = 0;

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->act_wait = random(0, 50);
		NPC->tgt_y = NPC->y;
		break;

	case 1:
		if (NPC->act_wait)
		{
			--NPC->act_wait;
		}
		else
		{
			NPC->act_no = 10;
			NPC->ym = 0x200;
		}
		break;

	default:
		break;
	}
	
	if (++NPC->ani_wait > 0)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 1)
		NPC->ani_no = 0;

	if (NPC->y >= NPC->tgt_y)
		NPC->ym -= 0x10;
	else
		NPC->ym += 0x10;

	if (NPC->ym > 0x200)
		NPC->ym = 0x200;
	if (NPC->ym < -0x200)
		NPC->ym = -0x200;

	NPC->y += NPC->ym;

	if (NPC->direct)
	{
		if (currentPlayer.y < NPC->y + 40960 && currentPlayer.y > NPC->y - 40960 && currentPlayer.x < NPC->x + 81920 && currentPlayer.x > NPC->x)
			++NPC->count1;
	}
	else if (currentPlayer.y < NPC->y + 40960 && currentPlayer.y > NPC->y - 40960 && currentPlayer.x < NPC->x && currentPlayer.x > NPC->x - 81920)
	{
		++NPC->count1;
	}

	if (NPC->count1 > 20)
	{
		createNpc(172, NPC->x, NPC->y, 0, 0, NPC->direct, nullptr);
		NPC->count1 = -100;
		curlyShootWait = random(80, 100);
		curlyShootX = NPC->x;
		curlyShootY = NPC->y;
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct172(npc *NPC)
{
	RECT rect[3];

	rect[0].left = 248;
	rect[0].top = 48;
	rect[0].right = 264;
	rect[0].bottom = 80;
	rect[1].left = 264;
	rect[1].top = 48;
	rect[1].right = 280;
	rect[1].bottom = 80;
	rect[2].left = 280;
	rect[2].top = 48;
	rect[2].right = 296;
	rect[2].bottom = 80;
	
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
//Fallthrough
	case 1:
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 2)
			NPC->ani_no = 0;

		if (NPC->direct)
			NPC->x += 512;
		else
			NPC->x -= 512;

		if (NPC->flag & (leftWall | rightWall))
		{
			createCaret(NPC->x, NPC->y, 2, 0);
			NPC->init(3, NPC->x, NPC->y, 0, 0, 0, nullptr);
			return;
		}

	default:
		break;
	}

	NPC->rect = rect[NPC->ani_no];
}
