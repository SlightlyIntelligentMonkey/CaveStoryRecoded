#include "caret.h"
//yeah i like carrots

std::vector<caret> carets(0);

CARETSTAT caretStats[] = {
	{ 0x0,		0x0 },
	{ 0x800,	0x800 },
	{ 0x1000,	0x1000 },
	{ 0x1000,	0x1000 },
	{ 0x1000,	0x1000 },
	{ 0x800,	0x800 },
	{ 0x1000,	0x1000 },
	{ 0x800,	0x800 },
	{ 0x1000,	0x1000 },
	{ 0x1000,	0x1000 },
	{ 0x3800,	0x1000 },
	{ 0x800,	0x800 },
	{ 0x2000,	0x2000 },
	{ 0x800,	0x800 },
	{ 0x2800,	0x2800 },
	{ 0x800,	0x800 },
	{ 0x2800,	0x800 },
	{ 0x6800,	0x800 },
};

void updateCarets()
{
	if (carets.size())
	{
		//Remove dead carets
		for (size_t i = 0; i < carets.size(); i++) {
			if (!(carets[i].cond & 0x80))
			{
				carets.erase(carets.begin() + i);
				i--;
			}
		}

		//Update
		for (unsigned int i = 0; i < carets.size(); i++)
			carets[i].update();
	}
}

void drawCarets()
{
	if (carets.size())
	{
		for (unsigned int i = 0; i < carets.size(); i++)
		{
			if (carets[i].cond & 0x80)
				carets[i].draw();
		}
	}
}

//Action code
void caretAct000(caret *CARET) //Inexistente
{
	CARET->cond = 0;
}

void caretAct001(caret *CARET) //Pulsing Disc Particles
{
	RECT rcRight[4];
	RECT rcLeft[4];
	
	//A bunch of framerects
	rcLeft[0] = { 0, 64, 8, 72 };
	rcLeft[1] = { 8, 64, 16, 72 };
	rcLeft[2] = { 16, 64, 24, 72 };
	rcLeft[3] = { 24, 64, 32, 72 };
	
	rcRight[0] = { 64, 24, 72, 32 };
	rcRight[1] = { 72, 24, 80, 32 };
	rcRight[2] = { 80, 24, 88, 32 };
	rcRight[3] = { 88, 24, 96, 32 };
	
	//Movement
	if (!CARET->act_no)
	{
		CARET->act_no = 1;

		CARET->xm = random(-0x400, 0x400);
		CARET->ym = random(-0x400, 0);
	}
	
	CARET->ym += 0x40;
	
	CARET->x += CARET->xm;
	CARET->y += CARET->ym;

	//Animate
	if (++CARET->ani_wait > 5)
	{
		CARET->ani_wait = 0;
		if (++CARET->ani_no > 3) { CARET->cond = 0; }
	}

	//Set framerect
	if (CARET->direct)
		CARET->rect = rcRight[CARET->ani_no];
	else
		CARET->rect = rcLeft[CARET->ani_no];
	
	return;
}

void caretAct002(caret *CARET) //Rising Disc / Exploding Diamond
{
	RECT rcRight[4];
	RECT rcLeft[4];
	RECT rcUp[3];

	//A bunch of framerects
	rcLeft[0] = { 0, 32, 16, 48 };
	rcLeft[1] = { 16, 32, 32, 48 };
	rcLeft[2] = { 32, 32, 48, 48 };
	rcLeft[3] = { 48, 32, 64, 48 };

	rcRight[0] = { 176, 0, 192, 16 };
	rcRight[1] = { 192, 0, 208, 16 };
	rcRight[2] = { 208, 0, 224, 16 };
	rcRight[3] = { 224, 0, 240, 16 };

	rcUp[0] = { 0, 32, 16, 48 };
	rcUp[1] = { 32, 32, 48, 48 };
	rcUp[2] = { 16, 32, 32, 48 };

	//Act
	switch (CARET->direct)
	{
	case 0:
		CARET->ym -= 16;
		CARET->y += CARET->ym;

		//Animate
		if (++CARET->ani_wait > 5)
		{
			CARET->ani_wait = 0;
			++CARET->ani_no;
		}

		if (CARET->ani_no > 3)
			CARET->cond = 0;

		CARET->rect = rcLeft[CARET->ani_no];
		break;

	case 1:
		//Animate ??
		CARET->rect = rcUp[++CARET->ani_wait / 2 % 3];

		if (CARET->ani_wait > 24)
			CARET->cond = 0;
		break;

	case 2:
		//Animate
		if (++CARET->ani_wait > 2)
		{
			CARET->ani_wait = 0;
			++CARET->ani_no;
		}

		if (CARET->ani_no > 3)
			CARET->cond = 0;

		CARET->rect = rcRight[CARET->ani_no];
		break;
	}
}

