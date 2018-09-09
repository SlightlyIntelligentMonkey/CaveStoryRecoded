#include "mapBoss.h"

npc boss;

typedef void(*BOSSFUNCTION)(void);
BOSSFUNCTION gpBossFuncTbl[10] =
{
  &ActBossChar_0//,
  //&ActBossChar_Omega,
  //&ActBossChar_Frog,
  //&ActBossChar_MonstX,
  //&ActBossChar_Core,
  //&ActBossChar_Ironhead,
  //&ActBossChar_Twin,
  //&ActBossChar_Undead,
  //&ActBossChar_Press,
  //&ActBossChar_Ballos
}; // idb

void ActBossChar_0() 
{
	return; 
}