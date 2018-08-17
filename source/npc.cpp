#include "npc.h"
#include "npcAct.h"

std::vector<npc> npcs(0);

int superXPos = 0;	// Used by undead core related NPCs ?
int superYPos = 0;

//NPC Functions
void createSmoke(int x, int y, int w, int num)
{
	const int wa = w / 0x200;

	for (int i = 0; i < num; i++)
	{
		const int offsetX = random(-wa, wa) << 9;
		const int offsetY = random(-wa, wa) << 9;

		createNpc(4, x + offsetX, offsetY + y, 0, 0, 0, nullptr);
	}
}

void createNpc(int setCode, int setX, int setY, int setXm, int setYm, int setDir, npc *parentNpc)
{
	npc *repNpc = nullptr;

	if (npcs.size())
	{
		for (size_t i = 0; i < npcs.size(); ++i)
		{
			if (!(npcs[i].cond & npccond_alive))
			{
				repNpc = &npcs[i];
				break;
			}
		}
	}

	if (repNpc != nullptr)
		repNpc->init(setCode, setX, setY, setXm, setYm, setDir, parentNpc);
	else
	{
		npc newNpc;
		newNpc.init(setCode, setX, setY, setXm, setYm, setDir, parentNpc);
		npcs.push_back(newNpc);
	}
}

void changeNpc(int code_event, int code_char, int dir)
{
	for (size_t i = 0; i < npcs.size(); ++i)
	{
		if (npcs[i].cond & npccond_alive)
		{
			if (npcs[i].code_event == code_event)
			{
				const int code_flag = npcs[i].code_flag;
				int bits = npcs[i].bits;

				bits &= 0x7F00; //Only bits that seem to determine interactability and other stuff

				npcs[i].init(code_char, npcs[i].x, npcs[i].y, 0, 0, npcs[i].direct, npcs[i].pNpc);
				npcs[i].bits |= bits;
				npcs[i].code_flag = code_flag;
				npcs[i].code_event = code_event;

				if (dir != 5)
				{
					if (dir == 4)
					{
						if (npcs[i].x >= currentPlayer.x)
							npcs[i].direct = 0;
						else
							npcs[i].direct = 2;
					}
					else
					{
						npcs[i].direct = dir;
					}
				}

				npcActs[code_char](&npcs[i]);
			}
		}
	}
}

void updateNPC()
{
	if (npcs.size())
	{
		//Update
		for (size_t i = 0; i < npcs.size(); i++) {
			if (npcs[i].cond & npccond_alive)
			{
				npcs[i].update();
				npcHitMap(i);
			}
		}
	}
}

void drawNPC()
{
	if (npcs.size())
	{
		for (size_t i = 0; i < npcs.size(); i++) {
			if (npcs[i].cond & npccond_alive)
				npcs[i].draw();
		}
	}
}

//NPC Table
NPC_TABLE *npcTable;

void loadNpcTable()
{
	SDL_RWops *tblStream = SDL_RWFromFile("data/npc.tbl", "rb");

	if (tblStream == nullptr)
		doError();

	const auto tblSize = static_cast<int>(SDL_RWsize(tblStream));

	const int npcs = tblSize / 0x18;
	npcTable = static_cast<NPC_TABLE *>(malloc(0x18 * npcs));

	if (npcTable == nullptr)
		doCustomError("Could not allocate memory for NPC table");
	
	int i;

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

		drawTexture(sprites[surf], &rect, (x - side) / 0x200 - viewport.x / 0x200, (y - view.top) / 0x200 - viewport.y / 0x200);

		/*if (bits & npc_interact)
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		else if (damage > 0)
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		else
			SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);

		drawRect(((x - hit.left) / 0x200) - (viewport.x / 0x200), ((y - hit.top) / 0x200) - (viewport.y / 0x200), (hit.left + hit.right) / 0x200, (hit.top + hit.bottom) / 0x200);*/
	}
}
