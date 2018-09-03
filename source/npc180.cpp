#include "npc180.h"

#include "mathUtils.h"
#include "player.h"
#include "flags.h"
#include "sound.h"
#include "render.h"
#include "bullet.h"
#include "caret.h"

void npcAct180(npc * NPC) // Curly, AI
{
	RECT rcLeft[11];
	RECT rcRight[11];

	rcLeft[0] = { 0, 96, 16, 112 };
	rcLeft[1] = { 16, 96, 32, 112 };
	rcLeft[2] = rcLeft[0];
	rcLeft[3] = { 32, 96, 48, 112 };
	rcLeft[4] = rcLeft[0];
	rcLeft[5] = { 48, 96, 64, 112 };
	rcLeft[6] = { 64, 96, 80, 112 };
	rcLeft[7] = rcLeft[5];
	rcLeft[8] = { 80, 96, 96, 112 };
	rcLeft[9] = rcLeft[5];
	rcLeft[10] = { 144, 96, 160, 112 };

	rcRight[0] = { 0, 112, 16, 128 };
	rcRight[1] = { 16, 112, 32, 126 };
	rcRight[2] = rcRight[0];
	rcRight[3] = { 32, 112, 48, 128 };
	rcRight[4] = rcRight[0];
	rcRight[5] = { 48, 112, 64, 128 };
	rcRight[6] = { 64, 112, 80, 128 };
	rcRight[7] = rcRight[5];
	rcRight[8] = { 80, 112, 96, 128 };
	rcRight[9] = { 48, 112, 64, 128 };
	rcRight[10] = { 144, 112, 160, 128 };

	if (NPC->y >= currentPlayer.y - 0x14000)
	{
		if (curlyShootWait)
		{
			NPC->tgt_x = curlyShootX;
			NPC->tgt_y = curlyShootY;
		}
		else
		{
			NPC->tgt_x = currentPlayer.x;
			NPC->tgt_y = currentPlayer.y;
		}
	}
	else if (NPC->y >= 0x20000)
	{
		NPC->tgt_x = 0;
		NPC->tgt_y = NPC->y;
	}
	else
	{
		NPC->tgt_x = 0x280000;
		NPC->tgt_y = NPC->y;
	}
	if (NPC->xm < 0 && NPC->flag & leftWall)
		NPC->xm = 0;
	if (NPC->xm > 0 && NPC->flag & rightWall)
		NPC->xm = 0;

	enum
	{
		start = 20,
		knockedOut = 40,
		running = 100,
	};

	if (NPC->act_no > 200)
	{
		switch (NPC->act_no)
		{
		case 210:
			NPC->xm -= 0x20;
			NPC->direct = dirLeft;
			if (NPC->flag & ground)
				NPC->act_no = 100;
			break;

		case 300:
			NPC->xm += 0x20;
			NPC->direct = dirRight;
			if (NPC->flag & rightWall)
				++NPC->count1;
			else
				NPC->count1 = 0;
			break;

		case 310:
			NPC->xm += 0x20;
			NPC->direct = dirRight;
			if (NPC->flag & ground)
				NPC->act_no = 100;
			break;
		}
	}
	else if (NPC->act_no == 200)
	{
		NPC->xm -= 0x20;
		NPC->direct = dirLeft;
		if (NPC->flag & leftWall)
			++NPC->count1;
		else
			NPC->count1 = 0;
	}
	else
	{
		switch (NPC->act_no)
		{
		case start:
			NPC->x = currentPlayer.x;
			NPC->y = currentPlayer.y;
			NPC->act_no = running;
			NPC->ani_no = 0;
			createNpc(NPC_CurlyAirTankBubble, 0, 0, 0, 0, dirLeft, NPC);
			if (getFlag(563))
				createNpc(NPC_CurlyAIPolarStar, 0, 0, 0, 0, dirLeft, NPC);
			else
				createNpc(NPC_CurlyAIMachineGun, 0, 0, 0, 0, dirLeft, NPC);
			break;

		case knockedOut:
			NPC->act_no = knockedOut + 1;
			NPC->act_wait = 0;
			NPC->ani_no = 10;
			// Fallthrough
		case knockedOut + 1:
			if (++NPC->act_wait == 750)
			{
				NPC->flag &= ~npc_interact;
				NPC->ani_no = 0;
			}
			if (NPC->act_wait > 1000)
			{
				NPC->act_no = running;
				NPC->ani_no = 0;
				createNpc(NPC_CurlyAirTankBubble, 0, 0, 0, 0, dirLeft, NPC);
				if (getFlag(563))
					createNpc(NPC_CurlyAIPolarStar, 0, 0, 0, 0, dirLeft, NPC);
				else
					createNpc(NPC_CurlyAIMachineGun, 0, 0, 0, 0, dirLeft, NPC);
			}
			break;

		case running:
			NPC->ani_no = 0;
			NPC->xm = 7 * NPC->xm / 8;
			NPC->count1 = 0;
			if (NPC->x <= NPC->tgt_x + 0x2000)
			{
				if (NPC->x < NPC->tgt_x - 0x2000)
				{
					NPC->act_no = 300;
					NPC->ani_no = 1;
					NPC->direct = dirRight;
					NPC->act_wait = random(20, 60);
				}
			}
			else
			{
				NPC->act_no = 200;
				NPC->ani_no = 1;
				NPC->direct = dirLeft;
				NPC->act_wait = random(20, 60);
			}
			break;

		default:
			break;
		}
	}

	if (curlyShootWait)
		--curlyShootWait;
	if (curlyShootWait == 70)
		NPC->count2 = 10;
	if (curlyShootWait == 60 && NPC->flag & ground && random(0, 2))
	{
		NPC->count1 = 0;
		NPC->ym = -0x600;
		NPC->ani_no = 1;
		playSound(SFX_QuoteJump);
		if (NPC->x <= NPC->tgt_x)
			NPC->act_no = 310;
		else
			NPC->act_no = 210;
	}

	const auto xDistToTarget = abs(NPC->x - NPC->tgt_x);
	const auto yDistToTarget = NPC->y - NPC->tgt_y;

	if (NPC->act_no == 100)
	{
		if (xDistToTarget + pixelsToUnits(2) >= yDistToTarget)
			NPC->ani_no = 0;
		else
			NPC->ani_no = 5;
	}
	if (NPC->act_no == 210 || NPC->act_no == 310)
	{
		if (xDistToTarget + pixelsToUnits(2) >= yDistToTarget)
			NPC->ani_no = 1;
		else
			NPC->ani_no = 6;
	}
	if (NPC->act_no == 200 || NPC->act_no == 300)
	{
		++NPC->ani_wait;
		if (xDistToTarget + pixelsToUnits(2) >= yDistToTarget)
			NPC->ani_no = NPC->act_wait / 4 % 4 + 1;
		else
			NPC->ani_no = NPC->act_wait / 4 % 4 + 6;

		if (NPC->act_wait)
		{
			--NPC->act_wait;
			if (NPC->flag && NPC->count1 > 10)
			{
				NPC->count1 = 0;
				NPC->ym = -pixelsToUnits(3);
				NPC->act_no += 10;
				NPC->ani_no = 1;
				playSound(SFX_QuoteJump);
			}
		}
		else
		{
			NPC->act_no = 100;
			NPC->ani_no = 0;
		}
	}

	if (NPC->act_no >= 100 && NPC->act_no < 500)
	{
		if (NPC->x >= currentPlayer.x - tilesToUnits(5) && NPC->x <= currentPlayer.x + tilesToUnits(5))
			NPC->ym += 51;
		else if (NPC->flag)
			NPC->ym += 0x10;
		else
			NPC->ym += 51;
	}

	if (NPC->xm > 0x300)
		NPC->xm = 0x300;
	if (NPC->xm < -0x300)
		NPC->xm = -0x300;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->act_no >= 100 && !(NPC->flag & ground) && NPC->ani_no != 1000)
	{
		if (xDistToTarget + pixelsToUnits(2) >= yDistToTarget)
			NPC->ani_no = 1;
		else
			NPC->ani_no = 6;
	}

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void curlyNPCCommonStart(npc *NPC)
{
	if (NPC->pNpc->ani_no >= 5)
	{
		if (NPC->pNpc->direct != dirLeft)
		{
			NPC->direct = dirRight;
			NPC->x = NPC->pNpc->x;
		}
		else
		{
			NPC->direct = dirLeft;
			NPC->x = NPC->pNpc->x;
		}
		NPC->y = NPC->pNpc->y - 0x1400;
		NPC->ani_no = 1;
	}
	else
	{
		if (NPC->pNpc->direct != dirLeft)
		{
			NPC->direct = dirRight;
			NPC->x = NPC->pNpc->x + 0x1000;
		}
		else
		{
			NPC->direct = dirLeft;
			NPC->x = NPC->pNpc->x - 0x1000;
		}
		NPC->y = NPC->pNpc->y;
		NPC->ani_no = 0;
	}

	switch (NPC->pNpc->ani_no)
	{
	case 1:
	case 3:
	case 6:
	case 8:
		NPC->y -= 0x200;
		break;

	default:
		break;
	}
}

