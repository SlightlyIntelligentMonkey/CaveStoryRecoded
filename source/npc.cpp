#include "npc.h"
#include "npcAct.h"

BYTE *npcAttributes = nullptr;
char npcTblPath[256] = "data/npc.tbl";

int npcTblSize = loadFile(npcTblPath, &npcAttributes);

int npcAmmount = npcTblSize / 24;

int getNPCAttribute(UINT npcId, UINT offset, UINT offset2, UINT offsetSize, UINT returnSize)
{
	int absOffset = (offset * npcAmmount) + (offsetSize * npcId) + offset2;

	switch (returnSize) {
		case(1):
			return npcAttributes[absOffset];
			break;

		case(2):
			return readLEshort(npcAttributes, absOffset);
			break;

		case(4):
			return readLElong(npcAttributes, absOffset);
			break;

		default:
			doCustomError("INVALID getNPCAttribute RETURNSIZE VALUE");
			return -1;
			break;
	}
}

void npc::init(int spawnX, int spawnY, short flagId, short eventId, short npcType, short npcFlags)
{
	x = spawnX;
	y = spawnY;
	flag = flagId;
	event = eventId;
	type = npcType;
	flags = getNPCAttribute(npcType, 0, 0, 2, 2) | npcFlags;

	cond = npccond_alive;

	collideRect.left = getNPCAttribute(npcType, 16, 0, 4, 1) << 9;
	collideRect.top = getNPCAttribute(npcType, 16, 1, 4, 1) << 9;
	collideRect.right = getNPCAttribute(npcType, 16, 2, 4, 1) << 9;
	collideRect.bottom = getNPCAttribute(npcType, 16, 3, 4, 1) << 9;

	frameRect = { 0, 0, 0, 0 };

	offset.x = getNPCAttribute(npcType, 20, 0, 4, 1) << 9;
	offset.y = getNPCAttribute(npcType, 20, 1, 4, 1) << 9;

	spriteSheet = getNPCAttribute(npcType, 4, 0, 1, 1);

	targetX = 0;
	targetY = 0;

	xsp = 0;
	ysp = 0;

	action = 0;
	actionWait = 0;

	animation = 0;
	animationWait = 0;

	hit = 0;

	direction = 0;

	shock = 0;

	count1 = 0;

	return;
}

void npc::update()
{
	if (cond & npccond_alive)
	{
		if (npcActs[type])
		{
			npcActs[type](this);

			if (shock) { --shock; }
		}

		if (!(flags & npc_ignoresolid)) {
			bool wasGround = (hit & ground);
			hit = npcHitMap(&collideRect, &x, &y, true, wasGround);

			if (hit & ceiling && ysp < 0)
				ysp = 0;

			if (hit & ground && ysp > 0)
				ysp = 0;
		}
	}
}

void npc::draw()
{
	if (cond & npccond_alive)
	{
		ImageRect = { frameRect.left, frameRect.top, frameRect.right - frameRect.left, frameRect.bottom - frameRect.top };

		drawTexture(sprites[spriteSheet], x - offset.x, y - offset.y, false);

		/*/ COLLISION DEBUG
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

		drawRect((x - collideRect.left - viewX) >> 9, (y - collideRect.top - viewY) >> 9, (collideRect.left + collideRect.right) >> 9, (collideRect.top + collideRect.bottom) >> 9);
		*/
	}
}