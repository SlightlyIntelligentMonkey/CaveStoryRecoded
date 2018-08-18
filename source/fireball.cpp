#include "fireball.h"

void actBulletFireball1(bullet *bul)
{
	if (++bul->count1 <= bul->life_count)
	{
		bool bBreak = false;
		
		//Stop existing if possibly stuck in a wall
		if (bul->flag & ceiling && bul->flag & ground)
			bBreak = true;
		if (bul->flag & leftWall && bul->flag & rightWall)
			bBreak = true;

		//Changing direction on walls
		if (!bul->direct && bul->flag & 1)
			bul->direct = 2;
		if (bul->direct == 2 && bul->flag & 4)
			bul->direct = 0;

		if (bBreak)
		{
			bul->cond = 0;
			createCaret(bul->x, bul->y, 2, 0);
			playSound(28);
		}
		else
		{
			if (bul->act_no)
			{
				//Bounce off walls
				if (bul->flag & ground)
					bul->ym = -0x400;
				else if (bul->flag & leftWall)
					bul->xm = 0x400;
				else if (bul->flag & rightWall)
					bul->xm = -0x400;

				//Gravity
				bul->ym += 0x55;

				//Fall speed limit
				if (bul->ym > 0x3FF)
					bul->ym = 0x3FF;

				//Move
				bul->x += bul->xm;
				bul->y += bul->ym;

				//Play bouncing sound
				if (bul->flag & (leftWall | rightWall | ground))
					playSound(34);
			}
			else
			{
				bul->act_no = 1;
				
				//Shoot in direction player is facing
				switch (bul->direct)
				{
				case 0:
					bul->xm = -1024;
					break;

				case 1:
					//Move at same speed as player
					bul->xm = currentPlayer.xm;

					//Move in facing direction
					if (currentPlayer.xm >= 0)
						bul->direct = 2;
					else
						bul->direct = 0;

					//Shoot with increased velocity in facing direction
					if (currentPlayer.direct)
						bul->xm += 128;
					else
						bul->xm -= 128;

					bul->ym = -0x5FF;
					break;

				case 2:
					bul->xm = 1024;
					break;

				case 3:
					bul->xm = currentPlayer.xm;

					//Move in current moving direction
					if (currentPlayer.xm >= 0)
						bul->direct = 2;
					else
						bul->direct = 0;

					bul->ym = 0x5FF;
					break;
				}
			}

			RECT rect_right1[4];
			RECT rect_left1[4];
			RECT rect_right2[3];
			RECT rect_left2[3];

			rect_left1[0] = { 128, 0, 144, 16 };
			rect_left1[1] = { 144, 0, 160, 16 };
			rect_left1[2] = { 160, 0, 176, 16 };
			rect_left1[3] = { 176, 0, 192, 16 };
			rect_right1[0] = { 128, 16, 144, 32 };
			rect_right1[1] = { 144, 16, 160, 32 };
			rect_right1[2] = { 160, 16, 176, 32 };
			rect_right1[3] = { 176, 16, 192, 32 };

			rect_left2[0] = { 192, 16, 208, 32 };
			rect_left2[1] = { 208, 16, 224, 32 };
			rect_left2[2] = { 224, 16, 240, 32 };
			rect_right2[0] = { 224, 16, 240, 32 };
			rect_right2[1] = { 208, 16, 224, 32 };
			rect_right2[2] = { 192, 16, 208, 32 };

			if (++bul->ani_no > 3)
				bul->ani_no = 0;

			if (bul->direct)
				bul->rect = rect_right1[bul->ani_no];
			else
				bul->rect = rect_left1[bul->ani_no];
		}
	}
	else
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
	}
}

void actBulletFireball2(bullet *bul)
{
	if (++bul->count1 <= bul->life_count)
	{
		bool bBreak = false;

		//Stop existing if possibly stuck in a wall
		if (bul->flag & ceiling && bul->flag & ground)
			bBreak = true;
		if (bul->flag & leftWall && bul->flag & rightWall)
			bBreak = true;

		//Changing direction on walls
		if (!bul->direct && bul->flag & 1)
			bul->direct = 2;
		if (bul->direct == 2 && bul->flag & 4)
			bul->direct = 0;

		if (bBreak)
		{
			bul->cond = 0;
			createCaret(bul->x, bul->y, 2, 0);
			playSound(28);
		}
		else
		{
			if (bul->act_no)
			{
				//Bounce off walls
				if (bul->flag & ground)
					bul->ym = -0x400;
				else if (bul->flag & leftWall)
					bul->xm = 0x400;
				else if (bul->flag & rightWall)
					bul->xm = -0x400;

				//Gravity
				bul->ym += 0x55;

				//Fall speed limit
				if (bul->ym > 0x3FF)
					bul->ym = 0x3FF;

				//Move
				bul->x += bul->xm;
				bul->y += bul->ym;

				//Play bouncing sound
				if (bul->flag & (leftWall | rightWall | ground))
					playSound(34);
			}
			else
			{
				bul->act_no = 1;

				//Shoot in direction player is facing
				switch (bul->direct)
				{
				case 0:
					bul->xm = -1024;
					break;

				case 1:
					//Move at same speed as player
					bul->xm = currentPlayer.xm;

					//Move in facing direction
					if (currentPlayer.xm >= 0)
						bul->direct = 2;
					else
						bul->direct = 0;

					//Shoot with increased velocity in facing direction
					if (currentPlayer.direct)
						bul->xm += 128;
					else
						bul->xm -= 128;

					bul->ym = -0x5FF;
					break;

				case 2:
					bul->xm = 1024;
					break;

				case 3:
					bul->xm = currentPlayer.xm;

					//Move in current moving direction
					if (currentPlayer.xm >= 0)
						bul->direct = 2;
					else
						bul->direct = 0;

					bul->ym = 0x5FF;
					break;
				}
			}

			RECT rect_right1[4];
			RECT rect_left1[4];
			RECT rect_right2[3];
			RECT rect_left2[3];

			rect_left1[0] = { 128, 0, 144, 16 };
			rect_left1[1] = { 144, 0, 160, 16 };
			rect_left1[2] = { 160, 0, 176, 16 };
			rect_left1[3] = { 176, 0, 192, 16 };
			rect_right1[0] = { 128, 16, 144, 32 };
			rect_right1[1] = { 144, 16, 160, 32 };
			rect_right1[2] = { 160, 16, 176, 32 };
			rect_right1[3] = { 176, 16, 192, 32 };

			rect_left2[0] = { 192, 16, 208, 32 };
			rect_left2[1] = { 208, 16, 224, 32 };
			rect_left2[2] = { 224, 16, 240, 32 };
			rect_right2[0] = { 224, 16, 240, 32 };
			rect_right2[1] = { 208, 16, 224, 32 };
			rect_right2[2] = { 192, 16, 208, 32 };

			if (++bul->ani_no > 2)
				bul->ani_no = 0;

			if (bul->direct)
				bul->rect = rect_right2[bul->ani_no];
			else
				bul->rect = rect_left2[bul->ani_no];

			createNpc(129, bul->x, bul->y, 0, -512, bul->ani_no, nullptr);
		}
	}
	else
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
	}
}

