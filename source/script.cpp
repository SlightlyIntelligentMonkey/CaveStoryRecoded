#include "script.h"
#include "fade.h"
#include "weapons.h"
#include "render.h"
#include "sound.h"
#include "flags.h"
#include "npc.h"
#include "filesystem.h"
#include "fade.h"
#include "game.h"
#include "player.h"
#include "input.h"

#include <cstring>
#include <cstdlib>
#include <SDL_RWops.h>
#include <SDL_render.h>

using std::malloc;
using std::memset;
using std::strcpy;
using std::strcat;


//Variables
TSC tsc;
char tscText[0x100];
uint8_t *tscTextFlag = static_cast<uint8_t*>(malloc(0x100));

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
	gameFlags &= ~4;

	memset(tscText, 0, 0x100);
	memset(tscTextFlag, 0, 0x100);

	tsc.data = static_cast<uint8_t*>(malloc(0x5000u));
	return tsc.data != nullptr;
}

//Loading functions
void decryptTsc(uint8_t *data, size_t size)
{
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

void loadStageTsc(const char *name)
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
	SDL_RWops *bodyRW = SDL_RWFromFile(name, "rb");
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
	strcpy(tsc.path, name);
}

void loadTsc2(const char *name)
{
	//Load tsc file
	SDL_RWops *bodyRW = SDL_RWFromFile(name, "rb");
	if (!bodyRW)
		doError();
	tsc.size = static_cast<decltype(tsc.size)>(SDL_RWsize(bodyRW));

	//Put the data into memory
	bodyRW->read(bodyRW, tsc.data, 1, tsc.size);
	decryptTsc(tsc.data, tsc.size);
	tsc.data[tsc.size] = 0;

	bodyRW->close(bodyRW);

	//Finish off by setting some stuff in the tsc struct
	strcpy(tsc.path, name);
}

