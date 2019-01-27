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
		if (++gCurrentPlayer.rensha > 5)
			gCurrentPlayer.rensha = 0;
		else
			return;

		if (useWeaponAmmo(1) ^ 1)
		{
			playSound(SFX_NoAmmo);
			if (!gEmpty)
			{
				createCaret(gCurrentPlayer.x, gCurrentPlayer.y, 16, 0);
				gEmpty = 50;
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

		if (gCurrentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (gCurrentPlayer.direct - 1) * 1536;
			caretX = (gCurrentPlayer.direct - 1) * 1536;
			shootY = -0x1000;
			if (level == 3) { gCurrentPlayer.ym += 256; }
		}
		else if (gCurrentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (gCurrentPlayer.direct - 1) * 1536;
			caretX = (gCurrentPlayer.direct - 1) * 1536;
			shootY = 0x1000;
			if (level == 3)
			{
				if (gCurrentPlayer.ym > 0)
					gCurrentPlayer.ym /= 2;
				if (gCurrentPlayer.ym >= -1023)
				{
					gCurrentPlayer.ym -= 512;
					if (gCurrentPlayer.ym < -1024)
						gCurrentPlayer.ym = -1024;
				}
			}
		}
		else
		{
			shootDirect = gCurrentPlayer.direct;
			shootX = (gCurrentPlayer.direct - 1) * 0x1800;
			caretX = (gCurrentPlayer.direct - 1) * 0x1800;
			shootY = 0x600;
		}
		createBullet(bulletId, gCurrentPlayer.x + shootX, gCurrentPlayer.y + shootY, shootDirect);
		createCaret(gCurrentPlayer.x + caretX, gCurrentPlayer.y + shootY, 3, 0);
	}
	else
	{
		gCurrentPlayer.rensha = 6;
		rechargeWait++;
		if (gCurrentPlayer.equip & equip_turboCharge)
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
