#include "npc260.h"

#include <array>
#include "mathUtils.h"
#include "player.h"
#include "render.h"
#include "sound.h"
#include "stage.h"
#include "game.h"

using std::array;

void npcAct263(npc *NPC) // Doctor (boss)
{
    array<RECT, 9> rcLeft, rcRight;

    rcLeft[0] = {0, 0, 24, 32};
    rcLeft[1] = {24, 0, 48, 32};
    rcLeft[2] = {48, 0, 72, 32};
    rcLeft[3] = {0, 0, 0, 0};
    rcLeft[4] = {72, 0, 96, 32};
    rcLeft[5] = {96, 0, 120, 32};
    rcLeft[6] = {120, 0, 144, 32};
    rcLeft[7] = {144, 0, 168, 32};
    rcLeft[8] = {264, 0, 288, 32};

    rcRight[0] = {0, 32, 24, 64};
    rcRight[1] = {24, 32, 48, 64};
    rcRight[2] = {48, 32, 72, 64};
    rcRight[3] = rcLeft[3];
    rcRight[4] = {72, 32, 96, 64};
    rcRight[5] = {96, 32, 120, 64};
    rcRight[6] = {120, 32, 144, 64};
    rcRight[7] = {144, 32, 168, 64};
    rcRight[8] = {264, 32, 288, 64};

	enum
	{
		init = 0,
		transforming = 2,
		fighting = 10,
		fireWaveShot = 20,
		bigBlast = 30,
		afterBlast = 32,
		teleportAway = 100,
		waitingToReappear = 102,
		reappear = 103,
		defeated = 500,
		flashing = 501,
	};

    switch (NPC->act_no)
    {
    case init:
        NPC->act_no = init + 1;
        NPC->y += tilesToUnits(0.5);
        NPC->ani_no = 3;
        break;

    case transforming:
        if (++NPC->act_wait / 2 & 1)
            NPC->ani_no = 0;
        else
            NPC->ani_no = 3;

        if (NPC->act_wait > 50)
            NPC->act_no = fighting;
        break;

    case fighting:
        NPC->ym += 0x80;
        NPC->bits |= npc_shootable;
        NPC->damage = 3;
        if (NPC->flag & ground)
        {
            NPC->act_no = fireWaveShot;
            NPC->act_wait = 0;
            NPC->ani_no = 0;
            NPC->count2 = NPC->life;
            NPC->facePlayer();
        }
        break;

    case fireWaveShot:
        if (++NPC->act_wait < 50 && NPC->life < NPC->count2 - 20)
            NPC->act_wait = 50;
        if (NPC->act_wait == 50)
        {
            NPC->facePlayer();
            NPC->ani_no = 4;
        }

        if (NPC->act_wait == 80)
        {
            NPC->ani_no = 5;
            playSound(SFX_SillyExplosion);
            if (NPC->direct != dirLeft)
            {
                createNpc(NPC_ProjectileDoctorRedWave, NPC->x + tilesToUnits(1), NPC->y, 0, 0, dirRight);
                createNpc(NPC_ProjectileDoctorRedWave, NPC->x + tilesToUnits(1), NPC->y, 0, 0, 0x400 + dirRight);
            }
            else
            {
                createNpc(NPC_ProjectileDoctorRedWave, NPC->x - tilesToUnits(1), NPC->y, 0, 0, dirLeft);
                createNpc(NPC_ProjectileDoctorRedWave, NPC->x - tilesToUnits(1), NPC->y, 0, 0, 0x400 + dirLeft);
            }
        }

        if (NPC->act_wait == 120)
            NPC->ani_no = 0;
        if (NPC->act_wait > 130 && NPC->life < NPC->count2 - 50)
            NPC->act_wait = 161;

        if (NPC->act_wait > 160)
        {
            NPC->act_no = teleportAway;
            NPC->ani_no = 0;
        }
        break;

    case bigBlast:
        NPC->act_no = bigBlast + 1;
        NPC->act_wait = 0;
        NPC->ani_no = 6;
        NPC->tgt_x = NPC->x;
        NPC->bits |= npc_shootable;
        // Fallthrough
    case bigBlast + 1:
        if (++NPC->act_wait / 2 & 1)
            NPC->x = NPC->tgt_x;
        else
            NPC->x = NPC->tgt_x + pixelsToUnits(1);

        if (NPC->act_wait > 50)
        {
            NPC->act_no = afterBlast;
            NPC->act_wait = 0;
            NPC->ani_no = 7;
            playSound(SFX_Lightning);

            for (size_t i = 8; i < 0x100; i += 0x10)
                createNpc(NPC_ProjectileDoctorRedBallSlowVanish, NPC->x, NPC->y, 2 * getCos(i), 2 * getSin(i));
        }
        break;

    case afterBlast:
        if (++NPC->act_wait > 50)
            NPC->act_no = teleportAway;
        break;

    case teleportAway:
        NPC->act_no = teleportAway + 1;
        NPC->bits &= ~npc_shootable;
        NPC->damage = 0;
        NPC->act_wait = 0;
        playSound(SFX_Teleport);
        break;

    case teleportAway + 1:
        NPC->act_wait += 2;
        if (NPC->act_wait > 16)
        {
            NPC->act_no = waitingToReappear;
            NPC->act_wait = 0;
            NPC->ani_no = 3;
            NPC->tgt_x = tilesToUnits(random(5, 35));
            NPC->tgt_y = tilesToUnits(random(5, 7));
        }
        break;

    case waitingToReappear:
        if (++NPC->act_wait > 40)
        {
            NPC->act_no = reappear;
            NPC->act_wait = 16;
            NPC->ani_no = 2;
            NPC->ym = 0;
            NPC->x = NPC->tgt_x;
            NPC->y = NPC->tgt_y;
            NPC->facePlayer();
        }
        break;

    case reappear:
        NPC->act_wait -= 2;
        if (NPC->act_wait <= 0)
        {
            NPC->bits |= npc_shootable;
            NPC->damage = 3;
            if (NPC->count1 >= 3)
            {
                NPC->count1 = 0;
                NPC->act_no = bigBlast;
            }
            else
            {
                ++NPC->count1;
                NPC->act_no = fighting;
            }
        }
        break;

    case defeated:
        NPC->bits &= ~npc_shootable;
        NPC->ani_no = 6;
        NPC->ym += 0x10;

        if (NPC->flag & ground)
        {
            NPC->act_no = teleportAway + 1;
            NPC->act_wait = 0;
            NPC->tgt_x = NPC->x;
            NPC->facePlayer();
        }
        break;

    case defeated + 1:
        NPC->facePlayer();
        NPC->ani_no = 8;
        if (++NPC->act_wait / 2 & 1)
            NPC->x = NPC->tgt_x;
        else
            NPC->x = NPC->tgt_x + pixelsToUnits(1);
    }

    if (NPC->act_no >= fighting)
    {
        if (NPC->act_no == waitingToReappear)
        {
            superXPos = NPC->tgt_x;
            superYPos = NPC->tgt_y;
        }
        else
        {
            superXPos = NPC->x;
            superYPos = NPC->y;
        }
    }

    NPC->doGravity(0, 0x5FF);
    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    NPC->doRects(rcLeft, rcRight);

    if (NPC->act_no != teleportAway + 1 && NPC->act_no != reappear)
        NPC->view.top = tilesToUnits(1);
    else
    {
        NPC->rect.top += NPC->act_wait;
        NPC->rect.bottom -= NPC->act_wait;
        NPC->view.top = pixelsToUnits(16 - NPC->act_wait);
    }
}