void curlyNPCCommonBulletSpawn(const npc *NPC, int bulletCode)
{
	if (NPC->ani_no)
	{
		if (NPC->direct != dirLeft)
		{
			createBullet(bulletCode, NPC->x + 0x400, NPC->y - 0x800, dirUp);
			createCaret(NPC->x + 0x400, NPC->y - 0x800, effect_Star);
		}
		else
		{
			createBullet(bulletCode, NPC->x - 0x400, NPC->y - 0x800, dirUp);
			createCaret(NPC->x - 0x400, NPC->y - 0x800, effect_Star);
		}
	}
	else if (NPC->direct != dirLeft)
	{
		createBullet(bulletCode, NPC->x + 0x800, NPC->y + 0x600, dirRight);
		createCaret(NPC->x + 0x800, NPC->y + 0x600, effect_Star);
	}
	else
	{
		createBullet(bulletCode, NPC->x - 0x800, NPC->y + 0x600, dirLeft);
		createCaret(NPC->x - 0x800, NPC->y + 0x600, effect_Star);
	}
}

void npcAct181(npc *NPC) // Curly Machine Gun bullet spawner (projectile)
{
	constexpr RECT rcLeft[2] = { {184, 152, 200, 168}, {200, 152, 216, 168} };
	constexpr RECT rcRight[2] = { {184, 168, 200, 184}, {200, 168, 216, 184} };

	if (NPC->pNpc)
	{
		curlyNPCCommonStart(NPC);
		if (NPC->act_no)
		{
			if (NPC->act_no == 10)
			{
				if (++NPC->act_wait % 12 == 1)
					curlyNPCCommonBulletSpawn(NPC, bullet_PolarStarLevel3);
				if (NPC->act_wait == 60)
					NPC->act_no = 0;
			}
		}
		else if (NPC->pNpc->count2 == 10)
		{
			NPC->pNpc->count2 = 0;
			NPC->act_no = 10;
			NPC->act_wait = 0;
		}

		if (NPC->direct != dirLeft)
			NPC->rect = rcRight[NPC->ani_no];
		else
			NPC->rect = rcLeft[NPC->ani_no];
	}
}

