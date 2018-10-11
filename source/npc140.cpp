#include <array>

#include "npc140.h"
#include "sound.h"
#include "player.h"
#include "caret.h"
#include "mathUtils.h"
#include "flash.h"
#include "weapons.h"
#include "game.h"

using std::array;

void npcAct140(npc *NPC)//rabid toroko
{
	enum states
	{
		ini = 0,
		start1 = 1,
		start2 = 2,
		start3 = 3,
		iniState = 10,
		decideState = 11,
		jumpAndThrow = 20,
		jump = 21,
		windUpAir1 = 22,
		windUpAir2 = 23,
		throwAir = 24,
		land = 25,
		landPause = 26,
		standAndThrow = 50,
		windUpGround = 51,
		throwGround = 52,
		startDie = 100,

		die = 141
	};

	switch (NPC->act_no)
	{
	case ini:
		NPC->act_no = 1;
		NPC->ani_no = 9;
		NPC->act_wait = 0;
		NPC->bits &= ~npc_shootable;
	case start1:
		if (++NPC->act_wait > 50)
		{
			NPC->act_no = start2;
			NPC->act_wait = 0;
			NPC->ani_no = 8;
		}
		break;
	case start2:
		if (++NPC->ani_no > 10)
			NPC->ani_no = 9;
		if (++NPC->act_wait > 50)
		{
			NPC->act_no = start3;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
		}
		break;
	case start3:
		if (++NPC->act_wait > 50)
		{
			NPC->act_no = 10;
			NPC->bits |= npc_shootable;
		}
		break;
	case iniState://initializes the decideState
		NPC->bits = NPC->bits;
		NPC->act_no = decideState;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->act_wait = random(20, 130);
		NPC->xm = 0;
	case decideState://waits a period of time then decides between state 20 and state 50
		NPC->facePlayer();

		//animate
		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;

		if (weaponBullets(6) || weaponBullets(3) > 3)
			NPC->act_no = 20;
		if (NPC->act_wait)
		{
			--NPC->act_wait;
		}
		else if (random(0, 99) & 1)
		{
			NPC->act_no = 20;
		}
		else
		{
			NPC->act_no = 50;
		}
		break;
	case jumpAndThrow:
		NPC->act_no = jump;
		NPC->ani_no = 2;
		NPC->act_wait = 0;
	case jump:
		if (++NPC->act_wait > 10)
		{
			NPC->act_no = windUpAir1;
			NPC->act_wait = 0;
			NPC->ani_no = 3;
			NPC->ym = -1535;
			if (NPC->direct)
				NPC->xm = 512;
			else
				NPC->xm = -512;
		}
		break;
	case windUpAir1:
		if (++NPC->act_wait > 10)
		{
			NPC->act_no = windUpAir2;
			NPC->act_wait = 0;
			NPC->ani_no = 6;
			createNpc(NPC_ProjectileTorokoBlock, 0, 0, 0, 0, 0, NPC);
		}
		break;
	case windUpAir2:
		if (++NPC->act_wait > 30)
		{
			NPC->act_no = throwAir;
			NPC->act_wait = 0;
			NPC->ani_no = 7;
		}
		NPC->facePlayer();
		break;
	case throwAir:
		if (++NPC->act_wait > 3)
		{
			NPC->act_no = land;
			NPC->ani_no = 3;
		}
		break;
	case land:
		if (NPC->flag & ground)
		{
			NPC->act_no = landPause;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
			playSound(26);
			viewport.quake = 20;
		}
		break;
	case landPause:
		NPC->xm = 8 * NPC->xm / 9;
		if (++NPC->act_wait > 20)
		{
			NPC->act_no = iniState;
			NPC->ani_no = 0;
		}
		break;
	case standAndThrow:
		NPC->act_no = windUpGround;
		NPC->act_wait = 0;
		NPC->ani_no = 4;
		createNpc(NPC_ProjectileTorokoBlock, 0, 0, 0, 0, 0, NPC);
	case windUpGround:
		if (++NPC->act_wait > 30)
		{
			NPC->act_no = throwGround;
			NPC->act_wait = 0;
			NPC->ani_no = 5;
		}
		NPC->facePlayer();
		break;
	case throwGround:
		if (++NPC->act_wait > 3)
		{
			NPC->act_no = iniState;
			NPC->ani_no = 0;
		}
		break;
	case startDie:
		NPC->ani_no = 3;
		NPC->act_no = 101;
		NPC->bits &= ~npc_shootable;
		NPC->damage = 0;
		for (int i = 0; i <= 7; ++i)
		{
			createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9),
				random(-341, 341), random(-1536, 0), 0, 0, false);
		}
		break;
	case 0x65:
		if (NPC->flag & ground)
		{
			NPC->act_no = 102;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
			playSound(26);
			viewport.quake = 20;
		}
		break;
	case 0x66:
		NPC->xm = 8 * NPC->xm / 9;
		if (++NPC->act_wait > 50)
		{
			NPC->act_no = 103;
			NPC->act_wait = 0;
			NPC->ani_no = 10;
		}
		break;
	case 0x67:
		if (++NPC->act_wait > 50)
		{
			NPC->ani_no = 9;
			NPC->act_no = 104;
			NPC->act_wait = 0;
		}
		break;
	case 0x68:
		if (++NPC->ani_no > 10)
			NPC->ani_no = 9;
		if (++NPC->act_wait > 100)
		{
			NPC->act_wait = 0;
			NPC->ani_no = 9;
			NPC->act_no = 105;
		}
		break;
	case 0x69:
		if (++NPC->act_wait > 50)
		{
			NPC->ani_wait = 0;
			NPC->act_no = 106;
			NPC->ani_no = 11;
		}
		break;
	case 0x6A:
		if (++NPC->ani_wait > 50)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 12)
			NPC->ani_no = 12;
		break;
	case 0x8C:
		NPC->act_no = die;
		NPC->act_wait = 0;
		NPC->ani_no = 12;
		playSound(29);
	case die:
		if (++NPC->ani_no > 13)
			NPC->ani_no = 12;
		if (++NPC->act_wait > 100)
		{
			for (int i = 0; i <= 3; ++i)
			{
				createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9), 
					random(-341, 341), random(-1536, 0), 0, 0, false);
			}
			NPC->cond = 0;
		}
		break;
	default:
		break;
	}
	if (NPC->act_no > 100 && NPC->act_no <= 104 && !(NPC->act_wait % 9))
	{
		createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9),
			random(-341, 341), random(-1536, 0), 0, 0, false);
	}

	NPC->doGravity(32, 1535);
	if (NPC->ym < -1535)
		NPC->ym = -1535;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->rect.left = (NPC->ani_no % 8) * 32;
	NPC->rect.top = (((NPC->ani_no / 8) * 2) * 32) + ((NPC->direct / 2) * 32);
	NPC->rect.right = NPC->rect.left + 32;
	NPC->rect.bottom = NPC->rect.top + 32;

	return;
}

