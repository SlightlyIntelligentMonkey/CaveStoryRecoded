#include "org.h"
#include "sound.h"
#include "render.h"
#include "input.h"
#include "filesystem.h"
#include "mathUtils.h"
#include <memory>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <SDL.h>

const char *musicList[]
{
	"xxxx.org",
	"wanpaku.org",
	"anzen.org",
	"gameover.org",
	"gravity.org",
	"weed.org",
	"mdown2.org",
	"fireeye.org",
	"vivi.org",
	"mura.org",
	"fanfale1.org",
	"ginsuke.org",
	"cemetery.org",
	"plant.org",
	"kodou.org",
	"fanfale3.org",
	"fanfale2.org",
	"dr.org",
	"escape.org",
	"jenka.org",
	"maze.org",
	"access.org",
	"ironh.org",
	"grand.org",
	"curly.org",
	"oside.org",
	"requiem.org",
	"wanpak2.org",
	"quiet.org",
	"lastcave.org",
	"balcony.org",
	"lastbtl.org",
	"lastbt3.org",
	"ending.org",
	"zonbie.org",
	"bdown.org",
	"hell.org",
	"jenka2.org",
	"marine.org",
	"ballos.org",
	"toroko.org",
	"white.org"
};

//instruments in memory
int *waveTbl[MAXWAVES] = { nullptr };
DRUM drumTbl[MAXDRUMS] = { nullptr };
//org in memory
ORG *org;
Uint32 currentOrg = 0;
Uint32 prevOrg = 0;
Uint32 prevOrgPos = 0;
//filter variables
const int a = 2;
const int windowWidth = 128;
double lanczos[windowWidth] = { 0 };
const char *orgFolder = "data/Org/";
int16_t pan_tbl[13] = { 0,43,86,129,172,215,256,297,340,383,426,469,512 };

struct OCTWAVE
{
	int16_t wave_size;	//Waveform size
	int16_t oct_par;		//Multiplier for achieving octave (/ 8)
	int16_t oct_size;		//length multiplier or something
};

//octave wave table because pixel dummy
OCTWAVE oct_wave[8] =
{
{ 256,  1, 4 },		//0 Oct
{ 256,  2, 8 },		//1 Oct
{ 128,  4, 12 },	//2 Oct
{ 128,  8, 16 },	//3 Oct
{ 64,  16, 20 },	//4 Oct
{ 32,  32, 24 },	//5 Oct
{ 16,  64, 28 },	//6 Oct
{ 8,  128, 32 },	//7 Oct
};

//frequency table because pixel dummy
double freq_tbl[12] =
{
261.62556530060,
277.18263097687,
293.66476791741,
311.12698372208,
329.62755691287,
349.22823143300,
369.99442271163,
391.99543598175,
415.30469757995,
440.00000000000,
466.16376151809,
493.88330125612
};

// --org starting and exiting functions -- //
//loads the org wave table
void loadWaveTable()
{
	Sint8 *dat = nullptr;
	loadFile("data/Wave100.dat", reinterpret_cast<Uint8**>(&dat));
	if (dat == nullptr)
	{
		doError();
	}
	for (Uint16 w = 0; w < MAXWAVES; w++)
	{
		waveTbl[w] = static_cast<int*>(calloc(4 * 2, 0x100));
		for (Uint32 s = 0; s < 0x100; s++)
		{
			waveTbl[w][s << 1] = (0x7FFFFFFF / 0xFF) * (dat[(w * 0x100) + s]);
			waveTbl[w][(s << 1) + 1] = (0x7FFFFFFF / 0xFF) * (dat[(w * 0x100) + s]);	// Calculated through the same expression ? TBD : Either simplify this or correct this if it's an error
		}
	}
}

