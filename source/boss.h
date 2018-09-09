#pragma once
#include "npc.h"
#define BOSSNPCS 20

extern npc boss[BOSSNPCS];

void initBoss(int code);

void updateBoss();
void drawBoss();