void npcAct141(npc *NPC) //block toroko throws
{
	int deg = 0;

	if (NPC->act_no <= 1)
	{
		NPC->act_no = 1;
		NPC->act_wait = 0;
	}
	if (NPC->act_no == 1)
	{
		if (NPC->pNpc->direct)
			NPC->x = NPC->pNpc->x - 5120;
		else
			NPC->x = NPC->pNpc->x + 5120;
		NPC->y = NPC->pNpc->y - 4096;
		if (NPC->pNpc->act_no == 24 || NPC->pNpc->act_no == 52)
		{
			NPC->act_no = 10;
			if (NPC->pNpc->direct)
				NPC->x = NPC->pNpc->x + 0x2000;
			else
				NPC->x = NPC->pNpc->x - 0x2000;
			NPC->y = NPC->pNpc->y;
			deg = getAtan(NPC->x - currentPlayer.x, NPC->y - currentPlayer.y);
			NPC->ym = 4 * getSin(deg);
			NPC->xm = 4 * getCos(deg);
			playSound(39);
		}
	}

	if (NPC->act_no == 10)
	{
		if (NPC->flag & 0xF)
		{
			NPC->act_no = 20;
			NPC->act_wait = 0;
			createCaret(NPC->x, NPC->y, 2, 0);
			playSound(12);
			for (int i = 0; i <= 3; ++i)
			{
				createNpc(4, NPC->x, NPC->y, random(-512, 512), random(-512, 512), 0, 0, false);
			}
		}
		else
		{
			NPC->x += NPC->xm;
			NPC->y += NPC->ym;
		}
	}
	else if (NPC->act_no == 20)
	{
		NPC->x += NPC->xm;
		NPC->y += NPC->ym;

		if (++NPC->act_wait > 4)
		{
			for (int i = 0; i <= 3; ++i)
			{
				createNpc(4, NPC->x, NPC->y, random(-512, 512), random(-512, 512), 0, 0, false);
			}
			NPC->code_char = 142;
			NPC->ani_no = 0;
			NPC->act_no = 20;
			NPC->xm = 0;
			NPC->bits &= ~npc_invulnerable;
			NPC->bits |= npc_shootable;
			NPC->damage = 1;
		}
	}

	if (++NPC->ani_no > 1)
		NPC->ani_no = 0;
	NPC->rect.left = 288 + (NPC->ani_no*16);
	NPC->rect.top = 32;
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 16;

	return;
}

void npcAct142(npc *NPC) //toroko flowers
{
	enum states
	{
		ini = 10,
		prep = 11,
		lunge = 12,
		land = 20,
		landed = 21
	};
	switch (NPC->act_no)
	{
	case ini:
		NPC->act_no = prep;
		NPC->ani_no = 0;
		NPC->act_wait = 0;
	case prep:
		if (++NPC->act_wait > 30)
		{
			NPC->act_no = lunge;
			NPC->ani_no = 1;
			NPC->ani_wait = 0;
		}
		break;
	case lunge:
		if (++NPC->ani_wait > 8)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no == 3)
		{
			NPC->act_no = land;
			NPC->ym = -512;
			NPC->accelerateTowardsPlayer(512);
		}
		break;
	case land:
		if (NPC->ym < -127)
			NPC->ani_no = 3;
		else
			NPC->ani_no = 4;
		if (NPC->flag & ground)
		{
			NPC->ani_no = 2;
			NPC->act_no = landed;
			NPC->act_wait = 0;
			NPC->xm = 0;
			playSound(23);
		}
		break;
	case landed:
		if (++NPC->act_wait > 10)
		{
			NPC->act_no = ini;
			NPC->ani_no = 0;
		}
		break;
	default:
		break;
	}

	NPC->doGravity(64, 1535);
	if (NPC->ym < -1535)
		NPC->ym = -1535;
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->rect.left = NPC->ani_no*16;
	NPC->rect.top = 128;
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 16;
	return;
}

