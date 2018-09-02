#include "shootFunctions.h"
#include "input.h"
#include "sound.h"
#include "bullet.h"
#include "player.h"
#include "caret.h"

void shootFireball(int level)
{
	//Get the type of bullet, and limit how many on screen at once
	int bul_no;

	switch (level)
	{
	case 1:
		if (weaponBullets(bullet_FireballLevel1) > 1)
			return;
		bul_no = bullet_FireballLevel1;
		break;

	case 2:
		if (weaponBullets(bullet_FireballLevel1) > 2)
			return;
		bul_no = bullet_FireballLevel2;
		break;

	case 3:
		if (weaponBullets(bullet_FireballLevel1) > 3)
			return;
		bul_no = bullet_FireballLevel3;
		break;
	default:
		doCustomError("Invalid level");
	}

	if (isKeyPressed(keyShoot))
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			selectedWeapon = 0;
			weaponShiftX = 32;
			playSound(SFX_SwitchWeapon);
		}
		else
		{
			uint8_t shootDirect;
			int shootX;
			int shootY;
			int caretX;

			//Get bullet orientation and position
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

			//Create bullet
			createBullet(bul_no, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect);
			createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star);

			playSound(SFX_FireballBounce);
		}
	}
}
