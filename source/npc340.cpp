#include "npc340.h"

#include <array>
#include "player.h"
#include "mathUtils.h"
#include "sound.h"
#include "render.h"
#include "stage.h"
#include "game.h"
#include "flash.h"

void npcAct340(npc *NPC) // Ballos (boss) (first form)
{
    enum ballosStates
    {
        idleTalkingToPlayer = 0,
        fightBegin = 10,
        fightingStance = 100,
        flightAttack = 200,
        flyHorizontally = 210,
        flyUp = 220,
        flyDown = 230,
        returnToGround = 240,
        lightningStrike = 300,
        defeated = 1000,
        defeatedFallToGround = 1001,
        defeatedShaking = 1002,
        defeatedPrepareToJump = 1003,
        defeatedJumping = 1004,
    };

    enum ballosDamages
    {
        dmgNormal = 3,
        dmgRushing = 10,
    };

    switch (NPC->act_no)
    {
    case idleTalkingToPlayer:
        NPC->act_no = idleTalkingToPlayer + 1;
        NPC->cond = npccond_alive;
        NPC->exp = 1;
        NPC->direct = dirLeft;
        NPC->y -= pixelsToUnits(6);
        NPC->damage = 0;
        createNpc(NPC_Ballos1Head, NPC->x, NPC->y - tilesToUnits(1), 0, 0, 0, NPC);
        break;

    case fightBegin:
        NPC->act_no = fightBegin + 1;
        NPC->act_wait = 0;
        // Fallthrough
    case fightBegin + 1:
        if (++NPC->act_wait > secondsToFrames(2))
            NPC->act_no = fightingStance;
        break;

    case fightingStance:
        NPC->act_no = fightingStance + 10;
        NPC->act_wait = 0;
        NPC->ani_no = 1;
        NPC->ani_wait = 0;
        NPC->damage = 4;    // Wot ? PIXEEEEEEELLL !!! (useless - look 5 lines later)
        NPC->bits |= npc_shootable;
        // Fallthrough
    case fightingStance + 10:
        NPC->act_no = fightingStance + 11;
        NPC->damage = dmgNormal;
        NPC->tgt_x = NPC->life;
        // Fallthrough
    case fightingStance + 11:
        NPC->animate(10, 1, 2);
        if (++NPC->act_wait > 150 || NPC->life < NPC->tgt_x - 50)
        {
            auto tmp = NPC->count2 % 5;
            if (tmp >= 0)
            {
                if (tmp <= 3)
                    NPC->act_no = flightAttack;
                else	// tmp == 4
					NPC->act_no = lightningStrike;
            }
            ++NPC->count2;
        }
        NPC->facePlayer();
        break;

    case flightAttack:
        NPC->act_no = flightAttack + 1;
        NPC->count1 = 0;
        // Fallthrough
    case flightAttack + 1:
        if (NPC->xm)
            NPC->act_no = flightAttack + 3;
        else
            NPC->act_no = flightAttack + 2;
        NPC->act_wait = 0;
        NPC->ani_no = 3;
        NPC->damage = dmgNormal;
        ++NPC->count1;
        // Fallthrough
    case flightAttack + 2:
        NPC->facePlayer();
        NPC->xm = 8 * NPC->xm / 9;
        NPC->ym = 8 * NPC->ym / 9;
        if (++NPC->act_wait > 20)
            NPC->act_no = flyHorizontally;
        break;

    case flightAttack + 3:
        NPC->xm = 8 * NPC->xm / 9;
        NPC->ym = 8 * NPC->ym / 9;
        if (++NPC->act_wait > 20)
        {
            if (currentPlayer.y >= NPC->y + pixelsToUnits(12))
                NPC->act_no = flyDown;
            else
                NPC->act_no = flyUp;
        }
        break;

    case flyHorizontally:
        NPC->act_no = flyHorizontally + 1;
        NPC->ani_no = 6;
        NPC->ani_wait = 0;
        NPC->ym = 0;
        NPC->damage = dmgRushing;
        NPC->facePlayer();
        playSound(SFX_SillyExplosion);
        // Fallthrough
    case flyHorizontally + 1:
        NPC->moveInDir(pixelsToUnits(4));
        NPC->animate(1, 6, 7);

        if (NPC->direct == dirLeft && NPC->flag & leftWall)
        {
            NPC->act_no = flyHorizontally + 2;
            NPC->act_wait = 0;
            NPC->damage = dmgNormal;
            gViewport.quake2 = 10;
            playSound(SFX_LargeObjectHitGround);
        }

        if (NPC->count1 < 4 && currentPlayer.x > NPC->x - tilesToUnits(1) && currentPlayer.x < NPC->x + tilesToUnits(1))
            NPC->act_no = flightAttack + 1;
        break;

    case flyHorizontally + 2:
        ++NPC->act_wait;
        NPC->xm = 0;
        NPC->ani_no = 6;
        if (NPC->act_wait > 30)
        {
            if (NPC->count1 <= 3)
                NPC->act_no = flightAttack + 1;
            else
                NPC->act_no = returnToGround;
        }
        break;

    case flyUp:
        NPC->act_no = flyUp + 1;
        NPC->act_wait = 0;
        NPC->ani_no = 8;
        NPC->ani_wait = 0;
        NPC->xm = 0;
        NPC->damage = dmgRushing;
        NPC->direct = dirLeft;
        playSound(SFX_SillyExplosion);
        // Fallthrough
    case flyUp + 1:
        NPC->ym = pixelsToUnits(-4);
        NPC->animate(1, 8, 9);

        if (NPC->y < tilesToUnits(3))
        {
            NPC->y = tilesToUnits(3);
            NPC->ym = 0;
            NPC->act_no = flyUp + 2;
            NPC->act_wait = 0;
            NPC->damage = dmgNormal;
            for (size_t i = 0; i < 8; ++i)
                createNpc(NPC_Smoke, NPC->x + pixelsToUnits(random(-0x10, 0x10)), NPC->y - pixelsToUnits(10));
            createNpc(NPC_ProjectileBallosShockwave, NPC->x - pixelsToUnits(10), NPC->y - pixelsToUnits(10));
            createNpc(NPC_ProjectileBallosShockwave, NPC->x + pixelsToUnits(10), NPC->y + pixelsToUnits(10), 0, 0, dirRight);
            gViewport.quake2 = 10;
            playSound(SFX_LargeObjectHitGround);
        }

        if (NPC->count1 < 4 && currentPlayer.y > NPC->y - tilesToUnits(1) && currentPlayer.y < NPC->y + tilesToUnits(1))
            NPC->act_no = flightAttack + 1;
        break;

    case flyUp + 2:
        ++NPC->act_wait;
        NPC->xm = 0;
        NPC->ani_no = 8;
        if (NPC->act_wait > 30)
        {
            if (NPC->count1 <= 3)
                NPC->act_no = flightAttack + 1;
            else
                NPC->act_no = returnToGround;
        }
        break;

    case flyDown:
        NPC->act_no = flyDown + 1;
        NPC->act_wait = 0;
        NPC->ani_no = 8;
        NPC->ani_wait = 0;
        NPC->xm = 0;
        NPC->damage = dmgRushing;
        NPC->direct = dirRight;
        playSound(SFX_SillyExplosion);
        // Fallthrough
    case flyDown + 1:
        NPC->ym = pixelsToUnits(4);
        NPC->animate(1, 8, 9);

        if (NPC->flag & ground)
        {
            NPC->act_no = flyDown + 2;
            NPC->act_wait = 0;
            NPC->damage = dmgNormal;
            NPC->facePlayer();

            for (size_t i = 0; i < 8; ++i)
                createNpc(NPC_Smoke, NPC->x + pixelsToUnits(random(-0x10, 0x10)), NPC->y + pixelsToUnits(10));
            createNpc(NPC_ProjectileBallosShockwave, NPC->x - pixelsToUnits(10), NPC->y - pixelsToUnits(10));
            createNpc(NPC_ProjectileBallosShockwave, NPC->x + pixelsToUnits(10), NPC->y + pixelsToUnits(10), 0, 0, dirRight);
            
            gViewport.quake2 = 10;
            playSound(SFX_LargeObjectHitGround);
        }

        if (NPC->count1 < 4 && currentPlayer.y > NPC->y - tilesToUnits(1) && currentPlayer.y < NPC->y + tilesToUnits(1))
            NPC->act_no = flightAttack + 1;
        break;

    case flyDown + 2:
        ++NPC->act_wait;
        NPC->xm = 0;
        NPC->ani_no = 3;
        if (NPC->act_wait > 30)
        {
            if (NPC->count1 <= 3)
                NPC->act_no = flightAttack + 1;
            else
                NPC->act_no = returnToGround;
        }
        break;

    case returnToGround:
        NPC->act_no = returnToGround + 1;
        NPC->direct = dirLeft;
        // Fallthrough
    case returnToGround + 1:
        NPC->ym += pixelsToUnits(0.25);
        NPC->limitYVel(0x5FF);

        NPC->animate(1, 4, 5);

        if (NPC->flag & ground)
        {
            NPC->act_no = returnToGround + 2;
            NPC->act_wait = 0;
            NPC->ani_no = 3;
            NPC->facePlayer();
        }
        break;

    case returnToGround + 2:
        NPC->xm = 3 * NPC->xm / 4;
        NPC->ani_no = 3;
        if (++NPC->act_wait > 10)
            NPC->act_no = fightingStance + 10;
        break;

    case lightningStrike:
        NPC->act_no = lightningStrike + 10;
        NPC->act_wait = 0;
        NPC->ym = pixelsToUnits(-3);
        if (NPC->x <= tilesToUnits(20))
        {
            NPC->direct = dirLeft;
            NPC->tgt_x = currentPlayer.x;
            NPC->tgt_y = tilesToUnits(11);
        }
        else
        {
            NPC->direct = dirRight;
            NPC->tgt_x = currentPlayer.x;
            NPC->tgt_y = tilesToUnits(11);
        }
        NPC->ani_wait = 0;
        // Fallthrough
    case lightningStrike + 10:
        ++NPC->ani_wait;
        ++NPC->act_wait;
        if (NPC->act_wait <= secondsToFrames(4) || NPC->ani_wait >= secondsToFrames(0.4))
            NPC->direct = dirLeft;
        else
            NPC->direct = dirRight;

        if (NPC->ani_wait / 2 % 2)
            NPC->ani_no = 4;
        else
            NPC->ani_no = 5;

        NPC->accelerateTowardsXTarget(pixelsToUnits(0.125));
        NPC->accelerateTowardsYTarget(pixelsToUnits(0.125));

        NPC->limitXVel(pixelsToUnits(2));
        NPC->limitYVel(pixelsToUnits(2));

        if (NPC->act_wait > secondsToFrames(4) && NPC->act_wait % secondsToFrames(0.4) == 1)
        {
            NPC->ani_wait = 0;
            createNpc(NPC_ProjectileBallosLightning, currentPlayer.x, tilesToUnits(19));
        }

        if (NPC->act_wait > secondsToFrames(9.6))
        {
            NPC->act_no = lightningStrike + 20;
            NPC->act_wait = 0;
        }
        break;

    case lightningStrike + 20:
        NPC->xm = 0;
        NPC->ym = 0;
        NPC->direct = dirRight;
        if (++NPC->act_wait == secondsToFrames(0.8))
            setFlash(0, 0, flashModes::normal, 20);
        
        if (NPC->act_wait > secondsToFrames(1) && NPC->act_wait % secondsToFrames(0.2) == 1)
            createNpc(NPC_ProjectileBallosLightning
            , tilesToUnits((((4 * NPC->act_wait - secondsToFrames(4)) / 10 + 2))), tilesToUnits(19));

        if (NPC->act_wait > secondsToFrames(2.8))
            NPC->act_no = returnToGround;

        NPC->animate(1, 4, 5);
        break;

    case defeated:
        NPC->act_no = defeatedFallToGround;
        NPC->act_wait = 0;
        NPC->ani_no = 10;
        NPC->tgt_x = NPC->x;
        NPC->xm = 0;
        NPC->bits &= ~npc_shootable;
        createSmokeLeft(NPC->x, NPC->y, 0x10, 0x10);
        playSound(SFX_Explosion);
        // Fallthrough
    case defeatedFallToGround:
        NPC->ym += pixelsToUnits(0.0625);
        NPC->limitYVel(pixelsToUnits(3) - 1);
        if (++NPC->act_wait / 2 % 2)
            NPC->x = NPC->tgt_x + pixelsToUnits(1);
        else
            NPC->x = NPC->tgt_x - pixelsToUnits(1);

        if (NPC->flag & ground)
        {
            NPC->act_no = defeatedShaking;
            NPC->act_wait = 0;
        }
        break;

    case defeatedShaking:
        if (++NPC->act_wait > secondsToFrames(3))
        {
            NPC->act_wait = 0;
            NPC->act_no = defeatedPrepareToJump;
            NPC->ani_no = 3;
        }

        if (NPC->act_wait / 2 % 2)
            NPC->x = NPC->tgt_x + pixelsToUnits(1);
        else
            NPC->x = NPC->tgt_x - pixelsToUnits(1);
        break;

    case defeatedPrepareToJump:
        if (++NPC->act_wait > secondsToFrames(0.6))
        {
            NPC->act_wait = 0;
            NPC->act_no = defeatedJumping;
            NPC->ani_no = 3;
            NPC->ym -= pixelsToUnits(5);
            NPC->direct = dirLeft;
            NPC->bits |= npc_ignoreSolid;
        }
        break;

    case defeatedJumping:
        if (NPC->y < 0)
        {
            NPC->xm = 0;
            NPC->ym = 0;
            NPC->act_no = defeatedJumping + 1;
            NPC->act_wait = 0;
            setFlash(0, 0, flashModes::normal, 20);
            playSound(SFX_Teleport);
        }

        if (++NPC->act_wait / 2 % 2)
            NPC->ani_no = 8;
        else
            NPC->ani_no = 9;
        break;
    }

    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    constexpr std::array<RECT, 11> rcLeft =
    {{
        {0, 0, 48, 40},
        {48, 0, 96, 40},
        {96, 0, 144, 40},
        {144, 0, 192, 40},
        {192, 0, 240, 40},
        {240, 0, 288, 40},
        {0, 80, 48, 120},
        {48, 80, 96, 120},
        {96, 80, 144, 120},
        {144, 80, 192, 120},
        {192, 80, 240, 120},
    }};

    constexpr std::array<RECT, 11> rcRight =
    {{
        {0, 40, 48, 80},
        {48, 40, 96, 80},
        {96, 40, 144, 80},
        {144, 40, 192, 80},
        {192, 40, 240, 80},
        {240, 40, 288, 80},
        {0, 120, 48, 160},
        {48, 120, 96, 160},
        {96, 120, 144, 160},
        {144, 120, 192, 160},
        {192, 120, 240, 160},
    }};

    NPC->doRects(rcLeft, rcRight);
}

