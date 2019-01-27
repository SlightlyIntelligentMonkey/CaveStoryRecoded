#include "script.h"

#include <string>
#include <cstring>
#include <cstdlib>
#include <SDL_messagebox.h>
#include <SDL_rwops.h>
#include <SDL_render.h>
#include "fade.h"
#include "weapons.h"
#include "render.h"
#include "sound.h"
#include "boss.h"
#include "flags.h"
#include "npc.h"
#include "filesystem.h"
#include "fade.h"
#include "game.h"
#include "player.h"
#include "input.h"
#include "org.h"
#include "main.h"
#include "stage.h"
#include "flash.h"

using std::string;

//Variables
TSC tsc;
char tscText[0x100];
int tscNumber[4];
uint8_t *tscTextFlag = new uint8_t[0x100];

//Mode enum
enum TSC_mode
{
	END = 0,
	PARSE = 1,
	NOD = 2,
	SCROLL = 3,
	WAI = 4,
	FADE = 5,
	YNJ = 6,
	WAS = 7
};

//Init function
bool initTsc()
{
	tsc.mode = 0;
	gGameFlags &= ~4;

	memset(tscText, 0, 0x100);
	memset(tscTextFlag, 0, 0x100);

	tsc.data = new uint8_t[0x5000];
	return tsc.data != nullptr;
}

//Loading functions
void decryptTsc(uint8_t *data, size_t size)
{
	if (data == nullptr)
		doCustomError("data was nullptr in decryptTsc");

	const size_t half = size / 2;
	uint8_t key = data[half];

	if (!key)
		key = 7;

	for (size_t i = 0; i < size; ++i)
	{
		if (i != half)
			data[i] -= key;
	}
}

void loadStageTsc(const string& name)
{
	//Load Head.tsc file
	SDL_RWops *headRW = SDL_RWFromFile("data/Head.tsc", "rb");
	if (headRW == nullptr)
		doError();
	auto headSize = static_cast<size_t>(SDL_RWsize(headRW));

	//Put the data into memory
	headRW->read(headRW, tsc.data, 1, headSize);
	decryptTsc(tsc.data, headSize);
	tsc.data[headSize] = 0;

	headRW->close(headRW);

	//Load stage's tsc file
	SDL_RWops *bodyRW = SDL_RWFromFile(("data/Stage/" + name + ".tsc").c_str(), "rb");
	if (!bodyRW)
		doError();
	auto bodySize = static_cast<size_t>(SDL_RWsize(bodyRW));

	//Put the data into memory
	bodyRW->read(bodyRW, tsc.data + headSize, 1, bodySize);
	decryptTsc(tsc.data + headSize, bodySize);
	tsc.data[headSize + bodySize] = 0;

	bodyRW->close(bodyRW);

	//Finish off by setting some stuff in the tsc struct
	tsc.size = static_cast<int>(headSize + bodySize);
	tsc.path = name;
}

void loadTsc2(const string& name)
{
	//Load tsc file
	SDL_RWops *bodyRW = SDL_RWFromFile(name.c_str(), "rb");
	if (!bodyRW)
		doError();
	tsc.size = static_cast<decltype(tsc.size)>(SDL_RWsize(bodyRW));

	//Put the data into memory
	bodyRW->read(bodyRW, tsc.data, 1, tsc.size);
	decryptTsc(tsc.data, tsc.size);
	tsc.data[tsc.size] = 0;

	bodyRW->close(bodyRW);

	//Finish off by setting some stuff in the tsc struct
	tsc.path = name;
}

//Get number function
attrPure int getTSCNumber(TSC &ptsc, int a);
int getTSCNumber(TSC &ptsc, int a)
{
	return			(static_cast<char>(ptsc.data[a + 3]) - 0x30) +
	                10 *	(static_cast<char>(ptsc.data[a + 2]) - 0x30) +
	                100 *	(static_cast<char>(ptsc.data[a + 1]) - 0x30) +
	                1000 *	(static_cast<char>(ptsc.data[a]) - 0x30);
}

void tscClearText()
{
	for (int i = 0; i < 4; ++i)
	{
		tsc.ypos_line[i] = 16 * i;

		memset(tscText + (i * 0x40), 0, 0x40);
		memset(tscTextFlag + (i * 0x40), 0, 0x40);
	}
}

//TSC run event functions
int startTscEvent(TSC &ptsc, int no)
{
	ptsc.mode = 1;
	gGameFlags |= 5;
	ptsc.line = 0;
	ptsc.p_write = 0;
	ptsc.wait = 4;
	ptsc.flags = 0;
	ptsc.wait_beam = 0;
	ptsc.face = 0;
	ptsc.item = 0;
	ptsc.offsetY = 0;
	currentPlayer.shock = 0;

	ptsc.rcText = { 52, 184, 268, 234 };

	//Get event id
	int event_no;
	for (ptsc.p_read = 0; ; ++ptsc.p_read)
	{
		if (!ptsc.data[ptsc.p_read])
			return 0;
		if (ptsc.data[ptsc.p_read] != 35)
			continue;

		event_no = getTSCNumber(tsc, ++ptsc.p_read);
		if (no == event_no)
			break;
		if (no < event_no)
			return 0;
	}

	while (ptsc.data[ptsc.p_read] != '\n')
		++ptsc.p_read;
	++ptsc.p_read;
	return 1;
}

