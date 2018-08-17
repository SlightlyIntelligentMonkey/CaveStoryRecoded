#include "bulletCollision.h"

void bulletVanish(bullet *bul)
{
	if (bul->code_bullet != 37 && bul->code_bullet != 38 && bul->code_bullet != 39)
		playSound(28);
	else
		createCaret(bul->x, bul->y, 2, 1);

	bul->cond = 0;
	createCaret(bul->x, bul->y, 2, 2);
}

int bulletJudgeBlock(int x, int y, bullet *bul) //For judging breakable blocks
{
	int hit = 0;

	if (bul->x - bul->blockXL < (2 * x + 1) << 12
		&& bul->blockXL + bul->x > (2 * x - 1) << 12
		&& bul->y - bul->blockYL < (2 * y + 1) << 12
		&& bul->blockYL + bul->y > (2 * y - 1) << 12)
	{
		hit = 0x200;
	}

	//Destroy breakable blocks
	if (hit && bul->bbits & (bullet_breakBlocks | bullet_pierceBlocks) && getTileAttribute(x, y) == 0x43)
	{
		if (!(bul->bbits & bullet_pierceBlocks))
			bul->cond = 0;

		createCaret(bul->x, bul->y, 2, 0);
		playSound(12);

		for (int i = 0; i < 4; ++i)
			createNpc(4, x << 13, y << 13, random(-512, 512), random(-512, 512), 0, nullptr);

		shiftTile(x, y);
	}

	return hit;
}

int bulletJudgeBlock2(int x, int y, bullet *bul) //For judging actual solid blocks
{
	int block[4];
	int workX;
	int workY;
	int hit = 0;

	if (bul->bbits & 0x40)
	{
		block[0] = getTileAttribute(x, y);
		block[1] = getTileAttribute(x + 1, y);
		block[2] = getTileAttribute(x, y + 1);
		block[3] = getTileAttribute(x + 1, y + 1);

		for (int i = 0; i < 4; i++)
			block[i] = block[i] == 0x41 || block[i] == 0x61;
	}
	else
	{
		block[0] = getTileAttribute(x, y);
		block[1] = getTileAttribute(x + 1, y);
		block[2] = getTileAttribute(x, y + 1);
		block[3] = getTileAttribute(x + 1, y + 1);

		for (int i = 0; i < 4; i++)
			block[i] = block[i] == 0x41 || block[i] == 0x43 || block[i] == 0x61;
	}

	workX = (2 * x + 1) << 12;
	workY = (2 * y + 1) << 12;

	if (block[0] && block[2])
	{
		if (bul->x - bul->blockXL < workX)
			hit |= 1u;
	}
	else if (!block[0] || block[2])
	{
		if (!block[0] && block[2] && bul->x - bul->blockXL < workX && bul->blockYL + bul->y > workY + 1536)
			hit |= 1u;
	}
	else if (bul->x - bul->blockXL < workX && bul->y - bul->blockYL < workY - 1536)
	{
		hit |= 1u;
	}

	if (block[1] && block[3])
	{
		if (bul->x + bul->blockXL > workX)
			hit |= 4u;
	}
	else if (!block[1] || block[3])
	{
		if (!block[1] && block[3] && bul->x + bul->blockXL > workX && bul->blockYL + bul->y > workY + 1536)
			hit |= 4u;
	}
	else if (bul->x + bul->blockXL > workX && bul->y - bul->blockYL < workY - 1536)
	{
		hit |= 4u;
	}

	if (block[0] && block[1])
	{
		if (bul->y - bul->blockYL < workY)
			hit |= 2u;
	}
	else if (!block[0] || block[1])
	{
		if (!block[0] && block[1] && bul->y - bul->blockYL < workY && bul->blockXL + bul->x > workX + 1536)
			hit |= 2u;
	}
	else if (bul->y - bul->blockYL < workY && bul->x - bul->blockXL < workX - 1536)
	{
		hit |= 2u;
	}

	if (block[2] && block[3])
	{
		if (bul->y + bul->blockYL > workY)
			hit |= 8u;
	}
	else if (!block[2] || block[3])
	{
		if (!block[2] && block[3] && bul->y + bul->blockYL > workY && bul->blockXL + bul->x > workX + 1536)
			hit |= 8u;
	}
	else if (bul->y + bul->blockYL > workY && bul->x - bul->blockXL < workX - 1536)
	{
		hit |= 8u;
	}

	if (bul->bbits & 8)
	{
		if (hit & 1)
		{
			bul->x = workX + bul->blockXL;
		}
		else if (hit & 4)
		{
			bul->x = workX - bul->blockXL;
		}
		else if (hit & 2)
		{
			bul->y = workY + bul->blockYL;
		}
		else if (hit & 8)
		{
			bul->y = workY - bul->blockYL;
		}
	}
	else if (hit & 15)
	{
		bulletVanish(bul);
	}

	return hit;
}

