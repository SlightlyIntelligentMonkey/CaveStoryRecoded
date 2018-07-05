#pragma once
#include "common.h"

extern BYTE *npcAttributes;

class npc
{
	//Variables
	public:
		int x;
		int y;
		short flag;
		short event;
		short type;
		short flags;

		int targetX;
		int targetY;

		int xsp;
		int ysp;

		int action;
		int actionWait;

		BYTE spriteSheet;

		int animation;
		int animationWait;

		short hit;

		int direction;

		UINT damage;

		int shock;

		int count1;

		int cond;

		RECT frameRect;

		RECT collideRect;

		POINT offset;

	private:

	//Functions
	public:
		void init(int spawnX, int spawnY, short flagId, short eventId, short npcType, short npcFlags);

		void update();
		void draw();
};

extern std::vector<npc> npcs;