void npcAct143(npc *NPC) //Jenka collapsed
{
	RECT rcLeft[1];
	RECT rcRight[1];

	rcLeft[0].left = 208;
	rcLeft[0].top = 32;
	rcLeft[0].right = 224;
	rcLeft[0].bottom = 48;
	rcRight[0].left = 208;
	rcRight[0].top = 48;
	rcRight[0].right = 224;
	rcRight[0].bottom = 64;

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct144(npc *NPC) //Toroko teleport in
{
	RECT rcLeft[5];
	RECT rcRight[5];

	rcLeft[0] = { 0, 64, 16, 80 };
	rcLeft[1] = { 16, 64, 32, 80 };
	rcLeft[2] = { 32, 64, 48, 80 };
	rcLeft[3] = { 16, 64, 32, 80 };
	rcLeft[4] = { 128, 64, 144, 80 };
	rcRight[0] = { 0, 80, 16, 96 };
	rcRight[1] = { 16, 80, 32, 96 };
	rcRight[2] = { 32, 80, 48, 96 };
	rcRight[3] = { 16, 80, 32, 96 };
	rcRight[4] = { 128, 80, 144, 96 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->tgt_x = NPC->x;
		playSound(SFX_Teleport);
//Fallthrough
	case 1:
		if (++NPC->act_wait == 64)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
		}
		break;

	case 2:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 3)
			NPC->ani_no = 2;

		if (NPC->flag & ground)
		{
			NPC->act_no = 4;
			NPC->act_wait = 0;
			NPC->ani_no = 4;
			playSound(SFX_HitGround);
		}
		break;

	case 10:
		NPC->act_no = 11;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
//Fallthrough
	case 11:
		if (random(0, 120) == 10)
		{
			NPC->act_no = 12;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 12:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 11;
			NPC->ani_no = 0;
		}
		break;

	default:
		break;
	}

	if (NPC->act_no > 1)
	{
		NPC->ym += 0x20;
		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;

		NPC->y += NPC->ym;
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];

	if (NPC->act_no == 1)
	{
		NPC->rect.bottom = NPC->act_wait / 4 + NPC->rect.top;

		if (NPC->act_wait / 2 & 1)
			NPC->x = NPC->tgt_x;
		else
			NPC->x = NPC->tgt_x + 0x200;
	}
}

void npcAct145(npc *NPC) //King's blade
{
	RECT rcLeft = { 96, 32, 112, 48 };
	RECT rcRight = { 112, 32, 128, 48 };

	if (!NPC->act_no)
	{
		//Some orientation stuff
		if (NPC->pNpc->count2)
		{
			if (NPC->pNpc->direct != dirLeft)
				NPC->direct = dirLeft;
			else
				NPC->direct = dirRight;
		}
		else if (NPC->pNpc->direct != dirLeft)
		{
			NPC->direct = dirRight;
		}
		else
		{
			NPC->direct = dirLeft;
		}

		//Set position
		if (NPC->direct != dirLeft)
			NPC->x = NPC->pNpc->x + 0x1400;
		else
			NPC->x = NPC->pNpc->x - 0x1400;
		NPC->y = NPC->pNpc->y;
	}

	//Set framerect
	NPC->doRects(rcLeft, rcRight);
}

void npcAct146(npc *NPC) //Lightning
{
	RECT rect[5];
	rect[0] = { 0, 0, 0, 0 };
	rect[1] = { 260, 0, 275, 240 };
	rect[2] = { 275, 0, 290, 240 };
	rect[3] = { 290, 0, 305, 240 };
	rect[4] = { 305, 0, 320, 240 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;

	if (NPC->direct == 2)
		setFlash(0, 0, normal, 20);
	// Fallthrough
	case 1:
		if (++NPC->act_wait > 10)
		{
			NPC->act_no = 2;
			playSound(SFX_Lightning);
		}
		break;

	case 2:
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no == 2)
			NPC->damage = 10;

		if (NPC->ani_no > 4)
		{
			createSmokeLeft(NPC->x, NPC->y, 0x1000, 8);
			NPC->cond = 0;
		}

	default:
		break;
	}

	NPC->rect = rect[NPC->ani_no];
}

