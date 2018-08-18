#include "shootFunctions.h"

void shootFireball(int level)
{
	//Get the type of bullet, and limit how many on screen at once
	int bul_no;

	switch (level)
	{
	case 1:
		if (weaponBullets(3) > 1)
			return;
		bul_no = 7;
		break;

	case 2:
		if (weaponBullets(3) > 2)
			return;
		bul_no = 8;
		break;

	case 3:
		if (weaponBullets(3) > 3)
			return;
		bul_no = 9;
		break;
	}

	if (isKeyPressed(keyShoot))
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			selectedWeapon = 0;
			weaponShiftX = 32;
			playSound(4);
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
				shootX = (currentPlayer.direct - 1) * 0x800;
				caretX = (currentPlayer.direct - 1) * 0x800;
				shootY = -0x1000;
			}
			else if (currentPlayer.down)
			{
				shootDirect = 3;
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

			//Create bullet
			createBullet(bul_no, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect);
			createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, 3, 0);

			playSound(34);
		}
	}
}
