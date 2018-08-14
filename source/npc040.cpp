#include "npc040.h"

void npcAct046(npc *NPC) //H/V trigger
{
	NPC->bits |= npc_eventtouch;

	if (NPC->direct)
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

	NPC->rect = { 0, 0, 16, 16 };
}

void npcAct052(npc *NPC)
{
	NPC->rect = { 240, 96, 256, 112 };
}

void npcAct058(npc *NPC) //Basu 1
{
	int action = NPC->act_no;
	
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
			NPC->ym = -256;

			NPC->tgt_x = NPC->x;
			NPC->tgt_y = NPC->y;

			NPC->act_no = 1;
			NPC->act_wait = 0;

			NPC->count1 = NPC->direct; //a
			NPC->count2 = 0;

			NPC->damage = 6;

			if (NPC->direct)
			{
				NPC->x = currentPlayer.x - 0x20000;
				NPC->xm = 767;
			}
			else
			{
				NPC->x = currentPlayer.x + 0x20000;
				NPC->xm = -767;
			}
		}
	}
	else if (action)
	{
		if (action == 1)
		{
			//Face towards player
			if (NPC->x <= currentPlayer.x)
			{
				NPC->direct = 2;
				NPC->xm += 16;
			}
			else
			{
				NPC->direct = 0;
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

						int fireXm = 2 * getCos(deg);
						int fireYm = 2 * getSin(deg);

						createNpc(84, NPC->x, NPC->y, fireXm, fireYm, 0, nullptr);
						playSound(39);
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

			if (NPC->direct)
				NPC->rect = { 192 + (NPC->ani_no * 24), 24, 216 + (NPC->ani_no * 24), 48 };
			else
				NPC->rect = { 192 + (NPC->ani_no * 24), 0, 216 + (NPC->ani_no * 24), 24 };
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
	RECT *setRect;
	RECT rect[4];

	rect[0] = { 224, 16, 240, 40 };
	rect[1] = { 208, 80, 224, 104 };
	rect[2] = { 224, 80, 240, 104 };
	rect[3] = { 240, 80, 256, 104 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;

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

	if (NPC->act_no == 1) //replaces some dumb label and goto ewww acts the same anyways.
	{
		if (NPC->x - 0x8000 < currentPlayer.x && NPC->x + 0x8000 > currentPlayer.x && NPC->y - 0x8000 < currentPlayer.y && NPC->y + 0x8000 > currentPlayer.y)
		{
			NPC->act_no = 2;
			NPC->ani_wait = 0;
		}
	}

	if (NPC->shock)
		setRect = &rect[3];
	else
		setRect = &rect[NPC->ani_no];

	NPC->rect = { setRect->left, setRect->top, setRect->right, setRect->bottom };
}
