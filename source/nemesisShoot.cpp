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

		if (gCurrentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (gCurrentPlayer.direct - 1) * 512;
			caretX = (gCurrentPlayer.direct - 1) * 512;
			shootY = -6144;
		}
		else if (gCurrentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (gCurrentPlayer.direct - 1) * 512;
			caretX = (gCurrentPlayer.direct - 1) * 512;
			shootY = 6144;
		}
		else
		{
			shootDirect = gCurrentPlayer.direct;
			shootX = (gCurrentPlayer.direct - 1) * 11264;
			caretX = (gCurrentPlayer.direct - 1) * 8192;
			shootY = 1536;
		}

		createBullet(bulletId, gCurrentPlayer.x + shootX, gCurrentPlayer.y + shootY, shootDirect, 12);
		createCaret(gCurrentPlayer.x + caretX, gCurrentPlayer.y + shootY, effect_Star);

	
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