void npcAct182(npc * NPC) // Curly Polar Star bullet spawner (projectile)
{
	constexpr RECT rcLeft[2] = { {184, 152, 200, 168}, {200, 152, 216, 168} };
	constexpr RECT rcRight[2] = { {184, 168, 200, 184}, {200, 168, 216, 184} };

	if (NPC->pNpc)
	{
		curlyNPCCommonStart(NPC);

		if (NPC->act_no)
		{
			if (NPC->act_no == 10)
			{
				if (++NPC->act_wait % 6 == 1)
					curlyNPCCommonBulletSpawn(NPC, bullet_MachineGunLevel3);
				if (NPC->act_wait == 60)
					NPC->act_no = 0;
			}
		}
		else if (NPC->pNpc->count2 == 10)
		{
			NPC->pNpc->count2 = 0;
			NPC->act_no = 10;
			NPC->act_wait = 0;
		}

		if (NPC->direct != dirLeft)
			NPC->rect = rcRight[NPC->ani_no];
		else
			NPC->rect = rcLeft[NPC->ani_no];
	}
}

void npcAct183(npc * NPC) // Curly Air Bubble
{
	constexpr RECT rcNPC[2] = { {56, 96, 80, 120}, {80, 96, 104, 120} };

	if (NPC->pNpc)
	{
		if (!NPC->act_no)
		{
			NPC->x = NPC->pNpc->x;
			NPC->y = NPC->pNpc->y;
			NPC->act_no = 1;
		}
		NPC->x += (NPC->pNpc->x - NPC->x) / 2;
		NPC->y += (NPC->pNpc->y - NPC->y) / 2;

		NPC->animate(1, 0, 1);

		if (NPC->pNpc->flag & water)
			NPC->rect = rcNPC[NPC->ani_no];
		else
			NPC->rect.right = 0;
	}
}

