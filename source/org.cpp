#include "org.h"

#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <SDL_rwops.h>
#include <SDL_video.h>
#include "pxt.h"
#include "sound.h"
#include "filesystem.h"
#include "mathUtils.h"
#include "log.h"
#include "render.h"
#include "main.h"

SOUND *gOrgWaves[8][8][2];
SOUND **gOrgDrums/*[8]*/ = gSounds + 150;

std::vector<std::string> gMusicList;

MUSICINFO gOrg;

Uint32 gCurrentOrg = 0;
Uint32 gPrevOrg = 0;
Uint32 gPrevOrgPos = 0;

int gOrgVolume = 100;
bool gOrgFadeout = false;

bool gDisableOrg = false;

void organyaAllocNote(uint16_t alloc)
{
	for (auto& j : gOrg.tdata) {
		j.wave_no = 0;
		j.note_list = nullptr; //I want to make the constructor
		j.note_p = static_cast<NOTELIST*>(calloc(alloc, sizeof(NOTELIST))); //new NOTELIST[alloc];

		if (j.note_p == nullptr)
			return;

		for (uint16_t i = 0; i < alloc; i++) {
			j.note_p[i].from = nullptr;
			j.note_p[i].to = nullptr;
			j.note_p[i].length = 0;
			j.note_p[i].pan = 0xFF;
			j.note_p[i].volume = 0xFF;
			j.note_p[i].y = 0xFF;
		}
	}
}

void organyaReleaseNote()
{
	for (auto& i : gOrg.tdata)
		free(i.note_p); //delete gOrg.tdata[i].note_p;
}

//sound function things
struct OCTWAVE {
	int16_t wave_size;
	int16_t oct_par;
	int16_t oct_size;
};

OCTWAVE gOctWave[8] = {
	{ 256,  1,  4 }, //0 Oct
	{ 256,  2,  8 }, //1 Oct
	{ 128,  4, 12 }, //2 Oct
	{ 128,  8, 16 }, //3 Oct
	{  64, 16, 20 }, //4 Oct
	{  32, 32, 24 }, //5 Oct
	{  16, 64, 28 }, //6 Oct
	{   8,128, 32 }, //7 Oct
};

void updateOrg(int len)
{
	gOrg.playing = SDL_GetWindowFlags(gWindow) & SDL_WINDOW_INPUT_FOCUS;

	if (gOrg.playing)
	{
		if (gOrg.loaded)
		{
			gOrg.samples -= len;

			while (gOrg.samples <= 0)
			{
				gOrg.samples += sampleRate * gOrg.wait / 1000; // samples per beat

				if (gOrgFadeout && gOrgVolume > 0)
					gOrgVolume -= 2;
				if (gOrgVolume < 0)
					gOrgVolume = 0;
				playData();
			}
		}
	}
	else
	{
		for (int j = 0; j < 8; j++)
			for (int k = 0; k < 8; k++)
				for (int m = 0; m < 2; m++)
					SoundObject_Stop(gOrgWaves[j][k][m]);
	}
}

//Allocate notes
void noteAlloc(uint16_t alloc)
{
	for (int j = 0; j < 16; j++) {
		gOrg.tdata[j].wave_no = 0;
		gOrg.tdata[j].note_list = nullptr;
		gOrg.tdata[j].note_p = new NOTELIST[alloc];

		if (gOrg.tdata[j].note_p == nullptr)
			return;

		for (int i = 0; i < alloc; i++) {
			(gOrg.tdata[j].note_p + i)->from = nullptr;
			(gOrg.tdata[j].note_p + i)->to = nullptr;
			(gOrg.tdata[j].note_p + i)->length = 0;
			(gOrg.tdata[j].note_p + i)->pan = 0xFF;
			(gOrg.tdata[j].note_p + i)->volume = 0xFF;
			(gOrg.tdata[j].note_p + i)->y = 0xFF;
		}
	}
}

//Release allocated notes
void releaseNote(void)
{
	for (int i = 0; i < 16; i++) {
		delete[] gOrg.tdata[i].note_p;
	}
}

