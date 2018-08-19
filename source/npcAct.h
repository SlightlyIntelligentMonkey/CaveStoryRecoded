#pragma once
#include "common.h"

#include "npc.h"

using npcAct = void(*)(npc *);
extern npcAct npcActs[361];
