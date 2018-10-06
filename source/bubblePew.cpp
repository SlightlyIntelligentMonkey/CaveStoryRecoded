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
		if (isKeyPressed(keyShoot) && weaponBullets(7) <= 3)
		{
			if (useWeaponAmmo(1) ^ 1)
			{
				playSound(SFX_NoAmmo);
				if (!_empty)
				{
					createCaret(currentPlayer.x, currentPlayer.y, 16, 0);
					_empty = 50;
				}
				return;
			}

			if (currentPlayer.up)
			{
				shootDirect = dirUp;
				shootX = (currentPlayer.direct - 1) * 0x200;
				caretX = (currentPlayer.direct - 1) * 0x200;
				shootY = -0x400;
			}
			else if (currentPlayer.down)
			{
				shootDirect = dirDown;
				shootX = (currentPlayer.direct - 1) * 0x200;
				caretX = (currentPlayer.direct - 1) * 0x200;
				shootY = 0x400;
			}
			else
			{
				shootDirect = currentPlayer.direct;
				shootX = (currentPlayer.direct - 1) * 0xC00;
				caretX = (currentPlayer.direct - 1) * 0x1800;
				shootY = 0x600;
			}

			createBullet(bullet_BubblerLevel1, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect, 7);
			createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star, 0);
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
		if (isKeyDown(keyShoot))
		{
			if (weaponBullets(7) <= 15 && ++currentPlayer.rensha > 6)
			{
				currentPlayer.rensha = 0;
				if (useWeaponAmmo(1) ^ 1)
				{
					playSound(SFX_NoAmmo);
					if (!_empty)
					{
						createCaret(currentPlayer.x, currentPlayer.y, 16, 0);
						_empty = 50;
					}
					return;
				}

				if (currentPlayer.up)
				{
					shootDirect = dirUp;
					shootX = (currentPlayer.direct - 1) * 1536;
					caretX = (currentPlayer.direct - 1) * 1536;
					shootY = -0x1000;
				}
				else if (currentPlayer.down)
				{
					shootDirect = dirDown;
					shootX = (currentPlayer.direct - 1) * 1536;
					caretX = (currentPlayer.direct - 1) * 1536;
					shootY = 0x1000;
				}
				else
				{
					shootDirect = currentPlayer.direct;
					shootX = (currentPlayer.direct - 1) * 0xC00;
					caretX = (currentPlayer.direct - 1) * 0x1800;
					shootY = 0x600;
				}

				createBullet(bullet_BubblerLevel1 + (level - 1), currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect, 7);
				createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star, 0);
				playSound(SFX_BubblerShoot);
			}
		}
		else
		{
			currentPlayer.rensha = 6;
			if (++rechargeWait > 1)
			{
				rechargeWait = 0;
				giveWeaponAmmo(1);
			}
		}
	}
}