//Load wave100
char *gWaveData = nullptr;

bool loadWave100()
{
	//Allocate data
	if (gWaveData == nullptr)
		gWaveData = new char[0x100 * 100];

	FILE *fp;
	if ((fp = fopen("data/Wave100.dat", "rb")) == nullptr)
		return false;

	if (fread(gWaveData, sizeof(char), 0x100 * 100, fp) != 0x100 * 100)
		doCustomError("Couldn't read from data/Wave100.dat"); 
	fclose(fp);
	return true;
}

bool deleteWave100()
{
	delete[] gWaveData;
	return true;
}

//Make Organya Wave
void releaseOrganyaObject(uint8_t track)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			SoundObject_Destroy(gOrgWaves[track][i][j]);
			gOrgWaves[track][i][j] = nullptr;
		}
	}
}

bool makeSoundObject8(const char *wavep, uint8_t track, bool pipi)
{
	size_t i, j, k;
	size_t wave_size;
	size_t data_size;

	for (j = 0; j < 8; j++) {
		for (k = 0; k < 2; k++) {
			wave_size = gOctWave[j].wave_size;

			if (pipi)
				data_size = wave_size * gOctWave[j].oct_size;
			else
				data_size = wave_size;

			gOrgWaves[track][j][k] = SoundObject_Create(data_size, 22050);

			//Allocate wave
			uint8_t *wp = new uint8_t[data_size];
			uint8_t *wp_sub = wp;

			//Copy data to wave
			size_t wavePos = 0;
			for (i = 0; i < data_size; i++) {
				int work = wavep[wavePos];
				work += 0x80;	// Convert to unsigned 8-bit PCM
				*wp_sub = work;
				wavePos += 0x100 / wave_size;
				if (wavePos >= 0x100)
					wavePos -= 0x100;
				++wp_sub;
			}

			uint8_t *sound_object_buffer;
			SoundObject_Lock(gOrgWaves[track][j][k], &sound_object_buffer, nullptr);
			memcpy(sound_object_buffer, wp, data_size);
			SoundObject_Unlock(gOrgWaves[track][j][k]);

			SoundObject_SetPosition(gOrgWaves[track][j][k], 0);

			delete[] wp;
		}
	}

	return true;
}

bool makeOrganyaWave(uint8_t track, uint8_t wave_no, bool pipi)
{
	if (wave_no >= 100)
		return false;
	releaseOrganyaObject(track);
	makeSoundObject8(gWaveData + (wave_no * 0x100), track, pipi);
	return true;
}

// Load gMusicList from gMusicList.txt
void loadMusicList(const std::string& path)
{
	gMusicList = getLinesFromFile(path);
}

void initOrganya()
{
	if (!loadWave100())
		doCustomError("Couldn't open data/Wave100.dat");
	loadMusicList("data/Org/musicList.txt");
}

//Play melody functions

// Ripped from Cave Story
short gFrequenceTable[12] = { 262,277,294,311,330,349,370,392,415,440,466,494 };
// From Org3
//short gFrequenceTable[12] = { 261,278,294,311,329,349,371,391,414,440,466,494 };
//double gFrequenceTable[12] = { 261.62556530060, 277.18263097687, 293.66476791741, 311.12698372208, 329.62755691287, 349.22823143300, 369.99442271163, 391.99543598175, 415.30469757995, 440.00000000000, 466.16376151809, 493.88330125612 };

void changeOrganFrequency(unsigned char key, uint8_t track, int32_t a)
{
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 2; i++)
			SoundObject_SetFrequency(gOrgWaves[track][j][i], ((gOctWave[j].wave_size*gFrequenceTable[key]) * gOctWave[j].oct_par) / 8 + (a - 1000));
}

short gPanTable[13] = { 0,43,86,129,172,215,256,297,340,383,426,469,512 };
unsigned char gOldKey[16] = { 255,255,255,255,255,255,255,255 };
unsigned char gKeyOn[16] = { 0 };
unsigned char gKeyTwin[16] = { 0 };