void npcAct147(npc *NPC)
{
	array<RECT, 6> rcLeft;
	array<RECT, 6> rcRight;
	uint8_t deg;

	rcLeft[0].left = 0;
	rcLeft[0].top = 96;
	rcLeft[0].right = 16;
	rcLeft[0].bottom = 112;
	rcLeft[1].left = 16;
	rcLeft[1].top = 96;
	rcLeft[1].right = 32;
	rcLeft[1].bottom = 112;
	rcLeft[2].left = 32;
	rcLeft[2].top = 96;
	rcLeft[2].right = 48;
	rcLeft[2].bottom = 112;
	rcLeft[3].left = 48;
	rcLeft[3].top = 96;
	rcLeft[3].right = 64;
	rcLeft[3].bottom = 112;
	rcLeft[4].left = 64;
	rcLeft[4].top = 96;
	rcLeft[4].right = 80;
	rcLeft[4].bottom = 112;
	rcLeft[5].left = 80;
	rcLeft[5].top = 96;
	rcLeft[5].right = 96;
	rcLeft[5].bottom = 112;

	rcRight[0].left = 0;
	rcRight[0].top = 112;
	rcRight[0].right = 16;
	rcRight[0].bottom = 128;
	rcRight[1].left = 16;
	rcRight[1].top = 112;
	rcRight[1].right = 32;
	rcRight[1].bottom = 128;
	rcRight[2].left = 32;
	rcRight[2].top = 112;
	rcRight[2].right = 48;
	rcRight[2].bottom = 128;
	rcRight[3].left = 48;
	rcRight[3].top = 112;
	rcRight[3].right = 64;
	rcRight[3].bottom = 128;
	rcRight[4].left = 64;
	rcRight[4].top = 112;
	rcRight[4].right = 80;
	rcRight[4].bottom = 128;
	rcRight[5].left = 80;
	rcRight[5].top = 112;
	rcRight[5].right = 96;
	rcRight[5].bottom = 128;

	switch (NPC->act_no)
	{
	case 0:
		NPC->y += 0x600;
		NPC->act_no = 1;
//Fallthrough
	case 1:
		if (NPC->act_wait <= 7
			|| NPC->x - 49152 >= currentPlayer.x
			|| NPC->x + 49152 <= currentPlayer.x
			|| NPC->y - 49152 >= currentPlayer.y
			|| NPC->y + 0x4000 <= currentPlayer.y)
		{
			if (NPC->act_wait < 8)
				++NPC->act_wait;

			NPC->ani_no = 0;
		}
		else
		{
			if (NPC->x <= currentPlayer.x)
				NPC->direct = 2;
			else
				NPC->direct = 0;
			NPC->ani_no = 1;
		}

		if (NPC->shock)
		{
			NPC->act_no = 2;
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}

		if (NPC->act_wait > 7 && NPC->x - 24576 < currentPlayer.x && NPC->x + 24576 > currentPlayer.x && NPC->y - 49152 < currentPlayer.y && NPC->y + 0x4000 > currentPlayer.y)
		{
			NPC->act_no = 2;
			NPC->ani_no = 0;
			NPC->act_wait = 0;
		}
		break;

	case 2:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = 3;
			NPC->ani_no = 2;
			NPC->ym = -0x5FF;

			playSound(30);

			if (NPC->x <= currentPlayer.x)
				NPC->direct = 2;
			else
				NPC->direct = 0;
		}
		break;

	case 3:
		if (NPC->ym > 0x100)
		{
			NPC->tgt_y = NPC->y;
			NPC->act_no = 4;
			NPC->ani_no = 3;
			NPC->act_wait = 0;
		}
		break;

	case 4:
		if (NPC->x >= currentPlayer.x)
			NPC->direct = 0;
		else
			NPC->direct = 2;

		++NPC->act_wait;

		if (NPC->flag & (leftWall | ceiling | rightWall) || NPC->act_wait > 60)
		{
			NPC->damage = 3;
			NPC->act_no = 5;
			NPC->ani_no = 2;
		}
		else
		{
			if (NPC->act_wait % 4 == 1)
				playSound(109);

			if (NPC->flag & ground)
				NPC->ym = -0x200;

			if (NPC->act_wait % 30 == 6)
			{
				deg = getAtan(NPC->x - currentPlayer.x, NPC->y - currentPlayer.y);
				deg += random(-6, 6);
				createNpc(NPC_ProjectileCritter, NPC->x, NPC->y, 3 * getCos(deg), 3 * getSin(deg), 0, nullptr);
				playSound(39);
			}

			if (++NPC->ani_wait > 0)
			{
				NPC->ani_wait = 0;
				++NPC->ani_no;
			}

			if (NPC->ani_no > 5)
				NPC->ani_no = 3;
		}
		break;

	case 5:
		if (NPC->flag & ground)
		{
			NPC->damage = 2;
			NPC->xm = 0;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
			NPC->act_no = 1;

			playSound(23);
		}
		break;

	default:
		break;
	}

	if (NPC->act_no == 4)
	{
		if (NPC->y <= NPC->tgt_y)
			NPC->ym += 0x10;
		else
			NPC->ym -= 0x10;

		if (NPC->ym > 0x200)
			NPC->ym = 0x200;
		if (NPC->ym < -0x200)
			NPC->ym = -0x200;
		if (NPC->xm > 0x200)
			NPC->xm = 0x200;
		if (NPC->xm < -0x200)
			NPC->xm = -0x200;
	}
	else
	{
		NPC->ym += 0x20;
		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;
	}

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);
}

void npcAct148(npc *NPC)
{
	array<RECT, 2> rect;

	if (NPC->flag & 0xFF)
	{
		createCaret(NPC->x, NPC->y, 2, 0);
		NPC->cond = 0;
	}

	NPC->y += NPC->ym;
	NPC->x += NPC->xm;

	rect[0].left = 96;
	rect[0].top = 96;
	rect[0].right = 104;
	rect[0].bottom = 104;
	rect[1].left = 104;
	rect[1].top = 96;
	rect[1].right = 112;
	rect[1].bottom = 104;

	if (++NPC->ani_no > 1)
		NPC->ani_no = 0;

	NPC->doRects(rect);

	if (++NPC->count1 > 300)
	{
		createCaret(NPC->x, NPC->y, 2, 0);
		NPC->cond = 0;
	}
}