//frees the org wave table
void freeWaveTable()
{
	for (Uint16 w = 0; w < 100; w++)
	{
		free(waveTbl[w]);
	}
}
void iniOrg()
{
	loadWaveTable();
	org = static_cast<ORG *>(malloc(sizeof(ORG)));
	memset(org, 0, sizeof(ORG));
}
void exitOrg()
{
	freeWaveTable();
	org->freemem();
	free(org);
}
// -- other org functions -- //
void changeOrg(const int num)
{
	char path[64];
	if (num == currentOrg)
	{
		return;
	}
	org->samplesPerStep = 0;
	org->stepBufLen = 0;
	org->freemem();
	prevOrg = currentOrg;
	prevOrgPos = org->pos;
	currentOrg = num;
	if (num == 0)
	{
		return;
	}
	strcpy(path, orgFolder);
	strcat(path, musicList[num]);
	org->load(path);
}
void resumeOrg()
{
	char path[64];
	Uint32 temp = 0;
	org->samplesPerStep = 0;
	org->stepBufLen = 0;
	org->freemem();
	temp = currentOrg;
	currentOrg = prevOrg;
	prevOrg = temp;
	strcpy(path, orgFolder);
	strcat(path, musicList[currentOrg]);
	org->load(path);

	temp = org->pos;
	org->setPos(prevOrgPos);
	prevOrgPos = temp;
}
// -- note buffer functions -- //
//calculates the attenuation for a volume, algorithm based off of directsound documentation
double calcVolume(int volume)
{
	if (volume > 0)
	{
		volume = 0;
	}
	if (volume < -2048)
	{
		volume = -2048;
	}
	double attenuation = 1 - (static_cast<double>(volume) / -2048);
	return attenuation;
}
//calculates the attenuation for panning
void calcPan(int pan, double *lpan, double *rpan)
{
	if (pan > 2560)
	{
		pan = 2560;
	}
	if (pan < -2560)
	{
		pan = -2560;
	}
	if (pan < 0)
	{
		*lpan = 1 - (static_cast<double>(pan) / -2560);
		*rpan = 1;
	}
	else
	{
		*rpan = 1 - (static_cast<double>(pan) / 2560);
		*lpan = 1;
	}
}

//generates filter coefficients
void genFilter(int nt, int fc, int bw, int g, int fsr)
{
	double x, windowGain, window, ys, yf = 0;
	for (int i = 0; i < nt; i++)
	{
		x = (i - (static_cast<double>(nt) / 2)) * 2.0*M_PI*bw / fsr; //  scale Sinc width
		if (x == 0)
		{
			ys = 1;
		} //  calculate Sinc function
		else
		{
			ys = sinc(x);
		}
		windowGain = g * (4.0 * bw / fsr); //  correct window gain
		//  lanczos window
		window = a * sin(M_PI*(i - (nt / 2)))*sin((M_PI*(i - (nt / 2))) / a) /
			(pow(M_PI, 2)*pow(i - (nt / 2), 2));
		yf = cos((i - nt / 2) * 2.0*M_PI*fc / fsr); //  spectral shift to fc
		lanczos[i] = yf * window * windowGain * ys; // assign coefficient
	}
}

int resamp(int x, const int *indat, int alim, int fmax, int fsr, int wnwdth)
{
	int j = 0;
	double window, r_snc, r_a = 0;
	double r_g = 2 * static_cast<double>(fmax) / fsr; //Calc gain correction factor
	double r_y = 0;
	for (int i = -wnwdth / 2; i < (wnwdth / 2); i++)// For 1 window width
	{
		j = x + i;// Calc input sample index
		// calculate window and calculate Sinc
		window = a * sin(M_PI*(j - x))*sin((M_PI*(j - x)) / a) / (pow(M_PI, 2)*pow((j - x), 2));
		r_a = 2 * M_PI*(j - x)*fmax / fsr;
		if (r_a == 0)
		{
			r_snc = 1;
		}
		else
		{
			r_snc = sinc(r_a);
		}
		if ((j >= 0) && (j < alim))
		{
			r_y += r_g * window * r_snc * indat[j];
		}
	}
	return lround(r_y + 0.5);// Return new filtered sample
}

//changes the frequency of a sound buffer, with len being length in samples
void changeFrequency(int **buf, Uint32 *len, double frequency, Uint32 curFreq)
{
	double inverse = frequency / curFreq;
	auto newLen = static_cast<uint32_t>((*len*curFreq) / frequency);
	auto *tBuf = static_cast<int *>(calloc(4, newLen));
	for (Uint32 s = 0; s < newLen; s++)
	{
		tBuf[(s)] = (*buf)[lround(((s)*inverse) + 0.5)];
	}

	free(*buf);
	*buf = tBuf;
	*len = newLen >> 1;
}

