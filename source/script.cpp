#include "script.h"
#include "npc.h"
#include "filesystem.h"
#include "weapons.h"
#include "player.h"
#include "fade.h"

// -- TSC Code --
// This is a string, not a fixed buffer.
static char *tsc = 0;

// -- TSC State --
int tscMode = 0;
int tscPrevMode = 0;

int fadeDirection = 0;

int tscPos = -1;
unsigned int tscCounter = 0;
int tscWait = 0;
int tscDisplayFlags = 0;

int tscUpdateFlags = 0;

// -- command variables --
unsigned int waitAmount = 0;
int yesnoSelect = 0;
unsigned int faceNo = 0;
RECT rcView;

int mapNameDisplayTimer = 0;

int textOffset = 0;

char msgText[256];
int msgBoxX = 0;
int msgBoxY = 0;

// -- display variables --
int tscNumber = 0;
int faceX = 0;
int gitNo = 0;
int ynjBoxYOffset = 0;


RECT rcFrame1 = { 0, 0, 244, 8 };
RECT rcFrame2 = { 0, 8, 244, 16 };
RECT rcFrame3 = { 0, 16, 244, 24 };
RECT rect_yesno = { 152, 48, 244, 80 };
RECT rect_cur = { 112, 88, 128, 104 };
RECT rcItemBox1 = { 0, 0, 72, 16 };
RECT rcItemBox2 = { 0, 8, 72, 24 };
RECT rcItemBox3 = { 240, 0, 244, 8 };
RECT rcItemBox4 = { 240, 8, 244, 16 };
RECT rcItemBox5 = { 240, 16, 244, 24 };
RECT rcFace = { 0, 0, 48, 48 };
RECT gRect_line = { 0, 0, 216, 16 };
SDL_Rect rcSYmbol = { 64, 48, 8, 8 };

// -- font stuff --
SDL_Texture *font;
const int charWidth = 6;
const int charHeight = 12;
const int charSeparation = 0;
//const int 

// Decrypt into tsc buffer
void tscDecrypt(int start, BYTE * code, int size) {
	int headHalf = size >> 1;
	int headCipher = code[headHalf];

	for (int i = 0; i < size; i++) {
		int ret = code[i];

		if (i != headHalf)
			ret -= headCipher;

		tsc[start + i] = (char)ret;
	}
}

void loadTsc(const char * path) {
	tscPos = -1;
	tscWait = 0;
	tscUpdateFlags = 0;
	tscCounter = 0;

	// Load head tsc
	BYTE *headTsc = nullptr;
	int tscHeadSize = loadFile("data/Head.tsc", &headTsc);
	if (tscHeadSize < 0) {
		doCustomError("TSC: Failed to load data/Head.tsc");
		return;
	}

	// Load stage tsc
	BYTE *stageTsc = nullptr;
	int tscSize = loadFile(path, &stageTsc);
	if (tscSize < 0) {
		free(headTsc);
		char msg[256];
		snprintf(msg, 256, "TSC: %s failed to load.", path);
		doCustomError(msg);
		return;
	}

	if (tsc)
		free(tsc);

	tsc = (char *)malloc(tscSize + tscHeadSize + 1);
	tscDecrypt(0, headTsc, tscHeadSize);
	tscDecrypt(tscHeadSize, stageTsc, tscSize);
	tsc[tscSize + tscHeadSize] = 0;

	free(headTsc);
	free(stageTsc);
}

int getEventPos(int event_num)
{
	int position = 4;
	char *location;
	char eventChar[6] = "#0000";

	if (event_num > 9999) { event_num = 9999; }

	while (event_num > 0)
	{
		eventChar[position] = (char)((event_num % 10) + 0x30);
		position--;
		event_num /= 10;
	}

	location = strstr(tsc, eventChar);

	if (location == NULL)
		doCustomError("Event wasn't found.");

	return (location - tsc) + 7;
}

void runScriptEvent(int event_num) {

	tscPos = getEventPos(event_num);
	tscMode = PARSE;
	faceNo = 0;
	tscDisplayFlags = 0;
	gameFlags |= 5;
}

