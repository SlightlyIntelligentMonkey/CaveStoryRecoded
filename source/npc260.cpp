#include "npc260.h"

#include "mathUtils.h"

void npcAct278(npc * NPC)
{
	constexpr RECT rcLittleMan[2] = { {0, 120, 8, 128}, {8, 120, 16, 128} };
	constexpr RECT rcLittleWoman[2] = { {16, 120, 24, 128}, {24, 120, 32, 128} };
	constexpr RECT rcLittleChild[2] = { {32, 120, 40, 128}, {40, 120, 48, 128} };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
		// Fallthrough
	case 1:
		if (random(0, 60) == 1)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		if (random(0, 60) == 1)
		{
			NPC->act_no = 10;
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

	case 10:
		NPC->act_no = 11;
		NPC->act_wait = random(0, 16);
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->direct = random(0, 1) ? dirLeft : dirRight;
		// Fallthrough
	case 11:
		if (NPC->direct != dirLeft || !(NPC->flag & leftWall))
		{
			if (NPC->direct == dirRight && NPC->flag & rightWall)
				NPC->direct = dirLeft;
		}
		else
			NPC->direct = dirRight;

		if (NPC->direct != dirLeft)
			NPC->xm = 0x100;
		else
			NPC->xm = -0x100;

		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;

		if (++NPC->act_wait > 0x20)
			NPC->act_no = 0;
		break;
		
	default:
		break;
	}

	NPC->ym += 0x20;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->code_event == 200)
		NPC->rect = rcLittleMan[NPC->ani_no];
	else if (NPC->code_event == 210)
		NPC->rect = rcLittleWoman[NPC->ani_no];
	else
		NPC->rect = rcLittleChild[NPC->ani_no];
}