void actBulletFireball3(bullet *bul)
{
	if (++bul->count1 <= bul->life_count)
	{
		bool bBreak = false;

		//Stop existing if possibly stuck in a wall
		if (bul->flag & ceiling && bul->flag & ground)
			bBreak = true;
		if (bul->flag & leftWall && bul->flag & rightWall)
			bBreak = true;

		//Changing direction on walls
		if (!bul->direct && bul->flag & 1)
			bul->direct = 2;
		if (bul->direct == 2 && bul->flag & 4)
			bul->direct = 0;

		if (bBreak)
		{
			bul->cond = 0;
			createCaret(bul->x, bul->y, 2, 0);
			playSound(28);
		}
		else
		{
			if (bul->act_no)
			{
				//Bounce off walls
				if (bul->flag & ground)
					bul->ym = -0x400;
				else if (bul->flag & leftWall)
					bul->xm = 0x400;
				else if (bul->flag & rightWall)
					bul->xm = -0x400;

				//Gravity
				bul->ym += 0x55;

				//Fall speed limit
				if (bul->ym > 0x3FF)
					bul->ym = 0x3FF;

				//Move
				bul->x += bul->xm;
				bul->y += bul->ym;

				//Play bouncing sound
				if (bul->flag & (leftWall | rightWall | ground))
					playSound(34);
			}
			else
			{
				bul->act_no = 1;

				//Shoot in direction player is facing
				switch (bul->direct)
				{
				case 0:
					bul->xm = -1024;
					break;

				case 1:
					//Move at same speed as player
					bul->xm = currentPlayer.xm;

					//Move in facing direction
					if (currentPlayer.xm >= 0)
						bul->direct = 2;
					else
						bul->direct = 0;

					//Shoot with increased velocity in facing direction
					if (currentPlayer.direct)
						bul->xm += 128;
					else
						bul->xm -= 128;

					bul->ym = -0x5FF;
					break;

				case 2:
					bul->xm = 1024;
					break;

				case 3:
					bul->xm = currentPlayer.xm;

					//Move in current moving direction
					if (currentPlayer.xm >= 0)
						bul->direct = 2;
					else
						bul->direct = 0;

					bul->ym = 0x5FF;
					break;
				}
			}

			RECT rect_right1[4];
			RECT rect_left1[4];
			RECT rect_right2[3];
			RECT rect_left2[3];

			rect_left1[0] = { 128, 0, 144, 16 };
			rect_left1[1] = { 144, 0, 160, 16 };
			rect_left1[2] = { 160, 0, 176, 16 };
			rect_left1[3] = { 176, 0, 192, 16 };
			rect_right1[0] = { 128, 16, 144, 32 };
			rect_right1[1] = { 144, 16, 160, 32 };
			rect_right1[2] = { 160, 16, 176, 32 };
			rect_right1[3] = { 176, 16, 192, 32 };

			rect_left2[0] = { 192, 16, 208, 32 };
			rect_left2[1] = { 208, 16, 224, 32 };
			rect_left2[2] = { 224, 16, 240, 32 };
			rect_right2[0] = { 224, 16, 240, 32 };
			rect_right2[1] = { 208, 16, 224, 32 };
			rect_right2[2] = { 192, 16, 208, 32 };

			if (++bul->ani_no > 2)
				bul->ani_no = 0;

			if (bul->direct)
				bul->rect = rect_right2[bul->ani_no];
			else
				bul->rect = rect_left2[bul->ani_no];

			createNpc(129, bul->x, bul->y, 0, -512, bul->ani_no + 3, nullptr);
		}
	}
	else
	{
		bul->cond = 0;
		createCaret(bul->x, bul->y, 3, 0);
	}
}
