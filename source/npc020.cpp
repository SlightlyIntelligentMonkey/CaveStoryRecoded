#include "npc020.h"

#include <array>
#include "mathUtils.h"
#include "caret.h"
#include "player.h"
#include "sound.h"
#include "render.h"
#include "game.h"
#include "stage.h"

using std::array;

void npcAct020(npc *NPC) // Computer
{
	RECT rcLeft = { 288, 16, 320, 40 };
	array<RECT, 3> rcRight = { {{288, 40, 320, 64}, {288, 40, 320, 64}, {288, 64, 320, 88}} };

	NPC->animate(3, 0, 2);

	if (NPC->direct == dirLeft)
		NPC->rect = rcLeft;
	else
		NPC->rect = rcRight.at(NPC->ani_no);
}

void npcAct021(npc *NPC) //Open chest
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;

		if (NPC->direct == dirRight)
			NPC->y += 0x2000;
	}

	NPC->doRects({ 224, 40, 240, 48 });
}

void npcAct022(npc *NPC) //Teleporter
{
	constexpr array<RECT, 2> rcNPC = { {{240, 16, 264, 48}, {248, 152, 272, 184}} };

	if (NPC->act_no)
	{
		if (NPC->act_no == 1 && ++NPC->ani_no > 1)
			NPC->ani_no = 0;
	}
	else
		NPC->ani_no = 0;

	NPC->doRects(rcNPC);
}

void npcAct023(npc *NPC) //Teleporter lights
{
    NPC->animate(1, 0, 7);
	NPC->doRects({ 264, 16 + (NPC->ani_no * 4), 288, 20 + (NPC->ani_no * 4) });
}

void npcAct024(npc *NPC) // Power Critter (enemy)
{
	constexpr array<RECT, 6> rcLeft =
	{{
        {0, 0, 24, 24},
        {24, 0, 48, 24},
        {48, 0, 72, 24},
        {72, 0, 96, 24},
        {96, 0, 120, 24},
        {120, 0, 144, 24},
    }};

	constexpr array<RECT, 6> rcRight =
	{{
        {0, 24, 24, 48},
	    {24, 24, 48, 48},
	    {48, 24, 72, 48},
	    {72, 24, 96, 48},
	    {96, 24, 120, 48},
	    {120, 24, 144, 48},
    }};

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
            NPC->moveInDir(pixelsToUnits(0.5));
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
            gViewport.quake = 30;
        }
        break;

    default:
        break;
	}

	if (NPC->act_no == flying)
    {
        NPC->accelerateTowardsPlayer(0x20);
        NPC->accelerateTowardsYTarget(0x10);
        NPC->limitYVel(pixelsToUnits(1));
        NPC->limitXVel(pixelsToUnits(1));
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
		NPC->x += tilesToUnits(0.5);
//Fallthrough
	case 1:
		if (++NPC->act_wait > 150)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		break;
	case 2:
		if (++NPC->act_wait > 64)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		else
			NPC->y -= 0x200;
		break;
	case 3:
		if (++NPC->act_wait > 150)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		break;
	case 4:
		if (++NPC->act_wait > 64)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		else
			NPC->y -= 0x200;
		break;
	case 5:
		if (++NPC->act_wait > 150)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		break;
	case 6:
		if (++NPC->act_wait > 64)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		else
			NPC->y += 0x200;
		break;
	case 7:
		if (++NPC->act_wait > 150)
		{
			NPC->act_wait = 0;
			++NPC->act_no;
		}
		break;
	case 8:
		if (++NPC->act_wait > 64)
		{
			NPC->act_wait = 0;
			NPC->act_no = 1;
		}
		else
			NPC->y += 0x200;
		break;
	default:
		break;
	}

	switch (NPC->act_no)
	{
	case 2:
	case 4:
	case 6:
	case 8:
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}
		if (NPC->ani_no > 1)
			NPC->ani_no = 0;
		break;
	default:
		break;
	}

	NPC->doRects({256, 64 + (16 * NPC->ani_no), 288, 80 + (16 * NPC->ani_no)});
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

		NPC->accelerateTowardsXTarget(0x10);
		NPC->accelerateTowardsYTarget(0x10);
		NPC->limitXVel(pixelsToUnits(1));
		NPC->limitYVel(pixelsToUnits(1));

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

	array<RECT, 4> rcLeft;
	array<RECT, 4> rcRight;

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
	NPC->doRects({ 96, 64, 128, 88 });
}