void changeOrganPan(unsigned char /*key*/, unsigned char pan, uint8_t track)
{
	if (gOldKey[track] != 0xFF)
		SoundObject_SetPan(gOrgWaves[track][gOldKey[track] / 12][gKeyTwin[track]], 10 * (gPanTable[pan] - 256));
}

void changeOrganVolume(int /*no*/, long volume, uint8_t track)
{
	if (gOldKey[track] != 0xFF)
		SoundObject_SetVolume(gOrgWaves[track][gOldKey[track] / 12][gKeyTwin[track]], (volume - 255) * 8);
}

void playOrganObject(unsigned char key, int play_mode, uint8_t track, int32_t freq)
{
	if (gOrgWaves[track][key / 12][gKeyTwin[track]])
	{
		switch (play_mode) {
		case 0: //Stop
			if (gOldKey[track] != 255) {
				SoundObject_Stop(gOrgWaves[track][gOldKey[track] / 12][gKeyTwin[track]]);
				SoundObject_SetPosition(gOrgWaves[track][gOldKey[track] / 12][gKeyTwin[track]], 0);
			}
			break;
		case 1:
			break;
		case 2: //Stop
			if (gOldKey[track] != 255) {
				SoundObject_Play(gOrgWaves[track][gOldKey[track] / 12][gKeyTwin[track]], false);
				gOldKey[track] = 255;
			}
			break;
		case -1: //Play
			if (gOldKey[track] == 255) //New note
			{
				changeOrganFrequency(key % 12, track, freq);
				SoundObject_Play(gOrgWaves[track][key / 12][gKeyTwin[track]], true);
				gOldKey[track] = key;
				gKeyOn[track] = 1;
			}
			else if (gKeyOn[track] == 1 && gOldKey[track] == key) //Same note
			{
				SoundObject_Play(gOrgWaves[track][gOldKey[track] / 12][gKeyTwin[track]], false);
				gKeyTwin[track]++;
				if (gKeyTwin[track] == 2)gKeyTwin[track] = 0;
				SoundObject_Play(gOrgWaves[track][key / 12][gKeyTwin[track]], true);
			}
			else //Different note
			{
				SoundObject_Play(gOrgWaves[track][gOldKey[track] / 12][gKeyTwin[track]], false);
				gKeyTwin[track]++;
				if (gKeyTwin[track] == 2)gKeyTwin[track] = 0;
				changeOrganFrequency(key % 12, track, freq);
				SoundObject_Play(gOrgWaves[track][key / 12][gKeyTwin[track]], true);
				gOldKey[track] = key;
			}
			break;
		}
	}
}

//Play drum functions
void changeDrumFrequency(unsigned char key, uint8_t track)
{
	SoundObject_SetFrequency(gOrgDrums[track], key * 800 + 100);
}

void changeDrumPan(unsigned char pan, uint8_t track)
{
	SoundObject_SetPan(gOrgDrums[track], 10 * (gPanTable[pan] - 256));
}

void changeDrumVolume(long volume, uint8_t track)
{
	SoundObject_SetVolume(gOrgDrums[track], (volume - 255) * 8);
}

void playDrumObject(uint8_t key, int play_mode, uint8_t track)
{
	switch (play_mode) {
	case 0: //Stop
		SoundObject_Stop(gOrgDrums[track]);
		SoundObject_SetPosition(gOrgDrums[track], 0);
		break;
	case 1: //Play
		SoundObject_Stop(gOrgDrums[track]);
		SoundObject_SetPosition(gOrgDrums[track], 0);
		changeDrumFrequency(key, track);
		SoundObject_Play(gOrgDrums[track], false);
		break;
	case 2: //Stop
		break;
	case -1:
		break;
	}
}

//Playing functions
long gPlaybackPosition; //Current playback position
NOTELIST *gReadyNotes[16]; //Currently ready to play notes
long gNowLength[8] = { 0 };
int gTrackVolume[16] = { 0 };

void clearPlayNp()
{
	memset(gReadyNotes, 0, sizeof(gReadyNotes));

	for (int i = 0; i < 16; i++)
		gReadyNotes[i] = new NOTELIST;
}

