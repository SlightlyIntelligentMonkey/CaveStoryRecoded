#include "npc160.h"
#include "sound.h"
#include "player.h"
#include "caret.h"
#include "mathUtils.h"

void npcAct171(npc *NPC) //Fire whirr
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

void npcAct172(npc *NPC) //Fire whirr projectile
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

void npcAct173(npc *NPC) //Armoured Gaudi
{
	RECT rcLeft[4];
	RECT rcRight[4];
	uint8_t deg;

	rcLeft[0].left = 0;
	rcLeft[0].top = 128;
	rcLeft[0].right = 24;
	rcLeft[0].bottom = 152;
	rcLeft[1].left = 24;
	rcLeft[1].top = 128;
	rcLeft[1].right = 48;
	rcLeft[1].bottom = 152;
	rcLeft[2].left = 48;
	rcLeft[2].top = 128;
	rcLeft[2].right = 72;
	rcLeft[2].bottom = 152;
	rcLeft[3].left = 72;
	rcLeft[3].top = 128;
	rcLeft[3].right = 96;
	rcLeft[3].bottom = 152;
	rcRight[0].left = 0;
	rcRight[0].top = 152;
	rcRight[0].right = 24;
	rcRight[0].bottom = 176;
	rcRight[1].left = 24;
	rcRight[1].top = 152;
	rcRight[1].right = 48;
	rcRight[1].bottom = 176;
	rcRight[2].left = 48;
	rcRight[2].top = 152;
	rcRight[2].right = 72;
	rcRight[2].bottom = 176;
	rcRight[3].left = 72;
	rcRight[3].top = 152;
	rcRight[3].right = 96;
	rcRight[3].bottom = 176;

	if (NPC->x <= currentPlayer.x + 163840 && NPC->x >= currentPlayer.x - 163840 && NPC->y <= currentPlayer.y + 122880 && NPC->y >= currentPlayer.y - 122880)
	{
		switch (NPC->act_no)
		{
		case 0:
			NPC->tgt_x = NPC->x;
			NPC->act_no = 1;
//Fallthrough
		case 1:
			NPC->ani_no = 0;
			NPC->xm = 0;

			if (NPC->act_wait > 4)
			{
				if (NPC->x - 98304 < currentPlayer.x && NPC->x + 98304 > currentPlayer.x && NPC->y - 81920 < currentPlayer.y && NPC->y + 81920 > currentPlayer.y)
				{
					NPC->act_no = 10;
					NPC->act_wait = 0;
					NPC->ani_no = 1;
				}
			}
			else
			{
				++NPC->act_wait;
			}
			break;

		case 10:
			if (++NPC->act_wait > 3)
			{
				if (++NPC->count1 == 3)
				{
					playSound(30);
					NPC->count1 = 0;
					NPC->act_no = 25;
					NPC->act_wait = 0;
					NPC->ani_no = 2;
					NPC->ym = -0x600;

					if (NPC->x >= NPC->tgt_x)
						NPC->xm = -0x80;
					else
						NPC->xm = 0x80;
				}
				else
				{
					playSound(30);
					NPC->act_no = 20;
					NPC->ani_no = 2;
					NPC->ym = -0x200;

					if (NPC->x >= NPC->tgt_x)
						NPC->xm = -0x200;
					else
						NPC->xm = 0x200;
				}
			}
			break;
		case 20:
			++NPC->act_wait;

			if (NPC->flag & ground)
			{
				playSound(23);
				NPC->ani_no = 1;
				NPC->act_no = 30;
				NPC->act_wait = 0;
			}
			break;

		case 25:
			if (++NPC->act_wait == 30 || NPC->act_wait == 40)
			{
				deg = getAtan(NPC->x - currentPlayer.x, NPC->y - currentPlayer.y);
				deg += random(-6, 6);
				createNpc(174, NPC->x, NPC->y, 3 * getCos(deg), 3 * getSin(deg), 0, nullptr);
				playSound(39);

				NPC->ani_no = 3;
				curlyShootWait = random(80, 100);
				curlyShootX = NPC->x;
				curlyShootY = NPC->y;
			}

			if (NPC->act_wait == 35 || NPC->act_wait == 45)
				NPC->ani_no = 2;

			if (NPC->flag & ground)
			{
				playSound(23);
				NPC->ani_no = 1;
				NPC->act_no = 30;
				NPC->act_wait = 0;
			}
			break;

		case 30:
			NPC->xm = 7 * NPC->xm / 8;

			if (++NPC->act_wait > 3)
			{
				NPC->ani_no = 0;
				NPC->act_no = 1;
				NPC->act_wait = 0;
			}
			break;

		default:
			break;
		}

		if (currentPlayer.x >= NPC->x)
			NPC->direct = 2;
		else
			NPC->direct = 0;

		NPC->ym += 0x33;

		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;
		if (NPC->ym < -0x5FF)
			NPC->ym = 0x5FF;

		NPC->x += NPC->xm;
		NPC->y += NPC->ym;

		if (NPC->direct)
			NPC->rect = rcRight[NPC->ani_no];
		else
			NPC->rect = rcLeft[NPC->ani_no];

		if (NPC->life <= 985)
		{
			createSmokeLeft(NPC->x, NPC->y, 0, 2);
			NPC->code_char = 154;
			NPC->act_no = 0;
		}
	}
}