int *createWaveBuf(int wave, Uint32 *size, Uint8 note, Uint16 freq)
{
	double calcFrequency = 0;
	//creates buffer for wave note
	auto buf = static_cast<int *>(calloc(4 * 2, 0x100));
	memcpy(buf, waveTbl[wave], 4 * 2 * 0x100);

	//calculates the frequency of the wave
	calcFrequency =
		(((static_cast<double>(oct_wave[note / 12].wave_size) * freq_tbl[note % 12]) * static_cast<double>(oct_wave[note / 12].oct_par)) / 8.00f)
		+ static_cast<double>(a - 1000.0f);
	*size = 0x200;
	changeFrequency(&buf, size, calcFrequency, 22050);
	return buf;
}
int *createDrumBuf(int drum, Uint32 *size, Uint8 note, Uint16 freq)
{
	//creates buffer for drum 
	auto buf = static_cast<int *>(calloc(4, drumTbl[drum].len));
	memcpy(buf, drumTbl[drum].buf, drumTbl[drum].len << 2);
	//calculates the frequency of the wave
	double calcFrequency = (static_cast<double>(note) * 800) + 100;
	*size = drumTbl[drum].len;
	changeFrequency(&buf, size, calcFrequency, 32500);
	return buf;
}
// -- org class function definitions -- //
void ORG::load(const char *path)
{
	SDL_RWops *orgRaw = SDL_RWFromFile(path, "rb");
	if (orgRaw == nullptr)
	{
		doError();
	}
//reads the weird string thing at the start
	orgRaw->read(orgRaw, header, 1, 6);
	//reads basic information about the song
	wait = SDL_ReadLE16(orgRaw);
	stepPerBar = SDL_ReadU8(orgRaw);
	beatsPerStep = SDL_ReadU8(orgRaw);
	loopStart = SDL_ReadLE32(orgRaw);
	loopEnd = SDL_ReadLE32(orgRaw);
	//reads specific track data
	for (Uint16 i = 0; i < 16; i++)
	{
		track[i].freq = SDL_ReadLE16(orgRaw);
		track[i].wave = SDL_ReadU8(orgRaw);
		track[i].pi = SDL_ReadU8(orgRaw);
		track[i].num_notes = SDL_ReadLE16(orgRaw);
		track[i].noteBuf = nullptr;
		track[i].noteBufLen = 0;
	}
	//reads notes
	for (Uint32 t = 0; t < MAXTRACK; t++)
	{
		if (track[t].num_notes > 0)
			track[t].note_list = (NOTELIST*)calloc(track[t].num_notes, sizeof(NOTELIST));
		else
		{
			track[t].note_list = nullptr;
			continue;
		}
		//sets the pointers correctly
		track[t].note_list->from = nullptr;
		track[t].note_list->to = &track[t].note_list[1];
		for (int n = 1; n < track[t].num_notes; n++)
		{
			track[t].note_list[n].from = &track[t].note_list[n - 1];
			track[t].note_list[n].to = &track[t].note_list[n + 1];
		}
		track[t].note_list[track[t].num_notes - 1].to = nullptr;
		//reads the start for every note in the track
		for (int n = 0; n < track[t].num_notes; n++)
		{
			track[t].note_list[n].start = SDL_ReadLE32(orgRaw);
		}
		//reads the frequency/pitch for the notes
		for (int n = 0; n < track[t].num_notes; n++)
		{
			track[t].note_list[n].note = SDL_ReadU8(orgRaw);
		}
		//reads the length of the notes
		for (int n = 0; n < track[t].num_notes; n++)
		{
			track[t].note_list[n].length = SDL_ReadU8(orgRaw);
		}
		//reads the volume for the notes
		for (int n = 0; n < track[t].num_notes; n++)
		{
			track[t].note_list[n].volume = SDL_ReadU8(orgRaw);
		}
		//reads the pan of the notes
		for (int n = 0; n < track[t].num_notes; n++)
		{
			track[t].note_list[n].pan = SDL_ReadU8(orgRaw);
		}
	}
	//make sure pos is set to start
	setPos(0);

	//wait is in milliseconds, and the sample rate is in hertz (samples/second)
	//so convert frequency first to milliseconds then multiply by wait
	samplesPerStep = (sampleRate*wait) / 1000;
}

