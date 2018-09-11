#include "npc240.h"

#include <vector>
#include "render.h"
#include "player.h"
#include "level.h"
#include "caret.h"
#include "sound.h"
#include "mathUtils.h"
#include "render.h"
#include "sound.h"
#include "game.h"

using std::vector;

void npcAct241(npc *NPC) // Critter, Hopping Red (enemy)
{
    vector<RECT> rcLeft = {{0, 0, 16, 16}, {16, 0, 32, 16}, {32, 0, 48, 16}};
    vector<RECT> rcRight = {{0, 16, 16, 32}, {16, 16, 32, 32}, {32, 16, 48, 32}};

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

        if (NPC->shock || (NPC->act_wait >= 8 && NPC->isPlayerWithinDistance(0xC000, 0xA000, 0xC000)))
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
            NPC->moveInDir(0x200);
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
    if (NPC->x < 0 || NPC->x > tilesToUnits(levelWidth))
    {
        killNpc(NPC, true);
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

    vector<RECT> rcLeft = {{32, 32, 48, 48}, {48, 32, 64, 48}, {64, 32, 80, 48}, {80, 32, 96, 48}};
    vector<RECT> rcRight = {{32, 48, 48, 64}, {48, 48, 64, 64}, {64, 48, 80, 64}, {80, 48, 96, 64}};

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

        NPC->rect = {96, 0, 104, 16};
    }
}

void npcAct245(npc *NPC) // Generator - Lava Drop
{
	vector<RECT> rcNPC = {{0, 0, 0, 0}, {104, 0, 112, 16}, {112, 0, 120, 16}, {120, 0, 128, 16}};

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
    vector<RECT> rcNPC = {{144, 112, 160, 136}, {160, 112, 176, 136}, {176, 112, 196, 136}};

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
                    auto yVel = random(-0x600, 0);
                    auto xVel = random(-0x155, 0x155);
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

	NPC->doRects({ {0, 64, 16, 80}, {16, 64, 32, 80} });
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

    vector<RECT> rcLeft = {{0, 0, 128, 64}};
    vector<RECT> rcRight = {{0, 64, 128, 128}};
    NPC->doRects(rcLeft, rcRight);
}

void npcAct255(npc *NPC) // Helicopter Blades
{
    vector<RECT> rcLeft = {{128, 0, 240, 16}, {128, 16, 240, 32}, {128, 32, 240, 48}, {128, 16, 240, 32}};
    vector<RECT> rcRight = {{240, 0, 320, 16}, {240, 16, 320, 32}, {240, 32, 320, 48}, {240, 16, 320, 32}};

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
    NPC->doRects({{48, 32, 64, 48}});
}

void npcAct259(npc *NPC) // Sleeping mimiga
{
	NPC->rect = { 48, 32, 64, 48 };
}
