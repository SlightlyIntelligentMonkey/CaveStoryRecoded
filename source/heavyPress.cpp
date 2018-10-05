#include "boss.h"

#include <array>
#include "render.h"
#include "sound.h"
#include "mathUtils.h"
#include "level.h"

using std::array;

enum
{
    init = 0,
	smokingInPassageway = 20,
	fallingIntoPassageway = 30,
	beginFight = 100,
	defeated = 500,
};

void actBoss_HeavyPress(npc *boss)
{
	switch (boss->act_no)
	{
    case init:
        boss->act_no = 10;
        boss->cond = npccond_alive;
        boss->exp = 1;
        boss->direct = dirRight;
        boss->x = 0;
        boss->y = 0;
        boss->view = {pixelsToUnits(40), pixelsToUnits(60), pixelsToUnits(40), pixelsToUnits(60)};
        boss->hit_voice = SFX_HeavyPressLargeObjectHurt;
        boss->hit = {pixelsToUnits(49), pixelsToUnits(60), pixelsToUnits(40), pixelsToUnits(48)};
        boss->bits = (npc_showDamage | npc_eventDie | npc_solidHard | npc_ignoreSolid);
        boss->size = 3;
        boss->damage = 10;
        boss->code_event = 1000;
        boss->life = 700;
        break;

    case 5:
        boss->act_no = 6;
        boss->x = 0;
        boss->y = 0;
        bossObj[1].cond = 0;
        bossObj[2].cond = 0;
        break;

    case 10:
        boss->act_no = 11;
        boss->x = tilesToUnits(10);
        boss->y = pixelsToUnits(74);
        break;

    case smokingInPassageway:
        boss->damage = 0;
        boss->act_no = smokingInPassageway + 1;
        boss->x = tilesToUnits(10);
        boss->y = pixelsToUnits(413);
        boss->bits &= ~npc_solidHard;
        bossObj[1].cond = 0;
        bossObj[2].cond = 0;
        // Fallthrough
    case smokingInPassageway + 1:
        if (!(++boss->act_wait % 16))
            createSmokeLeft(boss->x + pixelsToUnits(random(-40, 40)), boss->y + pixelsToUnits(random(-60, 60)), 1, 1);
        break;

    case fallingIntoPassageway:
        boss->act_no = fallingIntoPassageway + 1;
        boss->ani_no = 2;
        boss->x = tilesToUnits(10);
        boss->y = tilesToUnits(4);
        // Fallthrough
	case fallingIntoPassageway + 1:
		boss->y += pixelsToUnits(4);
		if (boss->y >= pixelsToUnits(413))
		{
			boss->y = pixelsToUnits(413);
			boss->ani_no = 0;
			boss->act_no = smokingInPassageway;
            playSound(SFX_MissileImpact);
            for (size_t i = 0; i < 5; ++i)
                createNpc(NPC_Smoke, boss->x + pixelsToUnits(random(-40, 40)), boss->y + pixelsToUnits(60));
		}
		break;

    case beginFight:
        boss->act_no = beginFight + 1;
        boss->count2 = 9;
        boss->act_wait = -100;

        bossObj[1].cond = npccond_alive;
        bossObj[1].hit = {pixelsToUnits(14), pixelsToUnits(14), pixelsToUnits(8), pixelsToUnits(8)};
        bossObj[1].bits = (npc_ignoreSolid | npc_invulnerable);
        bossObj[2] = bossObj[1];

        bossObj[3].cond = (npccond_alive | npccond_dmgboss);
        bossObj[3].bits |= npc_shootable;
        bossObj[3].hit = {pixelsToUnits(6), pixelsToUnits(6), pixelsToUnits(8), pixelsToUnits(8)};
        createNpc(NPC_ProjectileHeavyPress, boss->x, boss->y + pixelsToUnits(60));
        // Fallthrough
    case beginFight + 1:
        if (boss->count2 > 1 && boss->life < 70 * boss->count2)
        {
            --boss->count2;
            for (size_t i = 8; i < 13; ++i)
            {
                changeTile(i, boss->count2, 0);
                createSmokeLeft(tilesToUnits(i), boss->count2, 0, 4);
                playSound(SFX_DestroyBreakableBlock);
            }
        }

        if (++boss->act_wait == 81 || boss->act_wait == 241)
            createNpc(NPC_EnemyButeGenerator, tilesToUnits(3), tilesToUnits(15), 0, 0, dirUp);
        if (boss->act_wait == 1 || boss->act_wait == 161)
            createNpc(NPC_EnemyButeGenerator, tilesToUnits(17), tilesToUnits(15), 0, 0, dirUp);

        if (boss->act_wait >= 300)
        {
            boss->act_wait = 0;
            createNpc(NPC_ProjectileHeavyPress, boss->x, boss->y + pixelsToUnits(60));
        }
        break;

    case defeated:
        bossObj[3].bits &= ~npc_shootable;
        boss->act_no = defeated + 1;
        boss->act_wait = 0;
        boss->count1 = 0;
        killNpcsByType(NPC_ProjectileHeavyPress);
        killNpcsByType(NPC_EnemyRolling);
        // Fallthrough
    case defeated + 1:
        if (!(++boss->act_wait % 16))
        {
            playSound(SFX_DestroyBreakableBlock);
            createSmokeLeft(boss->x + pixelsToUnits(random(-40, 40)), boss->y + pixelsToUnits(random(-60, 60)), 1, 1);
        }

        if (boss->act_wait == 95)
            boss->ani_no = 1;
        if (boss->act_wait == 98)
            boss->ani_no = 2;
        if (boss->act_wait > 100)
            boss->act_no = 510;
        break;

    case defeated + 10:
        boss->ym += 0x40;
        boss->damage = 127;
        boss->y += boss->ym;
        if (!boss->count1 && boss->y > tilesToUnits(10))
        {
            boss->count1 = 1;
            boss->ym = pixelsToUnits(-1);
            boss->damage = 0;

            for (size_t i = 7; i < 14; ++i)
            {
                changeTile(i, 14, 0);
                createSmokeLeft(tilesToUnits(i), tilesToUnits(14), 0, 0);
                playSound(SFX_DestroyBreakableBlock);
            }
        }
        if (boss->y > tilesToUnits(30))
            boss->act_no = defeated + 20;
        break;
	}

    bossObj[1].x = boss->x - tilesToUnits(1.5);
    bossObj[1].y = boss->y + pixelsToUnits(52);
    bossObj[2].x = bossObj[1].x;
    bossObj[2].y = bossObj[1].y;
    bossObj[3].x = boss->x;
    bossObj[3].y = boss->y + tilesToUnits(2.5);

    constexpr array<RECT, 3> rcNormal = {{{0, 0, 80, 120}, {80, 0, 160, 120}, {160, 0, 240, 120}}};
    constexpr array<RECT, 3> rcBeingHit = {{{0, 120, 80, 240}, {80, 120, 160, 240}, {160, 120, 240, 240}}};

    if (boss->shock)
    {
        static uint8_t flashAlternator = 0;
        if ((flashAlternator >> 1) & 1)
            boss->rect = rcNormal.at(boss->ani_no);
        else
            boss->rect = rcBeingHit.at(boss->ani_no);
    }
    else
        boss->rect = rcNormal.at(boss->ani_no);
}
