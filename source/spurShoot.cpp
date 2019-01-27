#include "shootFunctions.h"

#include "input.h"
#include "sound.h"
#include "caret.h"
#include "bullet.h"
#include "player.h"
#include "main.h"

int gSpurCharge = 0;
bool gMax = false;

void shootSpur(int level)
{
	bool bShot = false;

	if (isKeyDown(gKeyShoot))
	{
		//Build up experience
		if (gCurrentPlayer.equip & equip_armsBarrier)
			giveWeaponExperience(3);
		else
			giveWeaponExperience(2);

		//Play sound
		if (++gSpurCharge / 2 & 1)
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
		if (gSpurCharge)
			bShot = true;
		gSpurCharge = 0;
	}

	//Play the sound when max level is reached
	if (weaponMaxExp())
	{
		if (!gMax)
		{
			gMax = true;
			playSound(65);
		}
	}
	else
	{
		gMax = false;
	}

	//Clear experience when shoot button released
	if (!isKeyDown(gKeyShoot))
		resetSelectedWeaponLevel();

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
		if (gMax)
			bul_no = 39;
		else
			bul_no = 38;
		break;
	default:
		doCustomError("level argument of shootSpur was invalid");
	}

	//Shoot bullet
	const bool cantShoot = weaponBullets(13) > 0 || weaponBullets(14) > 0;

	if (!cantShoot && (isKeyPressed(gKeyShoot) || bShot))
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			playSound(37);
		}
		else
		{
			uint8_t shootDirect;
			int shootX;
			int shootY;
			int caretX;

			//Get bullet orientation and position
			if (gCurrentPlayer.up)
			{
				shootDirect = dirUp;
				shootX = (gCurrentPlayer.direct - 1) * 0x200;
				caretX = (gCurrentPlayer.direct - 1) * 0x200;
				shootY = -0x1000;
			}
			else if (gCurrentPlayer.down)
			{
				shootDirect = dirDown;
				shootX = (gCurrentPlayer.direct - 1) * 0x200;
				caretX = (gCurrentPlayer.direct - 1) * 0x200;
				shootY = 0x1000;
			}
			else
			{
				shootDirect = gCurrentPlayer.direct;
				shootX = (gCurrentPlayer.direct - 1) * 0xC00;
				caretX = (gCurrentPlayer.direct - 1) * 0x1800;
				shootY = 0x600;
			}

			//Create bullet
			if(bul_no == bullet_PolarStarLevel3)
				createBullet(bul_no, gCurrentPlayer.x + shootX, gCurrentPlayer.y + shootY, shootDirect);
			else
				createBullet(bul_no, gCurrentPlayer.x + shootX, gCurrentPlayer.y + shootY, shootDirect, 13);
			createCaret(gCurrentPlayer.x + caretX, gCurrentPlayer.y + shootY, effect_Star);

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
