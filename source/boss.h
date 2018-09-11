#pragma once
#include "npc.h"

#define BOSSNPCS 20
extern npc bossObj[BOSSNPCS];

void initBoss(int code);

void setBossAction(int a);

void updateBoss();
void drawBoss();

//void actBoss_Omega();
void actBoss_Frog(npc *boss);
//void actBoss_MonstX();
//void actBoss_Core();
//void actBoss_Ironhead();
//void actBoss_Twin();
//void actBoss_Undead();
//void actBoss_Press();
//void actBoss_Ballos();
