#include "boss.h"
#include <SDL_messagebox.h>
#include <string>
#include "stage.h"
#include "game.h"

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
	constexpr int phase2Speed = pixelsToUnits(1.83203125);
	constexpr int floorY = tilesToUnits(13);

	static inline void phase1(npc& boss)
	{
		switch (boss.act_no)
		{
		case states::phase1::defeated:
			boss.act_no = states::phase1::defeated + 1;
			boss.life = 1200;
			bossObj[1].act_no = eyeStates::closing;
			bossObj[2].act_no = eyeStates::closing;
			boss.xm = 0;
			boss.ani_no = 0;
			boss.shock = 0;
			ballosFlashAlternator = false;
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
				// This isn't finished
			}
		}
	}

	static inline void phase2(npc& boss)
	{
		switch (boss.act_no)
		{
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
		}
	}
}

static bool ballosFlashAlternator = false;

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