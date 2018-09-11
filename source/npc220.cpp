#include "npc220.h"

#include <vector>
#include "player.h"
#include "sound.h"
#include "mathUtils.h"
#include "caret.h"
#include "render.h"

using std::vector;

void npcAct220(npc *NPC) // Shovel Brigade, standing
{
	vector<RECT> rcLeft = { {0, 64, 16, 80}, {16, 64, 32, 80} };
	vector<RECT> rcRight = { {0, 80, 16, 96}, {16, 80, 32, 96} };

	enum
	{
		aniStanding = 0,
		aniBlinking = 1,
	};

	enum
	{
		init = 0,
		standing = 1,
		blinking = 2,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = standing;
		NPC->ani_no = aniStanding;
		NPC->ani_wait = 0;
		// Fallthrough
	case standing:
		if (!random(0, 120))
		{
			NPC->act_no = blinking;
			NPC->act_wait = 0;
			NPC->ani_no = aniBlinking;
		}
		break;

	case blinking:
		if (++NPC->act_wait > 8)
		{
			NPC->act_no = standing;
			NPC->ani_no = aniStanding;
		}
	}

	NPC->doRects(rcLeft, rcRight);
}

void npcAct222(npc *NPC) // Prison bars
{
	if (!NPC->act_no)
	{
		++NPC->act_no;
		NPC->y -= 0x1000;
	}

	NPC->rect = { 96, 168, 112, 200 };
}

void npcAct223(npc *NPC) // Momorin
{
    vector<RECT> rcLeft = {{80, 192, 96, 216}, {96, 192, 112, 216}, {112, 192, 128, 216}};
    vector<RECT> rcRight = {{80, 216, 96, 240}, {96, 216, 112, 240}, {112, 216, 128, 240}};

    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->ani_no = 0;
        NPC->ani_wait = 0;
        // Fallthrough
    case 1:
        if (!random(0, 160))
        {
            NPC->act_no = 2;
            NPC->act_wait = 0;
            NPC->ani_no = 1;
        }
        break;

    case 2:
        if (++NPC->act_wait > 12)
        {
            NPC->act_no = 1;
            NPC->ani_no = 0;
        }
        break;

    case 3:
        NPC->ani_no = 2;
        break;

    default:
        break;
    }

    if (NPC->act_no < 2 && currentPlayer.y < NPC->y + 0x2000 && currentPlayer.y > NPC->y - 0x2000)
        NPC->facePlayer();

    NPC->doRects(rcLeft, rcRight);
}

void npcAct224(npc *NPC) // Chie
{
	vector<RECT> rcLeft = {{112, 32, 128, 48}, {128, 32, 144, 48}};
	vector<RECT> rcRight = {{112, 48, 128, 64}, {128, 48, 144, 64}};

	enum
	{
		aniNormal = 0,
		aniBlink = 1,
	};

	enum
	{
		init = 0,
		normal = 1,
		blinking = 2,
	};

	switch (NPC->act_no)
	{
    case init:
        NPC->act_no = normal;
        NPC->ani_no = aniNormal;
        // Fallthrough
    case normal:
        if (!random(0, 160))
        {
            NPC->act_no = blinking;
            NPC->act_wait = 0;
            NPC->ani_no = aniBlink;
        }
        break;

    case blinking:
        if (++NPC->act_wait > 12)
        {
            NPC->act_no = normal;
            NPC->ani_no = aniNormal;
        }
        break;

    default:
        break;
	}

	if (NPC->act_no < blinking && currentPlayer.y < NPC->y + 0x2000 && currentPlayer.y > NPC->y - 0x2000)
        NPC->facePlayer();

    NPC->doRects(rcLeft, rcRight);
}

void npcAct225(npc *NPC) // Megane
{
	vector<RECT> rcLeft = {{64, 64, 80, 80}, {80, 64, 96, 80}};
	vector<RECT> rcRight = {{64, 80, 80, 96}, {80, 80, 96, 96}};

	enum
	{
		aniNormal = 0,
		aniBlink = 1,
	};

	enum
	{
		init = 0,
		normal = 1,
		blinking = 2,
	};

	switch (NPC->act_no)
	{
	case init:
		NPC->act_no = 1;
		NPC->ani_no = aniNormal;
		NPC->ani_wait = 0;
	// Fallthrough
	case normal:
		if (!random(0, 160))
		{
			NPC->act_no = blinking;
			NPC->act_wait = 0;
			NPC->ani_no = aniBlink;
		}
		break;

	case blinking:
		if (++NPC->act_wait > 12)
		{
			NPC->act_no = normal;
			NPC->ani_no = aniNormal;
		}
		break;

	default:
		break;
	}

	NPC->doRects(rcLeft, rcRight);
}