void npcAct192(npc * NPC)
{
	enum
	{
		parked = 0,
		mounted = 10,
		startEngine = 20,
		takeOff = 30,
		outOfControl = 40,
	};

	switch (NPC->act_no)
	{
	case parked:
		NPC->act_no = parked + 1;
		NPC->view = { 0x2000, 0x1000, 0x2000, 0x1000 };
		break;

	case mounted:
		NPC->act_no = mounted + 1;
		NPC->ani_no = 1;
		NPC->view.top = 0x2000;
		NPC->view.bottom = 0x2000;
		NPC->y -= 0xA00;
		break;

	case startEngine:
		NPC->act_no = startEngine + 1;
		NPC->act_wait = 1;
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
		// Fallthrough
	case startEngine + 1:
		NPC->x = NPC->tgt_x + pixelsToUnits(random(-1, 1));
		NPC->y = NPC->tgt_y + pixelsToUnits(random(-1, 1));
		if (++NPC->act_wait > 30)
			NPC->act_no = 30;
		break;

	case takeOff:
		NPC->act_no = takeOff + 1;
		NPC->act_wait = 1;
		NPC->xm = -0x800;
		NPC->x = NPC->tgt_x;
		NPC->y = NPC->tgt_y;
		playSound(SFX_MissileImpact);
		// Fallthrough
	case takeOff + 1:
		NPC->xm += 0x20;
		NPC->x += NPC->xm;
		++NPC->act_wait;
		NPC->y = NPC->tgt_y + pixelsToUnits(random(-1, 1));
		if (NPC->act_wait > 10)
			NPC->direct = dirRight;
		if (NPC->act_wait > 200)
			NPC->act_no = outOfControl;
		break;

	case outOfControl:
		NPC->act_no = outOfControl + 1;
		NPC->act_wait = 2;
		NPC->direct = dirLeft;
		NPC->y = -0x6000;
		NPC->xm = -0x1000;
		// Fallthrough
	case outOfControl + 1:
		NPC->x += NPC->xm;
		NPC->y += NPC->ym;
		NPC->act_wait += 2;
		if (NPC->act_wait > 1200)
			NPC->cond = 0;
		break;

	default:
		break;
	}
	if (!(NPC->act_wait % 4) && NPC->act_no >= startEngine)
	{
		playSound(SFX_FireballBounce);
		if (NPC->direct != dirLeft)
			createCaret(NPC->x - 0x1400, NPC->y + 0x1400, effect_BoosterSmoke, dirLeft);
		else
			createCaret(NPC->x + 0x1400, NPC->y + 0x1400, effect_BoosterSmoke, dirRight);
	}

	constexpr RECT rcLeft[2] = { {224, 64, 256, 80}, {256, 64, 288, 96} };
	constexpr RECT rcRight[2] = { {224, 80, 256, 96}, {288, 64, 320, 96} };

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct194(npc *NPC) // Blue Robot, destroyed
{
	if (NPC->act_no == 0)
	{
		NPC->act_no = 1;
		NPC->y += 0x800;
	}

	NPC->rect = { 192, 120, 224, 128 };
}

void npcAct195(npc *NPC) // Grate mouth
{
	NPC->rect = { 112, 64, 128, 80 };
}

void npcAct196(npc *NPC) //Stream floor
{
	NPC->x -= 0xC00;
	if (NPC->x <= 0x26000)
		NPC->x += 0x2C000;

	if (NPC->direct)
		NPC->rect = { 112, 80, 144, 96 };
	else
		NPC->rect = { 112, 64, 144, 80 };
}

void npcAct199(npc *NPC) //Current / fan effect
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->ani_no = random(0, 2);

		const int direction = NPC->direct;

		if (direction == 1)
		{
			NPC->ym = -1;
		}
		else if (direction > 1)
		{
			if (direction == 2)
			{
				NPC->xm = 1;
			}
			else if (direction == 3)
			{
				NPC->ym = 1;
			}
		}
		else if (!direction)
		{
			NPC->xm = -1;
		}

		NPC->xm = (random(4, 8) << 9) / 2 * NPC->xm;
		NPC->ym = (random(4, 8) << 9) / 2 * NPC->ym;
	}

	if (++NPC->ani_wait > 6)
	{
		NPC->ani_wait = 0;
		++NPC->ani_no;
	}

	if (NPC->ani_no > 4)
		NPC->cond = 0;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->rect = { 72 + (NPC->ani_no << 1),16,74 + (NPC->ani_no << 1),18 };
}