void jumpScriptEvent(int event_num)
{
	tscPos = getEventPos(event_num);
	tscMode = PARSE;
	tscDisplayFlags = 0;
	gameFlags |= 4;
}

//renders a line of text
void renderTextLine(int x, int y, char *str)
{
	SDL_Rect rcChar = { 0, 0, charWidth, charHeight };
	SDL_Rect dest = { 0, y, charWidth, charHeight };

	for (size_t c = 0; c < strlen(str); c++)
	{
		rcChar.x = ((str[c] - 0x20) % 32) * charWidth;
		rcChar.y = ((str[c] - 0x20) >> 5) * charHeight;
		dest.x = x + (c * (charWidth + charSeparation));

		//Draw shadow
		SDL_SetTextureColorMod(font, 0, 0, 32);
		dest.x++;
		dest.y++;
		SDL_RenderCopy(renderer, font, &rcChar, &dest);

		//Draw front
		SDL_SetTextureColorMod(font, 255, 255, 255);
		dest.x--;
		dest.y--;
		SDL_RenderCopy(renderer, font, &rcChar, &dest);
	}
	return;
}

void updateMessageBox()
{
	if (tscUpdateFlags & 0x30)
	{
		while (tsc[tscPos] != '<')
		{
			strncat(msgText, &tsc[tscPos], 1);
			tscPos++;
		}
	}
	else if (tscCounter++ % 4 == 0 || isKeyDown(keyJump) || isKeyDown(keyShoot))
	{
		strncat(msgText, &tsc[tscPos], 1);
		tscPos++;
		playSound(2);
	}
}

int scrollOffset = 0;
void drawMessageBox(int x, int y, char* str)
{
	SDL_Rect rcChar = { 0, 0, charWidth, charHeight };
	SDL_Rect dest = { 0, 0, charWidth, charHeight };
	SDL_Rect msgClip = { x + 8, y + 8, 228, 48 };
	int lineY = 0;
	int charX = 0;
	char buf[256] = { 0 };

	if (!(tscDisplayFlags & invisMSG))
	{
		drawTexture(sprites[TEX_TEXTBOX], &rcFrame1, x, y);
		drawTexture(sprites[TEX_TEXTBOX], &rcFrame2, x, y + 8);
		drawTexture(sprites[TEX_TEXTBOX], &rcFrame2, x, y + 16);
		drawTexture(sprites[TEX_TEXTBOX], &rcFrame2, x, y + 24);
		drawTexture(sprites[TEX_TEXTBOX], &rcFrame2, x, y + 32);
		drawTexture(sprites[TEX_TEXTBOX], &rcFrame2, x, y + 40);
		drawTexture(sprites[TEX_TEXTBOX], &rcFrame2, x, y + 48);
		drawTexture(sprites[TEX_TEXTBOX], &rcFrame3, x, y + 56);
	}

	SDL_RenderSetClipRect(renderer, &msgClip);
	for (size_t c = 0; c < strlen(str); c++)
	{
		//ignores non-displayable characters
		if ((str[c] < 0x7F && str[c] > 0x1F) || str[c] == '\n')
		{
			if (str[c] == '\n')
			{
				charX = -1;
				lineY++;
				if (lineY >= 3)
				{
					scrollOffset += 4;
				}

				if (scrollOffset > ((charHeight * 4) / 3) + 1)
				{
					scrollOffset = 0;
					lineY--;
					for (size_t c = 0; c < strlen(str); c++)
					{
						if (str[c] == '\n')
						{
							strcpy(buf, &str[c + 1]);
							strcpy(str, buf);
							break;
						}
					}
				}
			}
			rcChar.x = (((str[c] - 0x20) % 32)*charWidth);
			rcChar.y = (((str[c] - 0x20) >> 5)*charHeight);
			dest.x = 8 + x + (charX * (charWidth + charSeparation)) + charWidth + textOffset;
			dest.y = 8 + y + ((lineY * charHeight * 4) / 3) + 1 - scrollOffset;

			SDL_RenderCopy(renderer, font, &rcChar, &dest);
			charX++;
		}
	}

	SDL_RenderSetClipRect(renderer, NULL);
	return;
}

