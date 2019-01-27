#include "boss.h"
#include <SDL_messagebox.h>
#include <string>
#include <array>
#include "stage.h"
#include "game.h"
#include "sound.h"
#include "player.h"
#include "mathUtils.h"
#include "render.h"
#include "flash.h"

namespace ballos
{
	constexpr auto& boss = gBossObj[0];
	constexpr auto& eye1 = gBossObj[1];
	constexpr auto& eye2 = gBossObj[2];
	constexpr auto& body = gBossObj[3];
	constexpr auto& hitai = gBossObj[4];	// Name is from linux symbols
	constexpr auto& hara = gBossObj[5];	// Name is from linux symbols

	namespace eyeStates
	{
		enum
		{
			init = 0,
			opening = 100,
			openNoWait = 102,
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
	constexpr int phase3YPos = tilesToUnits(9.9375);

	static bool flashAlternator = false;

	static inline bool AIInitialize()
	{
		if (boss.act_no == states::init)
		{
			// Initialize main boss
			boss.act_no = states::init + 1;
			boss.cond = npccond_alive;
			boss.exp = 1;
			boss.direct = dirLeft;
			boss.x = tilesToUnits(20);
			boss.y = tilesToUnits(-4);
			boss.hit_voice = SFX_HeavyPressLargeObjectHurt;
			boss.hit.left = tilesToUnits(2);
			boss.hit.top = tilesToUnits(3);
			boss.hit.right = body.hit.left;
			boss.hit.bottom = body.hit.top;
			boss.bits = npc_ignoreSolid | npc_solidHard | npc_eventDie | npc_showDamage;
			boss.size = 3;
			boss.damage = 0;
			boss.code_event = 1000;
			boss.life = 800;

			// Initialize eyes
			eye1.cond = npccond_alive | npccond_dmgboss;
			eye1.direct = dirLeft;
			eye1.bits = npc_ignoreSolid;
			eye1.life = 10000;
			eye1.view.left = tilesToUnits(0.75);
			eye1.view.top = 0;
			eye1.view.right = eye1.view.left;
			eye1.view.bottom = tilesToUnits(1);
			eye1.hit = eye1.view;
			
			eye2 = eye1;
			eye2.direct = dirRight;

			// Initialize the body
			body.cond = npccond_alive | npccond_dmgboss;
			body.bits = npc_ignoreSolid | npc_invulnerable | npc_solidSoft;

			body.view.left = tilesToUnits(3.75);
			body.view.top = body.view.left;
			body.view.right = body.view.left;
			body.view.bottom = body.view.top;

			body.hit.left = tilesToUnits(3);
			body.hit.top = tilesToUnits(1.5);
			body.hit.right = tilesToUnits(3);
			body.hit.bottom = tilesToUnits(2);

			hitai.cond = npccond_alive | npccond_dmgboss;
			hitai.bits = npc_ignoreSolid | npc_invulnerable | npc_solidSoft;

			hitai.hit.left = tilesToUnits(2);
			hitai.hit.top = tilesToUnits(0.5);
			hitai.hit.right = hitai.hit.left;
			hitai.hit.bottom = hitai.hit.top;

			hara.cond = npccond_alive | npccond_dmgboss;
			hara.bits = npc_solidHard | npc_ignoreSolid | npc_invulnerable;
			hara.hit.left = tilesToUnits(2);
			hara.hit.top = 0;
			hara.hit.top = hara.hit.left;
			hara.hit.bottom = tilesToUnits(3);

			return true;
		}
		return false;
	}