void setPlayPointer(int32_t x)
{
	for (int i = 0; i < 16; i++)
	{
		gReadyNotes[i] = gOrg.tdata[i].note_list;

		while (gReadyNotes[i] != nullptr && gReadyNotes[i]->x < x)
			gReadyNotes[i] = gReadyNotes[i]->to; //Set notes to watch
	}

	gPlaybackPosition = x;
}

void playData()
{
	if (!gOrg.loaded)
		return;

	//Melody
	for (int i = 0; i < 8; i++)
	{
		if (gOrg.loaded && gReadyNotes[i] != nullptr && gPlaybackPosition == gReadyNotes[i]->x)
		{
			if (gReadyNotes[i]->y != 0xFF)
			{
				playOrganObject(gReadyNotes[i]->y, -1, i, gOrg.tdata[i].freq);
				gNowLength[i] = gReadyNotes[i]->length;
			}

			if (gReadyNotes[i]->pan < 0xFF)
				changeOrganPan(gReadyNotes[i]->y, gReadyNotes[i]->pan, i);
			if (gReadyNotes[i]->volume < 0xFF)
				gTrackVolume[i] = gReadyNotes[i]->volume; //changeOrganVolume(gReadyNotes[i]->y, gReadyNotes[i]->volume, i);

			gReadyNotes[i] = gReadyNotes[i]->to;
		}

		if (gNowLength[i] == 0)
			playOrganObject(0, 2, i, gOrg.tdata[i].freq);

		if (gNowLength[i] > 0)
			gNowLength[i]--;

		if (gReadyNotes[i])
			changeOrganVolume(gReadyNotes[i]->y, gOrgVolume * gTrackVolume[i] / (0x80 - 1), i);
	}

	//Drums
	for (int i = 8; i < 16; i++)
	{
		if (gOrg.loaded && gReadyNotes[i] != nullptr && gPlaybackPosition == gReadyNotes[i]->x)
		{
			if (gReadyNotes[i]->y != 0xFF)
				playDrumObject(gReadyNotes[i]->y, 1, i - 8);

			if (gReadyNotes[i]->pan != 0xFF)
				changeDrumPan(gReadyNotes[i]->pan, i - 8);
			if (gReadyNotes[i]->volume != 0xFF)
				gTrackVolume[i] = gReadyNotes[i]->volume; //changeDrumVolume(gReadyNotes[i]->volume, i - 8);

			gReadyNotes[i] = gReadyNotes[i]->to;
		}

		if (gReadyNotes[i])
			changeDrumVolume(gOrgVolume * gTrackVolume[i] / (0x80 - 1), i - 8);
	}

	//Looping
	gPlaybackPosition++;

	if (gPlaybackPosition >= gOrg.end_x)
		setPlayPointer(gOrg.repeat_x);
}

//Load function
char gPass[] = "Org-01";
char gPass2[] = "Org-02"; //Pipi
//char gPass3[7] = "Org-03"; //New drums