void npcAct341(npc *NPC) // Ballos head
{
	constexpr std::array<RECT, 3> rcNPC = {{{288, 32, 320, 48}, {288, 48, 320, 64}, {288, 64, 320, 80}}};

    if (NPC->pNpc)
    {
        if (NPC->pNpc->act_no == 11 && NPC->pNpc->act_wait > 50)
            NPC->animate(4, 2, 2);

        if (NPC->pNpc->ani_no != 0)
            NPC->cond = 0;
    }

    NPC->doRects(rcNPC);
}

void npcAct344(npc *NPC) // Ballos 2nd form display box
{
    NPC->doRects({272, 0, 296, 16}, {296, 0, 320, 16});

    if (NPC->pNpc)
    {
        if (NPC->direct != dirLeft)
            NPC->x = NPC->pNpc->x + tilesToUnits(1.5);
        else
            NPC->x = NPC->pNpc->x - tilesToUnits(1.5);
    }

    if (++NPC->act_wait > secondsToFrames(2))
        NPC->cond = 0;

    if (NPC->pNpc)
        NPC->y = NPC->pNpc->y - tilesToUnits(2.25);
}

void npcAct347(npc *NPC) // Hoppy (enemy)
{
    enum
    {
        init = 0,
        waitForPlayer = 1,
        startJump = 10,
        jumping = 12,
        land = 13,
    };

    switch (NPC->act_no)
    {
    case init:
        NPC->act_no = waitForPlayer;
        // Fallthrough
    case waitForPlayer:
        NPC->ani_no = 0;
        if (currentPlayer.y < NPC->y + tilesToUnits(8) && currentPlayer.y > NPC->y - tilesToUnits(8))
        {
            NPC->act_no = startJump;
            NPC->act_wait = 0;
            NPC->ani_no = 1;
        }
        break;

    case startJump:
        if (++NPC->act_wait == 4)
            NPC->ani_no = 2;
        if (NPC->act_wait > 12)
        {
            NPC->act_no = jumping;
            NPC->xm = 0x700;
            playSound(SFX_HighPitchCritterHop);
            NPC->ani_no = 3;
        }
        break;

    case jumping:
        if (currentPlayer.y >= NPC->y)
            NPC->ym = 0xAA;
        else
            NPC->ym = -0xAA;

        if (NPC->flag & leftWall)
        {
            NPC->act_no = land;
            NPC->act_wait = 0;
            NPC->ani_no = 2;
            NPC->xm = 0;
            NPC->ym = 0;
        }
        else
        {
            NPC->xm -= 42;
            if (NPC->xm < -0x5FF)
                NPC->xm = -0x5FF;

            NPC->x += NPC->xm;
            NPC->y += NPC->ym;
        }
        break;

    case land:
        if (++NPC->act_wait == 2)
            NPC->ani_no = 1;
        else if (NPC->act_wait == 6)
            NPC->ani_no = 0;
        else if (NPC->act_wait > 16)
            NPC->act_no = waitForPlayer;
        break;

    default:
        break;
    }

	constexpr std::array<RECT, 4> rcNPC = { {{256, 48, 272, 64}, {272, 48, 288, 64}, {288, 48, 304, 64}, {304, 48, 320, 64}} };

    NPC->doRects(rcNPC);
}