	static inline bool AIphase1()
	{
		switch (boss.act_no)
		{
		case states::phase1::comeDown:
			boss.act_no = states::phase1::comeDown + 1;
			boss.ani_no = 0;
			boss.x = gCurrentPlayer.x;
			createNpc(NPC_ProjectileBallosLightning, gCurrentPlayer.x, crashY, 0, 0, dirRight);
			boss.act_wait = 0;
			// Fallthrough
		case states::phase1::comeDown + 1:
			if (++boss.act_wait > secondsToFrames(0.6))
				boss.act_no = states::phase1::falling;
			break;

		case states::phase1::falling:
			boss.ym += pixelsToUnits(0.125);
			boss.limitYVel(pixelsToUnits(6));
			boss.y += boss.ym;

			if (boss.y > crashY - boss.hit.bottom)
			{
				boss.y = crashY - boss.hit.bottom;
				boss.ym = 0;
				boss.act_no = states::phase1::falling + 1;
				boss.act_wait = 0;
				gViewport.quake2 = 30;
				playSound(SFX_MissileImpact);
				if (gCurrentPlayer.y > boss.y + tilesToUnits(3) && boss.getXDistToPlayer() < tilesToUnits(1.5))
					gCurrentPlayer.damage(16);

				for (unsigned i = 0; i < 16; ++i)
					createNpc(NPC_Smoke, boss.x + pixelsToUnits(random(-40, 40)), boss.y + tilesToUnits(2.5));
				
				if (gCurrentPlayer.flag & ground)
					gCurrentPlayer.ym = pixelsToUnits(-1);
			}
			break;

		case states::phase1::falling + 1:
			if (++boss.act_wait == secondsToFrames(1))
			{
				boss.act_no = states::phase1::falling + 2;
				eye1.act_no = eyeStates::opening;
				eye2.act_no = eyeStates::opening;
			}
			break;

		case states::phase1::beginFight:
			boss.count1 = 0;
			// Fallthrough
		case states::phase1::prepareJump:
			boss.act_no = states::phase1::prepareJump + 2;
			boss.xm = 0;
			++boss.count1;
			boss.hit.bottom = tilesToUnits(3);
			boss.damage = 0;
			if (boss.count1 % 3)
				boss.act_wait = secondsToFrames(1);
			else
				boss.act_wait = secondsToFrames(3);	// Long wait every 3 jumps
			// Fallthrough
		case states::phase1::prepareJump + 2:
			if (--boss.act_wait <= 0)
			{
				boss.act_no = states::phase1::jumping;
				boss.ym = pixelsToUnits(-6);
				boss.xm = 0;
				boss.accelerateTowardsPlayer(pixelsToUnits(1));
			}
			break;

		case states::phase1::jumping:
			if (boss.x < tilesToUnits(5))
				boss.xm = pixelsToUnits(1);
			if (boss.x > tilesToUnits(34))
				boss.xm = pixelsToUnits(-1);
			
			boss.ym += 0x55;
			boss.limitYVel(pixelsToUnits(6));

			boss.x += boss.xm;
			boss.y += boss.ym;

			if (boss.y > crashY - boss.hit.bottom)
			{
				boss.y = crashY - boss.hit.bottom;
				boss.ym = 0;
				boss.act_no = states::phase1::prepareJump;
				boss.act_wait = 0;
				
				// Player smash damage
				if (gCurrentPlayer.y > boss.y + tilesToUnits(3.5))
					gCurrentPlayer.damage(16);

				// Bounce player
				if (gCurrentPlayer.flag & ground)
					gCurrentPlayer.ym = pixelsToUnits(-1);

				gViewport.quake2 = 30;
				playSound(SFX_LargeObjectHitGround);
				createNpc(NPC_ProjectileBallosShockwave
					, boss.x - tilesToUnits(0.75), boss.y + tilesToUnits(3.25), 0, 0, dirLeft);
				createNpc(NPC_ProjectileBallosShockwave
					, boss.x + tilesToUnits(0.75), boss.y + tilesToUnits(3.25), 0, 0, dirRight);

				playSound(SFX_MissileImpact);

				for (int i = 0; i < 16; ++i)
					createNpc(NPC_Smoke
						, boss.x + pixelsToUnits(random(-40, 40)), boss.y + tilesToUnits(2.5));
			}
			break;
			
		case states::phase1::defeated:
			boss.act_no = states::phase1::defeated + 1;
			boss.life = 1200;
			eye1.act_no = eyeStates::closing;
			eye2.act_no = eyeStates::closing;
			boss.xm = 0;
			boss.ani_no = 0;
			boss.shock = 0;
			flashAlternator = false;
			// Fallthrough
		case states::phase1::defeated + 1:
			boss.ym += pixelsToUnits(0.125);
			boss.limitYVel(pixelsToUnits(6));
			boss.y += boss.ym;

			if (boss.y > crashY - boss.hit.bottom)
			{
				boss.y = crashY - boss.hit.bottom;
				boss.ym = 0;
				boss.act_no = states::phase1::defeated + 2;
				boss.act_wait = 0;
				gViewport.quake2 = 30;
				playSound(SFX_LargeObjectHitGround);

				for (int i = 0; i < 16; ++i)
					createNpc(NPC_Smoke, boss.x + pixelsToUnits(random(-40, 40)), boss.y + tilesToUnits(2.5));
				 
				if (gCurrentPlayer.flag & ground)
					gCurrentPlayer.flag = pixelsToUnits(-1);
			}
			break;

		default:
			return false;
		}
		return true;
	}