int jumpTscEvent(TSC &ptsc, int no)
{
	ptsc.mode = 1;
	gGameFlags |= 4;
	ptsc.line = 0;
	ptsc.p_write = 0;
	ptsc.wait = 4;
	ptsc.wait_beam = 0;

	tscClearText();

	//Get event id
	int event_no;
	for (ptsc.p_read = 0; ; ++ptsc.p_read)
	{
		if (!ptsc.data[ptsc.p_read])
			return 0;
		if (ptsc.data[ptsc.p_read] != 35)
			continue;

		event_no = getTSCNumber(tsc, ++ptsc.p_read);
		if (no == event_no)
			break;
		if (no < event_no)
			return 0;
	}

	while (ptsc.data[ptsc.p_read] != '\n')
		++ptsc.p_read;
	++ptsc.p_read;
	return 1;
}

void stopTsc(TSC &ptsc)
{
	ptsc.mode = 0;
	gGameFlags &= ~4;
	gGameFlags |= 3u;
	ptsc.flags = 0;
}

//Check new line
void checkNewLine(TSC &ptsc)
{
	if (ptsc.ypos_line[ptsc.line % 4] == 48)
	{
		ptsc.mode = 3;
		gGameFlags |= 4u;
		memset(tscText + (ptsc.line % 4 << 6), 0, 0x40);
		memset(tscTextFlag + (ptsc.line % 4 << 6), 0, 0x40);
	}
}

void clearTextLine(TSC &ptsc)
{
	//Reset current writing position
	ptsc.line = 0;
	ptsc.p_write = 0;
	ptsc.offsetY = 0;

	//Go through each line and clear it
	for (int i = 0; i < 4; ++i)
	{
		ptsc.ypos_line[i] = 16 * i;

		memset(tscText + (i * 40), 0, 0x40);
		memset(tscTextFlag + (i * 40), 0, 0x40);
	}
}

//TSC Update
void tscCheck(const TSC &ptsc)
{
	//End tsc if in END state, continue if not
	if (ptsc.mode)
		gGameFlags |= 4;
	else
		gGameFlags &= ~4;
}

void tscCleanup(int numargs, TSC &ptsc)  //Function to shift the current read position after a command
{
	ptsc.p_read += 4 + (numargs * 4);

	if (numargs > 1)
		ptsc.p_read += (numargs - 1);
}

void tscPutNumber(TSC &ptsc, int index)
{
	int table[3];
	char str[5];
	bool bZero;
	int offset;
	table[0] = 1000;
	table[1] = 100;
	table[2] = 10;
	int a = tscNumber[index];
	bZero = 0;
	offset = 0;
	for (int i = 0; i <= 2; ++i)
	{
		if (a / table[i] || bZero)
		{
			const int b = a / table[i];
			str[offset] = b + 48;
			bZero = 1;
			a -= b * table[i];
			++offset;
		}
	}
	str[offset] = a + 48;
	str[offset + 1] = 0;
	strcat(tscText + (ptsc.line % 4 << 6), str);
	playSound(2);
	ptsc.wait_beam = 0;
	ptsc.p_write += strlen(str);
	if (ptsc.p_write > 34)
	{
		ptsc.p_write = 0;
		++ptsc.line;
		checkNewLine(ptsc);
	}
}

static inline void showTSCNotImplementedWarning(const string& message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Unimplemented command", message.c_str(), nullptr);
	logWarning(message);
}

int doTscModes(bool *bExit, TSC &ptsc)
{
	switch (ptsc.mode)
	{
	case END:
		tscCheck(ptsc);
		return 32;

	case PARSE:
		//Add to timer
		++ptsc.wait;

		//If jump or shoot are held, add 4, effectively ending the timer early
		if (!(gGameFlags & 2) && (isKeyDown(gKeyShoot) || isKeyDown(gKeyJump)))
			ptsc.wait += 4;

		//If timer value is less than 4, stop for this frame.
		if (ptsc.wait < 4)
        {
			tscCheck(ptsc);
            return 1;
        }

		//Reset timer and continue into parsing
		ptsc.wait = 0;
		*bExit = 0;
		return 0;

	case NOD:
		//If jump or shoot button pressed, continue script
		if (isKeyPressed(gKeyShoot) || isKeyPressed(gKeyJump))
			ptsc.mode = PARSE;

		tscCheck(ptsc);
		return 1;

	case SCROLL:
		//Go through every line
		for (auto& i : ptsc.ypos_line)
		{
			i -= 4;
			if (!i) //Check if done scrolling
				ptsc.mode = PARSE; //Continue like normal

			if (i == -16) //Check if scrolled off
				i = 48;
		}

		tscCheck(ptsc);
		return 1;

	case WAI:
		//These two checks are for <WAI9999 to fully freeze the script
		if (ptsc.wait_next != 9999)
		{
			if (ptsc.wait != 9999)
				++ptsc.wait;

			if (ptsc.wait >= ptsc.wait_next) //If waited enough frames, continue
			{
				ptsc.mode = PARSE;
				ptsc.wait_beam = 0;
			}
		}

		tscCheck(ptsc);
		return 1;

	case FADE:
		if (fade.mode == 0) //Wait until fade has ended
		{
			//Continue script
			ptsc.mode = PARSE;
			ptsc.wait_beam = 0;
		}
		tscCheck(ptsc);
		return 1;

	case YNJ:
		if (ptsc.wait >= 16) //Do nothing for 16 frames
		{
			if (isKeyPressed(gKeyJump)) //Select button pressed
			{
				//Play selection sound
				playSound(SFX_YNConfirm);

				if (ptsc.select)
				{
					jumpTscEvent(ptsc, ptsc.next_event); //Jump to specified event
				}
				else if (!ptsc.select)
				{
					//Continue like normal
					ptsc.mode = PARSE;
					ptsc.wait_beam = 0;
				}
				else
					doCustomError("Invalid YNJ result");
			}
			else if (isKeyPressed(gKeyLeft)) //Left pressed
			{
				ptsc.select = 0; //Select yes and play sound
				playSound(SFX_YNChangeChoice);
			}
			else if (isKeyPressed(gKeyRight)) //Right pressed
			{
				ptsc.select = 1; //Select no and play sound
				playSound(SFX_YNChangeChoice);
			}
		}
		else
		{
			++ptsc.wait; //Add to wait counter
		}

		tscCheck(ptsc);
		return 1;

	case WAS:
		//Wait until on the ground
		if (currentPlayer.flag & ground)
		{
			//Go into parse mode
			ptsc.mode = PARSE;
			ptsc.wait_beam = 0;
		}

		tscCheck(ptsc);
		return 1;

	default:
		return 0;
	}
}

