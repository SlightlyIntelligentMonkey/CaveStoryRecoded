#pragma once

#include <deque>
#include <vector>
#include "common.h"

struct NPC_RECT
{
	uint8_t front;
	uint8_t top;
	uint8_t back;
	uint8_t bottom;
};

struct NPC_TABLE
{
	uint16_t bits;
	uint16_t life;
	uint8_t surf;
	uint8_t hit_voice;
	uint8_t destroy_voice;
	uint8_t size;
	int32_t exp;
	int32_t damage;
	NPC_RECT hit;
	NPC_RECT view;
};

extern NPC_TABLE *npcTable;

class npc
{
	//Variables
public:
	//state things
	uint8_t cond;
	int flag;

	//position
	int x;
	int y;

	//speed
	int xm;
	int ym;
	int xm2;
	int ym2;

	int tgt_x;
	int tgt_y;

	//npc things
	int code_char;
	int code_flag;
	int code_event;

	// Sprite index
	int surf;

	//stuff?
	int hit_voice;
	int destroy_voice;

	//life stuff
	int life;
	int exp;

	/// Explosion size
	int size;

	/// Direction NPC is facing towards
	int direct;

	/// Entity flags
	unsigned __int16 bits;

	/// Rect location in spritesheet
	RECT rect;

	//state, counters, and action
	int ani_wait;
	int ani_no;
	int count1;
	int count2;
	int act_no;
	int act_wait;

	//hitbox
	RECT hit;

	//offset
	RECT view;

	// Damage
	uint8_t shock;
	int damage_view;
	int damage;

	// Parent NPC
	npc *pNpc;

	// Priority
	bool priority;

public:
	// These are kinda supposed to be internal, but I can't put them as protected/private
	void accelerateTowardsPlayer(int vel);
	void animate(int aniWait, int aniStart = -1, int aniMax = -1);
	void createSmokeWithVel(size_t num, int xVel, int yVel);
	void doGravity(int gravity, int maxYVel);

	void doRects(const std::vector<RECT>& rcLeft, const std::vector<RECT>& rcRight);
	void doRects(const std::vector<RECT>& rcNPC);
	void doRects(RECT rcLeft, RECT rcRight);
	void doRects(RECT rcNPC);

	void facePlayer();
    int getXDistToPlayer() attrPure;

	void limitXVel(int maxVel);
	void limitYVel(int maxVel);

	void moveInDir(int vel);

	void accelerateTowardsXTarget(int vel);
	void accelerateTowardsYTarget(int vel);

	bool isPlayerWithinDistance(int xDist, int yDistHigh, int yDistLow) attrPure;
	inline bool isPlayerWithinDistance(int xDist, int yDist) attrPure
	{
		return isPlayerWithinDistance(xDist, yDist, yDist);
	}

	bool isPlayerAligned(int xRay, int yRayHigh, int yRayLow) attrPure;
	inline bool isPlayerAligned(int xRay, int yRay) attrPure
	{
		return isPlayerAligned(xRay, yRay, yRay);
	}

	void init(int setCode, int setX, int setY, int setXm, int setYm, int setDir, npc *parentNpc);

	void update();
	void draw();
};

void loadNpcTable();

void createSmokeLeft(int x, int y, int w, size_t num);
void createSmokeUp(int x, int y, int w, int num);

void createNpc(int setCode, int setX = 0, int setY = 0, int setXm = 0, int setYm = 0, int setDir = dirLeft, npc *parentNpc = nullptr, bool setPriority = true);
void changeNpc(int code_event, int code_char, int dir = dirLeft);
int findEntityByType(int entityType) attrPure;
void setNPCState(int entityEventNum, int newNPCState, int direction);
void moveNPC(int entityEventNum, int xPos, int yPos, int direction);

void updateNPC();
void drawNPC();
void dropExperience(int x, int y, int exp);
void killNpc(npc *NPC, bool bVanish = true);

extern std::deque<npc> npcs;

extern int superXPos;
extern int superYPos;

extern int curlyShootWait;
extern int curlyShootX;
extern int curlyShootY;

