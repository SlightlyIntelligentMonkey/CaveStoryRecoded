#include "npc.h"
#include "npcAct.h"
#include "weapons.h"
#include "mathUtils.h"
#include "player.h"
#include "valueview.h"
#include "render.h"
#include "sound.h"
#include "flags.h"
#include "game.h"
#include "caret.h"

#include <deque>
#include <vector>
#include <string>
#include <cstring>
#include <SDL_rwops.h>

using std::memset;
using std::string;
using std::to_string;
using std::vector;
using std::deque;

deque<npc> npcs(0);

int superXPos = 0;	// Used by undead core related NPCs ?
int superYPos = 0;

int curlyShootWait = 0;
int curlyShootX = 0;
int curlyShootY = 0;

//NPC Functions
void createSmokeLeft(int x, int y, int w, size_t num)
{
	const int wa = w / 0x200;

	for (size_t i = 0; i < num; i++)
	{
		const int offsetX = random(-wa, wa) << 9;
		const int offsetY = random(-wa, wa) << 9;

		createNpc(NPC_Smoke, x + offsetX, offsetY + y, 0, 0, dirLeft);
	}
	createCaret(x, y, effect_BigExplosion);
}

void createSmokeUp(int x, int y, int w, int num)
{
	const int wa = w / 512;

	for (int i = 0; i < num; ++i)
	{
		const int offset_x = random(-wa, wa) << 9;
		const int offset_y = random(-wa, wa) << 9;

		createNpc(NPC_Smoke, x + offset_x, offset_y + y, 0, 0, dirUp);
	}

	createCaret(x, y, effect_BigExplosion);
}

void createNpc(int setCode, int setX, int setY, int setXm, int setYm, int setDir, npc *parentNpc)
{
	size_t n;
	for (n = 0; ; ++n)
	{
		if (!(n < npcs.size() && npcs[n].cond))
			break;
	}

	if (n != npcs.size())
	{
		npcs[n].init(setCode, setX, setY, setXm, setYm, setDir, parentNpc);
	}
	else
	{
		npc newNpc;
		newNpc.init(setCode, setX, setY, setXm, setYm, setDir, parentNpc);
		npcs.push_back(newNpc);
	}
}

void createNpcExp(int setCode, int setX, int setY, int setXm, int setYm, int setDir, npc *parentNpc, int setEvent, int exp)
{
	size_t n;
	for (n = 0; ; ++n)
	{
		if (!(n < npcs.size() && npcs[n].cond))
			break;
	}

	if (n != npcs.size())
	{
		npcs[n].init(setCode, setX, setY, setXm, setYm, setDir, parentNpc);
		npcs[n].code_event = setEvent;
		npcs[n].exp = exp;
	}
	else
	{
		npc newNpc;
		newNpc.init(setCode, setX, setY, setXm, setYm, setDir, parentNpc);
		newNpc.code_event = setEvent;
		newNpc.exp = exp;
		npcs.push_back(newNpc);
	}
}

void changeNpc(int code_event, int code_char, int dir)
{
	for (size_t i = 0; i < npcs.size(); ++i)
	{
		if (npcs[i].cond & npccond_alive && npcs[i].code_event == code_event)
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
					npcs[i].facePlayer();
				else
					npcs[i].direct = dir;
			}

			npcActs[code_char](&npcs[i]);
		}
	}
}

int findEntityByType(int entityType)
{
    for (size_t i = 0; i < npcs.size(); ++i)
        if (npcs[i].cond & npccond_alive && npcs[i].code_char == entityType)
            return i;
    return -1;
}

void updateNPC()
{
	if (npcs.size())
	{
		//Update
		for (size_t i = 0; i < npcs.size(); i++)
		{
			if (npcs[i].cond & npccond_alive)
			{
				npcs[i].update();
				npcHitMap(i);
			}
		}

		while (npcs.size() && !(npcs[npcs.size() - 1].cond & npccond_alive))
			npcs.erase(npcs.begin() + npcs.size() - 1);
	}
}

void drawNPC()
{
	if (npcs.size())
	{
		for (size_t i = 0; i < npcs.size(); i++)
		{
			if (npcs[i].cond & npccond_alive)
				npcs[i].draw();
		}
	}
}