void endTsc(TSC &ptsc, bool *bExit)
{
	ptsc.mode = END;
	currentPlayer.cond &= ~player_interact;
	gGameFlags |= 3;
	ptsc.face = 0;
	*bExit = true;
	return;
}

bool doTscCommand(int *retVal, bool *bExit, TSC &ptsc)
{
	int xt;
	int yt;
	int w, x, y, z;	// TODO: Merge with the above variables

	static bool notifiedAboutCIL = false;
	static bool notifiedAboutCPS = false;
	static bool notifiedAboutCRE = false;
	static bool notifiedAboutCSS = false;
	static bool notifiedAboutINP = false;
	static bool notifiedAboutSIL = false;
	static bool notifiedAboutSPS = false;
	static bool notifiedAboutSSS = false;
	static bool notifiedAboutSTC = false;
	static bool notifiedAboutXX1 = false;

	//Parse and run TSC commands
	switch (ptsc.data[ptsc.p_read + 3] + (ptsc.data[ptsc.p_read + 2] << 8) + (ptsc.data[ptsc.p_read + 1] << 16) + (ptsc.data[ptsc.p_read] << 24))
	{
	case('<AE+'):
		maxWeaponAmmo();
		tscCleanup(0, ptsc);
		break;
	case('<AM+'):
		tscNumber[0] = getTSCNumber(ptsc, ptsc.p_read + 9);
		tscNumber[1] = 0;
		playSound(SFX_ItemGet);
		giveWeapon(getTSCNumber(ptsc, ptsc.p_read + 4), getTSCNumber(ptsc, ptsc.p_read + 9));
		tscCleanup(2, ptsc);
		break;
	case('<AM-'):
		removeWeapon(getTSCNumber(ptsc, ptsc.p_read + 4));
		tscCleanup(1, ptsc);
		break;
	case('<AMJ'):
		if (checkWeapon(getTSCNumber(ptsc, ptsc.p_read + 4)))
			jumpTscEvent(ptsc, getTSCNumber(ptsc, ptsc.p_read + 9));
		else
			tscCleanup(2, ptsc);
		break;
	case('<ANP'):
        setNPCState(getTSCNumber(ptsc, ptsc.p_read + 4), getTSCNumber(ptsc, ptsc.p_read + 9), getTSCNumber(ptsc, ptsc.p_read + 14));

		tscCleanup(3, ptsc);
		break;
	case('<BOA'):
		setBossAction(getTSCNumber(ptsc, ptsc.p_read + 4));
		tscCleanup(1, ptsc);
		break;
	case('<BSL'):
		if (getTSCNumber(ptsc, ptsc.p_read + 4))
		{
			for (size_t i = 0; i < gNPC.size(); i++)
			{
				if (gNPC[i].code_event == getTSCNumber(ptsc, ptsc.p_read + 4))
				{
					gBossLife.flag = 1;
					gBossLife.max = gNPC[i].life;
					gBossLife.br = gNPC[i].life;
					gBossLife.pLife = &gNPC[i].life;
					break;
				}
			}
		}
		else
		{
			gBossLife.flag = 1;
			gBossLife.max = gBossObj[0].life;
			gBossLife.br = gBossObj[0].life;
			gBossLife.pLife = &gBossObj[0].life;
		}

		tscCleanup(1, ptsc);
		break;
	case('<BXL'):
		setFlash(getTSCNumber(ptsc, ptsc.p_read + 4)<<13, getTSCNumber(ptsc, ptsc.p_read + 9)<<13, flashModes::explosion, 0);
		tscCleanup(2, ptsc);
		break;
	case('<CAT'):
		ptsc.flags |= 0x40u;
		tscCleanup(0, ptsc);
		break;
	case('<CIL'):
		if (!notifiedAboutCIL && debugFlags & notifyOnNotImplemented)
		{
			notifiedAboutCIL = true;
			showTSCNotImplementedWarning("<CIL is not implemented");
		}

		tscCleanup(0, ptsc);
		break;
	case('<CLO'):
		ptsc.flags &= ~0x33;
		tscCleanup(0, ptsc);
		break;
	case('<CLR'):
		clearTextLine(ptsc);
		tscCleanup(0, ptsc);
		break;
	case('<CMP'):
		changeTile(getTSCNumber(ptsc, ptsc.p_read + 4), getTSCNumber(ptsc, ptsc.p_read + 9),
		           getTSCNumber(ptsc, ptsc.p_read + 14));
		tscCleanup(3, ptsc);
		break;
	case('<CMU'):
		changeOrg(getTSCNumber(ptsc, ptsc.p_read + 4));

		tscCleanup(1, ptsc);
		break;
	case('<CNP'):
		changeNpc(getTSCNumber(ptsc, ptsc.p_read + 4),
		          getTSCNumber(ptsc, ptsc.p_read + 9),
		          getTSCNumber(ptsc, ptsc.p_read + 14));
		tscCleanup(3, ptsc);
		break;
	case('<CPS'):
		if (!notifiedAboutCPS && debugFlags & notifyOnNotImplemented)
		{
			notifiedAboutCPS = true;
			showTSCNotImplementedWarning("<CPS is not implemented");
		}

		tscCleanup(0, ptsc);
		break;
	case('<CRE'):
		changeOrg(mus_TheWayBackHome); //because i smelly
		if (!notifiedAboutCRE && debugFlags & notifyOnNotImplemented)
		{
			notifiedAboutCRE = true;
			showTSCNotImplementedWarning("<CRE is not implemented");
		}

		tscCleanup(0, ptsc);
		break;
	case('<CSS'):
		if (!notifiedAboutCSS && debugFlags & notifyOnNotImplemented)
		{
			notifiedAboutCSS = true;
			showTSCNotImplementedWarning("<CSS is not implemented");
		}

		tscCleanup(0, ptsc);
		break;
	case('<DNA'):
		for (size_t i = 0; i < gNPC.size(); i++)
		{
			if (gNPC[i].cond & npccond_alive)
			{
				if (gNPC[i].code_char == getTSCNumber(ptsc, ptsc.p_read + 4))
				{
					gNPC[i].cond = 0;
					setFlag(gNPC[i].code_flag);
				}
			}
		}
		tscCleanup(1, ptsc);
		break;
	case('<DNP'):
		for (size_t i = 0; i < gNPC.size(); i++)
		{
			if (gNPC[i].cond & npccond_alive)
			{
				if (gNPC[i].code_event == getTSCNumber(ptsc, ptsc.p_read + 4))
				{
					gNPC[i].cond = 0;
					setFlag(gNPC[i].code_flag);
				}
			}
		}
		tscCleanup(1, ptsc);
		break;
	case('<ECJ'):
		for (size_t n = 0; n < gNPC.size(); n++)
		{
			if (gNPC[n].cond & npccond_alive)
			{
				if (gNPC[n].code_char == getTSCNumber(ptsc, ptsc.p_read + 4))
				{
					jumpTscEvent(ptsc, getTSCNumber(ptsc, ptsc.p_read + 9));
					break;
				}
			}
		}
		break;
	case('<END'):
		endTsc(ptsc, bExit);
		break;
	case('<EQ+'):
		currentPlayer.equip |= getTSCNumber(ptsc, ptsc.p_read + 4);
		tscCleanup(1, ptsc);
		break;
	case('<EQ-'):
		currentPlayer.equip &= ~getTSCNumber(ptsc, ptsc.p_read + 4);
		tscCleanup(1, ptsc);
		break;
	case('<ESC'):
		*retVal = 2;
		return true;
	case('<EVE'):
		jumpTscEvent(ptsc, getTSCNumber(ptsc, ptsc.p_read + 4));
		break;
	case('<FAC'):
		if (ptsc.face != getTSCNumber(ptsc, ptsc.p_read + 4))
		{
			ptsc.face_x = -48;
			ptsc.face = getTSCNumber(ptsc, ptsc.p_read + 4);
		}
		tscCleanup(1, ptsc);
		break;
	case('<FAI'):
		fade.mode = 1;
		fade.dir = getTSCNumber(ptsc, ptsc.p_read + 4);
		fade.count = 0;
		ptsc.mode = FADE;
		tscCleanup(1, ptsc);
		*bExit = true;
		break;
	case('<FAO'):
		fade.mode = 2;
		fade.dir = getTSCNumber(ptsc, ptsc.p_read + 4);
		fade.count = 0;
		ptsc.mode = FADE;
		tscCleanup(1, ptsc);
		*bExit = true;
		break;
	case('<FL+'):
		setFlag(getTSCNumber(ptsc, ptsc.p_read + 4));
		tscCleanup(1, ptsc);
		break;
	case('<FL-'):
		clearFlag(getTSCNumber(ptsc, ptsc.p_read + 4));
		tscCleanup(1, ptsc);
		break;
	case('<FLA'):
		setFlash(0, 0, flashModes::normal, 20);
		tscCleanup(0, ptsc);
		break;
	case('<FLJ'):
		if (getFlag(getTSCNumber(ptsc, ptsc.p_read + 4)))
			jumpTscEvent(ptsc, getTSCNumber(ptsc, ptsc.p_read + 9));
		else
			tscCleanup(2, ptsc);
		break;
	case('<FMU'):
		gOrgFadeout = true;
		tscCleanup(0, ptsc);
		break;
	case('<FOB'):
		x = getTSCNumber(ptsc, ptsc.p_read + 4);
		y = getTSCNumber(ptsc, ptsc.p_read + 9);
		SetFrameTargetBoss(x, y);
		tscCleanup(2, ptsc);
		break;
	case('<FOM'):
		z = getTSCNumber(ptsc, ptsc.p_read + 4);
		SetFrameTargetMyChar(z);
		tscCleanup(1, ptsc);
		break;
	case('<FON'):
		x = getTSCNumber(ptsc, ptsc.p_read + 4);
		y = getTSCNumber(ptsc, ptsc.p_read + 9);
		SetFrameTargetNpChar(x, y);
		tscCleanup(2, ptsc);
		break;
	case('<FRE'):
		gGameFlags |= 3;
		tscCleanup(0, ptsc);
		break;
	case('<GIT'):
		ptsc.item = getTSCNumber(ptsc, ptsc.p_read + 4);
		ptsc.item_y = 128;
		tscCleanup(1, ptsc);
		break;
	case('<HMC'):
		currentPlayer.cond &= ~player_visible;
		tscCleanup(0, ptsc);
		break;
	case('<INI'):
		initGame();
		*retVal = 1;
		return true;
	case('<INP'):
		if (!notifiedAboutINP && debugFlags & notifyOnNotImplemented)
		{
			notifiedAboutINP = true;
			showTSCNotImplementedWarning("<INP is not implemented");
		}

		tscCleanup(3, ptsc);
		break;
	case('<IT+'):
		playSound(SFX_ItemGet);
		for (xt = 0; xt < ITEMS && gItems[xt].code != getTSCNumber(ptsc, ptsc.p_read + 4) && gItems[xt].code; ++xt);
		if (xt == ITEMS)
		{
			tscCleanup(1, ptsc);
			break;
		}

		gItems[xt].code = getTSCNumber(ptsc, ptsc.p_read + 4);
		tscCleanup(1, ptsc);
		break;
	case('<IT-'):
		for (xt = 0; xt < ITEMS && gItems[xt].code != getTSCNumber(ptsc, ptsc.p_read + 4); ++xt);
		if (xt == ITEMS)
		{
			tscCleanup(1, ptsc);
			break;
		}

		for (yt = xt + 1; yt <= 31; ++yt)
			gItems[yt - 1] = gItems[yt];
		gItems[yt - 1].code = 0;

		gSelectedItem = 0;
		tscCleanup(1, ptsc);
		break;
	case('<ITJ'):
		for (xt = 0; xt < ITEMS && gItems[xt].code != getTSCNumber(ptsc, ptsc.p_read + 4); ++xt);
		if (xt == ITEMS)
			tscCleanup(2, ptsc);
		else
			jumpTscEvent(ptsc, getTSCNumber(ptsc, ptsc.p_read + 9));
		break;
	case('<KEY'):
		gGameFlags |= 1;
		gGameFlags &= ~2;
		currentPlayer.up = false;
		tscCleanup(0, ptsc);
		break;
	case('<LDP'):
		loadProfile();
		break;
	case('<LI+'):
		currentPlayer.life += getTSCNumber(ptsc, ptsc.p_read + 4);
		if (currentPlayer.life > currentPlayer.max_life)
			currentPlayer.life = currentPlayer.max_life;
		tscCleanup(1, ptsc);
		break;
	case('<ML+'):
		currentPlayer.max_life += getTSCNumber(ptsc, ptsc.p_read + 4);
		currentPlayer.life += getTSCNumber(ptsc, ptsc.p_read + 4);
		tscCleanup(1, ptsc);
		break;
	case('<MLP'):
		tscCleanup(0, ptsc);
		*bExit = true;
		xt = openMapSystem();
		if (!xt)
        {
            *retVal = 0;
            return true;
        }
		if (xt == 2)
        {
            *retVal = 2;
            return true;
        }
		break;
	case('<MM0'):
		currentPlayer.xm = 0;
		tscCleanup(0, ptsc);
		break;
	case('<MNA'):
		gMapName.wait = 0;
		gMapName.flag = 1;
		tscCleanup(0, ptsc);
		break;
	case('<MNP'):
        moveNPC(getTSCNumber(ptsc, ptsc.p_read + 4), getTSCNumber(ptsc, ptsc.p_read + 9) << 13, getTSCNumber(ptsc, ptsc.p_read + 14) << 13,
                getTSCNumber(ptsc, ptsc.p_read + 19));
		tscCleanup(4, ptsc);
		break;
	case('<MOV'):
		currentPlayer.setPos(tilesToUnits(getTSCNumber(ptsc, ptsc.p_read + 4)), tilesToUnits(getTSCNumber(ptsc, ptsc.p_read + 9)));
		tscCleanup(2, ptsc);
		break;
	case('<MPJ'):
		if (getMapFlag(gCurrentLevel))
			jumpTscEvent(ptsc, getTSCNumber(ptsc, ptsc.p_read + 4));
		else
			tscCleanup(1, ptsc);
		break;
	case('<MP+'):
		setMapFlag(getTSCNumber(ptsc, ptsc.p_read + 4));
		tscCleanup(1, ptsc);
		break;
	case('<MS2'):
		clearTextLine(ptsc);
		ptsc.flags |= 0x21;
		ptsc.flags &= ~0x12;
		if (ptsc.flags & 0x40)
			ptsc.flags |= 0x10u;
		ptsc.face = 0;
		tscCleanup(0, ptsc);
		*bExit = true;
		break;
	case('<MS3'):
		clearTextLine(ptsc);
		ptsc.flags |= 0x23;
		ptsc.flags &= ~0x10;
		if (ptsc.flags & 0x40)
			ptsc.flags |= 0x10u;
		tscCleanup(0, ptsc);
		*bExit = true;
		break;
	case('<MSG'):
		clearTextLine(ptsc);
		ptsc.flags |= 3;
		ptsc.flags &= ~0x30;
		if (ptsc.flags & 0x40)
			ptsc.flags |= 0x10u;
		tscCleanup(0, ptsc);
		*bExit = true;
		break;
	case('<MYB'):
        currentPlayer.backStep(getTSCNumber(ptsc, ptsc.p_read + 4));

		tscCleanup(1, ptsc);
		break;
	case('<MYD'):
		currentPlayer.setDir(getTSCNumber(ptsc, ptsc.p_read + 4));
		tscCleanup(1, ptsc);
		break;
	case('<NCJ'):
		xt = getTSCNumber(ptsc, ptsc.p_read + 4);
		yt = getTSCNumber(ptsc, ptsc.p_read + 9);

		if (findEntityByType(xt) != -1)
			jumpTscEvent(ptsc, yt);
		else
			tscCleanup(2, ptsc);
		break;
	case('<NOD'):
		ptsc.mode = NOD;
		tscCleanup(0, ptsc);
		*retVal = 1;
		return true;
	case('<NUM'):
		tscPutNumber(ptsc, getTSCNumber(ptsc, ptsc.p_read + 4));
		tscCleanup(1, ptsc);
		break;
	case('<PRI'):
		gGameFlags &= ~3;
		tscCleanup(0, ptsc);
		break;
	case('<PS+'):
		for (xt = 0; xt < 8 && gPermitStage[xt].index != getTSCNumber(ptsc, ptsc.p_read + 4) && gPermitStage[xt].index; ++xt);
		if (xt == 8)
			break;

		gPermitStage[xt].index = getTSCNumber(ptsc, ptsc.p_read + 4);
		gPermitStage[xt].event = getTSCNumber(ptsc, ptsc.p_read + 9);
		tscCleanup(2, ptsc);
		break;
	case('<QUA'):
		gViewport.quake = getTSCNumber(ptsc, ptsc.p_read + 4);
		tscCleanup(1, ptsc);
		break;
	case('<RMU'):
		resumeOrg();
		tscCleanup(0, ptsc);
		break;
	case('<SAT'):
		ptsc.flags |= 0x40u;	// <SAT is same as <CAT
		tscCleanup(0, ptsc);
		break;
	case('<SFC'):
		setFlashColor(
			getTSCNumber(ptsc, ptsc.p_read + 4),
			getTSCNumber(ptsc, ptsc.p_read + 9),
			getTSCNumber(ptsc, ptsc.p_read + 14),
			getTSCNumber(ptsc, ptsc.p_read + 19));
		tscCleanup(4, ptsc);
		break;
	case('<SIL'):
		if (!notifiedAboutSIL && debugFlags & notifyOnNotImplemented)
		{
			notifiedAboutSIL = true;
			showTSCNotImplementedWarning("<SIL is not implemented");
		}

		tscCleanup(1, ptsc);
		break;
	case('<SK+'):
		setSkipFlag(getTSCNumber(ptsc, ptsc.p_read + 4));
		tscCleanup(1, ptsc);
		break;
	case('<SK-'):
		clearSkipFlag(getTSCNumber(ptsc, ptsc.p_read + 4));
		tscCleanup(1, ptsc);
		break;
	case('<SKJ'):
		if (getSkipFlag(getTSCNumber(ptsc, ptsc.p_read + 4)))
			jumpTscEvent(ptsc, getTSCNumber(ptsc, ptsc.p_read + 9));
		else
			tscCleanup(2, ptsc);
		break;
	case('<SLP'):
		*bExit = true;
		yt = stageSelect(&xt);
		if (!yt)
        {
            *retVal = 0;
            return true;
        }
		if (yt == 2)
        {
            *retVal = 2;
            return true;
        }
		jumpTscEvent(ptsc, xt);
		gGameFlags &= ~3;
		break;
	case('<SMC'):
		currentPlayer.cond |= player_visible;
		tscCleanup(0, ptsc);
		break;
	case('<SMP'):
		shiftTile(getTSCNumber(ptsc, ptsc.p_read + 4), getTSCNumber(ptsc, ptsc.p_read + 9));
		tscCleanup(2, ptsc);
		break;
	case('<SNP'):

		createNpc(
		    getTSCNumber(ptsc, ptsc.p_read + 4),
		    tilesToUnits(getTSCNumber(ptsc, ptsc.p_read + 9)),
		    tilesToUnits(getTSCNumber(ptsc, ptsc.p_read + 14)),
		    0,
		    0,
		    getTSCNumber(ptsc, ptsc.p_read + 19));
		tscCleanup(4, ptsc);
		break;
	case('<SOU'):
		playSound(getTSCNumber(ptsc, ptsc.p_read + 4));
		tscCleanup(1, ptsc);
		break;
	case('<SPS'):
		if (!notifiedAboutSPS && debugFlags & notifyOnNotImplemented)
		{
			notifiedAboutSPS = true;
			showTSCNotImplementedWarning("<SPS is not implemented");
		}

		tscCleanup(0, ptsc);
		break;
	case('<SSS'):
		if (!notifiedAboutSSS && debugFlags & notifyOnNotImplemented)
		{
			notifiedAboutSSS = true;
			showTSCNotImplementedWarning("<SSS is not implemented");
		}

		tscCleanup(1, ptsc);
		break;
	case('<STC'):
		if (!notifiedAboutSTC && debugFlags & notifyOnNotImplemented)
		{
			notifiedAboutSTC = true;
			showTSCNotImplementedWarning("<STC is not implemented");
		}

		tscCleanup(0, ptsc);
		break;
	case('<SVP'):
		saveProfile();
		tscCleanup(0, ptsc);
		break;
	case('<TAM'):
		tradeWeapons(getTSCNumber(ptsc, ptsc.p_read + 4), getTSCNumber(ptsc, ptsc.p_read + 9), getTSCNumber(ptsc, ptsc.p_read + 14));
		tscCleanup(3, ptsc);
		break;
	case('<TRA'):
		z = getTSCNumber(ptsc, ptsc.p_read + 4);
		w = getTSCNumber(ptsc, ptsc.p_read + 9);
		x = getTSCNumber(ptsc, ptsc.p_read + 14);
		y = getTSCNumber(ptsc, ptsc.p_read + 19);
		loadLevel(z, w, x, y);
		*retVal = 1;
		return true;
	case('<TUR'):
		tscCleanup(0, ptsc);
		ptsc.flags |= 0x10;
		break;
	case('<UNI'):
		currentPlayer.unit = getTSCNumber(ptsc, ptsc.p_read + 4);
		tscCleanup(1, ptsc);
		break;
	case('<UNJ'):
		tscCleanup(2, ptsc);
		break;
	case('<WAI'):
		ptsc.mode = WAI;
		ptsc.wait = 0;
		ptsc.wait_next = getTSCNumber(ptsc, ptsc.p_read + 4);
		tscCleanup(1, ptsc);
		*bExit = true;
		break;
	case('<WAS'):
		ptsc.mode = WAS;
		tscCleanup(0, ptsc);
		*bExit = true;
		break;
	case('<XX1'):
		if (!notifiedAboutXX1 && debugFlags & notifyOnNotImplemented)
		{
			notifiedAboutXX1 = true;
			showTSCNotImplementedWarning("<XX1 is not implemented");
		}

		tscCleanup(1, ptsc);
		break;
	case('<YNJ'):
		ptsc.next_event = getTSCNumber(ptsc, ptsc.p_read + 4);
		ptsc.mode = YNJ;
		playSound(SFX_YNPrompt);
		ptsc.wait = 0;
		ptsc.select = 0;
		tscCleanup(1, ptsc);
		*bExit = true;
		break;
	case('<ZAM'):
		clearWeaponExperience();
		tscCleanup(0, ptsc);
		break;
	default:
		char wmsg[35] = { 0 };
		strncpy(wmsg, reinterpret_cast<char*>(ptsc.data + ptsc.p_read), 4);
		strcat(wmsg, " is not a valid tsc command!");
		showTSCNotImplementedWarning(wmsg);

		endTsc(ptsc, bExit);
		break;
	}
	return false;
}

