#include "npc240.h"

#include <array>
#include "render.h"
#include "player.h"
#include "stage.h"
#include "caret.h"
#include "sound.h"
#include "mathUtils.h"
#include "render.h"
#include "sound.h"
#include "game.h"

using std::array;

void npcAct240(npc *NPC) // Mimiga (jailed)
{
	RECT rcLeft[6];

	rcLeft[0].left = 160;
	rcLeft[0].top = 64;
	rcLeft[0].right = 176;
	rcLeft[0].bottom = 80;

	rcLeft[1].left = 176;
	rcLeft[1].top = 64;
	rcLeft[1].right = 192;
	rcLeft[1].bottom = 80;

	rcLeft[2].left = 192;
	rcLeft[2].top = 64;
	rcLeft[2].right = 208;
	rcLeft[2].bottom = 80;

	rcLeft[3].left = 160;
	rcLeft[3].top = 64;
	rcLeft[3].right = 176;
	rcLeft[3].bottom = 80;

	rcLeft[4].left = 208;
	rcLeft[4].top = 64;
	rcLeft[4].right = 224;
	rcLeft[4].bottom = 80;

	rcLeft[5].left = 160;
	rcLeft[5].top = 64;
	rcLeft[5].right = 176;
	rcLeft[5].bottom = 80;

	RECT rcRight[6];

	rcRight[0].left = 160;
	rcRight[0].top = 80;
	rcRight[0].right = 176;
	rcRight[0].bottom = 96;

	rcRight[1].left = 176;
	rcRight[1].top = 80;
	rcRight[1].right = 192;
	rcRight[1].bottom = 96;

	rcRight[2].left = 192;
	rcRight[2].top = 80;
	rcRight[2].right = 208;
	rcRight[2].bottom = 96;

	rcRight[3].left = 160;
	rcRight[3].top = 80;
	rcRight[3].right = 176;
	rcRight[3].bottom = 96;

	rcRight[4].left = 208;
	rcRight[4].top = 80;
	rcRight[4].right = 224;
	rcRight[4].bottom = 96;

	rcRight[5].left = 160;
	rcRight[5].top = 80;
	rcRight[5].right = 176;
	rcRight[5].bottom = 96;

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
		// fallthrough
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
		NPC->act_wait = random(0, 0x10);
		NPC->ani_no = 2;
		NPC->ani_wait = 0;

		if (random(0, 9) % 2)
			NPC->direct = dirLeft;
		else
			NPC->direct = dirRight;

		// fallthrough
	case 11:
		if (NPC->direct == dirLeft && (NPC->flag & leftWall))
			NPC->direct = dirRight;
		else if (NPC->direct == dirRight && (NPC->flag & rightWall))
			NPC->direct = dirLeft;

		if (NPC->direct != dirLeft)
			NPC->xm = 0x200;
		else
			NPC->xm = -0x200;

		if (++NPC->ani_wait > 4)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 5)
			NPC->ani_no = 2;

		if (++NPC->act_wait > 0x20)
			NPC->act_no = 0;

		break;
	}

	NPC->ym += 0x20;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct241(npc *NPC) // Critter, Hopping Red (enemy)
{
	constexpr array<RECT, 3> rcLeft = { {{0, 0, 16, 16}, {16, 0, 32, 16}, {32, 0, 48, 16}} };
	constexpr array<RECT, 3> rcRight = { {{0, 16, 16, 32}, {16, 16, 32, 32}, {32, 16, 48, 32}} };

    enum
    {
        init = 0,
        normal = 1,
        startHop = 2,
        jumping = 3,
    };

    switch (NPC->act_no)
    {
    case init:
        NPC->y += pixelsToUnits(3);
        NPC->act_no = normal;
        // Fallthrough
    case normal:
        NPC->facePlayer();
        if (NPC->act_wait < 8 || NPC->isPlayerAligned(tilesToUnits(9), tilesToUnits(5)))
        {
            if (NPC->act_wait < 8)
                ++NPC->act_wait;
            NPC->ani_no = 0;
        }
        else
            NPC->ani_no = 1;

        if (NPC->shock || (NPC->act_wait >= 8 && NPC->isPlayerWithinDistance(tilesToUnits(6), tilesToUnits(5), tilesToUnits(6))))
        {
            NPC->act_no = startHop;
            NPC->ani_no = 0;
            NPC->act_wait = 0;
        }
        break;

    case startHop:
        if (++NPC->act_wait > 8)
        {
            NPC->act_no = jumping;
            NPC->ani_no = 2;
            NPC->ym = -0x5FF;
            playSound(SFX_CritterHop);
            NPC->moveInDir(pixelsToUnits(1));
        }
        break;

    case jumping:
        if (NPC->flag & ground)
        {
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

    NPC->doGravity(0x55, 0x5FF);

    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    NPC->doRects(rcLeft, rcRight);
}

void npcAct242(npc *NPC) // Bat, Red Wave (enemy)
{
    if (NPC->x < 0 || NPC->x > tilesToUnits(map.width))
    {
		NPC->init(3, NPC->x, NPC->y, 0, 0, 0, nullptr);
        return;
    }

    switch(NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->tgt_x = NPC->x;
        NPC->tgt_y = NPC->y;
        NPC->act_wait = random(0, 50);
        // Fallthrough
    case 1:
        if (NPC->act_wait--)
            break;
        NPC->act_no = 2;
        NPC->ym = pixelsToUnits(2);
        // Fallthrough
    case 2:
		if (NPC->direct)
			NPC->xm = 0x100;
		else
			NPC->xm = -0x100;

        if (NPC->tgt_y < NPC->y)
            NPC->ym -= 0x10;
        else
            NPC->ym += 0x10;

        if (NPC->ym > 0x300)
            NPC->ym = 0x300;
        else if (NPC->ym < -0x300)
            NPC->ym = -0x300;
        break;

    default:
        break;
    }

    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

	constexpr array<RECT, 4> rcLeft = { {{32, 32, 48, 48}, {48, 32, 64, 48}, {64, 32, 80, 48}, {80, 32, 96, 48}} };
	constexpr array<RECT, 4> rcRight = { {{32, 48, 48, 64}, {48, 48, 64, 64}, {64, 48, 80, 64}, {80, 48, 96, 64}} };

    NPC->animate(1, 0, 2);
    NPC->doRects(rcLeft, rcRight);
}

void npcAct243(npc *NPC) // Generator - Red Bat
{
	if (NPC->act_no)
	{
		if (NPC->act_no != 1)
			return;
	}
	else
	{
		NPC->act_no = 1;
		NPC->act_wait = random(0, 500);
	}
	if (NPC->act_wait)
		--NPC->act_wait;
	else
	{
		NPC->act_no = 0;
		createNpc(NPC_EnemyBatRed, NPC->x, NPC->y + pixelsToUnits(random(-0x20, 0x20)), 0, 0, NPC->direct);
	}
}

void npcAct244(npc *NPC)
{
	NPC->ym += 0x40;
	bool die = false;
	if (NPC->flag & 0xFF || (NPC->act_wait > 10 && NPC->flag & 0x100))
		die = true;

	if (die)
	{
		for (int i = 0; i < 3; ++i)
			createCaret(NPC->x, NPC->y + pixelsToUnits(4), effect_fountainDisk, dirRight);
		if (NPC->x > currentPlayer.x - tilesToUnits(16)
		        && NPC->x < currentPlayer.x + tilesToUnits(16)
		        && NPC->y > currentPlayer.y - tilesToUnits(10)
		        && NPC->y < currentPlayer.y + tilesToUnits(10))
			playSound(SFX_Bubble);
        NPC->cond = 0;
	}
	else
    {
        if (NPC->ym > 0x5FF)
            NPC->ym = 0x5FF;
        NPC->y += NPC->ym;

        NPC->doRects({96, 0, 104, 16});
    }
}

void npcAct245(npc *NPC) // Generator - Lava Drop
{
	constexpr array<RECT, 4> rcNPC = { {{0, 0, 0, 0}, {104, 0, 112, 16}, {112, 0, 120, 16}, {120, 0, 128, 16}} };

	if (NPC->act_no)
	{
		if (NPC->act_no != 1)
		{
			if (NPC->act_no == 10)
			{
				if (++NPC->ani_wait > 10)
				{
					NPC->ani_wait = 0;
                    ++NPC->ani_no;
				}
				if (NPC->ani_no > 3)
				{
					NPC->ani_no = 0;
					NPC->act_no = 1;
                    NPC->act_wait = NPC->code_flag;
                    createNpc(NPC_AcidDrop, NPC->x, NPC->y);
				}
			}
			goto endOfAI;
		}
	}
	else
    {
        NPC->act_no = 1;
        NPC->tgt_x = NPC->x;
        NPC->act_wait = NPC->code_event;
    }

    NPC->ani_no = 0;
    if (NPC->act_wait)
    {
        --NPC->act_wait;
        return;
    }
    NPC->act_no = 10;
    NPC->act_wait = 0;

endOfAI:
    if (NPC->act_wait / 2 % 2)
        NPC->x = NPC->tgt_x;
    else
        NPC->x = NPC->tgt_x + 0x200;

    NPC->doRects(rcNPC);
}

void npcAct246(npc *NPC) // Press, Proximity (enemy)
{
	constexpr array<RECT, 3> rcNPC = { {{144, 112, 160, 136}, {160, 112, 176, 136}, {176, 112, 196, 136}} };

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->y -= pixelsToUnits(4);
        // Fallthrough
    case 1:
		if (currentPlayer.x < NPC->x + pixelsToUnits(8)
      && currentPlayer.x > NPC->x - pixelsToUnits(8)
      && currentPlayer.y > NPC->y + pixelsToUnits(8)
      && currentPlayer.y < NPC->y + tilesToUnits(8))
            NPC->act_no = 5;
        break;

    case 5:
        if (!(NPC->flag & ground))
        {
            NPC->act_no = 10;
            NPC->ani_wait = 0;
            NPC->ani_no = 1;
        }
        break;

    case 10:
        NPC->animate(2, 2, 2);

        if (currentPlayer.y <= NPC->y)
        {
            NPC->bits |= npc_solidHard;
            NPC->damage = 0;
        }
        else
        {
            NPC->bits &= ~npc_solidHard;
            NPC->damage = 127;
        }

        if (NPC->flag & ground)
        {
            if (NPC->ani_no > 1)
            {
                for (size_t i = 0; i < 4; ++i)
                {
                    const auto yVel = random(-0x600, 0);
                    const auto xVel = random(-0x155, 0x155);
                    createNpc(NPC_Smoke, NPC->x, NPC->y, xVel, yVel);
                }
                playSound(SFX_LargeObjectHitGround);
                viewport.quake = 10;
            }
            NPC->act_no = 20;
            NPC->ani_no = 0;
            NPC->ani_wait = 0;
            NPC->bits |= npc_solidHard;
            NPC->damage = 0;
        }
        break;

    default:
        break;
    }

    if (NPC->act_no >= 5)
    {
        NPC->doGravity(0x80, 0x5FF);
        NPC->y += NPC->ym;
    }

    NPC->doRects(rcNPC);
}

void npcAct247(npc *NPC)
{
    constexpr array<RECT, 9> rcLeft =
    {{
        { 0, 0, 16, 16 },
        { 16, 0, 32, 16},
        { 32, 0, 48, 16},
        { 48, 0, 64, 16 },
        { 64, 0, 80, 16 },
        { 80, 0, 96, 16 },
        { 96, 0, 112, 16},
        { 0, 0, 0, 0 },
        { 112, 0, 128, 16 },
    }};

    constexpr array<RECT, 9> rcRight =
    {{
        { 0, 16, 16, 32},
        { 16, 16, 32, 32},
        { 32, 16, 48, 32},
        { 48, 16, 64, 32},
        { 64, 16, 80, 32},
        { 80, 16, 96, 32},
        { 96, 16, 112, 32},
        { 0, 0, 0, 0},
        { 112, 0, 128, 32},
    }};

	enum
	{
		init = 0,
		stand = 1,
		blink = 2,
		jumpFromThrone = 20,
		standAfterJumpFromThrone = 21,
		blinkAfterJumpFromThrone = 22,
		fighting = 100,
		flashForSpell = 110,
		startTeleportAway = 112,
		summonBlock = 113,
		teleportingAway = 150,
		summonBalls = 160,
		defeated = 1000,
		shake = 1001,
		fallToGround = 1010,
	};


	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = 1;
		NPC->y += pixelsToUnits(6);
		NPC->tgt_y = tilesToUnits(4);
		// Fallthrough
    case stand:
        if (!random(0, 120))
        {
            NPC->act_no = blink;
            NPC->act_wait = 0;
            NPC->ani_no = 1;
        }
        break;

    case blink:
        if (++NPC->act_wait > 8)
        {
            NPC->act_no = stand;
            NPC->ani_no = 0;
        }
        break;

    case jumpFromThrone:
        NPC->xm = 0;
        NPC->ym += 0x40;
        if (NPC->flag & ground)
        {
            NPC->act_no = standAfterJumpFromThrone;
            NPC->ani_no = 2;
        }
        break;

    case standAfterJumpFromThrone:
        if (!random(0, 120))
        {
            NPC->act_no = blinkAfterJumpFromThrone;
            NPC->act_wait = 0;
            NPC->ani_no = 3;
        }
        break;

    case blinkAfterJumpFromThrone:
        if (++NPC->act_wait > 8)
        {
            NPC->act_no = standAfterJumpFromThrone;
            NPC->ani_no = 2;
        }
        break;

    case fighting:
        NPC->act_no = fighting + 1;
        NPC->act_wait = 0;
        NPC->ani_no = 0;
        NPC->xm = 0;
        NPC->bits |= npc_shootable;
        NPC->count2 = NPC->life;
        // Fallthrough
    case fighting + 1:
        NPC->facePlayer();
        NPC->accelerateTowardsYTarget(0x20);
        NPC->limitYVel(pixelsToUnits(1));

        if (++NPC->act_wait > secondsToFrames(4) || NPC->life <= NPC->count2 - 80)
        {
            NPC->act_wait = 0;
            NPC->act_no = flashForSpell;
        }
        break;

    case flashForSpell:
        NPC->act_no = flashForSpell + 1;
        NPC->act_wait = 0;
        NPC->xm = 0;
        NPC->ym = 0;
        NPC->bits &= ~npc_shootable;
        // Fallthrough
    case flashForSpell + 1:
        if (++NPC->act_wait & 1)
            NPC->ani_no = 5;
        else
            NPC->ani_no = 6;

        if (NPC->act_wait > 30)
        {
            NPC->act_wait = 0;
            if (++NPC->count1 % 3)
                NPC->act_no = startTeleportAway;
            else
                NPC->act_no = summonBlock;
            NPC->ani_no = 4;
        }
        break;

    case startTeleportAway:
        if (!(++NPC->act_wait % 6))
        {
            auto angle = getAtan(NPC->x - currentPlayer.x, NPC->y - currentPlayer.y);
            angle += random(-4, 4);
            const auto xVel = 4 * getCos(angle);
            const auto yVel = 4 * getSin(angle);
            createNpc(NPC_BossMiseryVanish, NPC->x, NPC->y + pixelsToUnits(4), xVel, yVel);
            playSound(SFX_FireballBounce);
        }
        if (NPC->act_wait > 30)
        {
            NPC->act_wait = 0;
            NPC->act_no = teleportingAway;
        }
        break;

    case summonBlock:
        if (++NPC->act_wait == secondsToFrames(0.2))
            createNpc(NPC_FallingBlockLarge, currentPlayer.x, currentPlayer.y - tilesToUnits(4), 0, 0, dirUp);
        if (NPC->act_wait > 30)
        {
            NPC->act_wait = 0;
            NPC->act_no = teleportingAway;
        }
        break;

    case teleportingAway:
        NPC->act_no = teleportingAway + 1;
        NPC->act_wait = 0;
        NPC->ani_no = 7;
        createNpc(NPC_ProjectileMiseryEnergyShot, NPC->x, NPC->y, 0, 0, dirLeft);
        createNpc(NPC_ProjectileMiseryEnergyShot, NPC->x, NPC->y, 0, 0, dirRight);

        NPC->tgt_x = tilesToUnits(random(9, 31));
        NPC->tgt_y = tilesToUnits(random(5, 7));
        playSound(SFX_Teleport);
        // Fallthrough
    case teleportingAway + 1:
        if (++NPC->act_wait == 42)
        {
            createNpc(NPC_ProjectileMiseryEnergyShot, NPC->tgt_x + tilesToUnits(1), NPC->tgt_y, 0, 0, dirLeft);
            createNpc(NPC_ProjectileMiseryEnergyShot, NPC->tgt_x - tilesToUnits(1), NPC->tgt_y, 0, 0, dirRight);
        }
        if (NPC->act_wait > 50)
        {
            NPC->act_wait = 0;
            NPC->ym = pixelsToUnits(-1);
            NPC->bits |= npc_shootable;
            NPC->x = NPC->tgt_x;
            NPC->y = NPC->tgt_y;

            if (NPC->life < 340)
            {
                createNpc(NPC_ProjectileMiseryBlackOrbitingRings, 0, 0, 0, 0, 0, NPC);
                createNpc(NPC_ProjectileMiseryBlackOrbitingRings, 0, 0, 0, 0, 128, NPC);
            }
            if (NPC->life < 180)
            {
                createNpc(NPC_ProjectileMiseryBlackOrbitingRings, 0, 0, 0, 0, 64, NPC);
                createNpc(NPC_ProjectileMiseryBlackOrbitingRings, 0, 0, 0, 0, 192, NPC);
            }

            if (currentPlayer.x >= NPC->x - tilesToUnits(7) && currentPlayer.x <= NPC->x + tilesToUnits(7))
                NPC->act_no = fighting;
            else
                NPC->act_no = summonBalls;
        }
        break;

    case summonBalls:
        NPC->act_no = summonBalls + 1;
        NPC->act_wait = 0;
        NPC->ani_no = 4;
        NPC->facePlayer();
        // Fallthrough
    case summonBalls + 1:
        NPC->accelerateTowardsYTarget(0x20);
        NPC->limitYVel(pixelsToUnits(1));

        if (!(++NPC->act_wait % 24))
        {
            createNpc(NPC_ProjectileMiseryLightningBall, NPC->x, NPC->y + pixelsToUnits(4));
            playSound(SFX_FireballBounce);
        }

        if (NPC->act_wait > 72)
        {
            NPC->act_wait = 0;
            NPC->act_no = fighting;
        }
        break;

    case defeated:
        NPC->bits &= ~npc_shootable;
        NPC->act_no = shake;
        NPC->act_wait = 0;
        NPC->ani_no = 4;
        NPC->tgt_x = NPC->x;
        NPC->tgt_y = NPC->y;
        NPC->xm = 0;
        NPC->ym = 0;
        killNpcsByType(NPC_ProjectileMiseryBlackOrbitingRings);
        for (int i = 0; i < 3; ++i)
            createNpc(NPC_Smoke, NPC->x, NPC->y);
        // Fallthrough
    case shake:
        if (++NPC->act_wait / 2 & 1)
            NPC->x = NPC->tgt_x + pixelsToUnits(1);
        else
            NPC->x = NPC->tgt_x;
        break;

    case fallToGround:
        NPC->ym += 0x10;
        if (NPC->flag & ground)
        {
            NPC->act_no = fallToGround + 10;
            NPC->ani_no = 8;
        }

    default:
        break;
    }

    NPC->limitXVel(pixelsToUnits(1));
    NPC->limitYVel(pixelsToUnits(2));
    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    NPC->doRects(rcLeft, rcRight);
}