void drawTSC()
{
	RECT rcGit = { 0, 0, 32, 16 };
	SDL_Rect rcClip = { 0, 0, 0, 0 };

	if (fadedOut == true)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
		SDL_RenderClear(renderer);
	}

	if (fadeCounter == 0xFFFFFFF) { tscDisplayFlags &= ~(FAI | FAO); }
	if (tscDisplayFlags & FAI)
	{
		fadeIn(fadeDirection);
	}
	if (tscDisplayFlags & FAO)
	{
		fadeOut(fadeDirection);
	}

	if (tscMode != 0 && tscDisplayFlags & TSCVIS)
	{

		if (tscDisplayFlags & 0x20)
		{
			rcView.top = 32;
			rcView.bottom = 80;
		}
		else
		{
			rcView.top = 184;
			rcView.bottom = 232;
		}

		if (gitNo != 0)
		{
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox1, (screenWidth >> 1) - 40, (screenHeight >> 1) + 8);
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox2, (screenWidth >> 1) - 40, (screenHeight >> 1) + 24);
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox3, (screenWidth >> 1) + 32, (screenHeight >> 1) + 8);
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox4, (screenWidth >> 1) + 32, (screenHeight >> 1) + 16);
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox4, (screenWidth >> 1) + 32, (screenHeight >> 1) + 24);
			drawTexture(sprites[TEX_TEXTBOX], &rcItemBox5, (screenWidth >> 1) + 32, (screenHeight >> 1) + 32);
			if (gitNo > 1000)
			{
				rcGit.left = ((gitNo - 1000) % 8) * 32;
				rcGit.top = ((gitNo - 1000) / 8) * 16;
				rcGit.right = rcGit.left + 32;
				rcGit.bottom = rcGit.top + 16;
				drawTexture(sprites[TEX_ITEMIMAGE], &rcGit, (screenWidth / 2) - 17, 136);
			}
			else
			{
				rcGit.left = gitNo * 16;
				rcGit.top = 0;
				rcGit.right = rcGit.left + 16;
				rcGit.bottom = 16;
				drawTexture(sprites[TEX_ARMSIMAGE], &rcGit, (screenWidth / 2) - 10, 136);
			}
		}


		//renders the yes no box thing
		if (tscMode == YNJ)
		{
			if (ynjBoxYOffset >= 4)
			{
				ynjBoxYOffset = 4;
			}
			else { ynjBoxYOffset++; }
			rcClip = { (screenWidth >> 1) + 56, ((screenHeight >> 1) + 22), 84, 32 };
			SDL_RenderSetClipRect(renderer, &rcClip);
			drawTexture(sprites[TEX_TEXTBOX], &rect_yesno, (screenWidth >> 1) + 56,
				((screenHeight >> 1) + 54) - (ynjBoxYOffset << 3));
			SDL_RenderSetClipRect(renderer, NULL);
			if (ynjBoxYOffset >= 4)
			{
				drawTexture(sprites[TEX_TEXTBOX], &rect_cur,
					41 * yesnoSelect + ((screenWidth >> 1) + 51),
					(screenHeight >> 1) + 34);
			}
		}
		if (tscDisplayFlags & MSGbox)
		{
			drawMessageBox(msgBoxX, msgBoxY, msgText);
		}

		//displaying face
		if (faceNo)
		{
			textOffset = 56;
			rcClip = { msgBoxX + 8, msgBoxY + 8, 48, 48 };

			rcFace.left = 48 * (faceNo % 6);
			rcFace.top = 48 * (faceNo / 6);
			rcFace.right = rcFace.left + 48;
			rcFace.bottom = rcFace.top + 48;

			faceX += 8;
			if (faceX >= msgBoxX + 8) { faceX = msgBoxX + 8; }
			SDL_RenderSetClipRect(renderer, &rcClip);
			drawTexture(sprites[TEX_FACE], &rcFace, faceX, msgBoxY + 8);
			SDL_RenderSetClipRect(renderer, NULL);
		}
		else { textOffset = 0; }

		//renders cursor during text when text scrolling scrolling is paused
		if (tscNumber++ % 20 > 12 && tscMode == NOD)
		{
			int num = 0;
			int some_num = 0;
			for (size_t i = 0; i < strlen(msgText); i++)
			{
				if (msgText[i] == '\n')
				{
					num++;
					some_num = i + 1;
				}
			}
			SDL_SetRenderDrawColor(renderer, 255, 255, 254, 255);
			drawRect(
				msgBoxX + 8 + charWidth + (strlen(msgText)*charWidth) - (some_num*charWidth) + textOffset,
				msgBoxY + 8 + ((num*charHeight * 4) / 3) + 1,
				charWidth, charHeight);
		}
	}
	
	if (mapNameDisplayTimer++ < 160)
	{
		renderTextLine((screenWidth >> 1) - ((strlen(stageTable[currentLevel].name) * charWidth) >> 1),
			80, stageTable[currentLevel].name);
	}
	return;
}