void npcAct149(npc *NPC) //moving block
{
	switch (NPC->act_no)
	{
	case 0:
		NPC->x += 4096;
		NPC->y += 4096;
		if (NPC->direct)
			NPC->act_no = 20;
		else
			NPC->act_no = 10;
		NPC->xm = 0;
		NPC->ym = 0;
		NPC->bits |= npc_solidHard;
		break;
	case 0xA:
		NPC->bits &= 0x7F;
		NPC->damage = 0;
		if (currentPlayer.x < NPC->x + 12800 && currentPlayer.x > NPC->x - 204800 &&
			currentPlayer.y < NPC->y + 12800 && currentPlayer.y > NPC->y - 12800)
		{
			NPC->act_no = 11;
			NPC->act_wait = 0;
		}
		break;
	case 0xB:
		if (++NPC->act_wait % 10 == 6)
			playSound(107);
		if (NPC->flag & leftWall)
		{
			NPC->xm = 0;
			NPC->direct = 2;
			NPC->act_no = 20;
			viewport.quake = 10;
			playSound(26);
			for (int i = 0; i <= 3; ++i)
			{
				createNpc(NPC_Smoke, NPC->x - 0x2000, NPC->y + (random(-12, 12) << 9),
					random(-341, 341), random(-1536, 0), 0, 0, false);
			}
		}
		else
		{
			if (currentPlayer.flag & leftWall)
			{
				NPC->bits |= npc_rearTop;
				NPC->damage = 100;
			}
			else
			{
				NPC->bits &= 0x7F;
				NPC->damage = 0;
			}
			NPC->xm -= 32;
		}
		break;
	case 0x14:
		NPC->bits &= 0x7F;
		NPC->damage = 0;
		if (currentPlayer.x > NPC->x - 12800 && currentPlayer.x < NPC->x + 204800 && 
			currentPlayer.y < NPC->y + 12800 && currentPlayer.y > NPC->y - 12800)
		{
			NPC->act_no = 21;
			NPC->act_wait = 0;
		}
		break;
	case 0x15:
		if (++NPC->act_wait % 10 == 6)
			playSound(107);
		if (NPC->flag & rightWall)
		{
			NPC->xm = 0;
			NPC->direct = 0;
			NPC->act_no = 10;
			viewport.quake = 10;
			playSound(26);
			for (int i = 0; i <= 3; ++i)
			{
				createNpc(NPC_Smoke, NPC->x + 0x2000, NPC->y + (random(-12, 12) << 9), 
					random(-341, 341), random(-1536, 0), 0, 0, false);
			}
		}
		else
		{
			if (currentPlayer.flag & rightWall)
			{
				NPC->bits |= npc_rearTop;
				NPC->damage = 100;
			}
			else
			{
				NPC->bits &= 0x7F;
				NPC->damage = 0;
			}
			NPC->xm += 32;
		}
		break;
	default:
		break;
	}

	if (NPC->xm > 512)
		NPC->xm = 512;
	if (NPC->xm < -512)
		NPC->xm = -512;
	NPC->x += NPC->xm;

	NPC->rect.left = 16;
	NPC->rect.top = 0;
	NPC->rect.right = 48;
	NPC->rect.bottom = 32;
	return;
}

void npcAct150(npc *NPC) // Quote
{
	array<RECT, 9> rcLeft;
	array<RECT, 9> rcRight;

	rcLeft[0] = { 0, 0, 16, 16 };
	rcLeft[1] = { 48, 0, 64, 16 };
	rcLeft[2] = { 144, 0, 160, 16 };
	rcLeft[3] = { 16, 0, 32, 16 };
	rcLeft[4] = { 0, 0, 16, 16 };
	rcLeft[5] = { 32, 0, 48, 16 };
	rcLeft[6] = { 0, 0, 16, 16 };
	rcLeft[7] = { 160, 0, 176, 16 };
	rcLeft[8] = { 112, 0, 128, 16 };

	rcRight[0] = { 0, 16, 16, 32 };
	rcRight[1] = { 48, 16, 64, 32 };
	rcRight[2] = { 144, 16, 160, 32 };
	rcRight[3] = { 16, 16, 32, 32 };
	rcRight[4] = { 0, 16, 16, 32 };
	rcRight[5] = { 32, 16, 48, 32 };
	rcRight[6] = { 0, 16, 16, 32 };
	rcRight[7] = { 160, 16, 176, 32 };
	rcRight[8] = { 112, 16, 128, 32 };

	enum
	{
		init = 0,
		lookUp = 2,
		getFlatenned = 10,
		teleportAway = 20,
		walking = 50,
		fallingUpsideDown = 60,
		lookAway = 80,
		walkingInPlace = 100,
	};

	switch(NPC->act_no)
	{
	case init:
		NPC->act_no = 1;
		NPC->ani_no = 0;

		if (NPC->direct > 10) // Hermit Gunsmith related ?
		{
			NPC->x = currentPlayer.x;
			NPC->y = currentPlayer.y;
			NPC->direct = dirLeft;
		}
		break;

	case lookUp:
		NPC->ani_no = 1;
		break;

	case getFlatenned:
		NPC->act_no = 11;

		for (size_t i = 0; i < 4; i++)
			createNpc(NPC_Smoke, NPC->x, NPC->y, random(-0x155, 0x155), random(-0x600, 0));

		playSound(SFX_QuoteSmashIntoGround);
	// Fallthrough
	case 11:
		NPC->ani_no = 2;
		break;

	case teleportAway:
		NPC->act_no = 21;
		NPC->act_wait = 0x40;
		playSound(SFX_Teleport);
	// Fallthrough
	case 21:
		if (!--NPC->act_wait)
			NPC->cond = 0;
		break;

	case walking:
		NPC->act_no = 51;
		NPC->ani_no = 3;
		NPC->ani_wait = 0;
	// Fallthrough
	case 51:
		if (++NPC->act_wait > 4)
		{
			NPC->act_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 6)
			NPC->ani_no = 3;

		if (NPC->direct != dirLeft)
			NPC->x += 0x200;
		else
			NPC->x -= 0x200;
		break;

	case fallingUpsideDown:
		NPC->act_no = 61;
		NPC->ani_no = 7;
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
	// Fallthrough
	case 61:
		NPC->tgt_y += 0x100;
		NPC->x = NPC->tgt_x + (random(-1, 1) << 9);
		NPC->y = NPC->tgt_y + (random(-1, 1) << 9);
		break;

	case 70:
		NPC->act_no = 71;
		NPC->act_wait = 0;
		NPC->ani_no = 3;
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
		if (NPC->ani_no > 6)
			NPC->ani_no = 3;
		break;

	case lookAway:
		NPC->ani_no = 8;
		break;

	case 99:
	case walkingInPlace:
		NPC->act_no = 101;
		NPC->ani_no = 3;
		NPC->ani_wait = 0;
	// Fallthrough
	case 101:	// Fall a short distance THEN walk in place
		NPC->ym += 0x40;
		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;

		if (NPC->flag & ground)
		{
			NPC->ym = 0;
			NPC->act_no = 102;
		}
		NPC->y += NPC->ym;
		break;

	case 102:
		if (++NPC->act_wait > 8)
		{
			NPC->act_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 6)
			NPC->ani_no = 3;
		break;

	default:
		break;
	}

    NPC->doRects(rcLeft, rcRight);

	if (NPC->act_no == teleportAway + 1)
	{
		NPC->rect.bottom = NPC->rect.top + NPC->act_wait / 4;
		if (NPC->act_wait / 2 % 2)
			++NPC->rect.left;	// For the shaking effect
	}

	if (currentPlayer.equip & equip_mimigaMask)
	{
		NPC->rect.top += 32;
		NPC->rect.bottom += 32;
	}
}