void npcAct267(npc *NPC) // Muscle Doctor (boss)
{
    array<RECT, 10> rcLeft, rcRight;

    rcLeft[0] = {0, 0, 0, 0};
    rcLeft[1] = {0, 64, 40, 112};
    rcLeft[2] = {40, 64, 80, 112};
    rcLeft[3] = {80, 64, 120, 112};
    rcLeft[4] = {120, 64, 160, 112};
    rcLeft[5] = {160, 64, 200, 112};
    rcLeft[6] = {200, 64, 240, 112};
    rcLeft[7] = {240, 64, 280, 112};
    rcLeft[8] = {280, 64, 320, 112};
    rcLeft[9] = {0, 160, 40, 208};

    rcRight[0] = rcLeft[0];
    rcRight[1] = {0, 112, 40, 160};
    rcRight[2] = {40, 112, 80, 160};
    rcRight[3] = {80, 112, 120, 160};
    rcRight[4] = {120, 112, 160, 160};
    rcRight[5] = {160, 112, 200, 160};
    rcRight[6] = {200, 112, 240, 160};
    rcRight[7] = {240, 112, 280, 160};
    rcRight[8] = {280, 112, 320, 160};
    rcRight[9] = {40, 160, 80, 208};

    enum
    {
        init = 0,
        stand = 5,
        defeated = 500,
        dissolve = 510,
    };

    switch (NPC->act_no)
    {
    case init:
        if (superXPos <= currentPlayer.x)
            NPC->direct = dirRight;
        else
            NPC->direct = dirLeft;

        if (NPC->direct != dirLeft)
            NPC->x = superXPos + pixelsToUnits(6);
        else
            NPC->x = superXPos - pixelsToUnits(6);
        NPC->y = superYPos;
        // Fallthrough
    case 1:
    case 2:
        NPC->ym += 0x80;
        if (++NPC->act_wait / 2 & 1)
            NPC->ani_no = 0;
        else
            NPC->ani_no = 3;
        break;

    case stand: // Unused afaik
        NPC->act_no = stand + 1;
        NPC->ani_no = 1;
        NPC->ani_wait = 0;
        // Fallthrough
    case stand + 1:
        NPC->ym += 0x80;
        NPC->animate(40, 1, 2);
        break;

    case 7:
        NPC->act_no = 8;
        NPC->act_wait = 0;
        NPC->ani_no = 3;
        // Fallthrough
    case 8:
        NPC->ym += 0x40;
        if (++NPC->act_wait > 40)
            NPC->act_no = 10;
        break;

    case 10:
        NPC->bits |= npc_invulnerable;
        NPC->xm = 0;
        NPC->act_no = 11;
        NPC->act_wait = 0;
        NPC->ani_no = 1;
        NPC->ani_wait = 0;
        NPC->count2 = NPC->life;
        // Fallthrough
    case 11:
        NPC->ym += 0x80;
        NPC->facePlayer();

        if (NPC->flag & ground)
        {
            if (NPC->life >= NPC->count2 - 20)
                NPC->animate(10, 1, 2);
            else if (currentPlayer.flag & ground
                     && currentPlayer.x > NPC->x - tilesToUnits(3) && currentPlayer.x < NPC->x + tilesToUnits(3)
                     && NPC->ani_no != 6)
            {
                // WHAT THE FUCK ARE YOU SERIOUS ???
                NPC->ani_no = 6;
                currentPlayer.damage(5);
                viewport.quake = 10;
                playSound(SFX_LargeObjectHitGround);
                currentPlayer.ym = pixelsToUnits(-2);
                if (NPC->x <= currentPlayer.x)
                    currentPlayer.xm = 0x5FF;
                else
                    currentPlayer.xm = -0x5FF;

                for (size_t i = 0; i < 100; ++i)
                    createNpc(NPC_RedEnergy,
                              NPC->x + pixelsToUnits(random(-0x10, 0x10)), NPC->y + pixelsToUnits(random(-0x10, 0x10)),
                              3 * random(-0x200, 0x200), 3 * random(-0x200, 0x200), dirDown);
            }
        }
        else
            NPC->ani_no = 4;

        if (++NPC->act_wait > 30 || NPC->life < NPC->count2 - 20)
        {
            if (++NPC->count1 > 10)
                NPC->count1 = 0;

            switch (NPC->count1)
            {
            case 8:
                NPC->act_no = 20;
                break;

            case 3:
            case 6:
                NPC->act_no = 30;
                break;

            case 1:
            case 9:
                NPC->act_no = 40;
                break;

            case 2:
            case 7:
                NPC->act_no = 100;
                break;

            default:
                NPC->act_no = 15;
                NPC->act_wait = 0;
                break;
            }
        }
        break;

    case 15:
        NPC->ani_no = 3;
        ++NPC->act_wait;
        NPC->facePlayer();

        if (NPC->act_wait > 20)
        {
            NPC->act_no = 16;
            NPC->ani_no = 4;
            NPC->ani_wait = 0;
            NPC->ym = pixelsToUnits(-3);
            NPC->moveInDir(pixelsToUnits(2));
        }
        break;

    case 16:
        NPC->ym += 0x40;
        NPC->animate(1, 4, 5);
        NPC->facePlayer();

        if (NPC->ym > 0 && NPC->flag & ground)
            NPC->act_no = 17;
        break;

    case 17:
        NPC->act_no = 18;
        NPC->act_wait = 0;
        viewport.quake = 10;
        playSound(SFX_LargeObjectHitGround);
        // Fallthrough
    case 18:
        NPC->ani_no = 3;
        ++NPC->act_wait;
        NPC->xm = 7 * NPC->xm / 8;
        NPC->ym += 0x80;
        if (NPC->act_wait > 10)
            NPC->act_no = 10;
        break;

    case 20:
        NPC->act_no = 21;
        NPC->act_wait = 0;
        // Fallthrough
    case 21:
        NPC->ani_no = 6;
        if (++NPC->act_wait > 20 && NPC->act_wait % 3 == 1)
        {
            auto xVel = 4 * random(pixelsToUnits(0.5), pixelsToUnits(1));
            auto yVel = random(pixelsToUnits(-1), pixelsToUnits(1));
            if (NPC->direct != dirLeft)
                createNpc(NPC_EnemyBatRedEnergy, NPC->x + tilesToUnits(0.5), NPC->y - tilesToUnits(0.25), xVel, yVel, dirRight);
            else
                createNpc(NPC_EnemyBatRedEnergy, NPC->x - tilesToUnits(0.5), NPC->y - tilesToUnits(0.25), -xVel, yVel, dirLeft);
            playSound(SFX_EnemyShootProjectile);
        }
        if (NPC->act_wait > 90)
            NPC->act_no = 10;
        break;

    case 30:
        NPC->act_no = 31;
        NPC->act_wait = 0;
        NPC->bits |= npc_solidSoft;
        NPC->bits &= ~npc_shootable;
        // Fallthrough
    case 31:
        NPC->ani_no = 3;
        if (++NPC->act_wait > 20)
        {
            NPC->act_no = 32;
            NPC->act_wait = 0;
            NPC->ani_no = 7;
            NPC->bits |= npc_rearTop;
            NPC->damage = 0;
            playSound(SFX_SillyExplosion);
            NPC->moveInDir(0x5FF);
        }
        break;

    case 32:
        NPC->ym = 0;
        if (++NPC->act_wait / 2 & 1)
            NPC->ani_no = 7;
        else
            NPC->ani_no = 8;

        if (NPC->act_wait > 30)
        {
            NPC->act_wait = 0;
            NPC->act_no = 18;
            NPC->damage = 5;
            NPC->bits &= ~(npc_rearTop | npc_solidSoft);
            NPC->bits |= npc_shootable;
        }

        if (NPC->flag & leftWall || NPC->flag & rightWall)
        {
            NPC->act_no = 15;
            NPC->act_wait = 0;
            NPC->damage = 5;
            NPC->bits &= ~(npc_rearTop | npc_solidSoft);
            NPC->bits |= npc_shootable;
        }
        break;

    case 40:
        NPC->ani_no = 3;
        NPC->facePlayer();
        if (++NPC->act_wait > 20)
        {
            NPC->act_no = 41;
            NPC->ani_no = 4;
            NPC->ani_wait = 0;
            NPC->ym = pixelsToUnits(-4);
            NPC->moveInDir(pixelsToUnits(2));
        }
        break;

    case 41:
        NPC->ym += 0x40;
        NPC->animate(1, 4, 5);

        if (currentPlayer.y > NPC->y && currentPlayer.x > NPC->x - 0x1000 && currentPlayer.x < NPC->x + 0x1000)
        {
            NPC->act_no = 16;
            NPC->ym = 0x5FF;
            NPC->xm = 0;
        }
        if (NPC->ym > 0 && NPC->flag & ground)
            NPC->act_no = 17;

        break;

    case 100:
        NPC->act_no = 101;
        NPC->act_wait = 0;
        NPC->bits &= ~npc_shootable;
        NPC->damage = 0;
        playSound(SFX_Teleport);
        // Fallthrough

    case 101:
        NPC->act_wait += 2;
        if (NPC->act_wait > 28)
        {
            NPC->act_no = 102;
            NPC->act_wait = 0;
            NPC->ani_no = 0;
            NPC->tgt_x = currentPlayer.x;
            NPC->tgt_y = currentPlayer.y - tilesToUnits(2);
            if (NPC->tgt_y < tilesToUnits(4))
                NPC->tgt_y = tilesToUnits(4);
            if (NPC->tgt_x < tilesToUnits(4))
                NPC->tgt_x = tilesToUnits(4);
            if (NPC->tgt_x > tilesToUnits(36))
                NPC->tgt_x = tilesToUnits(36);
        }
        break;

    case 102:
        if (++NPC->act_wait > 40)
        {
            NPC->act_no = 103;
            NPC->act_wait = 28;
            NPC->ani_no = 4;
            NPC->ym = 0;
            NPC->x = NPC->tgt_x;
            NPC->y = NPC->tgt_y;
            NPC->facePlayer();
        }
        break;

    case 103:
        NPC->act_wait -= 2;
        if (NPC->act_wait <= 0)
        {
            NPC->bits |= (npc_shootable | npc_invulnerable);
            NPC->damage = 5;
            NPC->act_no = 16;
            NPC->ym = pixelsToUnits(-1);
            NPC->xm = 0;
        }
        break;

    case defeated:
        killNpcsByType(NPC_EnemyBatRedEnergy);
        NPC->bits &= ~npc_shootable;
        NPC->ani_no = 4;
        NPC->ym += 0x20;
        NPC->xm = 0;

        if (NPC->flag & ground)
        {
            NPC->act_no = defeated + 1;
            NPC->act_wait = 0;
            NPC->tgt_x = NPC->x;
            NPC->facePlayer();
        }
        break;

    case defeated + 1:
        NPC->ani_no = 9;
        if (++NPC->act_wait / 2 & 1)
            NPC->x = NPC->tgt_x;
        else
            NPC->x = NPC->tgt_x + pixelsToUnits(1);
        break;

    case dissolve:
        NPC->act_no = dissolve + 1;
        NPC->act_wait = 0;
        NPC->ani_no = 9;
        NPC->tgt_x = NPC->x;
        NPC->y += tilesToUnits(1);
        NPC->bits |= npc_ignoreSolid;
        // Fallthrough
    case dissolve + 1:
        viewport.quake = 2;
        if (++NPC->act_wait % 6 == 3)
            playSound(SFX_SillyExplosion);

        if (NPC->act_wait / 2 & 1)
            NPC->x = NPC->tgt_x;
        else
            NPC->x = NPC->tgt_x + pixelsToUnits(1);

        if (NPC->act_wait > 0x160)
        {
            NPC->ani_no = 0;
            NPC->act_no = 512;
        }
        break;

    case 520:
        NPC->damage = 0;
        superYPos = tilesToUnits(-2);
        break;

    default:
        break;
    }

    if (NPC->act_no >= 11 && NPC->act_no < 501)
    {
        if (NPC->act_no == 102)
        {
            superXPos = NPC->tgt_x;
            superYPos = NPC->tgt_y;
        }
        else
        {
            superXPos = NPC->x;
            superYPos = NPC->y;
        }
    }

    NPC->doGravity(0, 0x5FF);
    NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->act_no < 512)
	{
		if (NPC->act_no >= 510)
		{
		    createNpc(NPC_RedEnergy,
                NPC->x + pixelsToUnits(random(-0x10, 0x10)), NPC->y - pixelsToUnits((0x150 - NPC->act_wait) / 8),
                random(pixelsToUnits(-1), pixelsToUnits(1)), 2 * random(pixelsToUnits(-1), 0), dirDown);
		    createNpc(NPC_RedEnergy,
                NPC->x + pixelsToUnits(random(-0x10, 0x10)), NPC->y - pixelsToUnits((0x150 - NPC->act_wait) / 8),
                random(pixelsToUnits(-1), pixelsToUnits(1)), 2 * random(pixelsToUnits(-1), 0), dirDown);
		    createNpc(NPC_RedEnergy,
                NPC->x + pixelsToUnits(random(-0x10, 0x10)), NPC->y - pixelsToUnits((0x150 - NPC->act_wait) / 8),
                0, 2 * random(pixelsToUnits(-1), 0), dirDown);
		    createNpc(NPC_RedEnergy,
                NPC->x + pixelsToUnits(random(-0x10, 0x10)), NPC->y - pixelsToUnits((0x150 - NPC->act_wait) / 8),
                0, 2 * random(pixelsToUnits(-1), 0), dirDown);
		}
		else if (NPC->act_no != 102 && NPC->act_no != 103 && random(0, 3) == 2)
            createNpc(NPC_RedEnergy, NPC->x + pixelsToUnits(random(-0x10, 0x10)), NPC->y + pixelsToUnits(random(-8, 4)),
                      NPC->xm, 0, dirDown);
	}

    NPC->doRects(rcLeft, rcRight);
}