enum NPC_cond
{
	npccond_dmgboss = 0x10, //when set damages the main boss
	npccond_alive = 0x80, //Determines if an npc is alive
};

enum NPC_flags
{
	npc_solidSoft = 0x1, //Pushes quote out
	npc_ignore44 = 0x2, //Ignores tile 44 (No NPC)
	npc_invulnerable = 0x4, //Can't get hit
	npc_ignoreSolid = 0x8, //Doesn't collide with anything
	npc_bouncy = 0x10, //Quote bounces on the top
	npc_shootable = 0x20, //Can be shot
	npc_solidHard = 0x40, //Essentially acts as level tiles
	npc_rearTop = 0x80, //Rear and top don't hurt
	npc_eventTouch = 0x100, //Run event when touched
	npc_eventDie = 0x200, //Run event when killed
	npc_appearSet = 0x800, //Only appear when flag is set
	npc_altDir = 0x1000, //Spawn facing to the right (or however the npc interprets the direction as)
	npc_interact = 0x2000, //Run event when interacted with
	npc_hideSet = 0x4000, //Hide when flag is set
	npc_showDamage = 0x8000, //Show #Damage taken
};

enum NPCNames
{
	NPC_Nothing = 0x0,
	NPC_WeaponEnergy = 0x1,
	NPC_EnemyBehemoth = 0x2,
	NPC_DeletesItself = 0x3,
	NPC_Smoke = 0x4,
	NPC_EnemyCritterHoppingGreen = 0x5,
	NPC_EnemyBeetleHorizontalGreen = 0x6,
	NPC_EnemyBasil = 0x7,
	NPC_EnemyBeetleFollow1 = 0x8,
	NPC_BalrogFalling = 0x9,
	NPC_BossBalrogShooting = 0xA,
	NPC_ProjectileBalrogEnergyBallInvincible = 0xB,
	NPC_BalrogStanding = 0xC,
	NPC_Forcefield = 0xD,
	NPC_SantasKey = 0xE,
	NPC_TreasureChestClosed = 0xF,
	NPC_SavePoint = 0x10,
	NPC_HealthAmmoRefill = 0x11,
	NPC_Door = 0x12,
	NPC_BalrogBustsIn = 0x13,
	NPC_Computer = 0x14,
	NPC_TreasureChestOpen = 0x15,
	NPC_Teleporter = 0x16,
	NPC_TeleporterLights = 0x17,
	NPC_PowerCritter = 0x18,
	NPC_LiftPlatform = 0x19,
	NPC_EnemyBatBlackCircling = 0x1A,
	NPC_DeathTrap = 0x1B,
	NPC_EnemyCritterFlying = 0x1C,
	NPC_Cthulhu = 0x1D,
	NPC_HermitGunsmith = 0x1E,
	NPC_EnemyBatBlackHanging = 0x1F,
	NPC_LifeCapsule = 0x20,
	NPC_ProjectileBalrogEnergyBounce = 0x21,
	NPC_Bed = 0x22,
	NPC_EnemyManann = 0x23,
	NPC_BossBalrogHovering = 0x24,
	NPC_Signpost = 0x25,
	NPC_FireplaceFire = 0x26,
	NPC_SaveSign = 0x27,
	NPC_Santa = 0x28,
	NPC_BustedDoorway = 0x29,
	NPC_Sue = 0x2A,
	NPC_Blackboard = 0x2B,
	NPC_EnemyPolish = 0x2C,
	NPC_EnemyBaby = 0x2D,
	NPC_HorizontalVerticalTrigger = 0x2E,
	NPC_EnemySandcrocGreen = 0x2F,
	NPC_ProjectileOmega = 0x30,
	NPC_EnemySkullhead = 0x31,
	NPC_ProjectileSkeleton = 0x32,
	NPC_EnemyCrowAndSkullhead = 0x33,
	NPC_BlueRobotSitting = 0x34,
	NPC_SkullstepFoot = 0x35,
	NPC_EnemySkullstep = 0x36,
	NPC_Kazuma = 0x37,
	NPC_EnemyBeetleHorizontalBrown = 0x38,
	NPC_EnemyCrow = 0x39,
	NPC_EnemyBasu1 = 0x3A,
	NPC_EnemyDoor = 0x3B,
	NPC_Toroko = 0x3C,
	NPC_King = 0x3D,
	NPC_KazumaComputer = 0x3E,
	NPC_TorokoAttacking = 0x3F,
	NPC_EnemyCritterHoppingBlue = 0x40,
	NPC_EnemyBatBlue = 0x41,
	NPC_ProjectileMiseryBubble = 0x42,
	NPC_MiseryFloating = 0x43,
	NPC_BossBalrogRunning = 0x44,
	NPC_EnemyPignon = 0x45,
	NPC_SparklingItem = 0x46,
	NPC_EnemyChinfish = 0x47,
	NPC_Sprinkler = 0x48,
	NPC_Waterdrop = 0x49,
	NPC_Jack = 0x4A,
	NPC_KanpachiFishing = 0x4B,
	NPC_Flowers = 0x4C,
	NPC_SandaimesPavillon = 0x4D,
	NPC_Pot = 0x4E,
	NPC_Mahin = 0x4F,
	NPC_EnemyGravekeeper = 0x50,
	NPC_EnemyGiantPignon = 0x51,
	NPC_MiseryStanding = 0x52,
	NPC_IgorCutscene = 0x53,
	NPC_ProjectileBasu1 = 0x54,
	NPC_Terminal = 0x55,
	NPC_Missile = 0x56,
	NPC_Heart = 0x57,
	NPC_BossIgor = 0x58,
	NPC_IgorDefeated = 0x59,
	NPC_Background = 0x5A,
	NPC_Cage = 0x5B,
	NPC_SueComputer = 0x5C,
	NPC_Chaco = 0x5D,
	NPC_EnemyKulala = 0x5E,
	NPC_EnemyJelly = 0x5F,
	NPC_FanLeft = 0x60,
	NPC_FanUp = 0x61,
	NPC_FanRight = 0x62,
	NPC_FanDown = 0x63,
	NPC_Grate = 0x64,
	NPC_PowerControlsScreen = 0x65,
	NPC_PowerControlsPowerflow = 0x66,
	NPC_ProjectileMannan = 0x67,
	NPC_EnemyFrog = 0x68,
	NPC_BalloonHeyLow = 0x69,
	NPC_BalloonHeyHigh = 0x6A,
	NPC_MalcoUndamaged = 0x6B,
	NPC_ProjectileBalfrog = 0x6C,
	NPC_MalcoDamaged = 0x6D,
	NPC_EnemyPuchi = 0x6E,
	NPC_QuoteTeleportsOut = 0x6F,
	NPC_QuoteTeleportsIn = 0x70,
	NPC_ProfessorBooster = 0x71,
	NPC_EnemyPress = 0x72,
	NPC_EnemyRavil = 0x73,
	NPC_RedFlowersPetals = 0x74,
	NPC_Curly = 0x75,
	NPC_BossCurly = 0x76,
	NPC_TableChair = 0x77,
	NPC_Colon1 = 0x78,
	NPC_Colon2 = 0x79,
	NPC_EnemyColon = 0x7A,
	NPC_ProjectileCurly = 0x7B,
	NPC_Sunstone = 0x7C,
	NPC_HiddenHeartMissile = 0x7D,
	NPC_PuppyRunsAway = 0x7E,
	NPC_ProjectileMachineGunTrailLevel2 = 0x7F,
	NPC_ProjectileMachineGunTrailLevel3 = 0x80,
	NPC_ProjectileFireballSnakeTrail = 0x81,
	NPC_PuppyTailWag = 0x82,
	NPC_PuppySleeping = 0x83,
	NPC_PuppyBark = 0x84,
	NPC_Jenka = 0x85,
	NPC_EnemyArmadillo = 0x86,
	NPC_EnemySkeleton = 0x87,
	NPC_PuppyCarried = 0x88,
	NPC_LargeDoorwayFrame = 0x89,
	NPC_LargeDoorwayDoors = 0x8A,
	NPC_DoctorWithCrown = 0x8B,
	NPC_BossFrenziedToroko = 0x8C,
	NPC_ProjectileTorokoBlock = 0x8D,
	NPC_EnemyFlowercub = 0x8E,
	NPC_JenkaCollapsed = 0x8F,
	NPC_TorokoTeleportsIn = 0x90,
	NPC_KingStruckByLightning = 0x91,
	NPC_Lightning = 0x92,
	NPC_EnemyCritterHover = 0x93,
	NPC_ProjectileCritter = 0x94,
	NPC_MovingBlockHorizontal = 0x95,
	NPC_Quote = 0x96,
	NPC_BlueRobot = 0x97,
	NPC_ShutterStuck = 0x98,
	NPC_EnemyGaudi = 0x99,
	NPC_EnemyGaudiDefeated = 0x9A,
	NPC_EnemyGaudiFlying = 0x9B,
	NPC_ProjectileGaudiFlying = 0x9C,
	NPC_MovingBlockVertical = 0x9D,
	NPC_ProjectileMonsterX = 0x9E,
	NPC_BossMonsterXDefeated = 0x9F,
	NPC_BossPoohBlack = 0xA0,
	NPC_ProjectilePoohBlack = 0xA1,
	NPC_PoohBlackDefeated = 0xA2,
	NPC_DrGero = 0xA3,
	NPC_NurseHasumi = 0xA4,
	NPC_CurlyCollapsed = 0xA5,
	NPC_Chaba = 0xA6,
	NPC_ProfessorBoosterFalling = 0xA7,
	NPC_Boulder = 0xA8,
	NPC_BossBalrogMissiles = 0xA9,
	NPC_ProjectileBalrogMissiles = 0xAA,
	NPC_EnemyFireWhirrr = 0xAB,
	NPC_ProjectileFireWhirrr = 0xAC,
	NPC_EnemyGaudiArmor = 0xAD,
	NPC_ProjectileGaudiArmor = 0xAE,
	NPC_EnemyGaudiEgg = 0xAF,
	NPC_EnemyBuyoBoyoBase = 0xB0,
	NPC_EnemyBuyoBuyo = 0xB1,
	NPC_ProjectileCoreSpinner = 0xB2,
	NPC_ProjectileCoreWisp = 0xB3,
	NPC_CurlyAI = 0xB4,
	NPC_CurlyAIMachineGun = 0xB5,
	NPC_CurlyAIPolarStar = 0xB6,
	NPC_CurlyAirTankBubble = 0xB7,
	NPC_ShutterLarge = 0xB8,
	NPC_ShutterSmall = 0xB9,
	NPC_LiftBlock = 0xBA,
	NPC_EnemyFuzzCore = 0xBB,
	NPC_FuzzEnemy = 0xBC,
	NPC_ProjectileHomingFlame = 0xBD,
	NPC_SurfaceRobot = 0xBE,
	NPC_WaterLevel = 0xBF,
	NPC_Scooter = 0xC0,
	NPC_ScooterPieces = 0xC1,
	NPC_BlueRobotPieces = 0xC2,
	NPC_GrateMouth = 0xC3,
	NPC_MotionWall = 0xC4,
	NPC_EnemyPorcupineFish = 0xC5,
	NPC_ProjectileIronhead = 0xC6,
	NPC_UnderwaterCurrent = 0xC7,
	NPC_EnemyDragonZombie = 0xC8,
	NPC_DragonZombieDead = 0xC9,
	NPC_ProjectileDragonZombie = 0xCA,
	NPC_EnemyCritterHoppingAqua = 0xCB,
	NPC_FallingSpikeSmall = 0xCC,
	NPC_FallingSpikeLarge = 0xCD,
	NPC_EnemyCounterBomb = 0xCE,
	NPC_BalloonCountdown = 0xCF,
	NPC_EnemyBasu2 = 0xD0,
	NPC_ProjectileBasu2 = 0xD1,
	NPC_EnemyBeetleFollow2 = 0xD2,
	NPC_Spikes = 0xD3,
	NPC_SkyDragon = 0xD4,
	NPC_EnemyNightSpirit = 0xD5,
	NPC_ProjectileNightSpirit = 0xD6,
	NPC_EnemySandcrocwhite = 0xD7,
	NPC_DebugCat = 0xD8,
	NPC_Itoh = 0xD9,
	NPC_ProjectileCoreLargeEnergyBall = 0xDA,
	NPC_GeneratorSmokeUnderwaterCurrent = 0xDB,
	NPC_ShovelBrigade = 0xDC,
	NPC_ShovelBrigadeWalking = 0xDD,
	NPC_PrisonBars = 0xDE,
	NPC_Momorin = 0xDF,
	NPC_Chie = 0xE0,
	NPC_Megane = 0xE1,
	NPC_Kanpachi = 0xE2,
	NPC_Bucket = 0xE3,
	NPC_DrollGuard = 0xE4,
	NPC_RedFlowersSprouts = 0xE5,
	NPC_RedFlowersBlooming = 0xE6,
	NPC_Rocket = 0xE7,
	NPC_EnemyOrangeBell = 0xE8,
	NPC_EnemyOrangeBellBat = 0xE9,
	NPC_RedFlowerSpicked = 0xEA,
	NPC_EnemyMidorin = 0xEB,
	NPC_EnemyGunfish = 0xEC,
	NPC_ProjectileGunfish = 0xED,
	NPC_EnemyPressKiller = 0xEE,
	NPC_CageBars = 0xEF,
	NPC_MimigaJailed = 0xF0,
	NPC_EnemyCritterHoppingRed = 0xF1,
	NPC_EnemyBatRed = 0xF2,
	NPC_GeneratorBatRed = 0xF3,
	NPC_AcidDrop = 0xF4,
	NPC_GeneratorAcidDrop = 0xF5,
	NPC_EnemyPressProximity = 0xF6,
	NPC_BossMisery = 0xF7,
	NPC_BossMiseryVanish = 0xF8,
	NPC_ProjectileMiseryEnergyShot = 0xF9,
	NPC_ProjectileMiseryLightningBall = 0xFA,
	NPC_ProjectileMiseryLightning = 0xFB,
	NPC_ProjectileMiseryBlackOrbitingRings = 0xFC,
	NPC_EnergyCapsule = 0xFD,
	NPC_Helicopter = 0xFE,
	NPC_HelicopterBlades = 0xFF,
	NPC_DoctorCrownedFacingAway = 0x100,
	NPC_RedCrystal = 0x101,
	NPC_MimigaSleeping = 0x102,
	NPC_CurlyCarriedAwayUnconscious = 0x103,
	NPC_ShovelBrigadeCaged = 0x104,
	NPC_ChieCaged = 0x105,
	NPC_ChacoCaged = 0x106,
	NPC_BossDoctor = 0x107,
	NPC_ProjectileDoctorRedWave = 0x108,
	NPC_ProjectileDoctorRedBallFastVanish = 0x109,
	NPC_ProjectileDoctorRedBallSlowVanish = 0x10A,
	NPC_BossMuscleDoctor = 0x10B,
	NPC_EnemyIgor = 0x10C,
	NPC_EnemyBatRedEnergy = 0x10D,
	NPC_RedEnergy = 0x10E,
	NPC_UnderwaterBlock = 0x10F,
	NPC_GeneratorUnderwaterBlock = 0x110,
	NPC_ProjectileDroll = 0x111,
	NPC_EnemyDroll = 0x112,
	NPC_PuppyWithItems = 0x113,
	NPC_BossRedDemon = 0x114,
	NPC_ProjectileRedDemon = 0x115,
	NPC_LittleFamily = 0x116,
	NPC_FallingBlockLarge = 0x117,
	NPC_SueTeleportedInByMisery = 0x118,
	NPC_DoctorRedEnergyForm = 0x119,
	NPC_MiniUndeadCoreFloatsForward = 0x11A,
	NPC_EnemyMiseryTransformed = 0x11B,
	NPC_EnemySueTransformed = 0x11C,
	NPC_ProjectileUndeadCoreOrangeSpiralShot = 0x11D,
	NPC_OrangeDot = 0x11E,
	NPC_OrangeSmoke = 0x11F,
	NPC_ProjectileUndeadCoreGlowingRockThing = 0x120,
	NPC_EnemyCritterHoppingOrange = 0x121,
	NPC_EnemyBatOrange = 0x122,
	NPC_MiniUndeadCoreStationary = 0x123,
	NPC_Quake = 0x124,
	NPC_ProjectileUndeadCoreLargeEnergyBall = 0x125,
	NPC_QuakeGeneratorFallingBlocks = 0x126,
	NPC_Cloud = 0x127,
	NPC_GeneratorCloud = 0x128,
	NPC_SueOnSkyDragon = 0x129,
	NPC_DoctorWithoutCrown = 0x12A,
	NPC_BalrogMiseryInBubble = 0x12B,
	NPC_DemonCrown = 0x12C,
	NPC_EnemyFishMissileOrange = 0x12D,
	NPC_CameraFocusMarker = 0x12E,
	NPC_CurlysMachineGun = 0x12F,
	NPC_GaudiSitting = 0x130,
	NPC_PuppySmall = 0x131,
	NPC_BalrogNurse = 0x132,
	NPC_SantaCaged = 0x133,
	NPC_EnemyStumpy = 0x134,
	NPC_EnemyBute = 0x135,
	NPC_EnemyButeSword = 0x136,
	NPC_EnemyButeArcher = 0x137,
	NPC_ProjectileButeArcher = 0x138,
	NPC_BossMaPignon = 0x139,
	NPC_FallingIndestructible = 0x13A,
	NPC_EnemyHoppingDisappears = 0x13B,
	NPC_EnemyButeDefeated = 0x13C,
	NPC_EnemyMesa = 0x13D,
	NPC_EnemyMesaDefeated = 0x13E,
	NPC_ProjectileMesaBlock = 0x13F,
	NPC_CurlyCarriedShooting = 0x140,
	NPC_ProjectileCurlyNemesisBulletSpawner = 0x141,
	NPC_EnemyDeleet = 0x142,
	NPC_EnemyButeGenerator = 0x143,
	NPC_GeneratorBute = 0x144,
	NPC_ProjectileHeavyPress = 0x145,
	NPC_ItohSueTurningHuman = 0x146,
	NPC_ItohSueAhChoo = 0x147,
	NPC_Transmogrifier = 0x148,
	NPC_BuildingFan = 0x149,
	NPC_EnemyRolling = 0x14A,
	NPC_ProjectileBallosBone = 0x14B,
	NPC_ProjectileBallosShockwave = 0x14C,
	NPC_ProjectileBallosLightning = 0x14D,
	NPC_Sweat = 0x14E,
	NPC_Ikachan = 0x14F,
	NPC_GeneratorIkachan = 0x150,
	NPC_Numahachi = 0x151,
	NPC_EnemyGreenDevil = 0x152,
	NPC_GeneratorGreenDevil = 0x153,
	NPC_BossBallos = 0x154,
	NPC_Ballos1Head = 0x155,
	NPC_EnemyBallos3Eyeball = 0x156,
	NPC_Ballos2Cutscene = 0x157,
	NPC_Ballos2Eyes = 0x158,
	NPC_ProjectileBallosSkull = 0x159,
	NPC_Ballos4OrbitingPlatform = 0x15A,
	NPC_EnemyHoppy = 0x15B,
	NPC_BallosSpikesRising = 0x15C,
	NPC_Statue = 0x15D,
	NPC_EnemyButeArcherRed = 0x15E,
	NPC_StatueCanShoot = 0x15F,
	NPC_KingSword = 0x160,
	NPC_EnemyButeSwordRed = 0x161,
	NPC_InvisibleDeathTrapWall = 0x162,
	NPC_BalrogCrashingThroughWall = 0x163,
	NPC_BalrogRescue = 0x164,
	NPC_PuppyGhost = 0x165,
	NPC_MiseryWind = 0x166,
	NPC_GeneratorWaterDrop = 0x167,
	NPC_ThankYou = 0x168,
};