	static inline bool AIphase2()
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

		default:
			return false;
		}
		return true;
	}

	static inline void phase3DoPlatformWait(int waitTime, int nextPhase = -1)
	{
		if (++boss.ani_wait > waitTime)
		{
			boss.ani_wait = 0;
			if (nextPhase != -1)
				boss.act_no = nextPhase;
			else
				++boss.act_no;
		}
	}

	static inline bool AIphase3()
	{
		switch (boss.act_no)
		{
		case states::phase3::enterPhase:
			boss.act_no = states::phase3::enterPhase + 1;
			boss.act_wait = 0;
			boss.xm = 0;
			boss.ym = 0;
			killNpcsByType(NPC_GeneratorGreenDevil, false);
			// Fallthrough
		case states::phase3::enterPhase + 1:
			boss.y += (phase3YPos - boss.y) / 8;
			if (++boss.act_wait > secondsToFrames(1))
			{
				boss.act_wait = 0;
				boss.act_no = states::phase3::spawnSpikes;
				for (int i = 0; i < 0x100; i += 0x20)
					createNpc(NPC_Ballos4OrbitingPlatform, boss.x, boss.y, 0, 0, i, &boss, false /* 80 */);
				createNpc(NPC_Ballos2Cutscene, boss.x, boss.y, 0, 0, dirLeft, &boss, false /* 24 */);
				createNpc(NPC_Ballos2Eyes, boss.x - tilesToUnits(1.5), boss.y - tilesToUnits(2.25)
					, 0, 0, dirLeft, &boss, false /* 32 */);
				createNpc(NPC_Ballos2Eyes, boss.x + tilesToUnits(1.5), boss.y - tilesToUnits(2.25)
					, 0, 0, dirRight, &boss, false /* 32 */);
			}
			break;

		case states::phase3::spawnSpikes:
			if (++boss.act_wait > secondsToFrames(1))
			{
				boss.act_wait = 0;
				boss.act_no = states::phase3::spawnSpikes + 1;
			}
			break;

		case states::phase3::spawnSpikes + 1:
			if (++boss.act_wait % secondsToFrames(0.6) == 1)
				createNpc(NPC_BallosSpikesRising, tilesToUnits(2 * (boss.act_wait / 30) + 2), tilesToUnits(21));
			if (boss.act_wait / 3 % 2)
				playSound(SFX_LargeObjectHitGround);
			if (boss.act_wait > secondsToFrames(10.8))
				boss.act_no = states::phase3::explodeBloody;
			break;

		case states::phase3::explodeBloody:
			boss.act_no = states::phase3::spinPlatforms;
			boss.act_wait = 0;
			boss.ani_wait = 0;
			
			gViewport.quake2 = 30;
			playSound(SFX_LargeExplosion);
			
			eye1.act_no = eyeStates::openNoWait;
			eye2.act_no = eyeStates::openNoWait;

			for (int i = 0; i < 0x100; ++i)
				createNpc(NPC_Smoke, boss.x + pixelsToUnits(random(-60, 60))
					, boss.y + pixelsToUnits(random(-60, 60)));
			// Fallthrough
		case states::phase3::spinPlatforms:
		case states::phase3::spinPlatforms + 4:
			phase3DoPlatformWait(secondsToFrames(10));
			break;

		case states::phase3::spinPlatforms + 1:
			phase3DoPlatformWait(secondsToFrames(4));
			break;

		case states::phase3::spinPlatforms + 2:
		case states::phase3::spinPlatforms + 6:
			phase3DoPlatformWait(secondsToFrames(0.4));
			break;

		case states::phase3::spinPlatforms + 3:
		case states::phase3::spinPlatforms + 5:
			phase3DoPlatformWait(secondsToFrames(4));
			break;

		case states::phase3::spinPlatforms + 7:
			phase3DoPlatformWait(secondsToFrames(4), states::phase3::spinPlatforms);
			break;

		default:
			return false;
		}
		return true;
	}