//Get number function
int getTSCNumber(int a)
{
	return			(static_cast<char>(tsc.data[a + 3]) - 0x30) +
	                10 *	(static_cast<char>(tsc.data[a + 2]) - 0x30) +
	                100 *	(static_cast<char>(tsc.data[a + 1]) - 0x30) +
	                1000 *	(static_cast<char>(tsc.data[a]) - 0x30);
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
int startTscEvent(int no)
{
	tsc.mode = 1;
	gameFlags |= 5;
	tsc.line = 0;
	tsc.p_write = 0;
	tsc.wait = 4;
	tsc.flags = 0;
	tsc.wait_beam = 0;
	tsc.face = 0;
	tsc.item = 0;
	tsc.offsetY = 0;
	currentPlayer.shock = 0;

	tsc.rcText = { 52, 184, 268, 234 };

	//Get event id
	int event_no;
	for (tsc.p_read = 0; ; ++tsc.p_read)
	{
		if (!tsc.data[tsc.p_read])
			return 0;
		if (tsc.data[tsc.p_read] != 35)
			continue;

		event_no = getTSCNumber(++tsc.p_read);
		if (no == event_no)
			break;
		if (no < event_no)
			return 0;
	}

	while (tsc.data[tsc.p_read] != '\n')
		++tsc.p_read;
	++tsc.p_read;
	return 1;
}

int jumpTscEvent(int no)
{
	tsc.mode = 1;
	gameFlags |= 4;
	tsc.line = 0;
	tsc.p_write = 0;
	tsc.wait = 4;
	tsc.wait_beam = 0;

	tscClearText();

	//Get event id
	int event_no;
	for (tsc.p_read = 0; ; ++tsc.p_read)
	{
		if (!tsc.data[tsc.p_read])
			return 0;
		if (tsc.data[tsc.p_read] != 35)
			continue;

		event_no = getTSCNumber(++tsc.p_read);
		if (no == event_no)
			break;
		if (no < event_no)
			return 0;
	}

	while (tsc.data[tsc.p_read] != '\n')
		++tsc.p_read;
	++tsc.p_read;
	return 1;
}

void stopTsc()
{
	tsc.mode = 0;
	gameFlags &= ~4;
	gameFlags |= 3u;
	tsc.flags = 0;
}

//Check new line
void checkNewLine()
{
	if (tsc.ypos_line[tsc.line % 4] == 48)
	{
		tsc.mode = 3;
		gameFlags |= 4u;
		memset(tscText + (tsc.line % 4 << 6), 0, 0x40);
		memset(tscTextFlag + (tsc.line % 4 << 6), 0, 0x40);
	}
}

void clearTextLine()
{
	//Reset current writing position
	tsc.line = 0;
	tsc.p_write = 0;
	tsc.offsetY = 0;

	//Go through each line and clear it
	for (int i = 0; i < 4; ++i)
	{
		tsc.ypos_line[i] = 16 * i;

		memset(tscText + (i * 40), 0, 0x40u);
		memset(tscTextFlag + (i * 40), 0, 0x40u);
	}
}

//TSC Update
int tscCheck()
{
	//End tsc if in END state, continue if not
	if (tsc.mode)
		gameFlags |= 4;
	else
		gameFlags &= ~4;

	return 1;
}

void tscCleanup(int numargs) //Function to shift the current read position after a command
{
	tsc.p_read += 4 + (numargs * 4);

	if (numargs > 1)
		tsc.p_read += (numargs - 1);
}

int updateTsc()
{
	bool bExit = false;
	char str[72];
	char c[3];

	int xt;
	int yt;

	switch (tsc.mode)
	{
	case END:
		return tscCheck();

	case PARSE:
		//Add to timer
		++tsc.wait;

		//If jump or shoot are held, add 4, effectively ending the timer early
		if (!(gameFlags & 2) && (isKeyDown(keyShoot) || isKeyDown(keyJump)))
			tsc.wait += 4;

		//If timer value is less than 4, stop for this frame.
		if (tsc.wait < 4)
			return tscCheck();

		//Reset timer and continue into parsing
		tsc.wait = 0;
		bExit = false;
		break;

	case NOD:
		//If jump or shoot button pressed, continue script
		if (isKeyPressed(keyShoot) || isKeyPressed(keyJump))
			tsc.mode = PARSE;

		return tscCheck();

	case SCROLL:
		//Go through every line
		for (int i = 0; i < 4; ++i)
		{
			tsc.ypos_line[i] -= 4;
			if (!tsc.ypos_line[i]) //Check if done scrolling
				tsc.mode = PARSE; //Continue like normal

			if (tsc.ypos_line[i] == -16) //Check if scrolled off
				tsc.ypos_line[i] = 48;
		}

		return tscCheck();

	case WAI:
		//These two checks are for <WAI9999 to fully freeze the script
		if (tsc.wait_next != 9999)
		{
			if (tsc.wait != 9999)
				++tsc.wait;

			if (tsc.wait >= tsc.wait_next) //If waited enough frames, continue
			{
				tsc.mode = PARSE;
				tsc.wait_beam = 0;
			}
		}

		return tscCheck();

	case FADE:
		if (fade.mode == 0) //Wait until fade has ended
		{
			//Continue script
			tsc.mode = PARSE;
			tsc.wait_beam = 0;
		}
		return tscCheck();

	case YNJ:
		if (tsc.wait >= 16) //Do nothing for 16 frames
		{
			if (isKeyPressed(keyJump)) //Select button pressed
			{
				//Play selection sound
				playSound(SFX_YNConfirm);

				if (tsc.select == true)
				{
					jumpTscEvent(tsc.next_event); //Jump to specified event
				}
				else if (tsc.select == false)
				{
					//Continue like normal
					tsc.mode = PARSE;
					tsc.wait_beam = 0;
				}
				else
					doCustomError("Invalid YNJ result");
			}
			else if (isKeyPressed(keyLeft)) //Left pressed
			{
				tsc.select = 0; //Select yes and play sound
				playSound(SFX_YNChangeChoice);
			}
			else if (isKeyPressed(keyRight)) //Right pressed
			{
				tsc.select = 1; //Select no and play sound
				playSound(SFX_YNChangeChoice);
			}
		}
		else
		{
			++tsc.wait; //Add to wait counter
		}

		return tscCheck();

	case WAS:
		//Wait until on the ground
		if (currentPlayer.flag & ground)
		{
			//Go into parse mode
			tsc.mode = PARSE;
			tsc.wait_beam = 0;
		}

		return tscCheck();

	default:
		break;
	}

	while (true) //Parsing
	{
		if (bExit == 1)
			return tscCheck();

		if (tsc.data[tsc.p_read] != '<')
		{
			if (tsc.data[tsc.p_read] == 13) //Check for break line
			{
				//Shift read and write positions accordingly
				tsc.p_read += 2;
				tsc.p_write = 0;

				//Only move if message box visible?
				if (tsc.flags & 1)
				{
					++tsc.line;
					checkNewLine();
				}
			}
			else if (tsc.flags & 0x10) //CAT / SAT / TUR
			{
				//Go through until reaching a command indicator or break line
				int x;
				for (x = tsc.p_read; ; ++x)
				{
					const bool quit = !(tsc.data[x] == '<' || tsc.data[x] == 13);

					if (!quit)
						break;
					if ((tsc.data[x] & 0x80) != 0)
						++x;
				}

				//Get data to copy from tsc data
				int copy = x - tsc.p_read;
				memcpy(str, &tsc.data[tsc.p_read], copy);
				str[copy] = 0;

				//Copy data onto the text buffer
				strcpy(tscText + (tsc.line * 0x40), str);
				memset(tscTextFlag + (tsc.line * 0x40), 1, copy); //This sets the flag that makes drawn text thinner.

				//Shift read and write positions
				tsc.p_write = x; //Pixel uses an absolute value rather than a relative value
				tsc.p_read += copy;

				//Check for new lines
				if (tsc.p_write > 34)
					checkNewLine();

				bExit = true;
			}
			else
			{
				//Code for multibyte things?
				c[0] = tsc.data[tsc.p_read];

				if (c[0] >= 0)
				{
					c[1] = 0;
				}
				else
				{
					c[1] = tsc.data[tsc.p_read + 1];
					c[2] = 0;
				}

				//Copy onto the text buffer
				strcat(tscText + (tsc.line % 4 << 6), c);
				tscTextFlag[(tsc.line % 4 << 6) + tsc.p_write] = 0; //Make text display at normal width

				//Make equal signs draw as the circle thing
				if (c[0] == '=')
					tscTextFlag[(tsc.line % 4 << 6) + tsc.p_write] |= 2;

				//Play sound and reset cursor blinking timer.
				playSound(SFX_MessageTyping);
				tsc.wait_beam = 0;

				//Shift read and write positions
				if (c[0] >= 0)
				{
					++tsc.p_read;
					++tsc.p_write;
				}
				else
				{
					tsc.p_read += 2;
					tsc.p_write += 2;
				}

				//Check for new lines
				if (tsc.p_write > 34)
				{
					checkNewLine();
					tsc.p_write = 0;
					++tsc.line;
					checkNewLine();
				}

				bExit = true;
			}
		}
		else
		{
			//Parse and run TSC commands
			switch (tsc.data[tsc.p_read + 3] + (tsc.data[tsc.p_read + 2] << 8) + (tsc.data[tsc.p_read + 1] << 16) + (tsc.data[tsc.p_read] << 24))
			{
			case('<AE+'):
				maxWeaponAmmo();
				tscCleanup(0);
				break;
			case('<AM+'):
				giveWeapon(getTSCNumber(tsc.p_read + 4), getTSCNumber(tsc.p_read + 9));
				tscCleanup(2);
				break;
			case('<AM-'):
				removeWeapon(getTSCNumber(tsc.p_read + 4));
				tscCleanup(1);
				break;
			case('<AMJ'):
				if (checkWeapon(getTSCNumber(tsc.p_read + 4)))
					jumpTscEvent(getTSCNumber(tsc.p_read + 9));
				else
					tscCleanup(2);
				break;
			case('<ANP'):
				for (size_t i = 0; i < npcs.size(); i++)
				{
					if ((npcs[i].cond & npccond_alive) && npcs[i].code_event == getTSCNumber(tsc.p_read + 4))
					{
						npcs[i].act_no = getTSCNumber(tsc.p_read + 9);

						if (getTSCNumber(tsc.p_read + 14) != 5)
						{
							if (getTSCNumber(tsc.p_read + 14) == 4)
							{
								if (npcs[i].x >= currentPlayer.x)
									npcs[i].direct = dirLeft;
								else
									npcs[i].direct = dirRight;
							}
							else
							{
								npcs[i].direct = getTSCNumber(tsc.p_read + 14);
							}
						}
					}
				}
				tscCleanup(3);
				break;
			case('<BOA'):
				tscCleanup(1);
				break;
			case('<BSL'):
				tscCleanup(1);
				break;
			case('<CAT'):
				tsc.flags |= 0x40u;
				tscCleanup(0);
				break;
			case('<CIL'):
				tscCleanup(0);
				break;
			case('<CLO'):
				tsc.flags &= ~0x33;
				tscCleanup(0);
				break;
			case('<CLR'):
				clearTextLine();
				tscCleanup(0);
				break;
			case('<CMP'):
				changeTile(getTSCNumber(tsc.p_read + 4), getTSCNumber(tsc.p_read + 9),
				           getTSCNumber(tsc.p_read + 14));
				tscCleanup(3);
				break;
			case('<CMU'):
				tscCleanup(1);
				break;
			case('<CNP'):
				changeNpc(getTSCNumber(tsc.p_read + 4),
				          getTSCNumber(tsc.p_read + 9),
				          getTSCNumber(tsc.p_read + 14));
				tscCleanup(3);
				break;
			case('<CPS'):
				tscCleanup(0);
				break;
			case('<CRE'):
				tscCleanup(0);
				break;
			case('<CSS'):
				tscCleanup(0);
				break;
			case('<DNA'):
				for (size_t i = 0; i < npcs.size(); i++)
				{
					if (npcs[i].cond & npccond_alive)
					{
						if (npcs[i].code_char == getTSCNumber(tsc.p_read + 4))
						{
							npcs[i].cond = 0;
							setFlag(npcs[i].code_flag);
						}
					}
				}
				tscCleanup(1);
				break;
			case('<DNP'):
				for (size_t i = 0; i < npcs.size(); i++)
				{
					if (npcs[i].cond & npccond_alive)
					{
						if (npcs[i].code_event == getTSCNumber(tsc.p_read + 4))
						{
							npcs[i].cond = 0;
							setFlag(npcs[i].code_flag);
						}
					}
				}
				tscCleanup(1);
				break;
			case('<ECJ'):
				for (size_t n = 0; n < npcs.size(); n++)
				{
					if (npcs[n].cond & npccond_alive)
					{
						if (npcs[n].code_char == getTSCNumber(tsc.p_read + 4))
						{
							jumpTscEvent(getTSCNumber(tsc.p_read + 9));
							break;
						}
					}
				}
				break;
			case('<END'):
				tsc.mode = 0;
				currentPlayer.cond &= ~player_interact;
				gameFlags |= 3u;
				tsc.face = 0;
				bExit = true;
				break;
			case('<EQ+'):
				currentPlayer.equip |= getTSCNumber(tsc.p_read + 4);
				tscCleanup(1);
				break;
			case('<EQ-'):
				currentPlayer.equip &= ~getTSCNumber(tsc.p_read + 4);
				tscCleanup(1);
				break;
			case('<ESC'):
				return 2;
			case('<EVE'):
				jumpTscEvent(getTSCNumber(tsc.p_read + 4));
				break;
			case('<FAC'):
				if (tsc.face != getTSCNumber(tsc.p_read + 4))
				{
					tsc.face_x = -48;
					tsc.face = getTSCNumber(tsc.p_read + 4);
				}
				tscCleanup(1);
				break;
			case('<FAI'):
				fade.mode = 1;
				fade.dir = getTSCNumber(tsc.p_read + 4);
				fade.count = 0;
				tsc.mode = FADE;
				tscCleanup(1);
				bExit = true;
				break;
			case('<FAO'):
				fade.mode = 2;
				fade.dir = getTSCNumber(tsc.p_read + 4);
				fade.count = 0;
				tsc.mode = FADE;
				tscCleanup(1);
				bExit = true;
				break;
			case('<FL+'):
				setFlag(getTSCNumber(tsc.p_read + 4));
				tscCleanup(1);
				break;
			case('<FL-'):
				clearFlag(getTSCNumber(tsc.p_read + 4));
				tscCleanup(1);
				break;
			case('<FLA'):
				tscCleanup(0);
				break;
			case('<FLJ'):
				if (getFlag(getTSCNumber(tsc.p_read + 4)))
					jumpTscEvent(getTSCNumber(tsc.p_read + 9));
				else
					tscCleanup(2);
				break;
			case('<FMU'):
				tscCleanup(0);
				break;
			case('<FOB'):
				tscCleanup(2);
				break;
			case('<FOM'):
				viewport.lookX = &currentPlayer.x;
				viewport.lookY = &currentPlayer.y;
				viewport.speed = getTSCNumber(tsc.p_read + 4);
				tscCleanup(1);
				break;
			case('<FON'):
				for (size_t n = 0; n < npcs.size(); n++)
				{
					if (npcs[n].cond & npccond_alive)
					{
						if (npcs[n].code_event == getTSCNumber(tsc.p_read + 4))
						{
							viewport.lookX = &npcs[n].x;
							viewport.lookY = &npcs[n].y;
							viewport.speed = getTSCNumber(tsc.p_read + 9);
							break;
						}
					}
				}
				tscCleanup(2);
				break;
			case('<FRE'):
				gameFlags |= 3;
				tscCleanup(0);
				break;
			case('<GIT'):
				tsc.item = getTSCNumber(tsc.p_read + 4);
				tsc.item_y = 128;
				tscCleanup(1);
				break;
			case('<HMC'):
				currentPlayer.cond &= ~player_visible;
				tscCleanup(0);
				break;
			case('<INI'):
				initGame();
				return 1;
			case('<INP'):
				tscCleanup(3);
				break;
			case('<IT+'):
				tscCleanup(1);
				break;
			case('<IT-'):
				tscCleanup(1);
				break;
			case('<ITJ'):
				tscCleanup(2);
				break;
			case('<KEY'):
				gameFlags |= 1;
				gameFlags &= ~2;
				currentPlayer.up = false;
				tscCleanup(0);
				break;
			case('<LDP'):
				loadProfile();
				break;
			case('<LI+'):
				currentPlayer.life += getTSCNumber(tsc.p_read + 4);
				if (currentPlayer.life > currentPlayer.max_life)
					currentPlayer.life = currentPlayer.max_life;
				tscCleanup(1);
				break;
			case('<ML+'):
				currentPlayer.max_life += getTSCNumber(tsc.p_read + 4);
				currentPlayer.life += getTSCNumber(tsc.p_read + 4);
				tscCleanup(1);
				break;
			case('<MLP'):
				tscCleanup(0);
				break;
			case('<MM0'):
				currentPlayer.xm = 0;
				tscCleanup(0);
				break;
			case('<MNA'):
				mapName.wait = 0;
				mapName.flag = 1;
				tscCleanup(0);
				break;
			case('<MNP'):
				for (size_t i = 0; i < npcs.size(); i++)
				{
					if ((npcs[i].cond & npccond_alive) && npcs[i].code_event == getTSCNumber(tsc.p_read + 4))
					{
						npcs[i].x = tileToCoord(getTSCNumber(tsc.p_read + 9));
						npcs[i].y = tileToCoord(getTSCNumber(tsc.p_read + 14));

						if (getTSCNumber(tsc.p_read + 19) != 5)
						{
							if (getTSCNumber(tsc.p_read + 19) == 4)
							{
								if (npcs[i].x >= currentPlayer.x)
									npcs[i].direct = dirLeft;
								else
									npcs[i].direct = dirRight;
							}
							else
							{
								npcs[i].direct = getTSCNumber(tsc.p_read + 19);
							}
						}
					}
				}
				tscCleanup(4);
				break;
			case('<MOV'):
				currentPlayer.setPos(tileToCoord(getTSCNumber(tsc.p_read + 4)), tileToCoord(getTSCNumber(tsc.p_read + 9)));
				tscCleanup(2);
				break;
			case('<MPJ'):
				if (getMapFlag(currentLevel))
					jumpTscEvent(getTSCNumber(tsc.p_read + 4));
				else
					tscCleanup(1);
				break;
			case('<MP+'):
				setMapFlag(getTSCNumber(tsc.p_read + 4));
				tscCleanup(1);
				break;
			case('<MS2'):
				tscCleanup(0);
				break;
			case('<MS3'):
				tscCleanup(0);
				break;
			case('<MSG'):
				clearTextLine();
				tsc.flags |= 3u;
				tsc.flags &= ~0x30;
				if (tsc.flags & 0x40)
					tsc.flags |= 0x10u;
				tscCleanup(0);
				bExit = true;
				break;
			case('<MYB'):
				currentPlayer.cond &= ~player_interact;
				currentPlayer.ym = -0x200;

				switch (getTSCNumber(tsc.p_read + 4))
				{
				case 0:
					currentPlayer.direct = dirLeft;
					currentPlayer.xm = 0x200;
					break;

				case 2:
					currentPlayer.direct = dirRight;
					currentPlayer.xm = -0x200;
					break;

				default:
					for (size_t i = 0; i < npcs.size(); i++)
					{
						if (npcs[i].cond & npccond_alive && npcs[i].code_event == getTSCNumber(tsc.p_read + 4))
						{
							if (npcs[i].x >= currentPlayer.x)
							{
								currentPlayer.direct = dirRight;
								currentPlayer.xm = -0x200;
							}
							else
							{
								currentPlayer.direct = dirLeft;
								currentPlayer.xm = 0x200;
							}
						}
					}
					break;
				}

				tscCleanup(1);
				break;
			case('<MYD'):
				currentPlayer.setDir(getTSCNumber(tsc.p_read + 4));
				tscCleanup(1);
				break;
			case('<NCJ'):
				tscCleanup(2);
				break;
			case('<NOD'):
				tsc.mode = NOD;
				tscCleanup(0);
				return 1;
			case('<NUM'):
				tscCleanup(1);
				break;
			case('<PRI'):
				gameFlags &= ~3;
				tscCleanup(0);
				break;
			case('<PS+'):
				for (xt = 0; xt < 8 && permitStage[xt].index != getTSCNumber(tsc.p_read + 4) && permitStage[xt].index; ++xt);
				if (xt == 8)
					break;

				permitStage[xt].index = getTSCNumber(tsc.p_read + 4);
				permitStage[xt].event = getTSCNumber(tsc.p_read + 9);
				tscCleanup(2);
				break;
			case('<QUA'):
				viewport.quake = getTSCNumber(tsc.p_read + 4);
				tscCleanup(1);
				break;
			case('<RMU'):
				tscCleanup(0);
				break;
			case('<SAT'):
				tsc.flags |= 0x40u;
				tscCleanup(0);
				break;
			case('<SIL'):
				tscCleanup(1);
				break;
			case('<SK+'):
				setSkipFlag(getTSCNumber(tsc.p_read + 4));
				tscCleanup(1);
				break;
			case('<SK-'):
				clearSkipFlag(getTSCNumber(tsc.p_read + 4));
				tscCleanup(1);
				break;
			case('<SKJ'):
				if (getSkipFlag(getTSCNumber(tsc.p_read + 4)))
					jumpTscEvent(getTSCNumber(tsc.p_read + 9));
				else
					tscCleanup(2);
				break;
			case('<SLP'):
				bExit = true;
				yt = stageSelect(&xt);
				if (!yt)
					return 0;
				if (yt == 2)
					return 2;
				jumpTscEvent(xt);
				gameFlags &= ~3;
				break;
			case('<SMC'):
				currentPlayer.cond |= player_visible;
				tscCleanup(0);
				break;
			case('<SMP'):
				tscCleanup(2);
				break;
			case('<SNP'):

				createNpc(
				    getTSCNumber(tsc.p_read + 4),
				    tileToCoord(getTSCNumber(tsc.p_read + 9)),
				    tileToCoord(getTSCNumber(tsc.p_read + 14)),
				    0,
				    0,
				    getTSCNumber(tsc.p_read + 19),
				    nullptr);
				tscCleanup(4);
				break;
			case('<SOU'):
				playSound(getTSCNumber(tsc.p_read + 4));
				tscCleanup(1);
				break;
			case('<SPS'):
				tscCleanup(0);
				break;
			case('<SSS'):
				tscCleanup(1);
				break;
			case('<STC'):
				tscCleanup(0);
				break;
			case('<SVP'):
				saveProfile();
				tscCleanup(0);
				break;
			case('<TAM'):
				tradeWeapons(getTSCNumber(tsc.p_read + 4), getTSCNumber(tsc.p_read + 9), getTSCNumber(tsc.p_read + 14));
				tscCleanup(3);
				break;
			case('<TRA'):
				xt = getTSCNumber(tsc.p_read + 9);
				currentPlayer.setPos(
				    tileToCoord(getTSCNumber(tsc.p_read + 14)),
				    tileToCoord(getTSCNumber(tsc.p_read + 19)));
				loadLevel(getTSCNumber(tsc.p_read + 4));
				startTscEvent(xt);
				return 1;
			case('<TUR'):
				tscCleanup(0);
				tsc.flags |= 0x10;
				break;
			case('<UNI'):
				currentPlayer.unit = getTSCNumber(tsc.p_read + 4);
				tscCleanup(1);
				break;
			case('<UNJ'):
				tscCleanup(2);
				break;
			case('<WAI'):
				tsc.mode = WAI;
				tsc.wait = 0;
				tsc.wait_next = getTSCNumber(tsc.p_read + 4);
				tscCleanup(1);
				bExit = true;
				break;
			case('<WAS'):
				tsc.mode = WAS;
				tscCleanup(0);
				bExit = true;
				break;
			case('<XX1'):
				tscCleanup(1);
				break;
			case('<YNJ'):
				tsc.next_event = getTSCNumber(tsc.p_read + 4);
				tsc.mode = YNJ;
				playSound(SFX_YNPrompt);
				tsc.wait = 0;
				tsc.select = 0;
				tscCleanup(1);
				bExit = true;
				break;
			case('<ZAM'):
				clearWeaponExperience();
				tscCleanup(0);
				break;
			default:
				doCustomError("oops invalid tsc command fuck you");
				break;
			}
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
		tsc.rcText.left = (screenWidth / 2) - 108;
		tsc.rcText.right = (screenWidth / 2) + 108;

		if (tsc.flags & 0x20)
		{
			tsc.rcText.top = 32;
			tsc.rcText.bottom = 80;
		}
		else
		{
			tsc.rcText.top = 184;
			tsc.rcText.bottom = 232;
		}

		//Draw message box background (if not MS2)
		if (tsc.flags & 2)
		{
			rcFrame1 = { 0, 0, 244, 8 };
			rcFrame2 = { 0, 8, 244, 16 };
			rcFrame3 = { 0, 16, 244, 24 };

			drawTexture(sprites[TEX_TEXTBOX], &rcFrame1, tsc.rcText.left - 14, tsc.rcText.top - 10);

			int strip;
			for (strip = 1; strip <= 6; ++strip)
				drawTexture(sprites[TEX_TEXTBOX], &rcFrame2, tsc.rcText.left - 14, 8 * strip + tsc.rcText.top - 10);
			drawTexture(sprites[TEX_TEXTBOX], &rcFrame3, tsc.rcText.left - 14, 8 * strip + tsc.rcText.top - 10);
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

		drawTexture(sprites[TEX_FACE], &rcFace, tsc.rcText.left + tsc.face_x, tsc.rcText.top - 3);

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
			SDL_SetRenderDrawColor(renderer, 255, 255, 254, 255);
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
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox1, (screenWidth / 2) - 40, 128);
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox2, (screenWidth / 2) - 40, 144);
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox3, (screenWidth / 2) + 32, 128);
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox4, (screenWidth / 2) + 32, 136);
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox4, (screenWidth / 2) + 32, 144);
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox5, (screenWidth / 2) + 32, 152);

			//Move the item image into position
			if (tsc.item_y < 0x88)
				++tsc.item_y;

			if (tsc.item >= 1000) //Draw items if 1000 or greater
			{
				rcItem.left = 32 * ((tsc.item - 1000) % 8);
				rcItem.right = 32 * ((tsc.item - 1000) % 8) + 32;
				rcItem.top = 16 * ((tsc.item - 1000) / 8);
				rcItem.bottom = 16 * ((tsc.item - 1000) / 8) + 16;

				drawTexture(sprites[TEX_ITEMIMAGE], &rcItem, (screenWidth / 2) - 20, tsc.item_y);
			}
			else //Otherwise, draw weapon
			{
				rcItem.left = 16 * (tsc.item % 16);
				rcItem.right = 16 * (tsc.item % 16) + 16;
				rcItem.top = 16 * (tsc.item / 16);
				rcItem.bottom = 16 * (tsc.item / 16) + 16;

				drawTexture(sprites[TEX_ARMSIMAGE], &rcItem, (screenWidth / 2) - 12, tsc.item_y);
			}
		}

		//Yes/No selection
		rcYesNo = { 152, 48, 244, 80 };
		rcSelection = { 112, 88, 128, 104 };

		if (tsc.mode == YNJ)
		{
			int y;
			if (tsc.wait > 1)
				y = 144;
			else
				y = 4 * (38 - tsc.wait);

			drawTexture(sprites[TEX_TEXTBOX], &rcYesNo, tsc.rcText.left + 164, y);

			if (tsc.wait == 16)
				drawTexture(sprites[TEX_TEXTBOX], &rcSelection, 41 * tsc.select + tsc.rcText.left + 159, 154);
		}
	}
}
