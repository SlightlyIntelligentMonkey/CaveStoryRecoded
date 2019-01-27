#include "shootFunctions.h"
#include "bullet.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "caret.h"

void pewBubble(int level)
{
	static int rechargeWait;

	int shootDirect = 0;
	int shootX = 0;
	int shootY = 0;
	int caretX = 0;

	if (level == 1)
	{
		if (isKeyPressed(gKeyShoot) && weaponBullets(7) <= 3)
		{
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

			if (gCurrentPlayer.up)
			{
				shootDirect = dirUp;
				shootX = (gCurrentPlayer.direct - 1) * 0x200;
				caretX = (gCurrentPlayer.direct - 1) * 0x200;
				shootY = -0x400;
			}
			else if (gCurrentPlayer.down)
			{
				shootDirect = dirDown;
				shootX = (gCurrentPlayer.direct - 1) * 0x200;
				caretX = (gCurrentPlayer.direct - 1) * 0x200;
				shootY = 0x400;
			}
			else
			{
				shootDirect = gCurrentPlayer.direct;
				shootX = (gCurrentPlayer.direct - 1) * 0xC00;
				caretX = (gCurrentPlayer.direct - 1) * 0x1800;
				shootY = 0x600;
			}

			createBullet(bullet_BubblerLevel1, gCurrentPlayer.x + shootX, gCurrentPlayer.y + shootY, shootDirect, 7);
			createCaret(gCurrentPlayer.x + caretX, gCurrentPlayer.y + shootY, effect_Star, 0);
			playSound(SFX_BubblerShoot);
		}
		else if (++rechargeWait > 20)
		{
			rechargeWait = 0;
			giveWeaponAmmo(1);
		}
	}
	else
	{
		if (isKeyDown(gKeyShoot))
		{
			if (weaponBullets(7) <= 15 && ++gCurrentPlayer.rensha > 6)
			{
				gCurrentPlayer.rensha = 0;
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

				if (gCurrentPlayer.up)
				{
					shootDirect = dirUp;
					shootX = (gCurrentPlayer.direct - 1) * 1536;
					caretX = (gCurrentPlayer.direct - 1) * 1536;
					shootY = -0x1000;
				}
				else if (gCurrentPlayer.down)
				{
					shootDirect = dirDown;
					shootX = (gCurrentPlayer.direct - 1) * 1536;
					caretX = (gCurrentPlayer.direct - 1) * 1536;
					shootY = 0x1000;
				}
				else
				{
					shootDirect = gCurrentPlayer.direct;
					shootX = (gCurrentPlayer.direct - 1) * 0xC00;
					caretX = (gCurrentPlayer.direct - 1) * 0x1800;
					shootY = 0x600;
				}

				createBullet(bullet_BubblerLevel1 + (level - 1), gCurrentPlayer.x + shootX, gCurrentPlayer.y + shootY, shootDirect, 7);
				createCaret(gCurrentPlayer.x + caretX, gCurrentPlayer.y + shootY, effect_Star, 0);
				playSound(SFX_BubblerShoot);
			}
		}
		else
		{
			gCurrentPlayer.rensha = 6;
			if (++rechargeWait > 1)
			{
				rechargeWait = 0;
				giveWeaponAmmo(1);
			}
		}
	}
}