	static inline bool AIdefeated()
	{
		switch (boss.act_no)
		{
		case states::defeated:
			boss.act_no = states::defeated + 1;
			boss.act_wait = 0;
			eye1.act_no = eyeStates::exploding;
			eye2.act_no = eyeStates::exploding;

			boss.bits &= ~npc_solidHard | npc_solidSoft;
			body.bits &= ~npc_solidHard | npc_solidSoft;
			hitai.bits &= ~npc_solidHard | npc_solidSoft;
			hara.bits &= ~npc_solidHard | npc_solidSoft;

			// Fallthrough
		case states::defeated + 1:
			if (!(++boss.act_wait % secondsToFrames(0.24)))
				playSound(SFX_MissileImpact);
			createSmokeLeft(boss.x + pixelsToUnits(random(-60, 60)), boss.y + pixelsToUnits(random(-60, 60)), 1, 1);

			if (boss.act_wait > secondsToFrames(3))
			{
				boss.act_wait = 0;
				boss.act_no = states::defeated + 2;
				setFlash(boss.x, boss.y, flashModes::normal, 10);
				playSound(SFX_LargeExplosion);
			}
			break;

		case states::defeated + 2:
			gViewport.quake2 = 40;
			if (++boss.act_wait == secondsToFrames(1))
			{
				boss.cond = 0;
				eye1.cond = 0;
				eye2.cond = 0;
				body.cond = 0;
				hitai.cond = 0;
				hara.cond = 0;
				killNpcsByType(NPC_EnemyButeArcher);
				killNpcsByType(NPC_BallosSpikesRising);
			}
			break;

		default:
			return false;
		}
		return true;
	}

	static inline void AIEnd()
	{
		if (boss.act_no > states::phase3::explodeBloody && boss.act_no < 500)
		{
			body.bits |= npc_shootable;
			hitai.bits |= npc_shootable;
			hara.bits |= npc_shootable;

			if (++boss.act_wait > secondsToFrames(6))
			{
				boss.act_wait = 0;
				if (gCurrentPlayer.x <= boss.x)
				{
					for (unsigned i = 0; i < 8; ++i)
						createNpc(NPC_EnemyButeArcherRed
							, tilesToUnits(random(-4, 4)) / 4, tilesToUnits(random(8, 68)) / 4
								, 0, 0, dirRight);
				}
				else
				{
					for (unsigned i = 0; i < 8; ++i)
						createNpc(NPC_EnemyButeArcherRed
							, tilesToUnits(random(-4, 4) + 156) / 4, tilesToUnits(random(8, 68)) / 4
								, 0, 0, dirLeft);
				}
			}
		}

		if (boss.shock)
		{
			flashAlternator = !flashAlternator;
			body.ani_no = flashAlternator;
		}
		else
			body.ani_no = 0;

		if (boss.act_no > states::phase3::explodeBloody)
			body.ani_no += 2;
	}

