#include "npc160.h"

#include <array>
#include "sound.h"
#include "player.h"
#include "caret.h"
#include "mathUtils.h"
#include "game.h"

using std::array;

void npcAct160(npc *NPC) //poo black
{
	enum states
	{
		ini = 0,
		air = 1,
		land = 2,
		landed = 3,
		jump = 4,
		prep = 5,
		inCeiling = 6
	};

	switch (NPC->act_no)
	{
	case ini:
		NPC->bits &= ~npc_solidSoft;
		NPC->act_no = air;
		// Fallthrough
	case air:
		NPC->facePlayer();
		NPC->ym = 2560;
		if (NPC->y > 0xFFFF)
		{
			NPC->bits &= ~npc_ignoreSolid;
			NPC->act_no = land;
		}
		else
		{
			++NPC->count1;
		}
		break;
	case land:
		NPC->ym = 2560;
		if (NPC->flag & ground)
		{
			killNpcsByType(NPC_ProjectilePoohBlack, 1);
			for (int i = 0; i <= 3; ++i)
			{
				createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9),
					NPC->y + (random(-12, 12) << 9), random(-341, 341), random(-1536, 0));
			}
			NPC->act_no = landed;
			NPC->act_wait = 0;
			viewport.quake = 30;
			playSound(26);
			playSound(72);
		}

		if (NPC->y < currentPlayer.y && currentPlayer.flag & ground)
			NPC->damage = 20;
		else
			NPC->damage = 0;

		break;
	case landed:
		NPC->damage = 20;
		NPC->damage = 0;
		if (++NPC->act_wait > 24)
		{
			NPC->act_no = jump;
			NPC->count1 = 0;
			NPC->count2 = 0;
		}
		break;
	case jump:
		superXPos = NPC->x;
		superYPos = NPC->y;
		if (NPC->shock & 1)
		{
			createNpc(NPC_ProjectilePoohBlack, NPC->x + (random(-12, 12) << 9),
				NPC->y + (random(-12, 12) << 9), random(-1536, 1536),
				random(-1536, 1536));
			if (++NPC->count1 > 30)
			{
				NPC->count1 = 0;
				NPC->act_no = prep;
				NPC->ym = -3072;
				NPC->bits |= npc_ignoreSolid;
			}
		}
		break;
	case prep:
		superXPos = NPC->x;
		superYPos = NPC->y;
		if (++NPC->count1 > 60)
		{
			NPC->count1 = 0;
			NPC->act_no = inCeiling;
		}
		break;
	case inCeiling:
		superXPos = currentPlayer.x;
		superYPos = 3276800;
		if (++NPC->count1 > 110)
		{
			NPC->count1 = 10;
			NPC->x = currentPlayer.x;
			NPC->y = 0;
			NPC->ym = 1535;
			NPC->act_no = air;
		}
		break;
	default:
		break;
	}
	NPC->y += NPC->ym;
	switch (NPC->act_no)
	{
	case 0:
	case 1:
		NPC->ani_no = 3;
		break;
	case 2:
		NPC->ani_no = 3;
		break;
	case 3:
		NPC->ani_no = 2;
		break;
	case 4:
		NPC->ani_no = 0;
		break;
	case 5:
		NPC->ani_no = 3;
		break;
	case 6:
		NPC->ani_no = 3;
		break;
	default:
		break;
	}

	NPC->rect.left = NPC->ani_no * 40;
	NPC->rect.top = (NPC->direct / 2) * 24;
	NPC->rect.right = NPC->rect.left + 40;
	NPC->rect.bottom = NPC->rect.top + 24;
	return;
}

void npcAct161(npc *NPC) // poos projectiles
{
	NPC->exp = 0;
	if (NPC->x >= superXPos)
		NPC->xm -= 64;
	else
		NPC->xm += 64;

	if (NPC->y >= superYPos)
		NPC->ym -= 64;
	else
		NPC->ym += 64;

	if (NPC->xm < -4605)
		NPC->xm = -4605;
	if (NPC->xm > 4605)
		NPC->xm = 4605;
	if (NPC->ym < -4605)
		NPC->ym = -4605;
	if (NPC->ym > 4605)
		NPC->ym = 4605;
	if (NPC->life <= 99)
	{
		NPC->bits &= ~npc_shootable;
		NPC->bits &= ~npc_invulnerable;
		NPC->damage = 0;
		NPC->ani_no = 2;
	}
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;
	if (NPC->ani_no <= 1)
		NPC->ani_no = random(0, 10) & 1;

	NPC->rect.left = NPC->ani_no*16;
	NPC->rect.top = 48;
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 16;
	return;
}

