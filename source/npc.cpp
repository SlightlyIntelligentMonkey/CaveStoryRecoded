#include "npc.h"
#include "npcAct.h"

std::vector<npc> npcs(0);

NPC_TABLE *npcTable;

//NPC Functions
void createNpc(int setCode, int setX, int setY, int setXm, int setYm, int setDir, npc *parentNpc)
{
	npc newNPC;
	newNPC.init(setCode, setX, setY, setXm, setYm, setDir, parentNpc);

	npcs.push_back(newNPC);
}

void updateNPC()
{
	if (npcs.size())
	{
		//Remove stuff
		for (size_t i = npcs.size() - 1; i > 0; i--) {
			if (!(npcs[i].cond & npccond_alive))
				npcs.erase(npcs.begin() + i);
		}

		//Update
		for (unsigned int i = 0; i < npcs.size(); i++) {
			npcs[i].update();
			npcHitMap(i);
		}
	}
}

void drawNPC()
{
	if (npcs.size())
	{
		for (unsigned int i = 0; i < npcs.size(); i++)
		{
			if (npcs[i].cond & npccond_alive)
				npcs[i].draw();
		}
	}
}

void loadNpcTable()
{
	npcTable = nullptr;

	BYTE *tbl = nullptr;
	int tblSize = loadFile("data/npc.tbl", &tbl);
	if (tblSize < 0)
		doCustomError("Couldn't load npc table.");

	free(tbl);

	int npcs = tblSize / 0x18;
	npcTable = (NPC_TABLE *)malloc(0x18 * npcs);
	if (!npcTable)
		doCustomError("Couldn't malloc npcTable");

	SDL_RWops *tblStream = SDL_RWFromFile("data/npc.tbl", "rb");

	int i;

	if (tblStream)
	{
		for (i = 0; i < npcs; ++i) //bits
			npcTable[i].bits = SDL_ReadLE16(tblStream);
		for (i = 0; i < npcs; ++i) //life
			npcTable[i].life = SDL_ReadLE16(tblStream);
		for (i = 0; i < npcs; ++i) //surf
			tblStream->read(tblStream, &npcTable[i].surf, 1, 1);
		for (i = 0; i < npcs; ++i) //destroy_voice
			tblStream->read(tblStream, &npcTable[i].destroy_voice, 1, 1);
		for (i = 0; i < npcs; ++i) //hit_voice
			tblStream->read(tblStream, &npcTable[i].hit_voice, 1, 1);
		for (i = 0; i < npcs; ++i) //size
			tblStream->read(tblStream, &npcTable[i].size, 1, 1);
		for (i = 0; i < npcs; ++i) //exp
			npcTable[i].exp = SDL_ReadLE32(tblStream);
		for (i = 0; i < npcs; ++i) //damage
			npcTable[i].damage = SDL_ReadLE32(tblStream);
		for (i = 0; i < npcs; ++i) //hit
			tblStream->read(tblStream, &npcTable[i].hit, 4, 1);
		for (i = 0; i < npcs; ++i) //view
			tblStream->read(tblStream, &npcTable[i].view, 4, 1);
	}
	else
		doCustomError("Couldn't load npc table.");
}

void npc::init(int setCode, int setX, int setY, int setXm, int setYm, int setDir, npc *parentNpc)
{
	memset(this, 0, sizeof(*this));

	cond = npccond_alive;
	code_char = setCode;

	x = setX;
	y = setY;
	xm = setXm;
	ym = setYm;
	direct = setDir;

	pNpc = parentNpc;
	bits = npcTable[code_char].bits;
	exp = npcTable[code_char].exp;
	
	surf = npcTable[code_char].surf;
	hit_voice = npcTable[code_char].hit_voice;
	destroy_voice = npcTable[code_char].destroy_voice;

	damage = npcTable[code_char].damage;
	size = npcTable[code_char].size;
	life = npcTable[code_char].life;

	hit.left = npcTable[code_char].hit.front << 9;
	hit.right = npcTable[code_char].hit.back << 9;
	hit.top = npcTable[code_char].hit.top << 9;
	hit.bottom = npcTable[code_char].hit.bottom << 9;

	view.left = npcTable[code_char].view.front << 9;
	view.right = npcTable[code_char].view.back << 9;
	view.top = npcTable[code_char].view.top << 9;
	view.bottom = npcTable[code_char].view.bottom << 9;
}

void npc::update()
{
	npcActs[code_char](this);

	if (shock) { --shock; }
}

void npc::draw()
{
	if (cond & npccond_alive)
	{
		int side = view.left;

		if (direct)
			side = view.right;

		drawTexture(sprites[surf], &rect, (x - side) / 0x200 - viewX / 0x200, (y - view.top) / 0x200 - viewY / 0x200);

		/*if (bits & npc_interact)
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		else if (damage > 0)
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		else
			SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);

		drawRect(((x - hit.left) / 0x200) - (viewX / 0x200), ((y - hit.top) / 0x200) - (viewY / 0x200), (hit.left + hit.right) / 0x200, (hit.top + hit.bottom) / 0x200);*/
	}
}