int bulletJudgeTriangleA(int x, int y, bullet *bul)
{
	int hit = 0;

	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y - 0x400 < (y << 13) - (-0x2000 * x + bul->x) / 2 + 0x800
		&& bul->y + 0x400 > (2 * y - 1) << 12)
	{
		if (bul->bbits & bullet_goThroughWalls)
			bul->y = (y << 13) - (-0x2000 * x + bul->x) / 2 + 0xC00;
		else
			bulletVanish(bul);

		hit = 0x82;
	}

	return hit;
}

int bulletJudgeTriangleB(int x, int y, bullet *bul)
{
	int hit = 0;

	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y - 0x400 < (y << 13) - (-0x2000 * x + bul->x) / 2 - 0x800
		&& bul->y + 0x400 > (2 * y - 1) << 12)
	{
		if (bul->bbits & bullet_goThroughWalls)
			bul->y = (y << 13) - (-0x2000 * x + bul->x) / 2 - 0x400;
		else
			bulletVanish(bul);

		hit = 0x82;
	}

	return hit;
}

int bulletJudgeTriangleC(int x, int y, bullet *bul)
{
	int hit = 0;

	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y - 0x400 < (y << 13) + (-0x2000 * x + bul->x) / 2 - 0x800
		&& bul->y + 0x400 > (2 * y - 1) << 12)
	{
		if (bul->bbits & bullet_goThroughWalls)
			bul->y = (y << 13) + (-8192 * x + bul->x) / 2 - 0x400;
		else
			bulletVanish(bul);

		hit = 0x42;
	}

	return hit;
}

int bulletJudgeTriangleD(int x, int y, bullet *bul)
{
	int hit = 0;

	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y - 0x400 < (y << 13) + (-0x2000 * x + bul->x) / 2 + 0x800
		&& bul->y + 0x400 > (2 * y - 1) << 12)
	{
		if (bul->bbits & bullet_goThroughWalls)
			bul->y = (y << 13) + (-8192 * x + bul->x) / 2 + 0xC00;
		else
			bulletVanish(bul);

		hit = 0x42;
	}

	return hit;
}

int bulletJudgeTriangleE(int x, int y, bullet *bul)
{
	int hit = 0;

	if (bul->x < (2 * x + 1) << 12
		&& bul->x - 0x200 >(2 * x - 1) << 12
		&& bul->y + 0x400 > (y << 13) + (-0x2000 * x + bul->x) / 2 - 0x800
		&& bul->y - 0x400 < (2 * y + 1) << 12)
	{
		if (bul->bbits & bullet_goThroughWalls)
			bul->y = (y << 13) + (-0x2000 * x + bul->x) / 2 - 0xC00;
		else
			bulletVanish(bul);

		hit = 0x28;
	}

	return hit;
}

int bulletJudgeTriangleF(int x, int y, bullet *bul)
{
	int hit = 0;

	if (bul->x < (2 * x + 1) << 12
		&& bul->x - 0x200 >(2 * x - 1) << 12
		&& bul->y + 0x400 > (y << 13) + (-0x2000 * x + bul->x) / 2 + 0x800
		&& bul->y - 0x400 < (2 * y + 1) << 12)
	{
		if (bul->bbits & bullet_goThroughWalls)
			bul->y = (y << 13) + (-0x2000 * x + bul->x) / 2 + 0x400;
		else
			bulletVanish(bul);

		hit = 0x28;
	}

	return hit;
}

int bulletJudgeTriangleG(int x, int y, bullet *bul)
{
	int hit = 0;

	if (bul->x < (2 * x + 1) << 12
		&& bul->x - 0x200 >(2 * x - 1) << 12
		&& bul->y + 0x400 > (y << 13) - (-0x2000 * x + bul->x) / 2 + 0x800
		&& bul->y - 0x400 < (2 * y + 1) << 12)
	{
		if (bul->bbits & bullet_goThroughWalls)
			bul->y = (y << 13) - (-0x2000 * x + bul->x) / 2 + 0x400;
		else
			bulletVanish(bul);

		hit = 0x18;
	}

	return hit;
}

int bulletJudgeTriangleH(int x, int y, bullet *bul)
{
	int hit = 0;

	if (bul->x < (2 * x + 1) << 12
		&& bul->x - 0x200 >(2 * x - 1) << 12
		&& bul->y + 0x400 > (y << 13) - (-0x2000 * x + bul->x) / 2 - 0x800
		&& bul->y - 0x400 < (2 * y + 1) << 12)
	{
		if (bul->bbits & bullet_goThroughWalls)
			bul->y = (y << 13) - (-0x2000 * x + bul->x) / 2 - 0xC00;
		else
			bulletVanish(bul);

		hit = 0x18;
	}

	return hit;
}