void caretAct003(caret *CARET) //Star
{
	//Animate
	if (CARET->ani_wait++ > 2)
	{
		CARET->ani_wait = 0;

		if (CARET->ani_no++ > 3) 
			CARET->cond = 0;
	}
	
	CARET->rect = { (CARET->ani_no << 4), 48, 16 + (CARET->ani_no << 4), 64 };
}

void caretAct004(caret *CARET) //Fireball impact?
{
	//Set framerects
	RECT rcLeft[9];

	rcLeft[0] = { 64, 32, 80, 48 };
	rcLeft[1] = { 80, 32, 96, 48 };
	rcLeft[2] = { 96, 32, 112, 48 };

	rcLeft[3] = { 64, 48, 80, 64 };
	rcLeft[4] = { 80, 48, 96, 64 };
	rcLeft[5] = { 96, 48, 112, 64 };

	rcLeft[6] = { 64, 64, 80, 80 };
	rcLeft[7] = { 80, 64, 96, 80 };
	rcLeft[8] = { 96, 64, 112, 80 };

	//Animate
	if (++CARET->ani_wait > 1)
	{
		CARET->ani_wait = 0;
		if (++CARET->ani_no > 2) { CARET->cond = 0; }
	}

	//framerect
	CARET->rect = rcLeft[CARET->ani_no + CARET->direct * 3];
}

void caretAct005(caret *CARET) //Zzz...
{
	//Set framerects
	RECT rcLeft[7];

	rcLeft[0] = { 32, 64, 40, 72 };
	rcLeft[1] = { 32, 72, 40, 80 };
	rcLeft[2] = { 40, 64, 48, 72 };
	rcLeft[3] = { 40, 72, 48, 80 };
	rcLeft[4] = { 40, 64, 48, 72 };
	rcLeft[5] = { 40, 72, 48, 80 };
	rcLeft[6] = { 40, 64, 48, 72 };

	//Animate
	if (CARET->ani_wait++ > 4)
	{
		CARET->ani_wait = 0;
		CARET->ani_no++;
	}

	if (CARET->ani_no > 6)
		CARET->cond = 0;

	//Move
	CARET->x += 128;
	CARET->y -= 128;

	//framerect
	CARET->rect = rcLeft[CARET->ani_no];
}

void caretAct007(caret *CARET) //Booster particles
{
	//Animate and set framerect
	if (++CARET->ani_wait > 1)
	{
		CARET->ani_wait = 0;

		if (++CARET->ani_no > 6)
			CARET->cond = 0;
	}

	CARET->rect = { 56 + (CARET->ani_no << 3), 0, 64 + (CARET->ani_no << 3), 8 };

	//Move
	switch (CARET->direct)
	{
	case 0:
		CARET->x -= 0x400;
		break;

	case 1:
		CARET->y -= 0x400;
		break;

	case 2:
		CARET->x += 0x400;
		break;

	case 3:
		CARET->y += 0x400;
		break;
	}
}

void caretAct008(caret *CARET) //Drowned Quote
{
	if (CARET->direct)
		CARET->rect = { 32, 80, 48, 96 };
	else
		CARET->rect = { 16, 80, 32, 96 };
}

void caretAct009(caret *CARET) //Question mark (when down is pressed)
{
	//Move
	if (++CARET->ani_wait <= 4)
		CARET->y -= 2048;

	//Deletion
	if (CARET->ani_wait >= 32)
		CARET->cond = 0;

	if (CARET->direct)
		CARET->rect = { 48, 64, 64, 80 };
	else
		CARET->rect = { 0, 80, 16, 96 };
}

void caretAct010(caret *CARET) //Level up and level down
{
	RECT rcRight[2];
	RECT rcLeft[2];

	rcLeft[0] = { 0, 0, 56, 16 };
	rcLeft[1] = { 0, 16, 56, 32 };

	rcRight[0] = { 0, 96, 56, 112 };
	rcRight[1] = { 0, 112, 56, 128 };

	CARET->ani_wait++;

	if (CARET->direct)
	{
		if (CARET->ani_wait < 20) 
			CARET->y -= 0x200;

		if (CARET->ani_wait >= 80)
			CARET->cond = 0;
	}
	else
	{
		if (CARET->ani_wait < 20) 
			CARET->y -= 0x400;

		if (CARET->ani_wait >= 80)
			CARET->cond = 0;
	}

	if (CARET->direct)
	{
		CARET->rect = rcRight[CARET->ani_wait / 2 % 2];
	}
	else
	{
		CARET->rect = rcLeft[CARET->ani_wait / 2 % 2];
	}
}