void npcAct151(npc *NPC) //Blue robot standing
{
	array<RECT, 2> rcLeft;
	array<RECT, 2> rcRight;

	rcLeft[0] = { 192, 0, 208, 16 };
	rcLeft[1] = { 208, 0, 224, 16 };
	rcRight[0] = { 192, 16, 208, 32 };
	rcRight[1] = { 208, 16, 224, 32 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
//Fallthrough
	case 1:
		if (random(0, 100) == 0)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}
		break;

	case 2:
		if (++NPC->act_wait > 16)
		{
			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
	}

	NPC->doRects(rcLeft, rcRight);
}

void npcAct153(npc *NPC)
{
	array<RECT, 21> rcLeft =
	{ {
		{ 0, 0, 24, 24 },
		{ 24, 0, 48, 24 },
		{ 48, 0, 72, 24 },
		{ 0, 0, 24, 24 },
		{ 72, 0, 96, 24 },
		{ 0, 0, 24, 24 },
		{ 96, 0, 120, 24 },
		{ 120, 0, 144, 24 },
		{ 144, 0, 168, 24 },
		{ 168, 0, 192, 24 },
		{ 192, 0, 216, 24 },
		{ 216, 0, 240, 24 },
		{ 240, 0, 264, 24 },
		{ 264, 0, 288, 24 },
		{ 0, 48, 24, 72 },
		{ 24, 48, 48, 72 },
		{ 48, 48, 72, 72 },
		{ 72, 48, 96, 72 },
		{ 288, 0, 312, 24 },
		{ 24, 48, 48, 72 },
		{ 96, 48, 120, 72 }
	} };

	array<RECT, 21> rcRight =
	{ {
		{ 0, 24, 24, 48 },
		{ 24, 24, 48, 48 },
		{ 48, 24, 72, 48 },
		{ 0, 24, 24, 48 },
		{ 72, 24, 96, 48 },
		{ 0, 24, 24, 48 },
		{ 96, 24, 120, 48 },
		{ 120, 24, 144, 48 },
		{ 144, 24, 168, 48 },
		{ 168, 24, 192, 48 },
		{ 192, 24, 216, 48 },
		{ 216, 24, 240, 48 },
		{ 240, 24, 264, 48 },
		{ 264, 24, 288, 48 },
		{ 0, 72, 24, 96 },
		{ 24, 72, 48, 96 },
		{ 48, 72, 72, 96 },
		{ 72, 72, 96, 96 },
		{ 288, 24, 312, 48 },
		{ 24, 72, 48, 96 },
		{ 96, 72, 120, 96 }
	} };

	if (NPC->x <= currentPlayer.x + 0x28000 && NPC->x >= currentPlayer.x - 0x28000 && NPC->y <= currentPlayer.y + 0x1E000 && NPC->y >= currentPlayer.y - 0x1E000)
	{
		switch (NPC->act_no)
		{
		case 0:
			NPC->act_no = 1;
			NPC->xm = 0;
			NPC->ani_no = 0;
			NPC->y += 1536;
			//Fallthrough
		case 1:
			if (random(0, 100) == 1)
			{
				NPC->act_no = 2;
				NPC->ani_no = 1;
				NPC->act_wait = 0;
			}

			if (random(0, 100) == 1)
			{
				if (NPC->direct)
					NPC->direct = 0;
				else
					NPC->direct = 2;
			}

			if (random(0, 100) == 1)
				NPC->act_no = 10;

			break;

		case 2:
			if (++NPC->act_wait > 20)
			{
				NPC->act_no = 1;
				NPC->ani_no = 0;
			}
			break;

		case 10:
			NPC->act_no = 11;
			NPC->act_wait = random(25, 100);
			NPC->ani_no = 2;
			NPC->ani_wait = 0;
			//Fallthrough
		case 11:
			if (++NPC->ani_wait > 3)
			{
				NPC->ani_wait = 0;
				++NPC->ani_no;
			}

			if (NPC->ani_no > 5)
				NPC->ani_no = 2;

			if (NPC->direct)
				NPC->xm = 0x200;
			else
				NPC->xm = -0x200;

			if (NPC->act_wait)
			{
				--NPC->act_wait;
			}
			else
			{
				NPC->act_no = 1;
				NPC->ani_no = 0;
				NPC->xm = 0;
			}

			if (NPC->direct || !(NPC->flag & leftWall))
			{
				if (NPC->direct == 2)
				{
					if (NPC->flag & rightWall)
					{
						NPC->ani_no = 2;
						NPC->ym = -0x5FF;
						NPC->act_no = 20;

						if (!(currentPlayer.cond & player_removed))
							playSound(30);
					}
				}
			}
			else
			{
				NPC->ani_no = 2;
				NPC->ym = -0x5FF;
				NPC->act_no = 20;

				if (!(currentPlayer.cond & player_removed))
					playSound(30);
			}
			break;

		case 20:
			if (NPC->direct || !(NPC->flag & leftWall))
			{
				if (NPC->direct == 2 && NPC->flag & rightWall)
					++NPC->count1;
				else
					NPC->count1 = 0;
			}
			else
			{
				++NPC->count1;
			}

			if (NPC->count1 > 10)
			{
				if (NPC->direct)
					NPC->direct = 0;
				else
					NPC->direct = 2;
			}

			if (NPC->direct)
				NPC->xm = 0x100;
			else
				NPC->xm = -0x100;

			if (NPC->flag & ground)
			{
				NPC->act_no = 21;
				NPC->ani_no = 20;
				NPC->act_wait = 0;
				NPC->xm = 0;

				if (!(currentPlayer.cond & player_removed))
					playSound(23);
			}
			break;

		case 21:
			if (++NPC->act_wait > 10)
			{
				NPC->act_no = 1;
				NPC->ani_no = 0;
			}
			break;

		default:
			break;
		}

		//Gravity
		NPC->ym += 0x40;
		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;

		//Move
		NPC->x += NPC->xm;
		NPC->y += NPC->ym;

		//Set framerect
		NPC->doRects(rcLeft, rcRight);

		if (NPC->life <= 985)
		{
			NPC->code_char = 154;
			NPC->act_no = 0;
		}
	}
}

