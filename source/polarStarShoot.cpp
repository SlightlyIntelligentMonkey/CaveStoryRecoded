#include "shootFunctions.h"

void shootPolarStar(int level)
{
	if (weaponBullets(2) < 2 && isKeyPressed(keyShoot))
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			playSound(37);
		}
		else
		{
			int shootDirect;
			int shootX;
			int shootY;
			int caretX;

			//Get bullet orientation and position
			if (currentPlayer.up)
			{
				shootDirect = 1;
				shootX = (currentPlayer.direct - 1) * 0x200;
				caretX = (currentPlayer.direct - 1) * 0x200;
				shootY = -0x1000;
			}
			else if (currentPlayer.down)
			{
				shootDirect = 3;
				shootX = (currentPlayer.direct - 1) * 0x200;
				caretX = (currentPlayer.direct - 1) * 0x200;
				shootY = 0x1000;
			}
			else
			{
				shootDirect = currentPlayer.direct;
				shootX = (currentPlayer.direct - 1) * 0xC00;
				caretX = (currentPlayer.direct - 1) * 0x1800;
				shootY = 0x600;
			}

			//Create bullet
			createBullet(level + 3, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect);
			createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, 3, 0);

			//Play sound
			if (level == 3)
				playSound(49);
			else
				playSound(32);
		}
	}
}