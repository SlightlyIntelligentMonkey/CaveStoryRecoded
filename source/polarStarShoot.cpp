#include "shootFunctions.h"

#include "bullet.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "caret.h"

void shootPolarStar(int level)
{
	if (weaponBullets(2) < 2 && isKeyPressed(keyShoot))
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			playSound(SFX_NoAmmo);
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
				shootDirect = dirUp;
				shootX = (currentPlayer.direct - 1) * 0x200;
				caretX = (currentPlayer.direct - 1) * 0x200;
				shootY = -0x1000;
			}
			else if (currentPlayer.down)
			{
				shootDirect = dirDown;
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
			createBullet(level + bullet_PolarStarLevel1 - 1, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect);
			createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star, 0);

			//Play sound
			if (level == 3)
				playSound(SFX_PolarStarShootLevel3);
			else
				playSound(SFX_PolarStarShootLevel12);
		}
	}
}