void npcAct154(npc *NPC)
{
	array<RECT, 21> rcLeft =
	{ {
		{ 0, 0, 24, 24 },
		{ 24, 0, 48, 24 },
		{ 48, 0, 72, 24 },
		{ 0, 0, 24, 24 },
		{ 72, 0, 96, 24 },
		{ 0, 0, 24, 24 },
		{ 96, 0, 120, 24 },
		{ 120, 0, 144, 24 },
		{ 144, 0, 168, 24 },
		{ 168, 0, 192, 24 },
		{ 192, 0, 216, 24 },
		{ 216, 0, 240, 24 },
		{ 240, 0, 264, 24 },
		{ 264, 0, 288, 24 },
		{ 0, 48, 24, 72 },
		{ 24, 48, 48, 72 },
		{ 48, 48, 72, 72 },
		{ 72, 48, 96, 72 },
		{ 288, 0, 312, 24 },
		{ 24, 48, 48, 72 },
		{ 96, 48, 120, 72 }
	} };

	array<RECT, 21> rcRight =
	{ {
		{ 0, 24, 24, 48 },
		{ 24, 24, 48, 48 },
		{ 48, 24, 72, 48 },
		{ 0, 24, 24, 48 },
		{ 72, 24, 96, 48 },
		{ 0, 24, 24, 48 },
		{ 96, 24, 120, 48 },
		{ 120, 24, 144, 48 },
		{ 144, 24, 168, 48 },
		{ 168, 24, 192, 48 },
		{ 192, 24, 216, 48 },
		{ 216, 24, 240, 48 },
		{ 240, 24, 264, 48 },
		{ 264, 24, 288, 48 },
		{ 0, 72, 24, 96 },
		{ 24, 72, 48, 96 },
		{ 48, 72, 72, 96 },
		{ 72, 72, 96, 96 },
		{ 288, 24, 312, 48 },
		{ 24, 72, 48, 96 },
		{ 96, 72, 120, 96 }
	} };

	switch (NPC->act_no)
	{
	case 0:
		NPC->bits &= ~npc_shootable;
		NPC->bits &= ~npc_ignoreSolid;
		NPC->damage = 0;
		NPC->act_no = 1;
		NPC->ani_no = 9;
		NPC->ym = -0x200;

		if (NPC->direct)
			NPC->xm = -0x100;
		else
			NPC->xm = 0x100;

		playSound(53);
		break;

	case 1:
		if (NPC->flag & ground)
		{
			NPC->ani_no = 10;
			NPC->ani_wait = 0;
			NPC->act_no = 2;
			NPC->act_wait = 0;
		}
		break;

	case 2:
		NPC->xm = 8 * NPC->xm / 9;

		if (++NPC->ani_wait > 3)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 11)
			NPC->ani_no = 10;

		if (++NPC->act_wait > 50)
			NPC->cond |= 8;

		break;
	}

	NPC->ym += 0x20;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	//Set framerect
	NPC->doRects(rcLeft, rcRight);
}