void npcAct248(npc *NPC) // Misery (vanishing)
{
	if (NPC->flag & solid)
	{
		NPC->cond = 0;
		createCaret(NPC->x, NPC->y, effect_RisingDisc, dirLeft);
	}

	NPC->y += NPC->ym;
	NPC->x += NPC->xm;

	RECT rect_left[3];

	rect_left[0].left = 0;
	rect_left[0].top = 48;
	rect_left[0].right = 16;
	rect_left[0].bottom = 64;

	rect_left[1].left = 16;
	rect_left[1].top = 48;
	rect_left[1].right = 32;
	rect_left[1].bottom = 64;

	rect_left[2].left = 32;
	rect_left[2].top = 48;
	rect_left[2].right = 48;
	rect_left[2].bottom = 64;

	if (++NPC->ani_wait > 1)
	{
		NPC->ani_wait = 0;

		if (++NPC->ani_no > 2)
			NPC->ani_no = 0;
	}

	NPC->rect = rect_left[NPC->ani_no];

	if (++NPC->count1 > 300)
	{
		NPC->cond = 0;
		createCaret(NPC->x, NPC->y, effect_RisingDisc, dirLeft);
	}
}

void npcAct251(npc *NPC) // Misery black lightning (projectile)
{
    constexpr array<RECT, 2> rcNPC = {{{80, 32, 96, 64}, {96, 32, 112, 64}}};

    if (!NPC->act_no)
        NPC->act_no = 1;

    if (NPC->act_no == 1)
    {
        NPC->animate(0, 0, 1);

        NPC->y += tilesToUnits(0.5);

        if (NPC->flag & solid)
        {
            createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 3);
            NPC->cond = 0;
        }
    }

    NPC->doRects(rcNPC);
}