void npcAct162(npc *NPC) //dying poo black
{
	if (NPC->act_no == 1)
	{
	LABEL_18:
		if (!(++NPC->count1 & 3))
		{
			createNpc(NPC_ProjectilePoohBlack, NPC->x + (random(-12, 12) << 9),
				NPC->y + (random(-12, 12) << 9));
		}
		if (NPC->count1 > 160)
		{
			NPC->count1 = 0;
			NPC->act_no = 2;
			NPC->tgt_y = NPC->y;
		}
		goto LABEL_34;
	}
	if (NPC->act_no <= 1)
	{
		if (NPC->act_no)
			goto LABEL_34;
		killNpcsByType(NPC_ProjectilePoohBlack, 1);
		playSound(72);
		for (int i = 0; i <= 9; ++i)
		{
			createNpc(NPC_Smoke,
				NPC->x + (random(-12, 12) << 9),
				NPC->y + (random(-12, 12) << 9),
				random(-1536, 1536), random(-1536, 1536));
		}

		NPC->facePlayer();

		if (NPC->direct)
		{
			NPC->rect.left = 40;
			NPC->rect.top = 24;
			NPC->rect.right = 80;
			NPC->rect.bottom = 48;
		}
		else
		{
			NPC->rect.left = 40;
			NPC->rect.top = 0;
			NPC->rect.right = 80;
			NPC->rect.bottom = 24;
		}
		NPC->count1 = 0;
		NPC->act_no = 1;
		goto LABEL_18;
	}
	if (NPC->act_no == 2)
	{
		viewport.quake = 2;
		if (++NPC->count1 > 240)
		{
			NPC->rect.left = 0;
			NPC->rect.top = 0;
			NPC->rect.right = 0;
			NPC->rect.bottom = 0;
			NPC->count1 = 0;
			NPC->act_no = 3;
		}
		else
		{
			if (NPC->direct)
			{
				NPC->rect.left = 40;
				NPC->rect.top = 24;
				NPC->rect.right = 80;
				NPC->rect.bottom = 48;
			}
			else
			{
				NPC->rect.left = 40;
				NPC->rect.top = 0;
				NPC->rect.right = 80;
				NPC->rect.bottom = 24;
			}
			NPC->rect.top += NPC->count1 / 8;
			NPC->y = (NPC->count1 / 8 << 9) + NPC->tgt_y;
			NPC->rect.left -= NPC->count1 / 2 % 2;
		}
		if (NPC->count1 % 3 == 2)
		{
			createNpc(NPC_ProjectilePoohBlack, NPC->x + (random(-12, 12) << 9),
				NPC->y - 6144, random(-512, 512), 256);
		}
		if (NPC->count1 % 4 == 2)
			playSound(21);
	}
	else if (NPC->act_no == 3 && ++NPC->count1 > 59)
	{
		killNpcsByType(NPC_ProjectilePoohBlack, 1);
		NPC->cond = 0;
	}
LABEL_34:
	superXPos = NPC->x;
	superYPos = -512000;

	return;
}

void npcAct163(npc *NPC) // Dr. Gero
{
    constexpr array<RECT, 2> rcLeft = {{{192, 0, 208, 16}, {208, 0, 224, 16}}};
    constexpr array<RECT, 2> rcRight = {{{192, 16, 208, 32}, {208, 16, 224, 32}}};

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->ani_no = 0;
        NPC->ani_wait = 0;
        // Fallthrough
    case 1:
        if (!random(0, 120))
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
    }

    NPC->doRects(rcLeft, rcRight);
}

void npcAct164(npc *NPC) // Nurse Hasumi
{
    constexpr array<RECT, 2> rcLeft = {{{224, 0, 240, 16}, {240, 0, 256, 16}}};
    constexpr array<RECT, 2> rcRight = {{{224, 16, 240, 32}, {240, 16, 256, 32}}};

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->ani_no = 0;
        NPC->ani_wait = 0;
        // Fallthrough
    case 1:
        if (!random(0, 120))
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
    }

    NPC->doRects(rcLeft, rcRight);
}

void npcAct165(npc *NPC) //Curly (Collapsed)
{
	RECT rcLeft;
	RECT rcRight[2];

	rcLeft = { 144, 96, 160, 122 };
	rcRight[0] = { 192, 96, 208, 112 };
	rcRight[1] = { 208, 96, 224, 112 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->y += 0x1400;
//Fallthrough
	case 1:
		NPC->ani_no = NPC->direct == 2
			&& currentPlayer.x > NPC->x - 0x4000
			&& currentPlayer.x < NPC->x + 0x4000
			&& currentPlayer.y > NPC->y - 0x2000
			&& currentPlayer.y < NPC->y + 0x2000;
		break;

	default:
		break;
	}

	if (NPC->direct)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft;
}