	static inline void AIEye(npc& eye)
	{
		constexpr std::array<RECT, 5> rcLeft
		{{
			{272, 0, 296, 16},
			{272, 16, 296, 32},
			{272, 32, 296, 48},
			{0, 0, 0, 0},
			{240, 16, 264, 32},
		}};

		constexpr std::array<RECT, 5> rcRight
		{{
			{296, 0, 320, 16},
			{296, 16, 320, 32},
			{296, 32, 320, 48},
			{0, 0, 0, 0},
			{240, 32, 264, 48},
		}};

		switch (eye.act_no)
		{
		case eyeStates::opening:
			eye.act_no = eyeStates::opening + 1;
			eye.ani_no = 0;
			eye.ani_wait = 0;
			// Fallthrough
		case eyeStates::opening + 1:
			eye.animate(2);
			if (eye.ani_no > 2)
				eye.act_no = eyeStates::openNoWait;
			break;

		case eyeStates::openNoWait:
			eye.ani_no = 3;
			break;

		case eyeStates::closing:
			eye.act_no = eyeStates::closing + 1;
			eye.ani_no = 3;
			eye.ani_wait = 0;
			// Fallthrough
		case eyeStates::closing + 1:
			if (++eye.ani_wait > 2)
			{
				eye.ani_wait = 0;
				--eye.ani_no;
			}
			if (eye.ani_no <= 0)
				eye.act_no = eyeStates::closing + 2;
			break;

		case eyeStates::exploding:
			eye.act_no = eyeStates::exploding + 1;
			eye.ani_no = 4;
			if (eye.direct != dirLeft)
				createSmokeLeft(eye.x + tilesToUnits(0.25), eye.y, tilesToUnits(0.25), 10);
			else
				createSmokeLeft(eye.x - tilesToUnits(0.25), eye.y, tilesToUnits(0.25), 10);
			break;

		default:
			break;
		}

		if (eye.direct != dirLeft)
			eye.x = boss.x + tilesToUnits(1.5);
		else
			eye.x = boss.x - tilesToUnits(1.5);
		eye.y = boss.y - tilesToUnits(2.25);

		if (eye.act_no >= eyeStates::init && eye.act_no < eyeStates::exploding)
		{
			if (eye.ani_no == 3)
				eye.bits |= npc_shootable;
			else
				eye.bits &= ~npc_shootable;
		}

		eye.doRects(rcLeft, rcRight);
	}

	static inline void AIBody()
	{
		std::array<RECT, 4> rcNPC
		{{
			{0, 0, 120, 120},
			{120, 0, 240, 120},
			{0, 120, 120, 240},
			{120, 120, 240, 240},
		}};

		body.x = boss.x;
		body.y = boss.y;

		body.doRects(rcNPC);
	}

	static inline void AIHITAI()
	{
		hitai.x = boss.x;
		hitai.y = boss.y - tilesToUnits(2.75);
	}

	static inline void AIHARA()
	{
		hara.x = boss.x;
		hara.y = boss.y;
	}

	static inline void subNPCs()
	{
		AIEye(eye1);
		AIEye(eye2);
		AIBody();
		AIHITAI();
		AIHARA();
	}
}



void actBoss_Ballos(npc *boss)	// Ballos ball (boss)
{
	if (!ballos::AIInitialize())
		if (!ballos::AIphase1())
			if (!ballos::AIphase2())
				if (!ballos::AIphase3())
					if (!ballos::AIdefeated())
					{
						// Avoid running multiple states in a frame
					}
	ballos::AIEnd();
	ballos::subNPCs();
}