void npcAct174(npc *NPC)
{
	RECT rect[3];
	rect[0].left = 120;
	rect[0].top = 80;
	rect[0].right = 136;
	rect[0].bottom = 96;
	rect[1].left = 136;
	rect[1].top = 80;
	rect[1].right = 152;
	rect[1].bottom = 96;
	rect[2].left = 152;
	rect[2].top = 80;
	rect[2].right = 168;
	rect[2].bottom = 96;

	bool bHit;

	switch (NPC->act_no)
	{
	case 0:
		if (NPC->direct == 2)
			NPC->act_no = 2;
//Fallthrough
	case 1:
		NPC->x += NPC->xm;
		NPC->y += NPC->ym;

		bHit = false;

		if (NPC->flag & leftWall)
		{
			bHit = true;
			NPC->xm = 0x200;
		}

		if (NPC->flag & rightWall)
		{
			bHit = true;
			NPC->xm = -0x200;
		}

		if (NPC->flag & ceiling)
		{
			bHit = true;
			NPC->ym = 0x200;
		}

		if (NPC->flag & ground)
		{
			bHit = true;
			NPC->ym = -0x200;
		}

		if (bHit)
		{
			NPC->act_no = 2;
			++NPC->count1;
			playSound(31, 1);
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

	if (++NPC->ani_no > 2)
		NPC->ani_no = 0;

	NPC->rect = rect[NPC->ani_no];
}

void npcAct175(npc *NPC)
{
	RECT rcLeft[2];
	RECT rcRight[2];
	
	rcLeft[0].left = 168;
	rcLeft[0].top = 80;
	rcLeft[0].right = 192;
	rcLeft[0].bottom = 104;
	rcLeft[1].left = 192;
	rcLeft[1].top = 80;
	rcLeft[1].right = 216;
	rcLeft[1].bottom = 104;

	rcRight[0].left = 216;
	rcRight[0].top = 80;
	rcRight[0].right = 240;
	rcRight[0].bottom = 104;
	rcRight[1].left = 240;
	rcRight[1].top = 80;
	rcRight[1].right = 264;
	rcRight[1].bottom = 104;

	if (NPC->act_no <= 2 && NPC->life <= 89)
	{
		killNpc(NPC, 0);
		NPC->act_no = 10;
		NPC->ani_no = 1;
		NPC->bits &= ~npc_shootable;
		NPC->damage = 0;
	}

	if (!NPC->act_no)
	{
		NPC->ani_no = 0;
		NPC->act_no = 1;
	}

	if (NPC->direct)
		NPC->ym -= 0x20;
	else
		NPC->ym += 0x20;

	if (NPC->ym < -0x5FF)
		NPC->ym = -0x5FF;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->y += NPC->ym;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct176(npc *NPC)
{
	RECT rcRight[3];
	RECT rcLeft[3];

	rcLeft[0].left = 96;
	rcLeft[0].top = 128;
	rcLeft[0].right = 128;
	rcLeft[0].bottom = 144;
	rcLeft[1].left = 128;
	rcLeft[1].top = 128;
	rcLeft[1].right = 160;
	rcLeft[1].bottom = 144;
	rcLeft[2].left = 160;
	rcLeft[2].top = 128;
	rcLeft[2].right = 192;
	rcLeft[2].bottom = 144;
	rcRight[0].left = 96;
	rcRight[0].top = 144;
	rcRight[0].right = 128;
	rcRight[0].bottom = 160;
	rcRight[1].left = 128;
	rcRight[1].top = 144;
	rcRight[1].right = 160;
	rcRight[1].bottom = 160;
	rcRight[2].left = 160;
	rcRight[2].top = 144;
	rcRight[2].right = 192;
	rcRight[2].bottom = 160;

	if (NPC->act_no <= 2 && NPC->life <= 939)
	{
		killNpc(NPC, 0);
		NPC->act_no = 10;
		NPC->ani_no = 2;
		NPC->bits &= ~npc_shootable;
		NPC->damage = 0;
	}
	
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
//Fallthrough
	case 1:
		if (NPC->direct)
		{
			if (NPC->x < currentPlayer.x + 0x14000 && NPC->x > currentPlayer.x - 0x14000 && NPC->y < currentPlayer.y + 0x2000 && NPC->y > currentPlayer.y - 0x14000)
				++NPC->count1;
		}
		else if (NPC->x < currentPlayer.x + 0x14000 && NPC->x > currentPlayer.x - 0x14000 && NPC->y < currentPlayer.y + 0x14000 && NPC->y > currentPlayer.y - 0x2000)
		{
			++NPC->count1;
		}

		if (NPC->count1 > 10)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
		}
		break;

	case 2:
		if (++NPC->ani_wait > 3)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 1)
			NPC->ani_no = 0;

		if (++NPC->act_wait > 10)
		{
			if (++NPC->count2 <= 2)
			{
				NPC->count1 = -10;
			}
			else
			{
				NPC->count2 = 0;
				NPC->count1 = -90;
			}

			if (NPC->direct)
				createNpc(177, NPC->x, NPC->y + 4096, 0, 0, 2, nullptr);
			else
				createNpc(177, NPC->x, NPC->y - 4096, 0, 0, 0, nullptr);
			playSound(39);

			NPC->act_no = 0;
			NPC->ani_no = 0;

			curlyShootWait = random(80, 100);
			curlyShootX = NPC->x;
			curlyShootY = NPC->y;
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

void npcAct177(npc *NPC)
{
	RECT rect[2];

	rect[0].left = 192;
	rect[0].top = 128;
	rect[0].right = 208;
	rect[0].bottom = 144;
	rect[1].left = 208;
	rect[1].top = 128;
	rect[1].right = 224;
	rect[1].bottom = 144;

	if (NPC->flag & 0xFF)
	{
		createCaret(NPC->x, NPC->y, 3, 0);
		NPC->cond = 0;
		return;
	}

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;

		if (NPC->direct)
			NPC->ym = 0x600;
		else
			NPC->ym = -0x600;
//Fallthrough
	case 1:
		if (NPC->y < currentPlayer.y + 0x2000 && NPC->y > currentPlayer.y - 0x2000)
		{
			NPC->act_no = 10;
			NPC->tgt_x = NPC->x;
			NPC->tgt_y = NPC->y;

			if (NPC->x <= currentPlayer.x)
				NPC->direct = 2;
			else
				NPC->direct = 0;

			NPC->xm = (2 * random(0, 1) - 1) << 9;
			NPC->ym = (2 * random(0, 1) - 1) << 9;
		}
		break;

	case 10:
		if (NPC->x >= NPC->tgt_x)
			NPC->xm -= 0x20;
		else
			NPC->xm += 0x20;
		
		if (NPC->y >= NPC->tgt_y)
			NPC->ym -= 0x20;
		else
			NPC->ym += 0x20;
		
		if (++NPC->act_wait > 300)
		{
			createCaret(NPC->x, NPC->y, 3, 0);
			NPC->cond = 0;
			return;
		}

		if (NPC->direct)
			NPC->tgt_x += 512;
		else
			NPC->tgt_x -= 512;
		break;

	default:
		break;
	}

	if (NPC->xm > 0x400)
		NPC->xm = 0x400;
	if (NPC->xm < -0x400)
		NPC->xm = -0x400;
	if (NPC->ym > 0x400)
		NPC->ym = 0x400;
	if (NPC->ym < -0x400)
		NPC->ym = -0x400;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (++NPC->ani_wait > 6)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 1)
		NPC->ani_no = 0;

	NPC->rect = rect[NPC->ani_no];
}