void caretAct011(caret *CARET) //Damage effect
{
	//Move
	if (!CARET->act_no)
	{
		CARET->act_no = 1;

		int deg = random(0, 255);

		CARET->xm = 2 * getCos(deg);
		CARET->ym = 2 * getSin(deg);
	}
	
	CARET->x += CARET->xm;
	CARET->y += CARET->ym;

	//Animate
	if (++CARET->ani_wait > 2)
	{
		CARET->ani_wait = 0;

		if (++CARET->ani_no > 6)
			CARET->cond = 0;
	}

	CARET->rect = { 64 + (CARET->ani_no << 3), 8, 72 + (CARET->ani_no << 3), 16 };
}

void caretAct012(caret *CARET) //White "explosion" disc
{
	RECT rcLeft[2];

	rcLeft[0] = { 112, 0, 144, 32 };
	rcLeft[1] = { 144, 0, 176, 32 };
	
	if (++CARET->ani_wait > 2)
	{
		CARET->ani_wait = 0;

		if (++CARET->ani_no > 1)
			CARET->cond = 0;
	}

	CARET->rect = rcLeft[CARET->ani_no];
}

void caretAct013(caret *CARET) //Headbump sparks
{
	RECT rcLeft[2];

	rcLeft[0] = { 56, 24, 64, 32 };
	rcLeft[1] = { 0, 0, 0, 0 };
	
	if (!CARET->act_no)
	{
		CARET->act_no = 1;
		
		if (CARET->direct)
		{
			if (CARET->direct == 1)
				CARET->ym = -0x200 * random(1, 3);
		}
		else
		{
			CARET->xm = random(-0x600, 0x600);
			CARET->ym = random(-0x200, 0x200);
		}
	}

	if (!CARET->direct)
	{
		CARET->xm = 4 * CARET->xm / 5;
		CARET->ym = 4 * CARET->ym / 5;
	}

	CARET->x += CARET->xm;
	CARET->y += CARET->ym;

	//Destroy
	if (++CARET->ani_wait > 20)
		CARET->cond = 0;

	CARET->rect = rcLeft[CARET->ani_wait / 2 % 2];
}

void caretAct015(caret *CARET) //Small white pop
{
	RECT rcLeft[4];

	rcLeft[0] = { 0, 72, 8, 80 };
	rcLeft[1] = { 8, 72, 16, 80 };
	rcLeft[2] = { 16, 72, 24, 80 };
	rcLeft[3] = { 24, 72, 32, 80 };

	if (++CARET->ani_wait > 2)
	{
		CARET->ani_wait = 0;

		if (++CARET->ani_no > 3)
			CARET->cond = 0;
	}

	CARET->rect = rcLeft[CARET->ani_no];
}

void caretAct016(caret *CARET) //EMPTY
{
	RECT rcLeft[2]; // [esp+0h] [ebp-20h]

	rcLeft[0] = { 104, 96, 144, 104 };
	rcLeft[1] = { 104, 104, 144, 112 };

	if (++CARET->ani_wait < 10)
		CARET->y -= 1024;

	if (CARET->ani_wait >= 40)
		CARET->cond = 0;

	CARET->rect = rcLeft[CARET->ani_wait / 2 % 2];
}

void caretAct017(caret *CARET) //PUSH JUMP KEY!
{
	if (++CARET->ani_wait > 39)
		CARET->ani_wait = 0;

	if (CARET->ani_wait > 29)
		CARET->rect = { 0, 0, 0, 0 };
	else
		CARET->rect = { 0, 144, 144, 152 };
}

caretAct caretActs[] = {
	&caretAct000,
	&caretAct001,
	&caretAct002,
	&caretAct003,
	&caretAct004,
	&caretAct005,
		&caretAct004, //unused, apparently uses caret 4
	&caretAct007,
	&caretAct008,
	&caretAct009,
	&caretAct010,
	&caretAct011,
	&caretAct012,
	&caretAct013,
		&caretAct000, //unused
	&caretAct015,
	&caretAct016,
	&caretAct017,
};

//Init
void caret::init(int setX, int setY, int setType, int setDir)
{
	memset(this, 0, sizeof(*this));

	cond = 0x80;
	code = setType;

	x = setX;
	y = setY;

	direct = setDir;

	view_left = caretStats[code].offsetX;
	view_top = caretStats[code].offsetY;
}

//Update code
void caret::update()
{
	caretActs[code](this);
}

void caret::draw()
{
	drawTexture(sprites[0x13], &rect, (x - view_left) / 0x200 - viewX / 0x200, (y - view_top) / 0x200 - viewY / 0x200);
}

//Create code
void createCaret(int setX, int setY, int setType, int setDir)
{
	caret newCaret;
	newCaret.init(setX, setY, setType, setDir);
	carets.push_back(newCaret);
}