void npcAct348(npc *NPC) // Generator - Ballos spikes
{
    if (!NPC->act_no)
        NPC->act_no = 1;

    if (NPC->act_no == 1)
    {
        if (++NPC->act_wait >= 0x80)
        {
            NPC->act_no = 10;
            NPC->ani_no = 0;
            NPC->damage = 2;
        }
        else
        {
            NPC->y -= pixelsToUnits(0.25);

            if (NPC->act_wait / 2 % 2)
                NPC->ani_no = 1;
            else
                NPC->ani_no = 0;
        }
    }

    constexpr std::array<RECT, 2> rcNPC = {{{128, 152, 160, 176}, {160, 152, 192, 176}}};

    NPC->doRects(rcNPC);
}

void npcAct349(npc *NPC)
{
	if (NPC->act_no == 0)
	{
		NPC->act_no = 1;
		if (NPC->direct == dirLeft)
			NPC->x += pixelsToUnits(8);
		else if (NPC->direct == dirRight)
			NPC->y += tilesToUnits(1);
	}

	NPC->doRects({ 0, 0, 16, 16 });
}

void npcAct354(npc *NPC) // Invisible deathtrap wall
{
    switch (NPC->act_no)
    {
    case 0:
        NPC->hit.bottom = tilesToUnits(17.5);
        return;

    case 10:
        NPC->act_no = 11;
        NPC->act_wait = 0;
        if (NPC->direct != dirLeft)
            NPC->x -= tilesToUnits(1);
        else
            NPC->x += tilesToUnits(1);
        // Fallthrough
    case 11:
        if (++NPC->act_wait > 100)
        {
            NPC->act_wait = 0;
            gViewport.quake = 20;
            playSound(SFX_LargeObjectHitGround);
            playSound(SFX_DestroyBreakableBlock);
            if (NPC->direct != dirLeft)
                NPC->x += tilesToUnits(1);
            else
                NPC->x -= tilesToUnits(1);
            for (size_t i = 0; i < 20; ++i)
                changeTile(unitsToTiles(NPC->x), i + unitsToTiles(NPC->y), 109);
        }
    }
}

