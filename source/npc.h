#pragma once
#include "common.h"
#include "npcCollision.h"
#include "level.h"

struct NPC_RECT
{
	Uint8 front;
	Uint8 top;
	Uint8 back;
	Uint8 bottom;
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
		BYTE cond;
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

		//stuff?
		int surf;
		int hit_voice;
		int destroy_voice;

		//life stuff
		int life;
		int exp;
		int size;

		//what do i look like and bits
		int direct;
		unsigned __int16 bits;
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

		//damage
		uint8_t shock;
		int damage_view;
		int damage;

		//parent
		npc *pNpc;

	private:

	//Functions
	public:
		void init(int setCode, int setX, int setY, int setXm, int setYm, int setDir, npc *parentNpc);

		void update();
		void draw();
};

void loadNpcTable();

void createNpc(int setCode, int setX, int setY, int setXm, int setYm, int setDir, npc *parentNpc);

void updateNPC();
void drawNPC();

extern std::vector<npc> npcs;