int updateTsc(TSC &ptsc)
{
	bool bExit = false;
	char str[72];
	char c[3];

	const int mode = doTscModes(&bExit, ptsc);
    if (mode)
        return mode;

    while (true) //Parsing
	{
		if (bExit == true)
        {
			tscCheck(ptsc);
            return 1;
        }

		if (ptsc.data[ptsc.p_read] != '<')
		{
			if (tsc.data[tsc.p_read] == '\r') //Check for break line
			{
				//Shift read and write positions accordingly
				ptsc.p_read += 2;
				ptsc.p_write = 0;

				//Only move if message box visible?
				if (ptsc.flags & 1)
				{
					++ptsc.line;
					checkNewLine(ptsc);
				}
			}
			else if (ptsc.flags & 0x10) //CAT / SAT / TUR
			{
				//Go through until reaching a command indicator or break line
				int x;
				for (x = ptsc.p_read; ; ++x)
				{
					const bool quit = !(tsc.data[x] == '<' || tsc.data[x] == '\r');

					if (!quit)
						break;
					if ((ptsc.data[x] & 0x80) != 0)
						++x;
				}

				//Get data to copy from tsc data
				int copy = x - ptsc.p_read;
				memcpy(str, &ptsc.data[tsc.p_read], copy);
				str[copy] = 0;

				//Copy data onto the text buffer
				strcpy(tscText + (ptsc.line * 0x40), str);
				memset(tscTextFlag + (ptsc.line * 0x40), 1, copy); //This sets the flag that makes drawn text thinner.

				//Shift read and write positions
				ptsc.p_write = x; //Pixel uses an absolute value rather than a relative value
				ptsc.p_read += copy;

				//Check for new lines
				if (ptsc.p_write > 34)
					checkNewLine(ptsc);

				bExit = true;
			}
			else
			{
				//Code for multibyte things?
				c[0] = ptsc.data[ptsc.p_read];

				if (c[0] >= 0) // Isn't c unsigned ?
				{
					c[1] = 0;
				}
				else
				{
					c[1] = ptsc.data[ptsc.p_read + 1];
					c[2] = 0;
				}

				//Copy onto the text buffer
				strcat(tscText + (ptsc.line % 4 << 6), c);
				tscTextFlag[(ptsc.line % 4 << 6) + ptsc.p_write] = 0; //Make text display at normal width

				//Make equal signs draw as the circle thing
				if (c[0] == '=')
					tscTextFlag[(ptsc.line % 4 << 6) + ptsc.p_write] |= 2;

				//Play sound and reset cursor blinking timer.
				playSound(SFX_MessageTyping);
				ptsc.wait_beam = 0;

				//Shift read and write positions
				if (c[0] >= 0)	// Isn't c unsigned ?
				{
					++ptsc.p_read;
					++ptsc.p_write;
				}
				else
				{
					ptsc.p_read += 2;
					ptsc.p_write += 2;
				}

				//Check for new lines
				if (ptsc.p_write > 34)
				{
					checkNewLine(ptsc);
					ptsc.p_write = 0;
					++ptsc.line;
					checkNewLine(ptsc);
				}

				bExit = true;
			}
		}
		else
		{
		    int retVal;
            if (doTscCommand(&retVal, &bExit, ptsc))
                return retVal;
		}
	}
}

