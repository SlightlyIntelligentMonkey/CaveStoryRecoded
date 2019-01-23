#include "shootFunctions.h"
#include "bullet.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "caret.h"

void shootMachineGun(int level)
{
	static int rechargeWait = 0;

	if (isKeyDown(gKeyShoot))
	{
		if (++currentPlayer.rensha > 5)
			currentPlayer.rensha = 0;
		else
			return;

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
			bulletId = bullet_MachineGunLevel1;
			playSound(SFX_PolarStarShootLevel12);
			break;
		case(2):
			bulletId = bullet_MachineGunLevel2;
			playSound(SFX_PolarStarShootLevel12);
			break;
		case(3):
			bulletId = bullet_MachineGunLevel3;
			playSound(SFX_PolarStarShootLevel3);
			break;
		}

		int shootDirect = 0;
		int shootX = 0;
		int shootY = 0;
		int caretX = 0;

		if (currentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (currentPlayer.direct - 1) * 1536;
			caretX = (currentPlayer.direct - 1) * 1536;
			shootY = -0x1000;
			if (level == 3) { currentPlayer.ym += 256; }
		}
		else if (currentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (currentPlayer.direct - 1) * 1536;
			caretX = (currentPlayer.direct - 1) * 1536;
			shootY = 0x1000;
			if (level == 3)
			{
				if (currentPlayer.ym > 0)
					currentPlayer.ym /= 2;
				if (currentPlayer.ym >= -1023)
				{
					currentPlayer.ym -= 512;
					if (currentPlayer.ym < -1024)
						currentPlayer.ym = -1024;
				}
			}
		}
		else
		{
			shootDirect = currentPlayer.direct;
			shootX = (currentPlayer.direct - 1) * 0x1800;
			caretX = (currentPlayer.direct - 1) * 0x1800;
			shootY = 0x600;
		}
		createBullet(bulletId, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect);
		createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, 3, 0);
	}
	else
	{
		currentPlayer.rensha = 6;
		rechargeWait++;
		if (currentPlayer.equip & equip_turboCharge)
		{
			if (rechargeWait > 1)
			{
				rechargeWait = 0;
				giveWeaponAmmo(1);
			}
		}
		else if (rechargeWait > 4)
		{
			rechargeWait = 0;
			giveWeaponAmmo(1);
		}
	}
}