//Main functions
void bulletHitMap()
{
	for (size_t i = 0; i < bullets.size(); i++)
	{
		bullet *bul = &bullets[i];

		if (bul->cond & 0x80)
		{
			bul->flag = 0;

			const int fromX = (bul->x - bul->blockXL + 0x1000) >> 13;
			const int fromY = (bul->y - bul->blockYL + 0x1000) >> 13;

			const int toX = (bul->x + bul->blockXL + 0x1000) >> 13;
			const int toY = (bul->y + bul->blockYL + 0x1000) >> 13;

			if (!(bul->bbits & bullet_ignoreSolid))
			{
				for (int currentX = fromX; currentX <= toX; currentX++)
				{
					for (int currentY = fromY; currentY <= toY; currentY++)
					{
						switch (getTileAttribute(currentX, currentY))
						{
						case 0x41: case 0x43: case 0x44: case 0x61: case 0x64:
							bul->flag |= bulletJudgeBlock(currentX, currentY, bul);
							break;

						case 0x50u:
						case 0x70u:
							bul->flag |= bulletJudgeTriangleA(currentX, currentY, bul);
							break;
						case 0x51u:
						case 0x71u:
							bul->flag |= bulletJudgeTriangleB(currentX, currentY, bul);
							break;
						case 0x52u:
						case 0x72u:
							bul->flag |= bulletJudgeTriangleC(currentX, currentY, bul);
							break;
						case 0x53u:
						case 0x73u:
							bul->flag |= bulletJudgeTriangleD(currentX, currentY, bul);
							break;
						case 0x54u:
						case 0x74u:
							bul->flag |= bulletJudgeTriangleE(currentX, currentY, bul);
							break;
						case 0x55u:
						case 0x75u:
							bul->flag |= bulletJudgeTriangleF(currentX, currentY, bul);
							break;
						case 0x56u:
						case 0x76u:
							bul->flag |= bulletJudgeTriangleG(currentX, currentY, bul);
							break;
						case 0x57u:
						case 0x77u:
							bul->flag |= bulletJudgeTriangleH(currentX, currentY, bul);
							break;

						default:
							break;
						}
					}
				}

				bulletJudgeBlock2(fromX, fromY, bul);
			}
		}
	}
}

void bulletHitNpcs()
{
	for (size_t n = 0; n < npcs.size(); n++)
	{
		if (npcs[n].cond & npccond_alive && (!(npcs[n].bits & npc_shootable) || !(npcs[n].bits & npc_interact)))
		{
			for (size_t i = 0; ; i++)
			{
				if (i >= bullets.size())
				{
					if (npcs[n].cond & 8)
						killNpc(&npcs[n], true);
					break;
				}

				bullet *bul = &bullets[i];

				if (bul->cond & 0x80 && bul->damage != -1)
				{
					bool bHit = false;

					if (npcs[n].bits & npc_shootable
						&& npcs[n].x - npcs[n].hit.right < bul->x + bul->enemyXL
						&& npcs[n].x + npcs[n].hit.right > bul->x - bul->enemyXL
						&& npcs[n].y - npcs[n].hit.top < bul->y + bul->enemyYL
						&& npcs[n].y + npcs[n].hit.bottom > bul->y - bul->enemyYL)
						bHit = true;
					else if (npcs[n].bits & npc_invulnerable
						&& npcs[n].x - npcs[n].hit.right < bul->x + bul->blockXL
						&& npcs[n].x + npcs[n].hit.right > bul->x - bul->blockXL
						&& npcs[n].y - npcs[n].hit.top < bul->y + bul->blockYL
						&& npcs[n].y + npcs[n].hit.bottom > bul->y - bul->blockYL)
						bHit = true;

					if (bHit)
					{
						if (npcs[n].bits & npc_shootable)
						{
							npcs[n].life -= bul->damage;

							if (npcs[n].life > 0)
							{
								if (npcs[n].shock < 14)
								{
									createCaret((bul->x + npcs[n].x) / 2, (bul->y + npcs[n].y) / 2, 11, 0);
									createCaret((bul->x + npcs[n].x) / 2, (bul->y + npcs[n].y) / 2, 11, 0);
									createCaret((bul->x + npcs[n].x) / 2, (bul->y + npcs[n].y) / 2, 11, 0);

									playSound(npcs[n].hit_voice);
									npcs[n].shock = 16;
								}

								if (npcs[n].bits & npc_showdamage)
									npcs[n].damage_view -= bul->damage;
							}
							else
							{
								npcs[n].life = 0;

								if (npcs[n].bits & npc_showdamage)
									npcs[n].damage_view -= bul->damage;

								if (currentPlayer.cond & 0x80u && npcs[n].bits & npc_eventdie)
									startTscEvent(npcs[n].code_event);
								else
									npcs[n].cond |= 8u;
							}
						}
						else if (bul->code_bullet != 13
							&& bul->code_bullet != 14
							&& bul->code_bullet != 15
							&& bul->code_bullet != 28
							&& bul->code_bullet != 29
							&& bul->code_bullet != 30
							&& !(bul->bbits & 0x10))
						{
							createCaret((bul->x + npcs[n].x) / 2, (bul->y + npcs[n].y) / 2, 2, 2);
							playSound(31);
							bul->life = 0;
							continue;
						}

						--bul->life;
					}
				}
			}
		}
	}
}