void npcAct166(npc *NPC) //gaudi shop dude
{
	if (NPC->act_no != 2)
	{
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
	}
	if (NPC->act_no == 1 && random(0, 120) == 10)
	{
		NPC->act_no = 2;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
	}

	if (++NPC->act_wait > 8)
	{
		NPC->act_no = 1;
		NPC->ani_no = 0;
	}

	NPC->rect.left = 144 + (NPC->ani_no*40);
	NPC->rect.top = 104;
	NPC->rect.right = NPC->rect.left + 40;
	NPC->rect.bottom = 128;

	return;
}

void npcAct167(npc *NPC) //falling booster
{
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 1;
		break;
	case 10:
		NPC->ani_no = 0;
		NPC->ym += 64;
		if (NPC->ym > 1535)
			NPC->ym = 1535;
		NPC->y += NPC->ym;
		break;
	case 20:
		NPC->act_no = 21;
		NPC->act_wait = 0;
		NPC->ani_no = 0;
		playSound(29);
		// Fallthrough
	case 21:
		if (++NPC->ani_no > 2)
			NPC->ani_no = 1;
		if (++NPC->act_wait > 100)
		{
			for (int i = 0; i <= 3; ++i)
			{
				createNpc(NPC_Smoke, NPC->x + (random(-12, 12) << 9), NPC->y + (random(-12, 12) << 9),
					random(-341, 341), random(-1536, 0));
			}
			NPC->cond = 0;
		}
		break;
	default:
		break;
	}

	NPC->rect.left = 304;
	NPC->rect.top = NPC->ani_no*16;
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 16;
	if (NPC->ani_no == 2)
		NPC->rect = { 0, 0, 0, 0 };
	return;
}

void npcAct168(npc *NPC) //boulder
{
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		break;
	case 10:
		NPC->act_no = 11;
		NPC->act_wait = 0;
		NPC->tgt_x = NPC->x;
		// Fallthrough
	case 11:
		++NPC->act_wait;
		NPC->x = NPC->tgt_x;
		if (NPC->act_wait / 3 & 1)
			NPC->x += 512;
		break;
	case 20:
		NPC->act_no = 21;
		NPC->act_wait = 0;
		NPC->ym = -1024;
		NPC->xm = 256;
		playSound(25);
		// Fallthrough
	case 21:
		NPC->ym += 16;
		NPC->x += NPC->xm;
		NPC->y += NPC->ym;
		if (NPC->act_wait && NPC->flag & ground)
		{
			playSound(35);
			viewport.quake = 40;
			NPC->act_no = 0;
		}
		if (!NPC->act_wait)
			++NPC->act_wait;
		break;
	}

	NPC->rect.left = 264;
	NPC->rect.top = 56;
	NPC->rect.right = 320;
	NPC->rect.bottom = 96;
	return;
}

