#include "shootFunctions.h"
#include "input.h"
#include "sound.h"
#include "bullet.h"
#include "player.h"
#include "caret.h"

void shootSnake(int level)
{
	int bulletId = 0;
	int shootDirect = 0;
	int shootX = 0;
	int shootY = 0;
	int caretX = 0;

	if (weaponBullets(1) <= 3 && isKeyPressed(keyShoot))
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			playSound(SFX_NoAmmo);
		}
		switch (level)
		{
		case(1):
			bulletId = bullet_SnakeLevel1;
			break;
		case(2):
			bulletId = bullet_SnakeLevel2;
			break;
		case(3):
			bulletId = bullet_SnakeLevel3;
			break;
		}

		if (currentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (currentPlayer.direct - 1) * 1536;
			caretX = (currentPlayer.direct - 1) * 1536;
			shootY = -5120;
		}
		else if (currentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (currentPlayer.direct - 1) * 1536;
			caretX = (currentPlayer.direct - 1) * 1536 ;
			shootY = 5120;
		}
		else
		{
			shootDirect = currentPlayer.direct;
			shootX = (currentPlayer.direct - 1) * 0xC00;
			caretX = (currentPlayer.direct - 1) * 0x1800;
			shootY = 1024;
		}

		//Create bullet
		createBullet(bulletId, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect, 1);
		createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star);
		playSound(SFX_FireballShoot);
	}

	return;
}