void npcAct253(npc *NPC) // Energy Capsule
{
	if (!NPC->act_no)
		NPC->act_no = 1;
	if (NPC->act_no == 1)
		NPC->animate(4, 0, 1);

	if (NPC->life <= 100)
	{
		dropExperience(NPC->x, NPC->y, NPC->code_flag);
		createSmokeLeft(NPC->x, NPC->y, NPC->view.right, 8);
		playSound(SFX_SillyExplosion);
		NPC->cond = 0;
	}

	constexpr array<RECT, 2> rcNPC = { { {0, 64, 16, 80}, {16, 64, 32, 80} } };

	NPC->doRects(rcNPC);
}

void npcAct254(npc *NPC) // Helicopter
{
    enum
    {
        stopped = 0,
        bladesRunning = 20,
        momorinBladesRunning = 30,
        creditsBladesRunning = 40
    };

    switch (NPC->act_no)
    {
    case stopped:
        NPC->act_no = stopped + 1;
        createNpc(NPC_HelicopterBlades, NPC->x + pixelsToUnits(18), NPC->y - pixelsToUnits(57), 0, 0, dirLeft, NPC);
        createNpc(NPC_HelicopterBlades, NPC->x - pixelsToUnits(32), NPC->y - pixelsToUnits(52), 0, 0, dirRight, NPC);
        break;

    case bladesRunning:
        NPC->act_wait = 0;
        NPC->count1 = 60;
        NPC->act_no = bladesRunning + 1;
        break;

    case momorinBladesRunning:
        NPC->act_no = bladesRunning + 1;
        createNpc(NPC_Momorin, NPC->x - pixelsToUnits(11), NPC->y - pixelsToUnits(14));
        break;

    case creditsBladesRunning:
        NPC->act_no = bladesRunning + 1;
        createNpc(NPC_Momorin, NPC->x - pixelsToUnits(9), NPC->y - pixelsToUnits(14));
        createNpc(NPC_Santa, NPC->x - pixelsToUnits(22), NPC->y - pixelsToUnits(14));
        createNpc(NPC_Chaco, NPC->x - pixelsToUnits(35), NPC->y - pixelsToUnits(14));
        break;

    default:
        break;
    }

	NPC->doRects({ 0, 0, 128, 64 }, { 0, 64, 128, 128 });
}