void npcAct169(npc *NPC)
{
	constexpr int spriteX[] = { 0,  0, 0, 40, 0, 80, 120, 120, 80 };
	constexpr int spriteY[] = { 0, 48, 0, 48, 0, 48,  48, 0, 0 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->act_wait = 30;
		NPC->facePlayer();
		// Fallthrough
	case 1:
		if (!--NPC->act_wait)
		{
			NPC->act_no = 2;
			++NPC->count1;
		}
		break;
	case 2:
		NPC->act_no = 3;
		NPC->act_wait = 0;
		NPC->ani_no = 1;
		NPC->ani_wait = 0;
		// Fallthrough
	case 3:
		if (++NPC->ani_wait > 3)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
			if (NPC->ani_no == 2 || NPC->ani_no == 4)
				playSound(23);
		}
		if (NPC->ani_no > 4)
			NPC->ani_no = 1;

		if (NPC->direct)
			NPC->xm += 32;
		else
			NPC->xm -= 32;

		if (NPC->act_wait <= 7 || NPC->x - 6144 >= currentPlayer.x || NPC->x + 6144 <= currentPlayer.x ||
			NPC->y - 6144 >= currentPlayer.y || NPC->y + 4096 <= currentPlayer.y)
		{
			if (++NPC->act_wait <= 75)
			{
				if (NPC->flag & 5)
				{
					if (NPC->count2 > 4)
					{
						NPC->act_no = 4;
						NPC->act_wait = 0;
						NPC->ani_no = 7;
						NPC->ym = -1535;
					}
					else
					{
						++NPC->count2;
					}
				}
				else
				{
					NPC->count2 = 0;
				}
				if ((NPC->count1 & 1) != 1 && NPC->act_wait > 25)
				{
					NPC->act_no = 4;
					NPC->act_wait = 0;
					NPC->ani_no = 7;
					NPC->ym = -1535;
				}
			}
			else
			{
				NPC->act_no = 9;
				NPC->ani_no = 0;
			}
		}
		else
		{
			NPC->act_no = 10;
			NPC->ani_no = 5;
			currentPlayer.cond |= 2;
			currentPlayer.damage(5);
		}
		break;
	case 4:
		NPC->facePlayer();

		if (++NPC->act_wait <= 29 && NPC->act_wait % 6 == 1)
		{
			playSound(39);
			createNpc(NPC_ProjectileBalrogMissiles, NPC->x, NPC->y, 0, 0, NPC->direct);
		}
		if (NPC->flag & 8)
		{
			NPC->act_no = 9;
			NPC->ani_no = 8;
			viewport.quake = 30;
			playSound(26);
		}
		if (NPC->act_wait > 7 && NPC->x - 6144 < currentPlayer.x && NPC->x + 6144 > currentPlayer.x &&
			NPC->y - 6144 < currentPlayer.y && NPC->y + 4096 > currentPlayer.y)
		{
			NPC->act_no = 10;
			NPC->ani_no = 5;
			currentPlayer.cond |= 2;
			currentPlayer.damage(10);
		}
		break;
	case 9:
		NPC->xm = 4 * NPC->xm / 5;
		if (!NPC->xm)
			NPC->act_no = 0;
		break;
	case 10:
		currentPlayer.x = NPC->x;
		currentPlayer.y = NPC->y;
		NPC->xm = 4 * NPC->xm / 5;
		if (!NPC->xm)
		{
			NPC->act_no = 11;
			NPC->act_wait = 0;
			NPC->ani_no = 5;
			NPC->ani_wait = 0;
		}
		break;
	case 11:
		currentPlayer.x = NPC->x;
		currentPlayer.y = NPC->y;
		if (++NPC->ani_wait > 2)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 6)
			NPC->ani_no = 5;
		if (++NPC->act_wait > 100)
			NPC->act_no = 20;
		break;
	case 20:
		playSound(25, 1);
		currentPlayer.cond &= 0xFD;
		if (NPC->direct)
		{
			currentPlayer.x -= 2048;
			currentPlayer.y -= 4096;
			currentPlayer.xm = -1535;
			currentPlayer.ym = -512;
			currentPlayer.direct = dirLeft;
			NPC->direct = dirLeft;
		}
		else
		{
			currentPlayer.x += 2048;
			currentPlayer.y -= 4096;
			currentPlayer.xm = 1535;
			currentPlayer.ym = -512;
			currentPlayer.direct = dirRight;
			NPC->direct = dirRight;
		}
		NPC->act_no = 21;
		NPC->act_wait = 0;
		NPC->ani_no = 7;
		// Fallthrough
	case 21:
		if (++NPC->act_wait > 49)
			NPC->act_no = 0;
		break;
	default:
		break;
	}
	NPC->doGravity(32, 1535);
	if (NPC->xm < -768)
		NPC->xm = -768;
	if (NPC->xm > 768)
		NPC->xm = 768;
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->rect.left = spriteX[NPC->ani_no];
	NPC->rect.top = spriteY[NPC->ani_no] + ((NPC->direct/2) * 24);
	NPC->rect.right = NPC->rect.left + 40;
	NPC->rect.bottom = NPC->rect.top + 24;
	return;
}

void npcAct170(npc *NPC)
{
	bool bHit = false;
	if (NPC->direct == dirLeft && NPC->flag & leftWall)
		bHit = true;
	if (NPC->direct == dirRight && NPC->flag & rightWall)
		bHit = true;
	if (bHit)
	{
		playSound(44);
		createSmokeLeft(NPC->x, NPC->y, 0, 3);
		killNpc(NPC);
		return;
	}

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		if (NPC->direct)
			NPC->xm = random(-2, -1) << 9;
		else
			NPC->xm = random(1, 2) << 9;
		NPC->ym = random(-2, 0) << 9;
		goto LABEL_16;
	}
	if (NPC->act_no == 1)
	{
	LABEL_16:
		++NPC->count1;
		if (NPC->direct)
		{
			NPC->xm += 32;
			if (NPC->count1 % 3 == 1)
				createCaret(NPC->x - 4096, NPC->y, 7, 0);
		}
		else
		{
			NPC->xm -= 32;
			if (NPC->count1 % 3 == 1)
				createCaret(NPC->x + 4096, NPC->y, 7, 2);
		}
		if (NPC->count1 > 49)
		{
			NPC->ym = 0;
		}
		else
		{
			if (NPC->y >= currentPlayer.y)
				NPC->ym -= 32;
			else
				NPC->ym += 32;
		}
		if (++NPC->ani_no > 1)
			NPC->ani_no = 0;
	}
	if (NPC->xm < -1024)
		NPC->xm = -1536;
	if (NPC->xm > 1024)
		NPC->xm = 1536;
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->rect.left = 112 + (NPC->ani_no * 16);
	NPC->rect.top = 96 + ((NPC->direct / 2) * 8);
	NPC->rect.right = NPC->rect.left + 16;
	NPC->rect.bottom = NPC->rect.top + 8;
	return;
}