//puts org sound for a step into a buffer
void ORG::playData()
{
	//plays waves
	for (int i = 0; i < MAXMELODY; i++)
	{
		if (track[i].note_p != nullptr && pos == track[i].note_p->start)
		{
			if (track[i].note_p->note != 0xFF)
			{
				if (track[i].key == 0xFF)
				{
					//new sound
					free(track[i].noteBuf);
					track[i].noteBuf =
						createWaveBuf(track[i].wave, &track[i].noteBufLen, track[i].note_p->note, track[i].freq);
					track[i].key = track[i].note_p->note;
					track[i].keyOn = 1;
				}
				else if (track[i].keyOn == 1 && track[i].key == track[i].note_p->note)
				{
					//old sound
					track[i].keyTwin++;
					if (track[i].keyTwin == 2)
						track[i].keyTwin = 0;
					free(track[i].noteBuf);
					track[i].noteBuf =
						createWaveBuf(track[i].wave, &track[i].noteBufLen, track[i].note_p->note, track[i].freq);
				}
				else
				{
					if (track[i].noteBuf != nullptr)
					{
						free(track[i].noteBuf);
						track[i].noteBufLen = 0;
					}
					//new sound
					track[i].keyTwin++;
					if (track[i].keyTwin == 2)
						track[i].keyTwin = 0;
					track[i].noteBuf =
						createWaveBuf(track[i].wave, &track[i].noteBufLen, track[i].note_p->note, track[i].freq);
					track[i].key = track[i].note_p->note;
				}
				//set length of note
				if (track[i].pi == 0)
					track[i].length = track[i].note_p->length;
				else
					track[i].length = 1;
			}
			if (track[i].note_p->pan != 0xFF)
			{
				calcPan((pan_tbl[track[i].note_p->pan] - 256) * 10, &track[i].lpan, &track[i].rpan);
			}
			if (track[i].note_p->volume != 0xFF)
			{
				track[i].volume = calcVolume((track[i].note_p->volume - 255) * 8);
			}
			//point to next note
			track[i].note_p = track[i].note_p->to;
		}
		//stops note
		if (track[i].length == 0)
		{
			free(track[i].noteBuf);
			track[i].noteBuf = nullptr;
			track[i].key = 0xFF;
		}
		if (track[i].length > 0)
			track[i].length--;
	}
	//plays drums
	for (int i = MAXMELODY; i < MAXTRACK; i++)
	{
		if (track[i].note_p != nullptr && pos == track[i].note_p->start)
		{
			if (track[i].note_p->note != 0xFF)
			{
				if (track[i].noteBuf != nullptr)
				{
					free(track[i].noteBuf);
				}
				track[i].noteBuf = createDrumBuf(track[i].wave, &track[i].noteBufLen, track[i].note_p->note, track[i].freq);
				track[i].bufPos = 0;
			}
			if (track[i].note_p->pan != 0xFF)
			{
				calcPan((pan_tbl[track[i].note_p->pan] - 256) * 10, &track[i].lpan, &track[i].rpan);
			}
			if (track[i].note_p->volume != 0xFF)
			{
				track[i].volume = calcVolume((track[i].note_p->volume - 255) * 8);
			}
			//point to next note
			track[i].note_p = track[i].note_p->to;
		}
	}
	//increases the position and loops if necessary
	pos++;
	if (pos >= loopEnd)
		setPos(loopStart);

	//mixes tracks into buffer
	free(stepBuf);
	stepBuf = static_cast<int*>(calloc(4 * 2, samplesPerStep));
	//mixes waves
	for (int t = 0; t < 8; t++)
	{
		if (track[t].noteBuf != nullptr)
		{
			for (Uint32 i = 0; i < samplesPerStep; i++)
			{
				stepBuf[i << 1] +=
					static_cast<int>(static_cast<double>(track[t].noteBuf[i % track[t].noteBufLen])*track[t].volume*track[t].lpan);
				stepBuf[(i << 1) + 1] +=
					static_cast<int>(static_cast<double>(track[t].noteBuf[i % track[t].noteBufLen])*track[t].volume*track[t].rpan);
			}
		}
	}
	//mixes drums
	for (int t = 8; t < 16; t++)
	{
		if (track[t].noteBuf != nullptr)
		{
			for (Uint32 i = 0; i < samplesPerStep; i++)
			{
				stepBuf[(i << 1)] +=
					static_cast<int>(static_cast<double>(track[t].noteBuf[track[t].bufPos])*track[t].volume*track[t].volume*track[t].lpan);
				stepBuf[(i << 1) + 1] +=
					static_cast<int>(static_cast<double>(track[t].noteBuf[track[t].bufPos])*track[t].volume*track[t].volume*track[t].rpan);
				if (track[t].bufPos++ > track[t].noteBufLen)
				{
					track[t].noteBufLen = 0;
					track[t].bufPos = 0;
					free(track[t].noteBuf);
					track[t].noteBuf = nullptr;
					break;
				}
			}
		}
	}
	stepBufLen = samplesPerStep << 1;
}

