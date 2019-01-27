#include "shootFunctions.h"
#include "bullet.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "caret.h"

void throwBlade(int level)
{
	int bulletId = 0;
	int shootDirect = 0;
	int shootX = 0;
	int shootY = 0;

	if (isKeyPressed(gKeyShoot) && weaponBullets(9) <= 0)
	{
		switch (level)
		{
		case(1):
			bulletId = bullet_BladeLevel1;
			break;
		case(2):
			bulletId = bullet_BladeLevel2;
			break;
		case(3):
			bulletId = bullet_BladeLevel3;
			break;
		}

		if (gCurrentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (gCurrentPlayer.direct - 1) * 0x200;
			shootY = -0x800;
		}
		else if (gCurrentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (gCurrentPlayer.direct - 1) * 0x200;
			shootY = 0xC00;
		}
		else
		{
			shootDirect = gCurrentPlayer.direct;
			shootX = (gCurrentPlayer.direct - 1) * 0xC00;
			shootY = 0x600;
		}

		createBullet(bulletId, gCurrentPlayer.x + shootX, gCurrentPlayer.y + shootY, shootDirect, 9);

	}
}
