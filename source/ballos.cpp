#include "boss.h"
#include <SDL_messagebox.h>
#include <string>
#include "stage.h"
#include "game.h"
#include "sound.h"
#include "player.h"
#include "mathUtils.h"
#include "render.h"

namespace ballos
{
	namespace eyeStates
	{
		enum
		{
			opening = 100,
			closing = 200,
			exploding = 300,
		};
	}

	namespace states
	{
		namespace phase1
		{
			enum
			{
				comeDown = 100,
				falling = 102,
				beginFight = 200,
				prepareJump = 201,
				jumping = 204,
				defeated = 220,
			};
		}

		namespace phase2
		{
			enum
			{
				enterPhase = 300,
				fightBegin = 311,
				goLeft = 311,
				goUp = 312,
				goRight = 313,
				goDown = 314,
			};
		}

		namespace phase3
		{
			enum
			{
				enterPhase = 400,
				spawnSpikes = 410,
				explodeBloody = 420,
				spinPlatforms = 421,
			};
		}

		enum
		{
			init = 0,
			defeated = 1000,
		};
	}

	constexpr int arenaBottom = tilesToUnits(14.0625);
	constexpr int arenaLeft = tilesToUnits(6.9375);
	constexpr int arenaTop = tilesToUnits(6.9375);
	constexpr int arenaRight = tilesToUnits(32.0625);
	constexpr int phase2Speed = pixelsToUnits(1.83203125);
	constexpr int floorY = tilesToUnits(13);
	constexpr int crashY = tilesToUnits(19);

	static bool flashAlternator = false;

	static inline void phase1(npc& boss)
	{
		switch (boss.act_no)
		{
		case states::phase1::jumping:
			if (boss.x < tilesToUnits(5))
				boss.xm = pixelsToUnits(1);
			if (boss.x > tilesToUnits(34))
				boss.xm = pixelsToUnits(-1);
			
			boss.ym += 0x55;
			boss.limitYVel(tilesToUnits(6));

			boss.x += boss.xm;
			boss.y += boss.ym;

			if (boss.y > crashY - boss.hit.bottom)
			{
				boss.y = crashY - boss.hit.bottom;
				boss.ym = 0;
				boss.act_no = states::phase1::prepareJump;
			}
			// Unfinished
			break;
			
		case states::phase1::defeated:
			boss.act_no = states::phase1::defeated + 1;
			boss.life = 1200;
			bossObj[1].act_no = eyeStates::closing;
			bossObj[2].act_no = eyeStates::closing;
			boss.xm = 0;
			boss.ani_no = 0;
			boss.shock = 0;
			flashAlternator = false;
			// Fallthrough
		case states::phase1::defeated + 1:
			boss.ym += pixelsToUnits(0.125);
			boss.limitYVel(pixelsToUnits(6));
			boss.y += boss.ym;

			if (boss.y > floorY - boss.hit.bottom)
			{
				boss.y = floorY - boss.hit.bottom;
				boss.ym = 0;
				boss.act_no = states::phase1::defeated + 2;
				boss.act_wait = 0;
				viewport.quake2 = 30;
				playSound(SFX_LargeObjectHitGround);

				for (int i = 0; i < 16; ++i)
					createNpc(NPC_Smoke, boss.x + pixelsToUnits(random(-40, 40)), boss.y + tilesToUnits(2.5));
				 
				if (currentPlayer.flag & ground)
					currentPlayer.flag = pixelsToUnits(-1);
			}
			break;
		}
	}

	static inline void phase2(npc& boss)
	{
		switch (boss.act_no)
		{
		case states::phase2::enterPhase:
			boss.act_no = states::phase2::enterPhase + 1;
			boss.act_wait = 0;
			for (unsigned int i = 0; i < 0x100; i += 0x40)
			{
				createNpc(NPC_EnemyBallos3Eyeball, boss.x, boss.y, 0, 0, i, &boss, false /* 90 */);
				createNpc(NPC_EnemyBallos3Eyeball, boss.x, boss.y, 0, 0, i + 0x220, &boss, false /* 90 */);
			}
			createNpc(NPC_Ballos2Cutscene, boss.x, boss.y, 0, 0, dirLeft, &boss, false /* 24 */);
			createNpc(NPC_Ballos2Eyes, boss.x - tilesToUnits(1.5), boss.y - tilesToUnits(2.25), 0, 0, 0, &boss, false /* 32 */);
			createNpc(NPC_Ballos2Eyes, boss.x + tilesToUnits(1.5), boss.y - tilesToUnits(2.25), 0, 0, dirRight, &boss, false /* 32 */);
			// Fallthrough
		case states::phase2::enterPhase + 1:
			boss.y += (arenaBottom - boss.y) / 8;
			if (++boss.act_wait > secondsToFrames(1))
			{
				boss.act_no = states::phase2::enterPhase + 10;
				boss.act_wait = 0;
			}
			break;

		case states::phase2::fightBegin:
		// case states::phase2::goLeft:	// same number as fightBegin
			boss.direct = dirLeft;
			boss.xm = -phase2Speed;
			boss.ym = 0;
			boss.x += boss.xm;

			if (boss.x < arenaLeft)
			{
				boss.x = arenaLeft;
				boss.act_no = states::phase2::goUp;
			}
			break;

		case states::phase2::goUp:
			boss.direct = dirUp;
			boss.ym = -phase2Speed;
			boss.xm = 0;
			boss.y += boss.ym;
			if (boss.y < arenaTop)
			{
				boss.y = arenaTop;
				boss.act_no = states::phase2::goRight;
			}
			break;

		case states::phase2::goRight:
			boss.direct = dirRight;
			boss.xm = phase2Speed;
			boss.ym = 0;
			boss.x += boss.xm;

			if (boss.x > arenaRight)
			{
				boss.x = arenaRight;
				boss.act_no = states::phase2::goDown;
			}

			if (boss.count1)
				--boss.count1;
			// Check if all rotating thingos are dead and if is at center of room
			if (!boss.count1 && boss.x > tilesToUnits(19) && boss.x < tilesToUnits(21))
				boss.act_no = states::phase3::enterPhase;
			break;

		case states::phase2::goDown:
			boss.direct = dirDown;
			boss.ym = phase2Speed;
			boss.xm = 0;
			boss.y += boss.ym;
			if (boss.y > arenaBottom)
			{
				boss.y = arenaBottom;
				boss.act_no = states::phase2::goLeft;
			}
			break;
		}
	}
}



void actBoss_Ballos(npc *boss)	// Ballos ball (boss)
{
	ballos::phase1(*boss);
	ballos::phase2(*boss);

	static bool notified = false;

	if (!notified)
	{
		std::string msg = "Boss " + std::to_string(bossObj[0].code_char) + " is not completely implemented yet.";
		logWarning(msg);
		if (debugFlags & notifyOnNotImplemented)
    	    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Partially implemented boss", msg.c_str(), nullptr);
	}
}