#include "shootFunctions.h"
#include "bullet.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "caret.h"

void shootMissile(int level)
{

	if (isKeyPressed(keyShoot) && weaponBullets(5) < 1)
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			playSound(SFX_NoAmmo);
			if (!_empty)
			{
				createCaret(currentPlayer.x, currentPlayer.y, 16, 0);
				_empty = 50;
			}
			return;
		}

		int bulletId = 0;
		switch (level)
		{
		case(1):
			if (weaponBullets(5) >= 1) { return; }
			bulletId = bullet_MissileLauncherLevel1;
			break;
		case(2):
			if (weaponBullets(5) >= 1) { return; }
			bulletId = bullet_MissileLauncherLevel2;
			break;
		case(3):
			if (weaponBullets(5) >= 3) { return; }
			bulletId = bullet_MissileLauncherLevel3;
			break;
		}

		int shootDirect = 0;
		int shootX = 0;
		int shootY = 0;
		int caretX = 0;

		if (currentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (currentPlayer.direct - 1) * 0x800;
			caretX = (currentPlayer.direct - 1) * 0x800;
			shootY = -0x1000;
		}
		else if (currentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (currentPlayer.direct - 1) * 0x800;
			caretX = (currentPlayer.direct - 1) * 0x800;
			shootY = 0x1000;
		}
		else
		{
			shootDirect = currentPlayer.direct;
			shootX = (currentPlayer.direct - 1) * 0xC00;
			caretX = (currentPlayer.direct - 1) * 0x1800;
			shootY = 0x600;
		}

		playSound(SFX_PolarStarShootLevel12);
		if (level < 3)
		{
			createBullet(bulletId, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect, 5);
		}
		else
		{
			createBullet(bulletId, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect, 5);
			if (shootDirect == dirLeft || shootDirect == dirRight)
			{
				createBullet(bulletId, currentPlayer.x + 1536, currentPlayer.y, shootDirect, 5);
				createBullet(bulletId, currentPlayer.x - 1536, currentPlayer.y, shootDirect, 5);
			}
			else if (shootDirect == dirUp || shootDirect == dirDown)
			{
				createBullet(bulletId, currentPlayer.x, currentPlayer.y + 1536, shootDirect, 5);
				createBullet(bulletId, currentPlayer.x, currentPlayer.y - 1536, shootDirect, 5);
			}

		}
		createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star);
	}
}

void superMissileLaunch(int level)
{

	if (isKeyPressed(keyShoot))
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			playSound(SFX_NoAmmo);
			createCaret(currentPlayer.x, currentPlayer.y, 16, 0);
			return;
		}

		int bulletId = 0;
		switch (level)
		{
		case(1):
			if (weaponBullets(4) > 1) { return; }
			bulletId = bullet_SuperMissileLauncherLevel3;
			break;
		case(2):
			if (weaponBullets(4) > 1) { return; }
			bulletId = bullet_SuperMissileLauncherLevel3;
			break;
		case(3):
			if (weaponBullets(4) > 3) { return; }
			bulletId = bullet_SuperMissileLauncherLevel3;
			break;
		}

		int shootDirect = 0;
		int shootX = 0;
		int shootY = 0;
		int caretX = 0;

		if (currentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (currentPlayer.direct - 1) * 0x800;
			caretX = (currentPlayer.direct - 1) * 0x800;
			shootY = -0x1000;
		}
		else if (currentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (currentPlayer.direct - 1) * 0x800;
			caretX = (currentPlayer.direct - 1) * 0x800;
			shootY = 0x1000;
		}
		else
		{
			shootDirect = currentPlayer.direct;
			shootX = (currentPlayer.direct - 1) * 0xC00;
			caretX = (currentPlayer.direct - 1) * 0x1800;
			shootY = 0x600;
		}

		playSound(SFX_PolarStarShootLevel12);
		createBullet(bulletId, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect);
		createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star, 0);
	}
}