void drawTsc()
{
	RECT rcFrame1;
	RECT rcFrame2;
	RECT rcFrame3;

	RECT rcItemBox1;
	RECT rcItemBox2;
	RECT rcItemBox3;
	RECT rcItemBox4;
	RECT rcItemBox5;

	RECT rcItem;
	RECT rcFace;

	RECT rcYesNo;
	RECT rcSelection;

	if (tsc.mode && tsc.flags & 1)
	{
		//Set cliprect
		tsc.rcText.left = (gScreenWidth / 2) - 108;
		tsc.rcText.right = (gScreenWidth / 2) + 108;

		if (tsc.flags & 0x20)
		{
			tsc.rcText.top = 32;
			tsc.rcText.bottom = 80;
		}
		else
		{
			tsc.rcText.top = gScreenHeight - 56;
			tsc.rcText.bottom = gScreenHeight - 8;
		}

		//Draw message box background (if not MS2)
		if (tsc.flags & 2)
		{
			rcFrame1 = { 0, 0, 244, 8 };
			rcFrame2 = { 0, 8, 244, 16 };
			rcFrame3 = { 0, 16, 244, 24 };

			drawTexture(gSprites[TEX_TEXTBOX], &rcFrame1, tsc.rcText.left - 14, tsc.rcText.top - 10);

			int strip;
			for (strip = 1; strip <= 6; ++strip)
				drawTexture(gSprites[TEX_TEXTBOX], &rcFrame2, tsc.rcText.left - 14, 8 * strip + tsc.rcText.top - 10);
			drawTexture(gSprites[TEX_TEXTBOX], &rcFrame3, tsc.rcText.left - 14, 8 * strip + tsc.rcText.top - 10);
		}

		setCliprect(&tsc.rcText);

		//Move face into position
		if ((tsc.face_x += 8) > 0)
			tsc.face_x = 0;

		//Set face's framerect
		rcFace.left = 48 * (tsc.face % 6);
		rcFace.top = 48 * (tsc.face / 6);
		rcFace.right = rcFace.left + 48;
		rcFace.bottom = rcFace.top + 48;

		drawTexture(gSprites[TEX_FACE], &rcFace, tsc.rcText.left + tsc.face_x, tsc.rcText.top - 3);

		//Draw text
		int text_offset;
		if (tsc.face)
			text_offset = 56;
		else
			text_offset = 0;

		//Go through each and every line and draw it
		for (int i = 0; i < 4; i++)
		{
			drawString(tsc.rcText.left + text_offset, tsc.rcText.top + tsc.ypos_line[i] + tsc.offsetY, tscText + (i * 0x40), tscTextFlag + i * 0x40);
		}

		//End cliprect
		setCliprect(nullptr);

		//NOD cursor / beam?
		const bool flash = tsc.wait_beam++ % 20 > 12;

		if (flash && tsc.mode == NOD)
		{
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 254, 255);
			drawRect(tsc.rcText.left + text_offset + (tsc.p_write * 6),
			         tsc.rcText.top + tsc.ypos_line[tsc.line % 4] + tsc.offsetY,
			         5,
			         11);
		}

		//Define GIT rect
		rcItemBox1 = { 0, 0, 72, 16 };
		rcItemBox2 = { 0, 8, 72, 24 };
		rcItemBox3 = { 240, 0, 244, 8 };
		rcItemBox4 = { 240, 8, 244, 16 };
		rcItemBox5 = { 240, 16, 244, 24 };

		if (tsc.item) //Display item not 0
		{
			//Draw GIT background
			drawTexture(gSprites[TEX_TEXTBOX], &rcItemBox1, (gScreenWidth / 2) - 40, (gScreenHeight / 2) + 8);
			drawTexture(gSprites[TEX_TEXTBOX], &rcItemBox2, (gScreenWidth / 2) - 40, (gScreenHeight / 2) + 24);
			drawTexture(gSprites[TEX_TEXTBOX], &rcItemBox3, (gScreenWidth / 2) + 32, (gScreenHeight / 2) + 8);
			drawTexture(gSprites[TEX_TEXTBOX], &rcItemBox4, (gScreenWidth / 2) + 32, (gScreenHeight / 2) + 12);
			drawTexture(gSprites[TEX_TEXTBOX], &rcItemBox4, (gScreenWidth / 2) + 32, (gScreenHeight / 2) + 24);
			drawTexture(gSprites[TEX_TEXTBOX], &rcItemBox5, (gScreenWidth / 2) + 32, (gScreenHeight / 2) + 32);

			//Move the item image into position
			if (tsc.item_y < 0x88)
				++tsc.item_y;

			if (tsc.item >= 1000) //Draw items if 1000 or greater
			{
				rcItem.left = 32 * ((tsc.item - 1000) % 8);
				rcItem.right = 32 * ((tsc.item - 1000) % 8) + 32;
				rcItem.top = 16 * ((tsc.item - 1000) / 8);
				rcItem.bottom = 16 * ((tsc.item - 1000) / 8) + 16;

				drawTexture(gSprites[TEX_ITEMIMAGE], &rcItem, (gScreenWidth / 2) - 20, ((gScreenHeight - 240) / 2) + tsc.item_y);
			}
			else //Otherwise, draw weapon
			{
				rcItem.left = 16 * (tsc.item % 16);
				rcItem.right = 16 * (tsc.item % 16) + 16;
				rcItem.top = 16 * (tsc.item / 16);
				rcItem.bottom = 16 * (tsc.item / 16) + 16;

				drawTexture(gSprites[TEX_ARMSIMAGE], &rcItem, (gScreenWidth / 2) - 12, ((gScreenHeight - 240) / 2) + tsc.item_y);
			}
		}

		//Yes/No selection
		rcYesNo = { 152, 48, 244, 80 };
		rcSelection = { 112, 88, 128, 104 };

		if (tsc.mode == YNJ)
		{
			int y;
			if (tsc.wait > 1)
				y = gScreenHeight - 96;
			else
				y = (gScreenHeight - 240) + 4 * (38 - tsc.wait);

			drawTexture(gSprites[TEX_TEXTBOX], &rcYesNo, tsc.rcText.left + 164, y);

			if (tsc.wait == 16)
				drawTexture(gSprites[TEX_TEXTBOX], &rcSelection, 41 * tsc.select + tsc.rcText.left + 159, gScreenHeight - 86);
		}
	}
}