//Npc death things
void dropExperience(int x, int y, int exp)
{
	while (exp > 0)
	{
		int effectiveExp = 0;

		if (exp <= 19)
		{
			if (exp <= 4)
			{
				--exp;
				effectiveExp = 1;
			}
			else
			{
				exp -= 5;
				effectiveExp = 5;
			}
		}
		else
		{
			exp -= 20;
			effectiveExp = 20;
		}

		createNpcExp(1, x, y, 0, 0, 0, nullptr, 0, effectiveExp);
	}
}

int dropMissiles(int x, int y, int val)
{
	int tamakazu_ari[10];
	int t = 0;
	memset(tamakazu_ari, 0, 0x28u);

	int n;
	for (n = 0; n <= 7; ++n)
	{
		if (weapons[n].code == 5)
			tamakazu_ari[t++] = 0;
		else if (weapons[n].code == 10)
			tamakazu_ari[t++] = 1;
		else
			tamakazu_ari[t] = 0;
	}

	if (!t)
		return 0;

	n = random(1, 10 * t);
	const int bullet_no = tamakazu_ari[n % t];

	createNpcExp(86, x, y, 0, 0, 0, nullptr, bullet_no, val);

	return 1;
}

void killNpc(npc *NPC, bool bVanish)
{
	if (NPC == nullptr)
		doCustomError("NPC was nullptr in killNpc");

	const int x = NPC->x;
	const int y = NPC->y;
	const int flag = NPC->code_flag;
	const int voice = NPC->destroy_voice;
	const int size = NPC->size;
	const int explodeWidth = NPC->view.right;
	const int exp = NPC->exp;

	//Destroy npc
	if (!(NPC->bits & npc_showDamage))
	{
		NPC->cond = 0;
	}
	else
	{
		if (NPC->damage_view)
			createValueView(&NPC->x, &NPC->y, NPC->damage_view);

		if (bVanish)
			NPC->init(NPC_DeletesItself, NPC->x, NPC->y, 0, 0, 0, nullptr);
	}

	//Play sound
	playSound(voice);

	//Create explosion smoke
	switch (size)
	{
	case 1:
		createSmokeLeft(x, y, explodeWidth, 3);
		break;
	case 2:
		createSmokeLeft(x, y, explodeWidth, 7);
		break;
	case 3:
		createSmokeLeft(x, y, explodeWidth, 12);
		break;
	}

	//Drop experience, health, and missiles
	if (exp)
	{
		const int drop = random(1, 5);
		int missileDrop = 0;

		if (drop == 1) //Health drop
		{
			if (NPC->exp <= 6)
				createNpcExp(87, x, y, 0, 0, 0, nullptr, 0, 2);
			else
				createNpcExp(87, x, y, 0, 0, 0, nullptr, 0, 6);
		}
		else
		{
			if (drop != 2) //Drop experience
			{
				dropExperience(x, y, exp);
			}
			else //Drop missile, if doesn't drop, drop experience instead.
			{
				if (NPC->exp <= 6)
					missileDrop = dropMissiles(x, y, 1);
				else
					missileDrop = dropMissiles(x, y, 3);

				if (!missileDrop)
					dropExperience(x, y, exp);
			}
		}
	}

	setFlag(flag);
}

//NPC Table
NPC_TABLE *npcTable;

void loadNpcTable()
{
	SDL_RWops *tblStream = SDL_RWFromFile("data/npc.tbl", "rb");

	if (tblStream == nullptr)
		doError();

	const auto tblSize = static_cast<int>(SDL_RWsize(tblStream));

	const int npcCount = tblSize / 0x18;
	npcTable = new NPC_TABLE[npcCount];

	int i;

	for (i = 0; i < npcCount; ++i) //bits
		npcTable[i].bits = SDL_ReadLE16(tblStream);
	for (i = 0; i < npcCount; ++i) //life
		npcTable[i].life = SDL_ReadLE16(tblStream);
	for (i = 0; i < npcCount; ++i) //surf
		tblStream->read(tblStream, &npcTable[i].surf, 1, 1);
	for (i = 0; i < npcCount; ++i) //destroy_voice
		tblStream->read(tblStream, &npcTable[i].destroy_voice, 1, 1);
	for (i = 0; i < npcCount; ++i) //hit_voice
		tblStream->read(tblStream, &npcTable[i].hit_voice, 1, 1);
	for (i = 0; i < npcCount; ++i) //size
		tblStream->read(tblStream, &npcTable[i].size, 1, 1);
	for (i = 0; i < npcCount; ++i) //exp
		npcTable[i].exp = SDL_ReadLE32(tblStream);
	for (i = 0; i < npcCount; ++i) //damage
		npcTable[i].damage = SDL_ReadLE32(tblStream);
	for (i = 0; i < npcCount; ++i) //hit
		tblStream->read(tblStream, &npcTable[i].hit, 4, 1);
	for (i = 0; i < npcCount; ++i) //view
		tblStream->read(tblStream, &npcTable[i].view, 4, 1);
}

