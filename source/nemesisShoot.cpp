#include "shootFunctions.h"
#include "bullet.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "caret.h"

void shootNemesis(int level)
{
	int bulletId = 0;
	int shootDirect = 0;
	int shootX = 0;
	int shootY = 0;
	int caretX = 0;

	if (weaponBullets(12) < 2 && isKeyPressed(gKeyShoot))
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			playSound(SFX_NoAmmo);
			return;
		}

		switch (level)
		{
		case(1):
			bulletId = bullet_NemesisLevel1;
			break;
		case(2):
			bulletId = bullet_NemesisLevel2;
			break;
		case(3):
			bulletId = bullet_NemesisLevel3;
			break;
		}

		if (currentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (currentPlayer.direct - 1) * 512;
			caretX = (currentPlayer.direct - 1) * 512;
			shootY = -6144;
		}
		else if (currentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (currentPlayer.direct - 1) * 512;
			caretX = (currentPlayer.direct - 1) * 512;
			shootY = 6144;
		}
		else
		{
			shootDirect = currentPlayer.direct;
			shootX = (currentPlayer.direct - 1) * 11264;
			caretX = (currentPlayer.direct - 1) * 8192;
			shootY = 1536;
		}

		createBullet(bulletId, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect, 12);
		createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star);

	
		switch (level)
		{
		case(1):
			playSound(SFX_NemesisShot);
			break;
		case(2):
			playSound(SFX_PolarStarShootLevel3);
			break;
		case(3):
			playSound(SFX_SpurChargeLevel2);
			break;
		}
	}
}