void npcAct226(npc *NPC) // Kanpachi, standing
{
    vector<RECT> rcNPC(7);

    rcNPC[0] = {256, 56, 272, 80};
    rcNPC[1] = {272, 56, 288, 80};
    rcNPC[2] = {288, 56, 304, 80};
    rcNPC[3] = rcNPC[0];
    rcNPC[4] = {304, 56, 320, 80};
    rcNPC[5] = rcNPC[0];
    rcNPC[6] = {240, 56, 256, 80};

    enum
    {
        init = 0,
        stand = 1,
        blinking = 2,
        startWalk = 10,
        walking = 11,
        enterDoor = 20,
    };

    switch (NPC->act_no)
    {
    case init:
        NPC->act_no = stand;
        NPC->ani_no = 0;
        NPC->ani_wait = 0;
        NPC->xm = 0;
        // Fallthrough
    case stand:
        if (!random(0, 60))
        {
            NPC->act_no = blinking;
            NPC->act_wait = 0;
            NPC->ani_no = 1;
        }
        break;

    case blinking:
        if (++NPC->act_wait > 8)
        {
            NPC->act_no = 1;
            NPC->ani_no = 0;
        }
        break;

    case startWalk:
        NPC->act_no = walking;
        NPC->ani_no = 2;
        NPC->ani_wait = 0;
        // Fallthrough
    case walking:
        NPC->xm = 0x200;
        NPC->animate(4, 2, 5);
        ++NPC->act_wait;
        break;

    case enterDoor:
        NPC->xm = 0;
        NPC->ani_no = 6;
        break;

    default:
        break;
    }
    NPC->doGravity(0x20, 0x5FF);
    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    NPC->doRects(rcNPC);
}

void npcAct227(npc *NPC) // Bucket
{
	NPC->rect = { 208, 32, 224, 48 };
}

void npcAct228(npc *NPC) // Droll, guarding
{
    vector<RECT> rcLeft = {{0, 0, 32, 40}, {32, 0, 64, 40}, {64, 0, 96, 40}, {96, 0, 128, 40}};
    vector<RECT> rcRight = {{0, 40, 32, 80}, {32, 40, 64, 80}, {64, 40, 96, 80}, {96, 40, 128, 80}};

    enum
    {
        init = 0,
        startCrouch = 10,
        crouching = 11,
        jumping = 12,
        landed = 13,
    };

    switch (NPC->act_no)
    {
    case init:
        NPC->act_no = 1;
        NPC->y -= pixelsToUnits(8);
        // Fallthrough
    case 1:
        NPC->xm = 0;
        NPC->act_no = 2;
        NPC->ani_no = 0;
        // Fallthrough
    case 2:
        NPC->facePlayer();
        NPC->animate(50, 0, 1);
        break;

    case startCrouch:
        NPC->act_no = crouching;
        NPC->ani_no = 2;
        NPC->act_wait = 0;
        // Fallthrough
    case crouching:
        if (++NPC->act_wait > 10)
        {
            NPC->act_no = jumping;
            NPC->ani_no = 3;
            NPC->ym = -0x600;
            NPC->moveInDir(0x200);
        }
        break;

    case jumping:
        if (NPC->flag & ground)
        {
            NPC->ani_no = 2;
            NPC->act_no = landed;
            NPC->act_wait = 0;
        }
        break;

    case landed:
        NPC->xm /= 2;
        if (++NPC->act_wait > 10)
            NPC->act_no = 1;
        break;

    default:
        break;
    }

    NPC->doGravity(0x40, 0x5FF);

    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    NPC->doRects(rcLeft, rcRight);
}

void npcAct229(npc *NPC) // Red Flowers, small
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->y -= 0x2000;
	}
	if (NPC->direct)
		NPC->rect = { 0, 112, 59, 129 };
	else
		NPC->rect = { 0, 96, 48, 112 };
}