void npcAct268(npc *NPC)
{
	array<RECT, 10> rcLeft;
	array<RECT, 10> rcRight;

	rcLeft[0] = { 0, 0, 40, 40 };
	rcLeft[1] = { 40, 0, 80, 40 };
	rcLeft[2] = { 80, 0, 120, 40 };
	rcLeft[3] = rcLeft[0];
	rcLeft[4] = { 120, 0, 160, 40 };
	rcLeft[5] = rcLeft[0];
	rcLeft[6] = { 40, 80, 80, 120 };
	rcLeft[7] = { 0, 80, 40, 120 };
	rcLeft[8] = { 240, 0, 280, 40 };
	rcLeft[9] = { 280, 0, 320, 40 };

	rcRight[0] = { 0, 40, 40, 80 };
	rcRight[1] = { 40, 40, 80, 80 };
	rcRight[2] = { 80, 40, 120, 80 };
	rcRight[3] = rcRight[0];
	rcRight[4] = { 120, 40, 160, 80 };
	rcRight[5] = rcRight[0];
	rcRight[6] = { 160, 80, 200, 120 };
	rcRight[7] = { 120, 80, 160, 120 };
	rcRight[8] = { 240, 40, 280, 80 };
	rcRight[9] = { 280, 40, 320, 80 };

	enum
	{
		init = 0,
		stand = 1,
		startAttack = 10,
		walking = 11,
        startJump = 20,
        jumping = 30,
        waitShoot = 40,
        startShoot = 50,
        shooting = 51,
	};

    if (!NPC->isPlayerWithinDistance(tilesToUnits(20), tilesToUnits(15)))
		NPC->act_no = stand;

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = 1;
		NPC->y += tilesToUnits(0.5);
		// Fallthrough
	case stand:
		NPC->animate(20, 0, 1);

        if (NPC->shock || NPC->isPlayerWithinDistance(tilesToUnits(7), tilesToUnits(3), tilesToUnits(7)))
            NPC->act_no = startAttack;
		break;

	case startAttack:
        NPC->act_no = walking;
        NPC->act_wait = 0;
        NPC->ani_no = 0;
        NPC->ani_wait = 0;
        NPC->facePlayer();
        // Fallthrough
	case walking:
		NPC->moveInDir(pixelsToUnits(1));

		if (NPC->x < currentPlayer.x + tilesToUnits(4) && NPC->x > currentPlayer.x - tilesToUnits(4))
		{
			NPC->act_no = 20;
			NPC->act_wait = 0;
		}

		if ((NPC->xm < 0 && NPC->flag & leftWall) || (NPC->xm > 0 && NPC->flag & rightWall))
		{
			NPC->act_no = startJump;
            NPC->act_wait = 9;
		}

		NPC->animate(4, 2, 5);
		break;

	case startJump:
		NPC->xm = 0;
        NPC->ani_no = 6;
        if (++NPC->act_wait > 10)
		{
			NPC->act_no = 30;
			NPC->ym = -0x5FF;
			NPC->moveInDir(pixelsToUnits(1));
			playSound(SFX_PowerCritterLargeEnemyJump);
		}
		break;

	case jumping:
        NPC->ani_no = 7;
        if (NPC->flag & ground)
		{
			NPC->act_no = 40;
			NPC->act_wait = 0;
            viewport.quake = 20;
            playSound(SFX_LargeObjectHitGround);
		}
		break;

	case waitShoot:
        NPC->xm = 0;
        NPC->ani_no = 6;
        if (++NPC->act_wait > 30)
			NPC->act_no = startShoot;
		break;

	case startShoot:
        NPC->act_no = shooting;
        NPC->act_wait = 0;
        NPC->facePlayer();
        // Fallthrough

	case shooting:
		if (++NPC->act_wait > 30 && NPC->act_wait % 4 == 1)
		{
			auto angle = (NPC->direct != dirLeft) ? -8 : -120;
			angle += random(-0x10, 0x10);

			auto xVel = 5 * getCos(angle);
			auto yVel = 5 * getSin(angle);
			createNpc(NPC_ProjectileBalrogEnergyBallInvincible, NPC->x, NPC->y + pixelsToUnits(4), xVel, yVel);
			playSound(SFX_DestroyBreakableBlock);
		}

		if (NPC->act_wait < secondsToFrames(1) && NPC->act_wait / 2 & 1)
			NPC->ani_no = 9;
		else
			NPC->ani_no = 8;

		if (NPC->act_wait > 82)
		{
			NPC->act_no = startAttack;
			NPC->facePlayer();
		}
		break;

	default:
		break;
	}

	NPC->doGravity(0x33, 0x5FF);
	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->doRects(rcLeft, rcRight);
}

void npcAct272(npc *NPC) // Generator - Underwater Block
{
    if (!NPC->act_no)
    {
        NPC->act_no = 1;
        NPC->act_wait = random(0, 200);
    }
    else if (NPC->act_no != 1)
        return;

    if (NPC->act_wait)
        --NPC->act_wait;
    else
    {
        NPC->act_no = 0;
        createNpc(NPC_UnderwaterBlock, NPC->x, NPC->y + (unitsToPixels(random(-0x20, 0x20))), 0, 0, NPC->direct);
    }
}

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
