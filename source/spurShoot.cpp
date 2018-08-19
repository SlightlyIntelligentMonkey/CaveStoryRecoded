#include "shootFunctions.h"

#include "input.h"
#include "sound.h"
#include "caret.h"
#include "bullet.h"
#include "player.h"

int spurCharge = 0;
bool bMax = false;

void shootSpur(int level)
{
	bool bShot = false;

	if (isKeyDown(keyShoot))
	{
		//Build up experience
		if (currentPlayer.equip & equip_armsBarrier)
			giveWeaponExperience(3);
		else
			giveWeaponExperience(2);

		//Play sound
		if (++spurCharge / 2 & 1)
		{
			switch (level)
			{
			case 1:
				playSound(59);
				break;
			case 2:
				playSound(60);
				break;
			case 3:
				if (!weaponMaxExp())
					playSound(61);
				break;
			}
		}
	}
	else
	{
		if (spurCharge)
			bShot = true;
		spurCharge = 0;
	}

	//Play the sound when max level is reached
	if (weaponMaxExp())
	{
		if (!bMax)
		{
			bMax = true;
			playSound(65);
		}
	}
	else
	{
		bMax = false;
	}

	//Clear experience when shoot button released
	if (!isKeyDown(keyShoot))
	{
		weapons[selectedWeapon].level = 1;
		weapons[selectedWeapon].exp = 0;
	}

	//Determine bullet to use
	int bul_no;

	switch (level)
	{
	case 1:
		bul_no = 6;
		bShot = false;
		break;
	case 2:
		bul_no = 37;
		break;
	case 3:
		if (bMax)
			bul_no = 39;
		else
			bul_no = 38;
		break;
	}

	//Shoot bullet
	bool cantShoot = weaponBullets(13) > 0 || weaponBullets(14) > 0;

	if (!cantShoot && (isKeyPressed(keyShoot) || bShot))
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
			createBullet(bul_no, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect);
			createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, 3, 0);

			//Play sound
			switch (bul_no)
			{
			case 6:
				playSound(49);
				break;
			case 37:
				playSound(62);
				break;
			case 38:
				playSound(63);
				break;
			case 39:
				playSound(64);
				break;
			default:
				break;
			}
		}
	}
}