void npc::accelerateTowardsPlayer(int vel)
{
    if (this->direct != dirLeft)
        this->xm += 0x20;
    else
        this->xm -= 0x20;
}

void npc::animate(int aniWait, int aniStart, int aniMax)
{
	{
		if (++this->ani_wait > aniWait)
		{
			this->ani_wait = 0;
			++this->ani_no;
		}
		if (this->ani_no > aniMax)
			this->ani_no = aniStart;
	}
}

void npc::doGravity(int gravity, int maxYVel)
{
	this->ym += gravity;
	if (this->ym > maxYVel)
		this->ym = maxYVel;
}

void npc::doRects(const vector<RECT>& rcLeft, const vector<RECT>& rcRight)
{
	if (!rcRight.empty())
	{
		if (this->direct != dirLeft)
			this->rect = rcRight.at(this->ani_no);
		else
			this->rect = rcLeft.at(this->ani_no);
	}
	else
		this->rect = rcLeft.at(this->ani_no);
}

void npc::facePlayer()
{
	if (currentPlayer.x >= this->x)
		this->direct = dirRight;
	else
		this->direct = dirLeft;
}

void npc::moveInDir(int vel)
{
    if (this->direct != dirLeft)
        this->xm = vel;
    else
        this->xm = -vel;
}

void npc::moveTowardsPlayer(int vel)
{
	if (this->direct != dirLeft)
		this->xm = vel;
	else
		this->xm = -vel;
}

bool npc::isPlayerWithinDistance(int xDist, int yDistHigh, int yDistLow)
{
	return (this->x - xDist < currentPlayer.x
		&& this->x + xDist > currentPlayer.x
		&& this->y - yDistHigh < currentPlayer.y
		&& this->y + yDistLow > currentPlayer.y);
}

bool npc::isPlayerAligned(int xRay, int yRayHigh, int yRayLow)
{
	return (this->x - xRay >= currentPlayer.x
		|| this->x + xRay <= currentPlayer.x
		|| this->y - yRayHigh >= currentPlayer.y
		|| this->y + yRayLow <= currentPlayer.y);
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

	if (shock > 0)
		--shock;
}

void npc::draw()
{
	if (cond & npccond_alive)
	{
		int xOffset = 0;

		if (shock)
		{
			xOffset = 2 * ((shock >> 1) & 1) - 1;
		}
		else
		{
			if ((bits & npc_showDamage) != 0 && damage_view)
			{
				createValueView(&x, &y, damage_view);
				damage_view = 0;
			}
		}

		int side = view.left;

		if (direct != dirLeft)
			side = view.right;

		drawTexture(sprites[surf], &rect, (x - side) / 0x200 - viewport.x / 0x200 + xOffset, (y - view.top) / 0x200 - viewport.y / 0x200);

		if (debugFlags & showNPCId)
		{
			size_t index = 0;

			for (size_t i = 0; i < npcs.size(); i++)
			{
				if (&npcs[i] == this)
				{
					index = i;
					break;
				}
			}

			drawString((x - side) / 0x200 - viewport.x / 0x200 + xOffset, (y - view.top) / 0x200 - viewport.y / 0x200 - 16, to_string(index).c_str());
		}

		if (debugFlags & showNPCHealth && life)
		{
			RECT rcPer = { 72, 48, 80, 56 };

			drawNumber(life, (x - side) / 0x200 - viewport.x / 0x200 + xOffset, (y - view.top) / 0x200 - viewport.y / 0x200 - 24, true);
			drawTexture(sprites[TEX_TEXTBOX], &rcPer, (x - side) / 0x200 - viewport.x / 0x200 + xOffset + 32, (y - view.top) / 0x200 - viewport.y / 0x200 - 24);
			drawNumber(npcTable[code_char].life, (x - side) / 0x200 - viewport.x / 0x200 + xOffset + 40, (y - view.top) / 0x200 - viewport.y / 0x200 - 24, true);
		}
	}
}
