#include "shootFunctions.h"

#include "bullet.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "caret.h"

void shootPolarStar(int level)
{
	if (weaponBullets(2) > 1)
		return;

	if (isKeyPressed(gKeyShoot))
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
			createBullet(level + bullet_PolarStarLevel1 - 1, gCurrentPlayer.x + shootX, gCurrentPlayer.y + shootY, shootDirect, 2);
			createCaret(gCurrentPlayer.x + caretX, gCurrentPlayer.y + shootY, effect_Star);

			//Play sound
			if (level == 3)
				playSound(SFX_PolarStarShootLevel3);
			else
				playSound(SFX_PolarStarShootLevel12);
		}
	}
}