void npcAct155(npc *NPC) //flying gaudi
{
	unsigned __int8 deg;
	int temp = 0;

	int rcX[4] = { 0, 24, 288, 24 };
	int rcY[4] = { 48, 48, 0, 48};

	if (!NPC->isPlayerWithinDistance(163840, 122880))
		return;
	switch (NPC->act_no)
	{
	case 2:
		if (++NPC->ani_no > 3)
			NPC->ani_no = 2;
		if (++NPC->act_wait > 30)
		{
			temp = random(-6, 6) + getAtan(NPC->x - currentPlayer.x, NPC->y - currentPlayer.y);
			createNpc(NPC_ProjectileGaudiFlying, NPC->x, NPC->y, 3 * getCos(temp), 3 * getSin(temp), 0, 0, false);
			if (!(currentPlayer.cond & 2))
				playSound(39);
			NPC->act_no = 1;
			NPC->act_wait = random(70, 150);
			NPC->ani_no = 0;
			NPC->ani_wait = 0;
		}
		break;
	case 0:
		deg = random(0, 255);
		NPC->xm = getCos(deg);
		NPC->tgt_x = NPC->x + 8 * getCos(deg + 64);
		temp = random(0, 255);
		NPC->ym = getSin(temp);
		NPC->tgt_y = NPC->y + 8 * getSin(temp + 64);
		NPC->act_no = 1;
		NPC->count1 = 120;
		NPC->act_wait = random(70, 150);
		NPC->ani_no = 14;
	case 1:
		if (++NPC->ani_no > 1)
			NPC->ani_no = 0;
		if (NPC->act_wait)
		{
			--NPC->act_wait;
		}
		else
		{
			NPC->act_no = 2;
			NPC->ani_no = 2;
		}
		break;
	}

	NPC->facePlayer();

	if (NPC->tgt_x < NPC->x)
		NPC->xm -= 16;
	if (NPC->tgt_x > NPC->x)
		NPC->xm += 16;
	if (NPC->tgt_y < NPC->y)
		NPC->ym -= 16;
	if (NPC->tgt_y > NPC->y)
		NPC->ym += 16;
	if (NPC->xm > 512)
		NPC->xm = 512;
	if (NPC->xm < -512)
		NPC->xm = -512;
	if (NPC->ym > 512)
		NPC->ym = 512;
	if (NPC->ym < -512)
		NPC->ym = -512;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->rect.left = rcX[NPC->ani_no];
	NPC->rect.top = rcY[NPC->ani_no] + (NPC->direct / 2) * 24;
	NPC->rect.right = NPC->rect.left + 24;
	NPC->rect.bottom = NPC->rect.top + 24;

	if (NPC->life <= 985)
	{
		NPC->code_char = 154;
		NPC->act_no = 0;
	}
	return;
}

void npcAct156(npc *NPC)
{
	if (NPC->flag & 0xFF)
	{
		createCaret(NPC->x, NPC->y, 2, 0);
		NPC->cond = 0;
	}

	NPC->y += NPC->ym;
	NPC->x += NPC->xm;

	if (++NPC->ani_no > 2)
		NPC->ani_no = 0;

	NPC->rect.left = 96 + (NPC->ani_no*16);
	NPC->rect.top = 112;
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 16;

	if (++NPC->count1 > 300)
	{
		createCaret(NPC->x, NPC->y, 2, 0);
		NPC->cond = 0;
	}
	return;
}

void npcAct157(npc *NPC)
{
	switch (NPC->act_no)
	{
	case 0:
		NPC->x += 4096;
		NPC->y += 4096;
		if (NPC->direct)
			NPC->act_no = 20;
		else
			NPC->act_no = 10;
		NPC->xm = 0;
		NPC->ym = 0;
		NPC->bits |= npc_solidHard;
		break;
	case 0xA:
		NPC->bits &= 0x7F;
		NPC->damage = 0;
		if (currentPlayer.y < NPC->y + 12800 && currentPlayer.y > NPC->y - 204800 && 
			currentPlayer.x < NPC->x + 12800 && currentPlayer.x > NPC->x - 12800)
		{
			NPC->act_no = 11;
			NPC->act_wait = 0;
		}
		break;
	case 0xB:
		if (++NPC->act_wait % 10 == 6)
			playSound(107);
		if (NPC->flag & 2)
		{
			NPC->ym = 0;
			NPC->direct = 2;
			NPC->act_no = 20;
			viewport.quake = 10;
			playSound(26, 1);
			for (int i = 0; i <= 3; ++i)
			{
				createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y - 0x2000,
					random(-341, 341), random(-1536, 0), 0, 0, false);
			}
		}
		else
		{
			if (currentPlayer.flag & ceiling)
			{
				NPC->bits |= npc_rearTop;
				NPC->damage = 100;
			}
			else
			{
				NPC->bits &= 0x7F;
				NPC->damage = 0;
			}
			NPC->ym -= 32;
		}
		break;
	case 0x14:
		NPC->bits &= 0x7F;
		NPC->damage = 0;
		if (currentPlayer.y > NPC->y - 12800 && currentPlayer.y < NPC->y + 204800 && 
			currentPlayer.x < NPC->x + 12800 && currentPlayer.x > NPC->x - 12800)
		{
			NPC->act_no = 21;
			NPC->act_wait = 0;
		}
		break;
	case 0x15:
		if (++NPC->act_wait % 10 == 6)
			playSound(107, 1);
		if (NPC->flag & 8)
		{
			NPC->ym = 0;
			NPC->direct = 0;
			NPC->act_no = 10;
			viewport.quake = 10;
			playSound(26);
			for (int i = 0; i <= 3; ++i)
			{
				createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + 0x2000,
					random(-341, 341), random(-1536, 0), 0, 0, false);
			}
		}
		else
		{
			if (currentPlayer.flag & ground)
			{
				NPC->bits |= npc_rearTop;
				NPC->damage = 100;
			}
			else
			{
				NPC->bits &= 0x7F;
				NPC->damage = 0;
			}
			NPC->ym += 32;
		}
		break;
	default:
		break;
	}
	if (NPC->ym > 512)
		NPC->ym = 512;
	if (NPC->ym < -512)
		NPC->ym = -512;
	NPC->y += NPC->ym;
	NPC->rect.left = 16;
	NPC->rect.top = 0;
	NPC->rect.right = 48;
	NPC->rect.bottom = 32;
	return;
}