void npcAct171(npc *NPC) //Fire whirr
{
	array<RECT, 2> rcRight;
	array<RECT, 2> rcLeft;

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
		++NPC->count1;

	if (NPC->count1 > 20)
	{
		createNpc(NPC_ProjectileFireWhirrr, NPC->x, NPC->y, 0, 0, NPC->direct, nullptr);
		NPC->count1 = -100;
		curlyShootWait = random(80, 100);
		curlyShootX = NPC->x;
		curlyShootY = NPC->y;
	}

	NPC->doRects(rcLeft, rcRight);
}

void npcAct172(npc *NPC) //Fire whirr projectile
{
	array<RECT, 3> rect;

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

	NPC->doRects(rect);
}

void npcAct173(npc *NPC) //Armoured Gaudi
{
	array<RECT, 4> rcLeft;
	array<RECT, 4> rcRight;
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
				createNpc(NPC_ProjectileGaudiArmor, NPC->x, NPC->y, 3 * getCos(deg), 3 * getSin(deg), 0, nullptr);
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

		NPC->doRects(rcLeft, rcRight);

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
	array<RECT, 3> rect;
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

	NPC->doRects(rect);
}

void npcAct175(npc *NPC)
{
	array<RECT, 2> rcLeft;
	array<RECT, 2> rcRight;

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

	NPC->doRects(rcLeft, rcRight);
}

void npcAct176(npc *NPC)
{
	array<RECT, 3> rcRight;
	array<RECT, 3> rcLeft;

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
				createNpc(NPC_EnemyBuyoBuyo, NPC->x, NPC->y + 4096, 0, 0, 2, nullptr);
			else
				createNpc(NPC_EnemyBuyoBuyo, NPC->x, NPC->y - 4096, 0, 0, 0, nullptr);
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

	NPC->doRects(rcLeft, rcRight);
}

void npcAct177(npc *NPC)
{
	array<RECT, 2> rect;

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

	NPC->doRects(rect);
}

void npcAct178(npc *NPC) //core spinner
{
	RECT rect[3];
	rect[0] = { 0, 224, 16, 240 };
	rect[1] = { 16, 224, 32, 240 };
	rect[2] = { 32, 224, 48, 240 };

	if (NPC->flag & 0xFF)
	{
		createCaret(NPC->x, NPC->y, effect_RisingDisc, 0);
		NPC->cond = 0;
	}
	if (NPC->flag & water)
	{
		NPC->y += NPC->ym / 2;
		NPC->x += NPC->xm / 2;
	}
	else
	{
		NPC->y += NPC->ym;
		NPC->x += NPC->xm;
	}

	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}
	if (NPC->ani_no > 2)
		NPC->ani_no = 0;

	NPC->rect = rect[NPC->ani_no];

	if (++NPC->count1 > 150)
	{
		killNpc(NPC);
		createCaret(NPC->x, NPC->y, effect_RisingDisc, 0);
	}
	return;
}

void npcAct179(npc *NPC) //core wisp
{
	RECT rect[3];
	rect[0] = { 48, 224, 72, 240 };
	rect[1] = { 72, 224, 96, 240 };
	rect[2] = { 96, 224, 120, 240 };

	if (NPC->flag & 0xFF)
	{
		NPC->cond = 0;
		createCaret(NPC->x, NPC->y, effect_RisingDisc, 0);
	}
	NPC->xm -= 32;
	NPC->ym = 0;
	if (NPC->xm < -1024)
		NPC->xm = -1024;
	NPC->y += NPC->ym;
	NPC->x += NPC->xm;

	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}
	if (NPC->ani_no > 2)
		NPC->ani_no = 0;

	NPC->rect = rect[NPC->ani_no];

	if (++NPC->count1 > 300)
	{
		killNpc(NPC);
		createCaret(NPC->x, NPC->y, effect_RisingDisc, 0);
	}
	return;
}