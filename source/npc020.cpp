#include "npc020.h"

#include <vector>
#include "mathUtils.h"
#include "caret.h"
#include "player.h"
#include "sound.h"
#include "render.h"
#include "game.h"

using std::vector;

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

void npcAct024(npc *NPC) // Power Critter (enemy)
{
    vector<RECT> rcLeft(6);
    vector<RECT> rcRight(6);

    rcLeft[0] = {0, 0, 24, 24};
    rcLeft[1] = {24, 0, 48, 24};
    rcLeft[2] = {48, 0, 72, 24};
    rcLeft[3] = {72, 0, 96, 24};
    rcLeft[4] = {96, 0, 120, 24};
    rcLeft[5] = {120, 0, 144, 24};

    rcRight[0] = {0, 24, 24, 48};
    rcRight[1] = {24, 24, 48, 48};
    rcRight[2] = {48, 24, 72, 48};
    rcRight[3] = {72, 24, 96, 48};
    rcRight[4] = {96, 24, 120, 48};
	rcRight[5] = {120, 24, 144, 48};

	enum
	{
		init = 0,
		normal = 1,
		startJump = 2,
		jumping = 3,
		flying = 4,
		goingDownFromFlight = 5,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->y += pixelsToUnits(3);
		NPC->act_no = normal;
	// Fallthrough
	case normal:
		if (NPC->act_wait < 8 || NPC->isPlayerAligned(tilesToUnits(8), tilesToUnits(8), tilesToUnits(3)))
		{
			if (NPC->act_wait < 8)
				++NPC->act_wait;
			NPC->ani_no = 0;
		}
		else
        {
            NPC->facePlayer();
            NPC->ani_no = 1;
        }

        if (NPC->shock || (NPC->act_wait >= 8 && NPC->isPlayerWithinDistance(tilesToUnits(6), tilesToUnits(6), tilesToUnits(3))))
        {
            NPC->act_no = startJump;
            NPC->ani_no = 0;
            NPC->act_wait = 0;
        }
        break;

    case startJump:
        if (++NPC->act_wait > 8)
        {
            NPC->act_no = jumping;
            NPC->ani_no = 2;
            NPC->ym = -0x5FF;
            playSound(SFX_PowerCritterLargeEnemyJump);
            NPC->facePlayer();
            NPC->moveInDir(0x100);
        }
        break;

    case jumping:
        if (NPC->ym > pixelsToUnits(1))
        {
            NPC->tgt_y = NPC->y;
            NPC->act_no = flying;
            NPC->ani_no = 3;
            NPC->act_wait = 0;
        }
        break;

    case flying:
        NPC->facePlayer();
        ++NPC->act_wait;

        if (NPC->flag & (leftWall | rightWall | ceiling) || NPC->act_wait > 100)
        {
            NPC->damage = 12;
            NPC->act_no = goingDownFromFlight;
            NPC->ani_no = 2;
            NPC->xm /= 2;
        }
        else
        {
            if (NPC->act_wait % 4 == 1)
                playSound(SFX_PowerCritterFlys);
            NPC->animate(0, 3, 5);
        }
        break;

    case goingDownFromFlight:
        if (NPC->flag & ground)
        {
            NPC->damage = 2;
            NPC->xm = 0;
            NPC->act_wait = 0;
            NPC->ani_no = 0;
            NPC->act_no = normal;
            playSound(SFX_LargeObjectHitGround);
            viewport.quake = 30;
        }
        break;

    default:
        break;
	}

	if (NPC->act_no == flying)
    {
        NPC->accelerateTowardsPlayer(0x20);
        NPC->accelerateTowardsYTarget(0x10);
        NPC->limitYVel(0x200);
        NPC->limitXVel(0x200);
    }
    else
        NPC->doGravity(0x20, 0x5FF);

    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    NPC->doRects(rcLeft, rcRight);
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
	// Fallthrough
	case 1:
	case 3:
	case 5:
	case 7:
		if (++NPC->act_wait > 150)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		break;
	case 2:
	case 4:
	case 6:
	case 8:
		if (++NPC->act_wait > 64)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		else
			NPC->y -= 512;
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

void npcAct026(npc * NPC) // Bat, Black Circling (enemy)
{
	enum
	{
		init = 0,
		circleAroundTarget = 1,
		diveAttack = 3,
	};

	uint8_t angle;
	switch (NPC->act_no)
	{
	case init:
		angle = random(0, 255);
		NPC->xm = getCos(angle);
		NPC->tgt_x = NPC->x + 8 * getCos(angle + 64);

		angle = random(0, 255);
		NPC->ym = getSin(angle);
		NPC->tgt_y = NPC->y + 8 * getSin(angle + 64);

		NPC->act_no = circleAroundTarget;
		NPC->count1 = 128;
		break;

	case circleAroundTarget:
		NPC->facePlayer();

		if (NPC->tgt_x < NPC->x)
			NPC->xm -= 0x10;
		else if (NPC->tgt_x > NPC->x)
			NPC->xm += 0x10;

		if (NPC->tgt_y < NPC->y)
			NPC->ym -= 0x10;
		else if (NPC->tgt_y > NPC->y)
			NPC->ym += 0x10;

		if (NPC->xm > 0x200)
			NPC->xm = 0x200;
		else if (NPC->xm < -0x200)
			NPC->xm = -0x200;

		if (NPC->ym > 0x200)
			NPC->ym = 0x200;
		else if (NPC->ym < -0x200)
			NPC->ym = -0x200;

		if (NPC->count1 >= 120)
		{
			if (NPC->isPlayerWithinDistance(0x1000, 0, 0xC0000))
			{
				NPC->xm /= 2;
				NPC->ym = 0;
				NPC->act_no = diveAttack;
				NPC->bits &= ~npc_ignoreSolid;
			}
		}
		else
			++NPC->count1;
		break;

	case diveAttack:
		NPC->doGravity(0x40, 0x5FF);

		if (NPC->flag & ground)
		{
			NPC->ym = 0;
			NPC->xm *= 2;
			NPC->count1 = 0;
			NPC->act_no = circleAroundTarget;
			NPC->bits |= npc_ignoreSolid;
		}
		break;

	default:
		break;
	}

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	vector<RECT> rcLeft(4);
	vector<RECT> rcRight(4);

	rcLeft[0] = { 32, 80, 48, 96 };
	rcLeft[1] = { 48, 80, 64, 96 };
	rcLeft[2] = { 64, 80, 80, 96 };
	rcLeft[3] = { 80, 80, 96, 96 };

	rcRight[0] = { 32, 96, 48, 112 };
	rcRight[1] = { 48, 96, 64, 112 };
	rcRight[2] = { 64, 96, 80, 112 };
	rcRight[3] = { 80, 96, 96, 112 };

	if (NPC->act_no == diveAttack)
		NPC->ani_no = 3;
	else
		NPC->animate(1, 0, 2);

	NPC->doRects(rcLeft, rcRight);
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

void npcAct031(npc *NPC) // Bat, Black Hanging (enemy)
{
    vector<RECT> rcLeft = {{0, 80, 16, 96}, {16, 80, 32, 96}, {32, 80, 48, 96}, {48, 80, 65, 96}, {64, 80, 80, 96}};
    vector<RECT> rcRight = {{0, 96, 16, 112}, {16, 96, 32, 112}, {32, 96, 48, 112}, {48, 96, 64, 112}, {64, 96, 80, 112}};

    enum
    {
        init = 0,
        hangAndWait = 1,
        blinking = 2,
        payingAttention = 3,
        falling = 4,
        flying = 5,
    };

    switch (NPC->act_no)
    {
    case init:
        NPC->act_no = hangAndWait;
        // Fallthrough
    case hangAndWait:
        if (!random(0, 120))
        {
            NPC->act_no = blinking;
            NPC->act_wait = 0;
            NPC->ani_no = 1;
        }
        if (NPC->isPlayerWithinDistance(pixelsToUnits(8), pixelsToUnits(8), tilesToUnits(6)))
        {
            NPC->ani_no = 0;
            NPC->act_no = payingAttention;
        }
        break;

    case blinking:
        if (++NPC->act_wait > 8)
        {
            NPC->act_no = hangAndWait;
            NPC->ani_no = 0;
        }
        break;

    case payingAttention:
        NPC->ani_no = 0;
        if (NPC->shock || NPC->getXDistToPlayer() < pixelsToUnits(20))
        {
            NPC->ani_no = 1;
            NPC->ani_wait = 0;
            NPC->act_no = falling;
            NPC->act_wait = 0;
        }
        break;

    case falling:
        NPC->doGravity(0x20, 0x5FF);

        if ((++NPC->act_wait >= 20 && NPC->y > currentPlayer.y - 0x2000) || NPC->flag & ground)
        {
            NPC->ani_wait = 0;
            NPC->ani_no = 2;
            NPC->act_no = flying;
            NPC->tgt_y = NPC->y;
            if (NPC->flag & ground)
                NPC->xm = pixelsToUnits(-1);
        }
        break;

    case flying:
        NPC->animate(1, 2, 4);

        NPC->facePlayer();
        NPC->accelerateTowardsPlayer(0x10);
        NPC->accelerateTowardsYTarget(0x10);
        NPC->limitXVel(pixelsToUnits(1));
        NPC->limitYVel(pixelsToUnits(1));

        if (NPC->flag & ground)
            NPC->ym = pixelsToUnits(-1);
        if (NPC->flag & ceiling)
            NPC->ym = pixelsToUnits(1);
        break;

    default:
        break;
    }

    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    NPC->doRects(rcLeft, rcRight);
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
