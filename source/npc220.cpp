#include "npc220.h"

#include <array>
#include "player.h"
#include "sound.h"
#include "mathUtils.h"
#include "caret.h"
#include "render.h"
#include "stage.h"

using std::array;

void npcAct220(npc *NPC) // Shovel Brigade, standing
{
	constexpr array<RECT, 2> rcLeft = { { {0, 64, 16, 80}, {16, 64, 32, 80} } };
	constexpr array<RECT, 2> rcRight = { { {0, 80, 16, 96}, {16, 80, 32, 96} } };

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

void npcAct221(npc *NPC) // Shovel Brigade (walking)
{
	RECT rcLeft[6];

	rcLeft[0].left = 0;
	rcLeft[0].top = 64;
	rcLeft[0].right = 16;
	rcLeft[0].bottom = 80;

	rcLeft[1].left = 16;
	rcLeft[1].top = 64;
	rcLeft[1].right = 32;
	rcLeft[1].bottom = 80;

	rcLeft[2].left = 32;
	rcLeft[2].top = 64;
	rcLeft[2].right = 48;
	rcLeft[2].bottom = 80;

	rcLeft[3].left = 0;
	rcLeft[3].top = 64;
	rcLeft[3].right = 16;
	rcLeft[3].bottom = 80;

	rcLeft[4].left = 48;
	rcLeft[4].top = 64;
	rcLeft[4].right = 64;
	rcLeft[4].bottom = 80;

	rcLeft[5].left = 0;
	rcLeft[5].top = 64;
	rcLeft[5].right = 16;
	rcLeft[5].bottom = 80;

	RECT rcRight[6];

	rcRight[0].left = 0;
	rcRight[0].top = 80;
	rcRight[0].right = 16;
	rcRight[0].bottom = 96;

	rcRight[1].left = 16;
	rcRight[1].top = 80;
	rcRight[1].right = 32;
	rcRight[1].bottom = 96;

	rcRight[2].left = 32;
	rcRight[2].top = 80;
	rcRight[2].right = 48;
	rcRight[2].bottom = 96;

	rcRight[3].left = 0;
	rcRight[3].top = 80;
	rcRight[3].right = 16;
	rcRight[3].bottom = 96;

	rcRight[4].left = 48;
	rcRight[4].top = 80;
	rcRight[4].right = 64;
	rcRight[4].bottom = 96;

	rcRight[5].left = 0;
	rcRight[5].top = 80;
	rcRight[5].right = 16;
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

void npcAct222(npc *NPC) // Prison bars
{
	if (!NPC->act_no)
	{
		++NPC->act_no;
		NPC->y -= 0x1000;
	}

	NPC->doRects({ 96, 168, 112, 200 });
}

void npcAct223(npc *NPC) // Momorin
{
	constexpr array<RECT, 3> rcLeft = { {{80, 192, 96, 216}, {96, 192, 112, 216}, {112, 192, 128, 216}} };
	constexpr array<RECT, 3> rcRight = { {{80, 216, 96, 240}, {96, 216, 112, 240}, {112, 216, 128, 240}} };

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
	constexpr array<RECT, 2> rcLeft = { {{112, 32, 128, 48}, {128, 32, 144, 48}} };
	constexpr array<RECT, 2> rcRight = { {{112, 48, 128, 64}, {128, 48, 144, 64}} };

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
	constexpr array<RECT, 2> rcLeft = { {{64, 64, 80, 80}, {80, 64, 96, 80}} };
	constexpr array<RECT, 2> rcRight = { {{64, 80, 80, 96}, {80, 80, 96, 96}} };

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
    array<RECT, 7> rcNPC;

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
	NPC->doRects({ 208, 32, 224, 48 });
}

void npcAct228(npc *NPC) // Droll, guarding
{
	constexpr array<RECT, 4> rcLeft = { {{0, 0, 32, 40}, {32, 0, 64, 40}, {64, 0, 96, 40}, {96, 0, 128, 40}} };
	constexpr array<RECT, 4> rcRight = { {{0, 40, 32, 80}, {32, 40, 64, 80}, {64, 40, 96, 80}, {96, 40, 128, 80}} };

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
            NPC->ym = pixelsToUnits(-3);
            NPC->moveInDir(pixelsToUnits(1));
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

	NPC->doRects({0, 96, 48, 112}, {0, 112, 59, 129});
}

void npcAct230(npc *NPC) // Red Flowers, large
{
    if (!NPC->act_no)
    {
        NPC->act_no = 1;
        NPC->x -= tilesToUnits(1);
        NPC->y -= tilesToUnits(1);
    }

    NPC->doRects({48, 96, 96, 128}, {96, 96, 144, 128});
}

void npcAct231(npc *NPC) //Momorin's rocket
{
	array<RECT, 2> rcNPC;

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

	NPC->doRects(rcNPC);
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
			NPC->xm = -0x100;

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

	constexpr array<RECT, 3> rcLeft = { {{128, 0, 160, 32}, {160, 0, 192, 32}, {192, 0, 224, 32}} };
	constexpr array<RECT, 3> rcRight = { {{128, 32, 160, 64}, {160, 32, 192, 64}, {192, 32, 224, 64}} };
    NPC->doRects(rcLeft, rcRight);
}

void npcAct233(npc *NPC) // Unknown
{
	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->xm = getCos(random(0, 255));
		NPC->ym = getSin(random(0, 255));
		NPC->count1 = 120;
		NPC->count2 = pixelsToUnits(random(-32, 32));
		// fallthrough
	case 1:
		if (NPC->pNpc->code_char == 232)
		{
			NPC->tgt_x = NPC->pNpc->x;
			NPC->tgt_y = NPC->pNpc->y;
			NPC->direct = NPC->pNpc->direct;
		}

		if (NPC->tgt_x < NPC->x)
			NPC->xm -= 8;
		if (NPC->tgt_x > NPC->x)
			NPC->xm += 8;

		if (NPC->count2 + NPC->tgt_y < NPC->y)
			NPC->ym -= 0x20;
		if (NPC->count2 + NPC->tgt_y > NPC->y)
			NPC->ym += 0x20;

		if (NPC->xm > 0x400)
			NPC->xm = 0x400;
		if (NPC->xm < -0x400)
			NPC->xm = -0x400;
		if (NPC->ym > 0x400)
			NPC->ym = 0x400;
		if (NPC->ym < -0x400)
			NPC->ym = -0x400;

		if (NPC->count1 >= 120)
		{
			if (NPC->x - 0x1000 < currentPlayer.x && NPC->x + 0x1000 > currentPlayer.x && NPC->y < currentPlayer.y && NPC->y + pixelsToUnits(176) > currentPlayer.y)
			{
				NPC->xm /= 4;
				NPC->ym = 0;
				NPC->act_no = 3;
				NPC->bits &= ~npc_ignoreSolid;
			}
		}
		else
		{
			++NPC->count1;
		}

		break;
	case 3:
		NPC->ym += 0x40;

		if (NPC->ym > 0x5FF)
			NPC->ym = 0x5FF;

		if (NPC->flag & ground)
		{
			NPC->ym = 0;
			NPC->xm *= 2;
			NPC->count1 = 0;
			NPC->act_no = 1;
			NPC->bits |= npc_ignoreSolid;
		}

		break;
	}

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	RECT rcLeft[4];

	rcLeft[0].left = 256;
	rcLeft[0].top = 0;
	rcLeft[0].right = 272;
	rcLeft[0].bottom = 16;

	rcLeft[1].left = 272;
	rcLeft[1].top = 0;
	rcLeft[1].right = 288;
	rcLeft[1].bottom = 16;

	rcLeft[2].left = 288;
	rcLeft[2].top = 0;
	rcLeft[2].right = 304;
	rcLeft[2].bottom = 16;

	rcLeft[3].left = 304;
	rcLeft[3].top = 0;
	rcLeft[3].right = 320;
	rcLeft[3].bottom = 16;

	RECT rcRight[4];

	rcRight[0].left = 256;
	rcRight[0].top = 16;
	rcRight[0].right = 272;
	rcRight[0].bottom = 32;

	rcRight[1].left = 272;
	rcRight[1].top = 16;
	rcRight[1].right = 288;
	rcRight[1].bottom = 32;

	rcRight[2].left = 288;
	rcRight[2].top = 16;
	rcRight[2].right = 304;
	rcRight[2].bottom = 32;

	rcRight[3].left = 304;
	rcRight[3].top = 16;
	rcRight[3].right = 320;
	rcRight[3].bottom = 32;

	if (NPC->act_no == 3)
	{
		NPC->ani_no = 3;
	}
	else
	{
		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 2)
			NPC->ani_no = 0;
	}

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct234(npc * NPC) // Red Flowers, picked
{
	if (!NPC->act_no)
	{
		NPC->act_no = 1;
		NPC->y += 0x2000;
	}

	NPC->doRects({144, 96, 192, 112}, {144, 112, 192, 128});
}

void npcAct235(npc *NPC) //Midorin
{
	RECT rcLeft[4];

	rcLeft[0].left = 192;
	rcLeft[0].top = 96;
	rcLeft[0].right = 208;
	rcLeft[0].bottom = 112;

	rcLeft[1].left = 208;
	rcLeft[1].top = 96;
	rcLeft[1].right = 224;
	rcLeft[1].bottom = 112;

	rcLeft[2].left = 224;
	rcLeft[2].top = 96;
	rcLeft[2].right = 240;
	rcLeft[2].bottom = 112;

	rcLeft[3].left = 192;
	rcLeft[3].top = 96;
	rcLeft[3].right = 208;
	rcLeft[3].bottom = 112;

	RECT rcRight[4];

	rcRight[0].left = 192;
	rcRight[0].top = 112;
	rcRight[0].right = 208;
	rcRight[0].bottom = 128;

	rcRight[1].left = 208;
	rcRight[1].top = 112;
	rcRight[1].right = 224;
	rcRight[1].bottom = 128;

	rcRight[2].left = 224;
	rcRight[2].top = 112;
	rcRight[2].right = 240;
	rcRight[2].bottom = 128;

	rcRight[3].left = 192;
	rcRight[3].top = 112;
	rcRight[3].right = 208;
	rcRight[3].bottom = 128;

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->ani_no = 0;
		NPC->ani_wait = 0;
		NPC->xm = 0;
		// fallthrough
	case 1:
		if (random(0, 30) == 1)
		{
			NPC->act_no = 2;
			NPC->act_wait = 0;
			NPC->ani_no = 1;
		}

		if (random(0, 30) == 1)
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
			NPC->xm = 0x400;
		else
			NPC->xm = -0x400;

		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 3)
			NPC->ani_no = 2;

		if (++NPC->act_wait > 0x40)
			NPC->act_no = 0;

		break;
	}

	NPC->ym += 0x20;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	if (NPC->ani_no == 2)
		NPC->hit.top = 0xA00;
	else
		NPC->hit.top = 0x800;

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct236(npc *NPC) //Gunfish
{
	RECT rcLeft[6];

	rcLeft[0].left = 128;
	rcLeft[0].top = 64;
	rcLeft[0].right = 152;
	rcLeft[0].bottom = 88;

	rcLeft[1].left = 152;
	rcLeft[1].top = 64;
	rcLeft[1].right = 176;
	rcLeft[1].bottom = 88;

	rcLeft[2].left = 176;
	rcLeft[2].top = 64;
	rcLeft[2].right = 200;
	rcLeft[2].bottom = 88;

	rcLeft[3].left = 200;
	rcLeft[3].top = 64;
	rcLeft[3].right = 224;
	rcLeft[3].bottom = 88;

	rcLeft[4].left = 224;
	rcLeft[4].top = 64;
	rcLeft[4].right = 248;
	rcLeft[4].bottom = 88;

	rcLeft[5].left = 248;
	rcLeft[5].top = 64;
	rcLeft[5].right = 272;
	rcLeft[5].bottom = 88;

	RECT rcRight[6];

	rcRight[0].left = 128;
	rcRight[0].top = 88;
	rcRight[0].right = 152;
	rcRight[0].bottom = 112;

	rcRight[1].left = 152;
	rcRight[1].top = 88;
	rcRight[1].right = 176;
	rcRight[1].bottom = 112;

	rcRight[2].left = 176;
	rcRight[2].top = 88;
	rcRight[2].right = 200;
	rcRight[2].bottom = 112;

	rcRight[3].left = 200;
	rcRight[3].top = 88;
	rcRight[3].right = 224;
	rcRight[3].bottom = 112;

	rcRight[4].left = 224;
	rcRight[4].top = 88;
	rcRight[4].right = 248;
	rcRight[4].bottom = 112;

	rcRight[5].left = 248;
	rcRight[5].top = 88;
	rcRight[5].right = 272;
	rcRight[5].bottom = 112;

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		NPC->act_wait = random(0, 50);
		NPC->tgt_x = NPC->x;
		NPC->tgt_y = NPC->y;
		NPC->ym = 0;
		// fallthrough
	case 1:
		if (NPC->act_wait)
		{
			--NPC->act_wait;
		}
		else
		{
			NPC->ym = 0x200;
			NPC->act_no = 2;
		}

		break;
	case 2:
		if (NPC->x >= currentPlayer.x)
			NPC->direct = dirLeft;
		else
			NPC->direct = dirRight;

		if (currentPlayer.x < NPC->x + 0x10000 && currentPlayer.x > NPC->x - 0x10000 && currentPlayer.y < NPC->y + 0x4000 && currentPlayer.y > NPC->y - 0x14000)
			++NPC->act_wait;

		if (NPC->act_wait > 80)
		{
			NPC->act_no = 10;
			NPC->act_wait = 0;
		}

		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 1)
			NPC->ani_no = 0;

		break;
	case 10:
		if (++NPC->act_wait > 20)
		{
			NPC->act_wait = 0;
			NPC->act_no = 20;
		}

		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 3)
			NPC->ani_no = 2;

		break;
	case 20:
		if (++NPC->act_wait > 60)
		{
			NPC->act_wait = 0;
			NPC->act_no = 2;
		}

		if (NPC->act_wait % 10 == 3)
		{
			playSound(39, 1);

			if (NPC->direct != dirLeft)
				createNpc(237, NPC->x + 0x1000, NPC->y - 0x1000, 0x400, -0x400, 0, 0, false /*256*/);
			else
				createNpc(237, NPC->x - 0x1000, NPC->y - 0x1000, -0x400, -0x400, 0, 0, false /*256*/);
		}

		if (++NPC->ani_wait > 1)
		{
			NPC->ani_wait = 0;
			++NPC->ani_no;
		}

		if (NPC->ani_no > 5)
			NPC->ani_no = 4;

		break;
	}

	if (NPC->y >= NPC->tgt_y)
		NPC->ym -= 0x10;
	else
		NPC->ym += 0x10;

	if (NPC->ym > 0x100)
		NPC->ym = 0x100;
	if (NPC->ym < -0x100)
		NPC->ym = -0x100;

	NPC->y += NPC->ym;

	if (NPC->direct != dirLeft)
		NPC->rect = rcRight[NPC->ani_no];
	else
		NPC->rect = rcLeft[NPC->ani_no];
}

void npcAct237(npc *NPC) //Gunfish projectile
{
	bool bHit;

	switch (NPC->act_no)
	{
	case 0:
		NPC->act_no = 1;
		// fallthrough
	case 1:
		bHit = false;

		++NPC->act_wait;

		if (NPC->flag & solid)
			bHit = true;

		if (NPC->act_wait > 10 && (NPC->flag & water))
			bHit = true;

		if (bHit)
		{
			for (int i = 0; i < 5; ++i)
				createCaret(NPC->x, NPC->y, effect_fountainDisk, dirLeft);

			playSound(21, 1);
			NPC->cond = 0;
			return;
		}

		break;
	}

	NPC->ym += 0x20;

	if (NPC->ym > 0x5FF)
		NPC->ym = 0x5FF;

	NPC->x += NPC->xm;
	NPC->y += NPC->ym;

	NPC->rect.left = 312;
	NPC->rect.top = 32;
	NPC->rect.right = 320;
	NPC->rect.bottom = 40;
}

void npcAct238(npc *NPC) //Killer press
{
	array<RECT, 3> rcNPC;

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

	NPC->doRects(rcNPC);
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

    NPC->doRects({192, 48, 256, 80}, {96, 112, 144, 144});
}