void loadOrganya(const std::string& name)
{
	if (gDisableOrg)
		return;

	logDebug("Loading " + name);

	//Unload previous things
	gOrg.loaded = false;
	gOrg.playing = false;

	releaseNote();
	memset(&gOrg, 0, sizeof(gOrg));
	noteAlloc(0xFFFF);

	gOrg.loaded = false;
	gOrg.playing = false;

	//Stop currently playing notes
	clearPlayNp();
	memset(gOldKey, 0xFF, sizeof(gOldKey));
	memset(gKeyOn, 0, sizeof(gKeyOn));
	memset(gKeyTwin, 0, sizeof(gKeyTwin));
	memset(gNowLength, 0, sizeof(gNowLength));

	//Load
	SDL_RWops *fp = SDL_RWFromFile(name.c_str(), "rb");

	if (!fp) {
		std::string msg(name + "couldn't be accessed");
		doCustomError(msg);
		return;
	}

	//Version Check
	uint8_t ver = 0;
	char pass_check[6];

	SDL_RWread(fp, &pass_check[0], sizeof(char), 6);

	if (!memcmp(pass_check, gPass, 6))ver = 1;
	if (!memcmp(pass_check, gPass2, 6))ver = 2;
	//if (!memcmp(pass_check, pass3, 6))ver = 2;

	if (!ver) {
		SDL_RWclose(fp);
		doCustomError("File given is invalid version, or isn't a proper Organya file");
	}

	//Set song information
	gOrg.wait = SDL_ReadLE16(fp);
	gOrg.line = SDL_ReadU8(fp);
	gOrg.dot = SDL_ReadU8(fp);
	gOrg.repeat_x = SDL_ReadLE32(fp);
	gOrg.end_x = SDL_ReadLE32(fp);

	for (int i = 0; i < 16; i++) {
		gOrg.tdata[i].freq = SDL_ReadLE16(fp);
		gOrg.tdata[i].wave_no = SDL_ReadU8(fp);
		gOrg.tdata[i].pipi = SDL_ReadU8(fp);
		if (ver == 1)
			gOrg.tdata[i].pipi = 0;
		gOrg.tdata[i].note_num = SDL_ReadLE16(fp);
	}

	//Load notes
	NOTELIST *np;

	for (int j = 0; j < 16; j++) {
		//The first note from is NULL
		if (gOrg.tdata[j].note_num == 0) {
			gOrg.tdata[j].note_list = nullptr;
			continue;
		}

		//Make note list
		np = gOrg.tdata[j].note_p;
		gOrg.tdata[j].note_list = gOrg.tdata[j].note_p;
		np->from = nullptr;
		np->to = (np + 1);
		np++;

		for (int i = 1; i < gOrg.tdata[j].note_num; i++) {
			np->from = (np - 1);
			np->to = (np + 1);
			np++;
		}

		//The last note to is NULL
		np--;
		np->to = nullptr;

		//Set note properties
		np = gOrg.tdata[j].note_p; //X position
		for (int i = 0; i < gOrg.tdata[j].note_num; i++) {
			np->x = SDL_ReadLE32(fp);
			np++;
		}

		np = gOrg.tdata[j].note_p; //Y position
		for (int i = 0; i < gOrg.tdata[j].note_num; i++) {
			np->y = SDL_ReadU8(fp);
			np++;
		}

		np = gOrg.tdata[j].note_p; //Length
		for (int i = 0; i < gOrg.tdata[j].note_num; i++) {
			np->length = SDL_ReadU8(fp);
			np++;
		}

		np = gOrg.tdata[j].note_p; //Volume
		for (int i = 0; i < gOrg.tdata[j].note_num; i++) {
			np->volume = SDL_ReadU8(fp);
			np++;
		}

		np = gOrg.tdata[j].note_p; //Pan
		for (int i = 0; i < gOrg.tdata[j].note_num; i++) {
			np->pan = SDL_ReadU8(fp);
			np++;
		}
	}

	SDL_RWclose(fp);

	//Create waves
	for (int j = 0; j < 8; j++)
		makeOrganyaWave(j, gOrg.tdata[j].wave_no, gOrg.tdata[j].pipi);

	//Reset position
	setPlayPointer(0);

	//Set as loaded
	gOrg.loaded = true;
	gOrg.playing = true;
}

//Other functions
constexpr const char *orgFolder = "data/Org/";

void changeOrg(const uint32_t num)
{
	if (num == gCurrentOrg)
		return;
	gPrevOrg = gCurrentOrg;
	gPrevOrgPos = gPlaybackPosition;
	gCurrentOrg = num;
	std::string path(orgFolder + gMusicList[num]);
	gOrgVolume = 100;
	gOrgFadeout = false;
	loadOrganya(path);
}

void resumeOrg()
{
	uint32_t temp = 0;
	temp = gCurrentOrg;
	gCurrentOrg = gPrevOrg;
	gPrevOrg = temp;
	std::string path(orgFolder + gMusicList[gCurrentOrg]);
	temp = gPlaybackPosition;
	gOrgVolume = 100;
	gOrgFadeout = false;
	loadOrganya(path);
	setPlayPointer(gPrevOrgPos);
	gPrevOrgPos = temp;
}