void tscCheck()
{
	if (tscCounter)
		gameFlags |= 4;
	else
		gameFlags &= ~0x4;
	return;
}

//reads the next 4 characters and returns a number
int ascii2num(char *pStr, const int num)
{
	int result = 0;
	for (int i = 0; i < num; i++)
	{
		result += (pStr[i] - 0x30) * (int)(std::pow(10, (num - i) - 1));
	}

	return result;
}

//realigns the tsc position
void tscCleanup(int numargs)
{
	tscPos += 4 + (numargs * 4);
	if (numargs > 1)
		tscPos += (numargs - 1);
	return;
}


int updateTsc() {

	char str[72];
	bool bExit = false;
	int num = 0;

	msgBoxX = (screenWidth >> 1) - 122;

	tscDisplayFlags |= TSCVIS;
	switch (tscMode)
	{
	case END:
		tscCounter = 0;
		tscCheck();
		return 1;
	case PARSE:
		++tscCounter;
		tscCounter = 0;
		bExit = 0;
		break;
	case NOD:
		if (isKeyPressed(keyJump) || isKeyPressed(keyShoot))
			tscMode = tscPrevMode;
		tscCheck();
		return 1;
	case MSG:
		if (tsc[tscPos] == '<') { break; }
		updateMessageBox();
		tscCheck();
		return 1;
	case WAI:
		if (waitAmount != 9999)
		{
			if (tscCounter != 9999)
				tscCounter++;
			if (tscCounter >= waitAmount)
			{
				tscMode = tscPrevMode;
				tscCounter = 0;
			}
		}
		tscCheck();
		return 1;
	case FADE:
		tscCounter++;
		tscCheck();
		return 1;
	case YNJ:
		if (tscCounter > 15)
		{
			if (isKeyPressed(keyJump))
			{
				playSound(18);
				if (yesnoSelect == 1)
					jumpScriptEvent(ascii2num(&tsc[tscPos], 4));
				else
				{
					tscMode = tscPrevMode;
					tscPos += 4;
					//tscNumber = 0;
				}
			}
			else if (isKeyPressed(keyLeft))
			{
				yesnoSelect = 0;
				playSound(1);
			}
			else if (isKeyPressed(keyRight))
			{
				yesnoSelect = 1;
				playSound(1);
			}
		}
		else
		{
			++tscCounter;
		}
		tscCheck();
		return 1;
	case WAS:
		if (currentPlayer.flag & ground)
			tscMode = PARSE;

		tscCheck();
		return 1;
	}

	if (bExit == 1)
	{
		tscCheck();
		return 1;
	}

	while (true)
	{
		while (tsc[tscPos] == '\n' || tsc[tscPos] == '\r')
		{
			strncat(msgText, &tsc[tscPos], 1);
			tscPos++;
		}

		if (tsc[tscPos] != '<') { return 1; }

		//std::string debstr = &tsc[tscPos];
		//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "TSC command", debstr.substr(0, 4).c_str(), NULL);

		switch (tsc[tscPos + 3] + (tsc[tscPos + 2] << 8) + (tsc[tscPos + 1] << 16) + (tsc[tscPos] << 24))
		{
		case('<AE+'):
			tscCleanup(0);
			break;
		case('<AM+'):
			WEAPON weapon;
			memset(&weapon, 0, sizeof(WEAPON));
			weapon.id = ascii2num(&tsc[tscPos + 4], 4);
			weapon.maxAmmo = ascii2num(&tsc[tscPos + 9], 4);
			//weapons.push_back(weapon);
			tscCleanup(2);
			break;
		case('<AM-'):
			//weapons.erase(weapons.begin() + (ascii2num(&tsc[tscPos + 4], 4) - 1));
			tscCleanup(1);
			break;
		case('<AMJ'):
			//for (int w = 0; w < weapons.size(); w++)
			//{
			//	if (weapons[w].id == ascii2num(&tsc[tscPos + 4], 4))
			//	{
			//		jumpScriptEvent(ascii2num(&tsc[tscPos + 9], 4));
			//	}
			//}
			tscCleanup(2);
			break;
		case('<ANP'):
			for (size_t i = 0; i < npcs.size(); i++)
			{
				if ((npcs[i].cond & npccond_alive) && npcs[i].code_event == ascii2num(&tsc[tscPos + 4], 4))
				{
					npcs[i].act_no = ascii2num(&tsc[tscPos + 9], 4);
					if (ascii2num(&tsc[tscPos + 14], 4) != 5)
					{
						if (ascii2num(&tsc[tscPos + 14], 4) == 4)
						{
							if (npcs[i].x >= currentPlayer.x)
								npcs[i].direct = 0;
							else
								npcs[i].direct = 2;
						}
						else
						{
							npcs[i].direct = ascii2num(&tsc[tscPos + 14], 4);
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
			tscUpdateFlags |= 0x20;
			tscCleanup(0);
			break;
		case('<CIL'):
			tscCleanup(0);
			break;
		case('<CLO'):
			memset(msgText, 0, sizeof(msgText));
			tscDisplayFlags &= ~MSGbox;
			tscMode = PARSE;
			tscCleanup(0);
			break;
		case('<CLR'):
			memset(msgText, 0, sizeof(msgText));
			tscCleanup(0);
			break;
		case('<CMP'):
			changeTile(ascii2num(&tsc[tscPos + 4], 4), ascii2num(&tsc[tscPos + 9], 4),
				ascii2num(&tsc[tscPos + 14], 4));

			tscCleanup(3);
			break;
		case('<CMU'):
			tscCleanup(1);
			break;
		case('<CNP'):
			changeNpc(ascii2num(&tsc[tscPos + 4], 4),
				ascii2num(&tsc[tscPos + 9], 4),
				ascii2num(&tsc[tscPos + 14], 4));
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
					if (npcs[i].code_char == ascii2num(&tsc[tscPos + 4], 4))
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
					if (npcs[i].code_event == ascii2num(&tsc[tscPos + 4], 4))
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
					if (npcs[n].code_char == ascii2num(&tsc[tscPos + 4], 4))
					{
						jumpScriptEvent(ascii2num(&tsc[tscPos + 9], 4));
						break;
					}
				}
			}

			break;
		case('<END'):
			memset(msgText, 0, sizeof(msgText));
			gameFlags |= 3;
			tscUpdateFlags = 0;
			tscDisplayFlags = 0;
			faceNo = 0;
			gitNo = 0;
			currentPlayer.cond &= ~player_interact;
			tscMode = END;
			return 1;
		case('<EQ+'):
			currentPlayer.equip &= ascii2num(&tsc[tscPos + 4], 4);
			tscCleanup(1);
			break;
		case('<EQ-'):
			currentPlayer.equip &= ~ascii2num(&tsc[tscPos + 4], 4);
			tscCleanup(1);
			break;
		case('<ESC'):
			return 2;
		case('<EVE'):
			jumpScriptEvent(ascii2num(&tsc[tscPos + 4], 4));
			memset(msgText, 0, sizeof(msgText));
			faceNo = 0;
			gitNo = 0;
			break;
		case('<FAC'):
			faceX = -40;
			faceNo = ascii2num(&tsc[tscPos + 4], 4);
			tscCleanup(1);
			break;
		case('<FAI'):
			tscMode = FADE;
			tscDisplayFlags |= FAI;
			tscDisplayFlags &= ~FAO;
			fadeCounter = 0;
			fadeDirection = ascii2num(&tsc[tscPos + 4], 4);
			tscCleanup(1);
			return 1;
		case('<FAO'):
			tscMode = FADE;
			tscDisplayFlags |= FAO;
			tscDisplayFlags &= ~FAI;
			fadeCounter = 0;
			fadeDirection = ascii2num(&tsc[tscPos + 4], 4);
			tscCleanup(1);
			return 1;
		case('<FL+'):
			setFlag(ascii2num(&tsc[tscPos + 4], 4));
			tscCleanup(1);
			break;
		case('<FL-'):
			clearFlag(ascii2num(&tsc[tscPos + 4], 4));
			tscCleanup(1);
			break;
		case('<FLA'):
			tscCleanup(0);
			break;
		case('<FLJ'):
			if (getFlag(ascii2num(&tsc[tscPos + 4], 4)))
				jumpScriptEvent(ascii2num(&tsc[tscPos + 9], 4));
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
			viewport.speed = ascii2num(&tsc[tscPos + 4], 4);
			tscCleanup(1);
			break;
		case('<FON'):
			for (size_t n = 0; n < npcs.size(); n++)
			{
				if (npcs[n].cond & npccond_alive)
				{
					if (npcs[n].code_event == ascii2num(&tsc[tscPos + 4], 4))
					{
						viewport.lookX = &npcs[n].x;
						viewport.lookY = &npcs[n].y;
						viewport.speed = ascii2num(&tsc[tscPos + 9], 4);
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
			gitNo = ascii2num(&tsc[tscPos + 4], 4);
			tscCleanup(1);
			break;
		case('<HMC'):
			currentPlayer.cond &= ~player_visible;
			tscCleanup(0);
			break;
		case('<INI'):
			tscMode = END;
			gameFlags = 3;
			loadProfile();
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
			gameFlags = 3;
			tscMode = END;
			loadProfile();
			tscCleanup(0);
			return 1;
		case('<LI+'):
			currentPlayer.life += ascii2num(&tsc[tscPos + 4], 4);
			if (currentPlayer.life > currentPlayer.max_life) { currentPlayer.life = currentPlayer.max_life; }
			tscCleanup(1);
			break;
		case('<ML+'):
			currentPlayer.max_life += ascii2num(&tsc[tscPos + 4], 4);
			currentPlayer.life += ascii2num(&tsc[tscPos + 4], 4);
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
			mapNameDisplayTimer = 0;
			tscCleanup(0);
			break;
		case('<MNP'):
			tscCleanup(4);
			break;
		case('<MOV'):
			currentPlayer.setPos(TILE2COORD(ascii2num(&tsc[tscPos + 4], 4)),
				TILE2COORD(ascii2num(&tsc[tscPos + 9], 4)));
			tscCleanup(2);
			break;
		case('<MPJ'):
			tscCleanup(1);
			break;
		case('<MP+'):
			tscCleanup(1);
			break;
		case('<MS2'):
			scrollOffset = 0;
			tscMode = MSG;
			tscDisplayFlags |= (invisMSG | MSGbox);
			msgBoxX = (screenWidth >> 1) - 122;
			msgBoxY = 24;
			tscCounter = 4;
			tscUpdateFlags &= ~0x10;
			memset(msgText, 0, sizeof(msgText));
			tscCleanup(0);
			break;
		case('<MS3'):
			scrollOffset = 0;
			tscMode = MSG;
			tscDisplayFlags &= ~invisMSG;
			tscDisplayFlags |= MSGbox;
			msgBoxX = (screenWidth >> 1) - 122;
			msgBoxY = 24;
			tscCounter = 4;
			tscUpdateFlags &= ~0x10;
			memset(msgText, 0, sizeof(msgText));
			tscCleanup(0);
			break;
		case('<MSG'):
			scrollOffset = 0;
			tscMode = MSG;
			tscDisplayFlags &= ~invisMSG;
			tscDisplayFlags |= MSGbox;
			msgBoxX = (screenWidth >> 1) - 122;
			msgBoxY = (screenHeight - 2) - 64;
			tscCounter = 4;
			tscUpdateFlags &= ~0x10;
			memset(msgText, 0, sizeof(msgText));
			tscCleanup(0);
			break;
		case('<MYB'):
			tscCleanup(1);
			break;
		case('<MYD'):
			currentPlayer.setDir(ascii2num(&tsc[tscPos + 4], 4));
			tscCleanup(1);
			break;
		case('<NCJ'):
			tscCleanup(2);
			break;
		case('<NOD'):
			tscCleanup(0);
			tscPrevMode = tscMode;
			tscMode = NOD;
			return 1;
		case('<NUM'):
			tscCleanup(1);
			break;
		case('<PRI'):
			gameFlags &= ~3;
			tscCleanup(0);
			break;
		case('<PS+'):
			tscCleanup(2);
			break;
		case('<QUA'):
			tscCleanup(1);
			break;
		case('<RMU'):
			tscCleanup(0);
			break;
		case('<SAT'):
			tscUpdateFlags |= 0x20;
			tscCleanup(0);
			break;
		case('<SIL'):
			tscCleanup(1);
			break;
		case('<SK+'):
			setSkipFlag(ascii2num(&tsc[tscPos + 4], 4));
			tscCleanup(1);
			break;
		case('<SK-'):
			clearSkipFlag(ascii2num(&tsc[tscPos + 4], 4));
			tscCleanup(1);
			break;
		case('<SKJ'):
			if (getSkipFlag(ascii2num(&tsc[tscPos + 4], 4)) == true)
				jumpScriptEvent(ascii2num(&tsc[tscPos + 9], 4));
			else
				tscCleanup(2);
			break;
		case('<SLP'):
			tscCleanup(0);
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
				ascii2num(&tsc[tscPos + 4], 4),
				TILE2COORD(ascii2num(&tsc[tscPos + 9], 4)),
				TILE2COORD(ascii2num(&tsc[tscPos + 14], 4)),
				0,
				0,
				ascii2num(&tsc[tscPos + 19], 4),
				NULL);
			tscCleanup(4);
			break;
		case('<SOU'):
			playSound(ascii2num(&tsc[tscPos + 4], 4));
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
			tscCleanup(3);
			break;
		case('<TRA'):
			num = ascii2num(&tsc[tscPos + 9], 4);
			currentPlayer.setPos(TILE2COORD(ascii2num(&tsc[tscPos + 14], 4)),
				TILE2COORD(ascii2num(&tsc[tscPos + 19], 4)));
			loadLevel(ascii2num(&tsc[tscPos + 4], 4));
			currentPlayer.cond &= ~player_interact;
			jumpScriptEvent(num);
			return 1;
		case('<TUR'):
			tscCleanup(0);
			tscUpdateFlags |= 0x10;
			break;
		case('<UNI'):
			tscCleanup(1);
			break;
		case('<UNJ'):
			tscCleanup(2);
			break;
		case('<WAI'):
			tscPrevMode = tscMode;
			tscMode = WAI;
			tscCounter = 0;
			waitAmount = ascii2num(&tsc[tscPos + 4], 4);
			tscCleanup(1);
			return 1;
		case('<WAS'):
			tscMode = WAS;
			tscCleanup(0);
			return 1;
		case('<XX1'):
			tscCleanup(1);
			break;
		case('<YNJ'):
			tscPrevMode = tscMode;
			tscMode = YNJ;
			yesnoSelect = 0;
			ynjBoxYOffset = 0;
			playSound(5);
			tscCleanup(0);
			break;
		case('<ZAM'):
			tscCleanup(0);
			break;
		default:
			memset(str, 0x00, sizeof(str));
			strncpy(str, &tsc[tscPos], 4);
			strcat(str, " is not a valid tsc command.");
			doCustomError(str);
			break;
		}
	}

	return 1;
}