void npcAct230(npc *NPC) // Red Flowers, large
{
    if (!NPC->act_no)
    {
        NPC->act_no = 1;
        NPC->x -= tilesToUnits(1);
        NPC->y -= tilesToUnits(1);
    }

    NPC->doRects({{48, 96, 96, 128}}, {{96, 96, 144, 128}});
}

void npcAct231(npc *NPC) //Momorin's rocket
{
	RECT rcNPC[2];

	rcNPC[0] = { 176, 32, 208, 48 };
	rcNPC[1] = { 176, 48, 208, 64 };

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		break;

	case 1:
		NPC->ani_no = 0;
		break;

	case 10:
		NPC->act_no = 11;
		NPC->act_wait = 0;
	// fallthrough
	case 11:
		++NPC->act_wait;
		NPC->ym += 8;
		if (NPC->flag & ground)
		{
			if (NPC->act_wait > 9)
				NPC->act_no = 1;
			else
				NPC->act_no = 12;
		}
		break;

	case 12:
		NPC->bits &= ~npc_interact;

		NPC->act_no = 13;
		NPC->act_wait = 0;
		NPC->ani_no = 1;

		for (int i = 0; i < 10; ++i)
		{
			createNpc(NPC_Smoke, NPC->x + (random(-16, 16) << 9), NPC->y + (random(-8, 8) << 9));
			playSound(SFX_DestroyBreakableBlock);
		}
	// fallthrough
	case 13:
		NPC->ym -= 8;

		if (!(++NPC->act_wait & 1))
			createCaret(NPC->x - 0x1400, NPC->y + 0x1000, effect_BoosterSmoke, dirDown);

		if (NPC->act_wait % 2 == 1)
			createCaret(NPC->x + 0x1400, NPC->y + 0x1000, effect_BoosterSmoke, dirDown);

		if (NPC->act_wait % 4 == 1)
			playSound(SFX_FireballBounce);

		if (NPC->flag & ceiling || currentPlayer.flag & ceiling || NPC->act_wait > 450)
		{
			if (NPC->flag & ceiling || currentPlayer.flag & ceiling)
				NPC->ym = 0;

			NPC->act_no = 15;

			for (int i = 0; i < 6; ++i)
			{
				createNpc(NPC_Smoke, NPC->x + (random(-16, 16) << 9), NPC->y + (random(-8, 8) << 9));
				playSound(SFX_DestroyBreakableBlock);
			}
		}
		break;

	case 15:
		NPC->ym += 8;
		++NPC->act_wait;

		if (NPC->ym < 0)
		{
			if (!(NPC->act_wait & 7))
				createCaret(NPC->x - 0x1400, NPC->y + 0x1000, effect_BoosterSmoke, dirDown);

			if (NPC->act_wait % 8 == 4)
				createCaret(NPC->x + 0x1400, NPC->y + 0x1000, effect_BoosterSmoke, dirDown);

			if (NPC->act_wait % 16 == 1)
				playSound(SFX_FireballBounce);
		}

		if (NPC->flag & ground)
		{
			NPC->bits |= npc_interact;

			NPC->act_no = 1;
			NPC->ani_no = 0;
		}
		break;
	default:
		break;
	}

	if (NPC->ym < -0x5FF)
		NPC->ym = -0x5FF;
	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->y += NPC->ym;

	NPC->rect = rcNPC[NPC->ani_no];
}

