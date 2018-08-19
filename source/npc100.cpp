#include "npc100.h"

#include "player.h"
#include "sound.h"

void npcAct106(npc *NPC) // Speech balloon 'Hey' high
{
	if (NPC->act_no == 0)
	{
		createNpc(NPC_BalloonHeyLow, NPC->x, NPC->y - 0x1000, 0, 0, dirLeft, nullptr);
		NPC->act_no = 1;
	}
}

void npcAct111(npc *NPC) //Quote teleport out
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 16, 0, 32, 16 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 16, 16, 32, 32 };
	
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->y -= 0x2000;
		break;

	case 1:
		if (++NPC->act_wait > 20)
		{
			NPC->act_wait = 0;
			NPC->act_no = 2;
			NPC->ani_no = 1;
			NPC->ym = -0x2FF;
		}
		break;

	case 2:
		if (NPC->ym > 0)
			NPC->hit.bottom = 0x2000;

		if (NPC->flag & ground)
		{
			NPC->act_no = 3;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
		}

		break;
	case 3:
		if (++NPC->act_wait > 40)
		{
			NPC->act_no = 4;
			NPC->act_wait = 64;
			playSound(SFX_Teleport);
		}
		break;
	case 4:
		--NPC->act_wait;
		NPC->ani_no = 0;

		if (!NPC->act_wait)
			NPC->cond = 0;
		break;
	default:
		break;
	}

	NPC->ym += 0x40;
	NPC->y += NPC->ym;

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];

	if (currentPlayer.equip & equip_mimigaMask)
	{
		NPC->rect.top += 32;
		NPC->rect.bottom += 32;
	}

	if (NPC->act_no == 4)
	{
		NPC->rect.bottom = NPC->act_wait / 4 + NPC->rect.top;

		if (NPC->act_wait / 2 & 1)
			++NPC->rect.left;
	}
}

void npcAct112(npc *NPC) //Quote teleport in
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 16, 0, 32, 16 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 16, 16, 32, 32 };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->x += 0x2000;
		NPC->y += 4096;
		playSound(SFX_Teleport);
	}

	switch (NPC->act_no)
	{
		case 1:
			if (++NPC->act_wait >= 64)
			{
				NPC->act_no = 2;
				NPC->act_wait = 0;
			}
			break;

		case 2:
			if (++NPC->act_wait > 20)
			{
				NPC->act_no = 3;
				NPC->ani_no = 1;
				NPC->hit.bottom = 0x1000;
			}
			break;

		case 3:
			if (NPC->flag & ground)
			{
				NPC->act_no = 4;
				NPC->act_wait = 0;
				NPC->ani_no = 0;
			}
			break;
	}

	NPC->ym += 64;
	NPC->y += NPC->ym;

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];

	if (currentPlayer.equip & equip_mimigaMask)
	{
		NPC->rect.top += 32;
		NPC->rect.bottom += 32;
	}

	if (NPC->act_no == 1)
	{
		NPC->rect.bottom = NPC->act_wait / 4 + NPC->rect.top;

		if (NPC->act_wait / 2 & 1)
			++NPC->rect.left;
	}
}

