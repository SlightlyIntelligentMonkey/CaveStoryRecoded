#pragma once
#include "npc.h"

struct BOSS : public npc
{
	npc bossObj[20];
};

extern npc bossObj[20];

void actBoss_0(BOSS *boss);
//void actBoss_Omega();
//void actBoss_Frog();
//void actBoss_MonstX();
//void actBoss_Core();
//void actBoss_Ironhead();
//void actBoss_Twin();
//void actBoss_Undead();
//void actBoss_Press();
//void actBoss_Ballos();