void npcAct028(npc *NPC)
{
	array<RECT, 6> rcLeft;
	array<RECT, 6> rcRight;

	rcLeft[0] = { 0, 48, 16, 64 };
	rcLeft[1] = { 16, 48, 32, 64 };
	rcLeft[2] = { 32, 48, 48, 64 };
	rcLeft[3] = { 48, 48, 64, 64 };
	rcLeft[4] = { 64, 48, 80, 64 };
	rcLeft[5] = { 80, 48, 96, 64 };

	rcRight[0] = { 0, 64, 16, 80 };
	rcRight[1] = { 16, 64, 32, 80 };
	rcRight[2] = { 32, 64, 48, 80 };
	rcRight[3] = { 48, 64, 64, 80 };
	rcRight[4] = { 64, 64, 80, 80 };
	rcRight[5] = { 80, 64, 96, 80 };

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
		if (NPC->act_wait < 8 || NPC->isPlayerWithinDistance(tilesToUnits(8), tilesToUnits(8), tilesToUnits(3)))
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

		if (NPC->shock || (NPC->isPlayerWithinDistance(tilesToUnits(6), tilesToUnits(6), tilesToUnits(3))))
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

			NPC->ym = -0x4CC;
			playSound(SFX_CritterHop);
			NPC->facePlayer();
			NPC->moveInDir(pixelsToUnits(0.5));
		}
		break;

	case jumping:
		if (NPC->ym > pixelsToUnits(0.5))
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
			NPC->damage = 3;
			NPC->act_no = goingDownFromFlight;
			NPC->ani_no = 2;
			NPC->xm /= 2;
		}
		else
		{
			if (NPC->act_wait % 4 == 1)
				playSound(SFX_CritterFly);
			if (NPC->flag & ground)
				NPC->ym = pixelsToUnits(-1);
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
			playSound(SFX_HitGround);
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
		NPC->doGravity(0x40, 0x5FF);

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);
}

void npcAct029(npc *NPC) // Cthulhu
{
	constexpr array<RECT, 2> rcLeft = { { {0, 192, 16, 216}, {16, 192, 32, 216} } };
	constexpr array<RECT, 2> rcRight = { { {0, 216, 16, 240}, {16, 216, 32, 240} } };

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

	NPC->ani_no = NPC->isPlayerWithinDistance(tilesToUnits(3), tilesToUnits(3), tilesToUnits(1));

doRects:
	NPC->doRects(rcLeft, rcRight);
}

void npcAct030(npc *NPC) // Hermit Gunsmith
{
	constexpr array<RECT, 3> rcNPC = { { { 48, 0, 64, 16 }, { 48, 16, 64, 32 }, { 0, 32, 16, 48 } } };

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
	NPC->doRects(rcNPC);
}

void npcAct031(npc *NPC) // Bat, Black Hanging (enemy)
{
	constexpr array<RECT, 5> rcLeft = { {{0, 80, 16, 96}, {16, 80, 32, 96}, {32, 80, 48, 96}, {48, 80, 65, 96}, {64, 80, 80, 96}} };
	constexpr array<RECT, 5> rcRight = { {{0, 96, 16, 112}, {16, 96, 32, 112}, {32, 96, 48, 112}, {48, 96, 64, 112}, {64, 96, 80, 112}} };

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
	constexpr array<RECT, 2> rcNPC = { {{32, 96, 48, 112}, {48, 96, 64, 112}} };

    NPC->animate(2, 0, 1);

	NPC->doRects(rcNPC);
}

void npcAct033(npc *NPC) // Balrog energy ball bouncing (projectile)
{
    if (NPC->flag & (leftWall | rightWall))
    {
        createCaret(NPC->x, NPC->y, effect_RisingDisc);
        NPC->cond = 0;
    }
    else if (NPC->flag & ground)
        NPC->ym = pixelsToUnits(-4);

    NPC->ym += 42;
    NPC->y += NPC->ym;
    NPC->x += NPC->xm;

	constexpr array<RECT, 2> rcNPC = { {{240, 64, 256, 80}, {240, 80, 256, 96}} };
    NPC->animate(2, 0, 1);

    NPC->doRects(rcNPC);

    if (++NPC->act_wait > 250)
    {
        createCaret(NPC->x, NPC->y, effect_RisingDisc);
        NPC->cond = 0;  // NPC suicides after 5 seconds
    }
}

void npcAct034(npc * NPC) // Bed
{
    NPC->doRects({192, 48, 224, 64}, {192, 184, 224, 200});
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

	constexpr array<RECT, 4> rcLeft = { { {96, 64, 120, 96}, {120, 64, 144, 96}, {144, 64, 168, 98}, {168, 64, 192, 96} } };
	constexpr array<RECT, 4> rcRight = { { {96, 96, 120, 128}, {120, 96, 144, 128}, {144, 96, 168, 128}, {168, 96, 192, 128} } };

	NPC->doRects(rcLeft, rcRight);
}