void npcAct355(npc *NPC) // Balrog, crashing through wall
{
    constexpr std::array<RECT, 4> rcNPC = {{{80, 16, 96, 32}, {80, 96, 96, 112}, {128, 16, 144, 32}, {208, 96, 224, 112}}};

    if (!NPC->act_no && NPC->pNpc)
    {
        switch (NPC->direct)
        {
        case dirLeft:
            NPC->surf = 16;
            NPC->ani_no = 0;
            NPC->x = NPC->pNpc->x - pixelsToUnits(14);
            NPC->y = NPC->pNpc->y + pixelsToUnits(10);
            break;

        case dirUp:
            NPC->surf = 23;
            NPC->ani_no = 1;
            NPC->x = NPC->pNpc->x + pixelsToUnits(14);
            NPC->y = NPC->pNpc->y + pixelsToUnits(10);
            break;

        case dirRight:
            NPC->surf = 16;
            NPC->ani_no = 2;
            NPC->x = NPC->pNpc->x - pixelsToUnits(7);
            NPC->y = NPC->pNpc->y - pixelsToUnits(19);
            break;

        case dirDown:
            NPC->surf = 23;
            NPC->ani_no = 3;
            NPC->x = NPC->pNpc->x + pixelsToUnits(4);
            NPC->y = NPC->pNpc->y - pixelsToUnits(19);
        }
    }
    else
        NPC->ani_no = 0;

    NPC->doRects(rcNPC);
}

void npcAct359(npc *NPC)
{
	if (currentPlayer.x < NPC->x + tilesToUnits(20) && currentPlayer.x > NPC->x - tilesToUnits(20)
     && currentPlayer.y < NPC->y + tilesToUnits(20) && currentPlayer.y > NPC->y - tilesToUnits(20) && random(0, 100) == 2)
		createNpc(NPC_WaterDrop1, (random(-6, 6) << 9) + NPC->x, NPC->y - pixelsToUnits(7));
}