void npcAct232(npc *NPC) // Orangebell (enemy)
{
    switch (NPC->act_no)
    {
    case 0:
        NPC->act_no = 1;
        NPC->tgt_x = NPC->x;
        NPC->tgt_y = NPC->y;
        NPC->ym = 0x200;
        for (size_t i = 0; i < 8; i++)
            createNpc(NPC_EnemyOrangeBellBat, NPC->x, NPC->y, 0, 0, NPC->direct, NPC);
        // Fallthrough
    case 1:
        if (NPC->xm < 0 && NPC->flag & leftWall)
            NPC->direct = dirRight;
        else if (NPC->xm > 0 && NPC->flag & rightWall)
            NPC->direct = dirLeft;

		if (NPC->direct)
			NPC->xm = 0x100;
		else
			NPC->xm = 0x100;

        if (NPC->y >= NPC->tgt_y)
            NPC->ym -= 8;
        else
            NPC->ym += 8;

        if (NPC->ym > 0x200)
            NPC->ym = 0x200;
        else if (NPC->ym < -0x200)
            NPC->ym = -0x200;

        NPC->animate(5, 0, 2);
        break;

    default:
        break;
    }

    NPC->x += NPC->xm;
    NPC->y += NPC->ym;

    vector<RECT> rcLeft = {{128, 0, 160, 32}, {160, 0, 192, 32}, {192, 0, 224, 32}};
    vector<RECT> rcRight = {{128, 32, 160, 64}, {160, 32, 192, 64}, {192, 32, 224, 64}};
    NPC->doRects(rcLeft, rcRight);
}

void npcAct234(npc * NPC) // Red Flowers, picked
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->y += 0x2000;
	}

	if (NPC->direct != dirLeft)
		NPC->rect = { 144, 112, 192, 128 };
	else
		NPC->rect = { 144, 96, 192, 112 };
}

void npcAct238(npc *NPC) //Killer press
{
	RECT rcNPC[3];

	rcNPC[0] = { 184, 200, 208, 216 };
	rcNPC[1] = { 208, 200, 232, 216 };
	rcNPC[2] = { 232, 200, 256, 216 };

	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
		NPC->view.left = 0x2000;
		NPC->view.right = 0x1000;
	}

	switch (NPC->act_no)
	{
	case 1:
		if (NPC->direct == dirLeft && currentPlayer.x < NPC->x && currentPlayer.x > NPC->x - 0x18000 && currentPlayer.y > NPC->y - 0x800 && currentPlayer.y < NPC->y + 0x1000)
		{
			NPC->act_no = 10;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}

		if (NPC->direct == dirRight && currentPlayer.x > NPC->x && currentPlayer.x < NPC->x + 0x18000 && currentPlayer.y > NPC->y - 0x800 && currentPlayer.y < NPC->y + 0x1000)
		{
			NPC->act_no = 10;
			NPC->act_wait = 0;
			NPC->ani_no = 2;
		}

		break;

	case 10:
		NPC->damage = 127;

		if (NPC->direct != dirLeft)
			NPC->x += 0xC00;
		else
			NPC->x -= 0xC00;

		if (++NPC->act_wait == 8)
		{
			NPC->act_no = 20;
			NPC->act_wait = 0;

			for (int i = 0; i < 4; ++i)
			{
				createNpc(NPC_Smoke, NPC->x + (random(-16, 16) << 9), NPC->y + (random(-8, 8) << 9));
				playSound(SFX_DestroyBreakableBlock);
			}
		}
		break;

	case 20:
		NPC->damage = 0;

		if (++NPC->act_wait > 50)
		{
			NPC->act_wait = 0;
			NPC->act_no = 30;
		}

		break;

	case 30:
		NPC->damage = 0;
		NPC->ani_no = 1;

		if (++NPC->act_wait == 12)
		{
			NPC->act_no = 1;
			NPC->act_wait = 0;
			NPC->ani_no = 0;
		}

		if (NPC->direct != dirLeft)
			NPC->x -= 0x800;
		else
			NPC->x += 0x800;

		break;

	default:
		break;
	}

	if (NPC->direct != dirLeft || currentPlayer.x >= NPC->x)
	{
		if (NPC->direct != dirRight || currentPlayer.x <= NPC->x)
			NPC->hit.right = 0x1000;
		else
			NPC->hit.right = 0x2000;
	}
	else
		NPC->hit.right = 0x2000;

	NPC->rect = rcNPC[NPC->ani_no];
}

void npcAct239(npc *NPC) // Cage bars
{
    if (!NPC->act_no)
    {
        NPC->act_no = 1;
        if (NPC->direct != dirLeft)
        {
            NPC->view.left = 0x3000;
            NPC->view.right = 0x3000;
            NPC->view.top = 0x1000;
        }
        else
        {
            NPC->x += 0x1000;
            NPC->y += 0x2000;
        }
    }

    NPC->doRects({{192, 48, 256, 80}}, {{96, 112, 144, 144}});
}