void npcAct036(npc *NPC) // Balrog, Flying (boss)
{
    enum
    {
        init = 0,
        wait = 1,
        shootPlayer = 2,
        startJump = 3,
        jumpUp = 4,
        flying = 5,
        goDownFromJump = 6,
        landed = 7,
    };

    switch (NPC->act_no)
    {
    case init:
        NPC->act_no = wait;
        // Fallthrough
    case wait:
        if (++NPC->act_wait > 12)
        {
            NPC->act_no = shootPlayer;
            NPC->act_wait = 0;
            NPC->count1 = 3;
            NPC->ani_no = 1;
        }
        break;

    case shootPlayer:
        if (++NPC->act_wait > 16)
        {
            --NPC->count1;
            NPC->act_wait = 0;
            int16_t angle = getAtan(NPC->x - currentPlayer.x, NPC->y + pixelsToUnits(4) - currentPlayer.y);
            angle += random(-0x10, 0x10);
            const auto xVel = getCos(angle);
            const auto yVel = getSin(angle);
            createNpc(NPC_ProjectileBalrogEnergyBallInvincible, NPC->x, NPC->y + pixelsToUnits(4), xVel, yVel);
            playSound(SFX_EnemyShootProjectile);
            if (!NPC->count1)
            {
                NPC->act_no = startJump;
                NPC->act_wait = 0;
            }
        }
        break;

    case startJump:
        if (++NPC->act_wait > 3)
        {
            NPC->act_no = jumpUp;
            NPC->act_wait = 0;
            NPC->xm = (currentPlayer.x - NPC->x) / 100;
            NPC->ym = pixelsToUnits(-3);
            NPC->ani_no = 3;
        }
        break;

    case jumpUp:
        if (NPC->ym > pixelsToUnits(-1))
        {
            if (NPC->life <= 60)
                NPC->act_no = goDownFromJump;
            else
            {
                NPC->act_no = flying;
                NPC->ani_no = 4;
                NPC->ani_wait = 0;
                NPC->act_wait = 0;
                NPC->tgt_y = NPC->y;
            }
        }
        break;

    case flying:
        NPC->animate(1);
        if (NPC->ani_no > 5)
        {
            NPC->ani_no = 4;
            playSound(SFX_StrangeClick);
        }

        if (++NPC->act_wait > 100)
        {
            NPC->act_no = goDownFromJump;
            NPC->ani_no = 3;
        }

        NPC->accelerateTowardsYTarget(0x40);
        NPC->limitYVel(pixelsToUnits(1));
        break;

    case goDownFromJump:
        if (NPC->y + tilesToUnits(1) >= currentPlayer.y)
            NPC->damage = 0;
        else
            NPC->damage = 10;

        if (NPC->flag & ground)
        {
            NPC->act_no = landed;
            NPC->act_wait = 0;
            NPC->ani_no = 2;
            playSound(SFX_LargeObjectHitGround);
            playSound(SFX_SillyExplosion);
            gViewport.quake = 30;
            NPC->damage = 0;

            for (size_t i = 0; i < 8; ++i)
            {
                const auto yVel = random(pixelsToUnits(-3), 0);
                const auto xVel = random(-0x155, 0x155);
                const auto yPos = NPC->y + pixelsToUnits(random(-12, 12));
                const auto xPos = NPC->x + pixelsToUnits(random(-12, -12));
                createNpc(NPC_Smoke, xPos, yPos, xVel, yVel);
            }
            for (size_t i = 0; i < 8; ++i)
            {
                const auto yVel = random(pixelsToUnits(-2), 0);
                const auto xVel = random(pixelsToUnits(-2), pixelsToUnits(2));
                const auto yPos = NPC->y + pixelsToUnits(random(-12, 12));
                const auto xPos = NPC->x + pixelsToUnits(random(-12, -12));
                createNpc(NPC_ProjectileBalrogEnergyBallBouncing, xPos, yPos, xVel, yVel);
            }
        }
        break;

    case landed:
        NPC->xm = 0;
        if (++NPC->act_wait > 3)
            NPC->act_no = wait;
        break;

    default:
        break;
    }

    if (NPC->act_no != flying)
    {
        NPC->ym += 51;
        NPC->facePlayer();
    }

    NPC->doGravity(0, 0x5FF);

    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

	array<RECT, 6> rcLeft;
	array<RECT, 6> rcRight;

    rcLeft[0] = {0, 0, 40, 24};
    rcLeft[1] = {40, 0, 80, 24};
    rcLeft[2] = {80, 0, 120, 24};
    rcLeft[3] = {120, 0, 160, 24};
    rcLeft[4] = {160, 48, 200, 72};
    rcLeft[5] = {200, 48, 240, 72};

    rcRight[0] = {0, 24, 40, 48};
    rcRight[1]= {40, 24, 80, 48};
    rcRight[2] = {80, 24, 120, 40};
    rcRight[3] = {120, 24, 160, 48};
    rcRight[4] = {160, 72, 200, 96};
    rcRight[5] = {200, 72, 240, 96};

    NPC->doRects(rcLeft, rcRight);
}

void npcAct037(npc *NPC) //Sign
{
	constexpr array<RECT, 2> rcNPC = { {{ 192, 64, 208, 80 }, { 208, 64, 224, 80 }} };

	NPC->animate(1, 0, 1);

	NPC->doRects(rcNPC);
}

void npcAct038(npc * NPC)
{
	constexpr array<RECT, 4> rcNPC = { { {128, 64, 144, 80}, {144, 64, 160, 80}, {160, 64, 176, 80}, {176, 64, 192, 80} } };

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

	NPC->animate(3, 0, 3);

	NPC->doRects(rcNPC);
}

void npcAct039(npc *NPC) //Save Sign
{
	constexpr array<RECT, 2> rcNPC = { {{224, 64, 240, 80}, {240, 64, 256, 80}} };

	NPC->ani_no = (NPC->direct == dirLeft) ? 0 : 1;

	NPC->doRects(rcNPC);
}