void npcAct255(npc *NPC) // Helicopter Blades
{
	constexpr array<RECT, 4> rcLeft = { {{128, 0, 240, 16}, {128, 16, 240, 32}, {128, 32, 240, 48}, {128, 16, 240, 32}} };
	constexpr array<RECT, 4> rcRight = { { {240, 0, 320, 16}, {240, 16, 320, 32}, {240, 32, 320, 48}, {240, 16, 320, 32} } };

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        if (NPC->direct != dirLeft)
        {
            NPC->view.left = 0x5000;
            NPC->view.right = 0x5000;
        }
        else
        {
            NPC->view.left = 0x7000;
            NPC->view.right = 0x7000;
        }
        // Fallthrough
    case 1:
        if (NPC->pNpc && NPC->pNpc->act_no >= 20)
            NPC->act_no = 10;
        break;

    case 10:
        NPC->act_no = 11;
        // Fallthrough
    case 11:
        NPC->animate(0, 0, 3);
        break;

    default:
        break;
    }

    if (NPC->pNpc)
    {
        if (NPC->direct != dirLeft)
        {
            NPC->x = NPC->pNpc->x - tilesToUnits(2);
            NPC->y = NPC->pNpc->y - pixelsToUnits(52);
        }
        else
        {
            NPC->x = NPC->pNpc->x + pixelsToUnits(18);
            NPC->y = NPC->pNpc->y - pixelsToUnits(59);
        }
    }

    NPC->doRects(rcLeft, rcRight);
}

void npcAct258(npc *NPC) // Mimiga, sleeping
{
    NPC->doRects({48, 32, 64, 48});
}

void npcAct259(npc *NPC) // Sleeping mimiga
{
	NPC->doRects({ 48, 32, 64, 48 });
}
