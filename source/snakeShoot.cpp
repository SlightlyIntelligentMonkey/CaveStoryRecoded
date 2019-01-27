#include "shootFunctions.h"
#include "input.h"
#include "sound.h"
#include "bullet.h"
#include "player.h"
#include "caret.h"

void shootSnake(int level)
{
	if (weaponBullets(1) <= 3 && isKeyPressed(gKeyShoot))
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			gSelectedWeapon = 0;
			gWeaponShiftX = 32;
			playSound(SFX_SwitchWeapon);
		}

		int bulletId = 0;
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

		int shootDirect = 0;
		int shootX = 0;
		int shootY = 0;
		int caretX = 0;

		if (gCurrentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (gCurrentPlayer.direct - 1) * 1536;
			caretX = (gCurrentPlayer.direct - 1) * 1536;
			shootY = -5120;
		}
		else if (gCurrentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (gCurrentPlayer.direct - 1) * 1536;
			caretX = (gCurrentPlayer.direct - 1) * 1536 ;
			shootY = 5120;
		}
		else
		{
			shootDirect = gCurrentPlayer.direct;
			shootX = (gCurrentPlayer.direct - 1) * 0xC00;
			caretX = (gCurrentPlayer.direct - 1) * 0x1800;
			shootY = 1024;
		}

		//Create bullet
		createBullet(bulletId, gCurrentPlayer.x + shootX, gCurrentPlayer.y + shootY, shootDirect, 1);
		createCaret(gCurrentPlayer.x + caretX, gCurrentPlayer.y + shootY, effect_Star);
		playSound(SFX_FireballShoot);
	}
}