void npcAct117(npc *NPC)
{
	RECT rcLeft[10];
	RECT rcRight[10];

	rcLeft[0] = { 0x00, 0x60, 0x10, 0x70 };
	rcLeft[1] = { 0x10, 0x60, 0x20, 0x70 };
	rcLeft[2] = { 0x00, 0x60, 0x10, 0x70 };
	rcLeft[3] = { 0x20, 0x60, 0x30, 0x70 };
	rcLeft[4] = { 0x00, 0x60, 0x10, 0x70 };
	rcLeft[5] = { 0xB0, 0x60, 0xC0, 0x70 };
	rcLeft[6] = { 0x70, 0x60, 0x80, 0x70 };
	rcLeft[7] = { 0xA0, 0x60, 0xB0, 0x70 };
	rcLeft[8] = { 0x90, 0x60, 0xA0, 0x70 };
	rcLeft[9] = { 0x30, 0x60, 0x40, 0x70 };

	rcRight[0] = { 0x00, 0x70, 0x10, 0x80 };
	rcRight[1] = { 0x10, 0x70, 0x20, 0x80 };
	rcRight[2] = { 0x00, 0x70, 0x10, 0x80 };
	rcRight[3] = { 0x20, 0x70, 0x30, 0x80 };
	rcRight[4] = { 0x00, 0x70, 0x10, 0x80 };
	rcRight[5] = { 0xB0, 0x70, 0xC0, 0x80 };
	rcRight[6] = { 0x70, 0x70, 0x80, 0x80 };
	rcRight[7] = { 0xA0, 0x70, 0xB0, 0x80 };
	rcRight[8] = { 0x90, 0x70, 0xA0, 0x80 };
	rcRight[9] = { 0x30, 0x70, 0x40, 0x80 };

	switch (NPC->act_no)
	{
	case 0: //Stand
		//Look towards player if direction is 4
		if (NPC->direct == dirCenter)
		{
			if (NPC->x <= currentPlayer.x)
				NPC->direct = dirRight;
			else
				NPC->direct = dirLeft;
		}
		
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		// Fallthrough
	case 1:
		NPC->xm = 0;
		NPC->ym += 0x40;
		break;

	case 3: //Walking
		NPC->act_no = 4;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
		// Fallthrough
	case 4:
		//Animate
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		//Gravity
		NPC->ym += 0x40;

		//Walk in facing direction
		if (NPC->direct != dirLeft)
			NPC->xm = 0x200;
		else
			NPC->xm = -0x200;

		break;

	case 5: //Hit ground with poof
		NPC->act_no = 6;
		NPC->ani_no = 5;
		createSmoke(NPC->x, NPC->y, NPC->view.right, 8);
		break;

	case 6: //Lay on ground
		NPC->ani_no = 5;
		break;

	case 10: //Walk towards player
		NPC->act_no = 11;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;

		//Face towards player
		if (NPC->x <= currentPlayer.x)
			NPC->direct = dirRight;
		else
			NPC->direct = dirLeft;
		// Fallthrough
	case 11:
		//Animate
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		//Move forward
		if (NPC->direct != dirLeft)
			NPC->x += 512;
		else
			NPC->x -= 512;

		//Stop when close to the player
		if (NPC->x + 0x2800 > currentPlayer.x && NPC->x - 0x2800 < currentPlayer.x)
			NPC->act_no = 0;

		break;

	case 20: //Looking away
		NPC->xm = 0;
		NPC->ani_no = 6;
		break;

	case 21: //Looking up
		NPC->xm = 0;
		NPC->ani_no = 9;
		break;

	case 30: //Falling over
		NPC->act_no = 31;
		NPC->act_wait = 0;
		NPC->ym = -1024;
		// Fallthrough
	case 31:
		NPC->ani_no = 7;

		//Move opposite to facing direction
		if (NPC->direct != dirLeft)
			NPC->xm = -0x200;
		else
			NPC->xm = 0x200;

		//Gravity
		NPC->ym += 0x40;

		//Check if Curly's on the ground
		if (++NPC->act_wait > 1 && NPC->flag & ground)
			NPC->act_no = 32;

		break;

	case 32: //Fallen
		NPC->ym += 0x40;
		NPC->ani_no = 8;
		NPC->xm = 0;
		break;

	case 70: //Walk backwards slowly (Seal Chamber)
		NPC->act_no = 71;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
		// Fallthrough
	case 71:
		if (NPC->direct != dirLeft)
			NPC->x -= 0x100;
		else
			NPC->x += 0x100;

		if (++NPC->ani_wait > 8)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 4)
			NPC->ani_no = 1;
		break;

	default:
		break;
	}

	//Move and speed limit
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;
}

void npcAct116(npc *NPC) // Red flowers petals
{
	NPC->rect = { 272, 184, 320, 200 };
}

void npcAct119(npc *NPC) // Table & Chair
{
	NPC->rect = { 248, 184, 272, 200 };
}