//frees notes and shit
void ORG::freemem()
{
	//frees note information
	for (int t = 0; t < 16; t++)
	{
		if (track[t].note_list != nullptr)
		{
			free(track[t].note_list);
			track[t].note_list = nullptr;
		}
		if (track[t].noteBuf != nullptr)
		{
			free(track[t].noteBuf);
			track[t].noteBuf = nullptr;
		}
	}
}

// -- sound test functions -- //
void playWave(int wave, int *stream, Uint32 len, double frequency)
{
	Uint32 nlen = 0x200;
	auto *tBuf = static_cast<int*>(calloc(4 * 2, 0x100));
	memcpy(tBuf, waveTbl[wave], 0x200);
	changeFrequency(&tBuf, &nlen, frequency, 22050);
	for (Uint32 i = 0; (i << 2) < len >> 1; i++)
	{
		stream[i << 1] += tBuf[i % nlen];
		stream[(i << 1) + 1] += tBuf[i % nlen];
	}
	free(tBuf);
}

char input[6];
int dsEndPos = 0;
char *debugSound()
{
	char temp[16];
	static char retVal[32];
	Uint32 no = 0;
	if ((isKeyPressed(SDL_SCANCODE_1) || isKeyPressed(SDL_SCANCODE_KP_1)) && dsEndPos < sizeof(input))
	{
		strcat(input, "1");
	}
	if ((isKeyPressed(SDL_SCANCODE_2) || isKeyPressed(SDL_SCANCODE_KP_2)) && dsEndPos < sizeof(input))
	{
		strcat(input, "2");
	}
	if ((isKeyPressed(SDL_SCANCODE_3) || isKeyPressed(SDL_SCANCODE_KP_3)) && dsEndPos < sizeof(input))
	{
		strcat(input, "3");
	}
	if ((isKeyPressed(SDL_SCANCODE_4) || isKeyPressed(SDL_SCANCODE_KP_4)) && dsEndPos < sizeof(input))
	{
		strcat(input, "4");
	}
	if ((isKeyPressed(SDL_SCANCODE_5) || isKeyPressed(SDL_SCANCODE_KP_5)) && dsEndPos < sizeof(input))
	{
		strcat(input, "5");
	}
	if ((isKeyPressed(SDL_SCANCODE_6) || isKeyPressed(SDL_SCANCODE_KP_6)) && dsEndPos < sizeof(input))
	{
		strcat(input, "6");
	}
	if ((isKeyPressed(SDL_SCANCODE_7) || isKeyPressed(SDL_SCANCODE_KP_7)) && dsEndPos < sizeof(input))
	{
		strcat(input, "7");
	}
	if ((isKeyPressed(SDL_SCANCODE_8) || isKeyPressed(SDL_SCANCODE_KP_8)) && dsEndPos < sizeof(input))
	{
		strcat(input, "8");
	}
	if ((isKeyPressed(SDL_SCANCODE_9) || isKeyPressed(SDL_SCANCODE_KP_9)) && dsEndPos < sizeof(input))
	{
		strcat(input, "9");
	}
	if ((isKeyPressed(SDL_SCANCODE_0) || isKeyPressed(SDL_SCANCODE_KP_0)) && dsEndPos < sizeof(input))
	{
		strcat(input, "0");
	}
	for (dsEndPos = 0; dsEndPos < sizeof(input); dsEndPos++)
	{
		if (input[dsEndPos] == 0)
		{
			break;
		}
	}
	if (isKeyPressed(SDL_SCANCODE_BACKSPACE) && input[0] != 0)
	{
		input[dsEndPos - 1] = 0;
	}
	if (isKeyPressed(SDL_SCANCODE_RETURN) || isKeyPressed(SDL_SCANCODE_KP_ENTER))
	{
		for (int i = dsEndPos; i > 0; i--)
		{
			no += static_cast<uint32_t>((pow(10, (i - 1)))*(input[abs(i - dsEndPos)] - 0x30));
		}
		if (no > _countof(musicList))
		{
			memset(input, 0, sizeof(input));
			strcpy(retVal, "INVALID ORG");
			return retVal;
		}
		changeOrg(no);
		memset(input, 0, sizeof(input));
		if (musicList[no] != nullptr)
		{
			strcpy(retVal, musicList[no]);
		}
		else
		{
			strcpy(retVal, "nothing");
		}
		return retVal;
	}
	const char *play = "play: ";
	strcpy(temp, play);
	strcat(temp, input);
	drawString(0, 0, temp, nullptr);
	return nullptr;
}
