#include "shootFunctions.h"
#include "input.h"
#include "sound.h"
#include "bullet.h"
#include "player.h"
#include "caret.h"
#include "render.h"
#include "main.h"

void shootFireball(int level)
{
	//Get the type of bullet, and limit how many on screen at once
	int bul_no;
	uint8_t shootDirect;
	int shootX;
	int shootY;
	int caretX;

	if (isKeyPressed(keyShoot))
	{
		switch (level)
		{
		case 1:
			if (weaponBullets(3) >= 2)
				return;
			bul_no = bullet_FireballLevel1;
			break;

		case 2:
			if (weaponBullets(3) >= 3)
				return;
			bul_no = bullet_FireballLevel2;
			break;

		case 3:
			if (weaponBullets(3) >= 4)
				return;
			bul_no = bullet_FireballLevel3;
			break;
		default:
			doCustomError("Invalid level");
		}

		if (useWeaponAmmo(1) ^ 1)
		{
			selectedWeapon = 0;
			weaponShiftX = 32;
			playSound(SFX_SwitchWeapon);
			return;
		}

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
		createBullet(bul_no, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect, 3);
		createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star);

		playSound(SFX_FireballBounce);
	}
}
