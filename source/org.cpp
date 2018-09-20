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

using std::string;
using std::vector;
using std::ifstream;
using std::getline;
using std::FILE;
using std::fopen;
using std::fseek;
using std::ftell;
using std::malloc;
using std::fread;
using std::fclose;

WAVE orgWaves[8][8][2];
DRUM orgDrums[8];

vector<string> musicList;

MUSICINFO org;

Uint32 currentOrg = 0;
Uint32 prevOrg = 0;
Uint32 prevOrgPos = 0;

int orgVolume = 1;
bool orgFadeout = false;

bool disableOrg = false;

void organyaAllocNote(uint16_t alloc)
{
	for (auto& j : org.tdata) {
		j.wave_no = 0;
		j.note_list = nullptr; //I want to make the constructor
		j.note_p = static_cast<NOTELIST*>(calloc(alloc, sizeof(NOTELIST))); //new NOTELIST[alloc];

		if (j.note_p == nullptr)
			return;

		for (int i = 0; i < alloc; i++) {
			(j.note_p + i)->from = nullptr;
			(j.note_p + i)->to = nullptr;
			(j.note_p + i)->length = 0;
			(j.note_p + i)->pan = 0xFF;
			(j.note_p + i)->volume = 0xFF;
			(j.note_p + i)->y = 0xFF;
		}
	}
}

void organyaReleaseNote()
{
	for (auto& i : org.tdata)
		free(i.note_p); //delete org.tdata[i].note_p;

	for (auto& i : orgDrums)
		free(i.wave);
}

//sound function things
typedef struct {
	short wave_size;
	short oct_par;
	short oct_size;
}OCTWAVE;

OCTWAVE oct_wave[8] = {
	{ 256,  1,  4 }, //0 Oct
	{ 256,  2,  8 }, //1 Oct
	{ 128,  4, 12 }, //2 Oct
	{ 128,  8, 16 }, //3 Oct
	{  64, 16, 20 }, //4 Oct
	{  32, 32, 24 }, //5 Oct
	{  16, 64, 28 }, //6 Oct
	{   8,128, 32 }, //7 Oct
};

void mixOrg(int16_t *stream, int len)
{
	if (stream == nullptr)
		doCustomError("stream was nullptr in mixOrg");

	org.playing = SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS;

	if (org.loaded)
	{
		for (int i = 0; i < len; i++)
		{
			if (org.loaded)
			{
				if (org.playing)
				{
					//Update
					const int samplesPerBeat = sampleRate * org.wait / 1000;
					const int samplesPerFrame = sampleRate * framerate / 1000;

					if (org.samples-- == 0)
					{
						if (orgFadeout && orgVolume > 0)
							orgVolume -= 2;
						if (orgVolume < 0)
							orgVolume = 0;
						playData();
						org.samples = samplesPerBeat;
					}

					if (++org.samplesForFrame > samplesPerFrame)
						org.samplesForFrame = 0;
				}
				else
				{
					for (int j = 0; j < 8; j++)
					{
						for (int k = 0; k < 8; k++)
						{
							for (int m = 0; m < 2; m++)
							{
								orgWaves[j][k][m].playing = false;
							}
						}
					}
				}

				//Put current stream sample into temp samples (org's done first so this is typically 0 anyways)
				auto tempSampleL = (int32_t)stream[2 * i];
				auto tempSampleR = (int32_t)stream[2 * i + 1];

				//Play waves
				for (int wave = 0; wave < 8; wave++)
				{
					for (int j = 0; j < 8; j++)
					{
						for (int k = 0; k < 2; k++)
						{
							const long double waveSamples = (long double)orgWaves[wave][j][k].freq / (long double)sampleRate;

							if (orgWaves[wave][j][k].playing)
							{
								orgWaves[wave][j][k].pos = (orgWaves[wave][j][k].pos + waveSamples);

								if (orgWaves[wave][j][k].loops)
									orgWaves[wave][j][k].pos = fmod(orgWaves[wave][j][k].pos, (long double)orgWaves[wave][j][k].length);

								if (orgWaves[wave][j][k].loops == false && orgWaves[wave][j][k].pos >= orgWaves[wave][j][k].length)
								{
									orgWaves[wave][j][k].playing = false;
									orgWaves[wave][j][k].pos = 0;
								}
								else
								{
									const size_t s_offset_1 = (size_t)floor(orgWaves[wave][j][k].pos);

									const int sample1 = orgWaves[wave][j][k].wave[s_offset_1] << 7;
									int sample2 = 0;

									if (orgWaves[wave][j][k].loops || s_offset_1 < orgWaves[wave][j][k].length - 1)
										sample2 = orgWaves[wave][j][k].wave[(s_offset_1 + 1) % orgWaves[wave][j][k].length] << 7;

									const auto val = (int)(sample1 + (sample2 - sample1) * fmod(orgWaves[wave][j][k].pos, 1.0f));

									tempSampleL += (int32_t)((long double)val * orgWaves[wave][j][k].volume * orgWaves[wave][j][k].volume_l);
									tempSampleR += (int32_t)((long double)val * orgWaves[wave][j][k].volume * orgWaves[wave][j][k].volume_r);
								}
							}
						}
					}
				}

				//Play drums
				for (int wave = 0; wave < 8; wave++)
				{
					const long double waveSamples = (long double)orgDrums[wave].freq / (long double)sampleRate;

					if (orgDrums[wave].playing)
					{
						orgDrums[wave].pos = (orgDrums[wave].pos + waveSamples);

						if (orgDrums[wave].pos >= orgDrums[wave].length)
							orgDrums[wave].playing = false;
						else
						{
							const size_t s_offset_1 = (size_t)floor(orgDrums[wave].pos);

							const int sample1 = (orgDrums[wave].wave[s_offset_1] - 0x80) << 7;
							int sample2 = 0;

							if (s_offset_1 < orgDrums[wave].length - 1)
								sample2 = (orgDrums[wave].wave[s_offset_1 + 1] - 0x80) << 7;

							const auto val = (int)(sample1 + (sample2 - sample1) * fmod(orgDrums[wave].pos, 1.0f));

							tempSampleL += (int32_t)((long double)val * orgDrums[wave].volume * orgDrums[wave].volume_l);
							tempSampleR += (int32_t)((long double)val * orgDrums[wave].volume * orgDrums[wave].volume_r);
						}
					}
				}

				//Clip buffer
				tempSampleL = clamp(tempSampleL, -0x7FFF, 0x7FFF);
				tempSampleR = clamp(tempSampleR, -0x7FFF, 0x7FFF);

				//Put into main stream
				stream[2 * i] = tempSampleL;
				stream[2 * i + 1] = tempSampleR;
			}
		}
	}
}

//Allocate notes
void noteAlloc(uint16_t alloc)
{
	for (int j = 0; j < 16; j++) {
		org.tdata[j].wave_no = 0;
		org.tdata[j].note_list = nullptr;
		org.tdata[j].note_p = new NOTELIST[alloc];

		if (org.tdata[j].note_p == nullptr)
			return;

		for (int i = 0; i < alloc; i++) {
			(org.tdata[j].note_p + i)->from = nullptr;
			(org.tdata[j].note_p + i)->to = nullptr;
			(org.tdata[j].note_p + i)->length = 0;
			(org.tdata[j].note_p + i)->pan = 0xFF;
			(org.tdata[j].note_p + i)->volume = 0xFF;
			(org.tdata[j].note_p + i)->y = 0xFF;
		}
	}
}

//Release allocated notes
void releaseNote(void)
{
	for (int i = 0; i < 16; i++) {
		delete org.tdata[i].note_p;
	}
}

//Load wave100
char *wave_data = nullptr;

bool loadWave100()
{
	//Allocate data
	if (wave_data == nullptr)
		wave_data = (char *)malloc(0x100 * 100);

	FILE *fp;
	if ((fp = fopen("data/Wave100.dat", "rb")) == nullptr)
		return false;

	fread(wave_data, sizeof(char), 0x100 * 100, fp);
	fclose(fp);
	return true;
}

bool deleteWave100()
{
	free(wave_data);
	return true;
}

//Make Organya Wave
void releaseOrganyaObject(uint8_t track) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 2; j++)
		{
			free(orgWaves[track][i][j].wave);
		}
	}
	memset(orgWaves[track], 0, sizeof(orgWaves[track]));
}

bool makeSoundObject8(char *wavep, uint8_t track, bool pipi)
{
	size_t i, j, k;
	size_t wave_size;
	size_t data_size;

	for (j = 0; j < 8; j++) {
		for (k = 0; k < 2; k++) {
			wave_size = oct_wave[j].wave_size;

			if (pipi)
				data_size = wave_size * oct_wave[j].oct_size;
			else
				data_size = wave_size;

			//Allocate wave
			orgWaves[track][j][k].wave = (int8_t*)malloc(data_size);
			orgWaves[track][j][k].length = data_size;

			//Copy data to wave
			size_t wavePos = 0;
			for (i = 0; i < data_size; i++) {
				orgWaves[track][j][k].wave[i] = *(wavep + wavePos);
				wavePos += (uint16_t)(0x100 / wave_size);
				if (wavePos >= 0x100)
					wavePos -= 0x100;
			}

			//Set default things
			orgWaves[track][j][k].volume = 1.0;
			orgWaves[track][j][k].volume_l = 1.0;
			orgWaves[track][j][k].volume_r = 1.0;
		}
	}

	return true;
}

bool makeOrganyaWave(uint8_t track, uint8_t wave_no, bool pipi)
{
	if (wave_no >= 100)
		return false;
	releaseOrganyaObject(track);
	makeSoundObject8(wave_data + (wave_no * 0x100), track, pipi);
	return true;
}


//Init drum object
void releaseDrumObject(uint8_t track) {
	free(orgDrums[track].wave);
	memset(&orgDrums[track], 0, sizeof(DRUM));
}

const char *drumLookup[8] = {
	"data/Sound/96.pxt",
	"data/Sound/97.pxt",
	"data/Sound/98.pxt",
	"data/Sound/99.pxt",
	"data/Sound/9A.pxt",
	"data/Sound/9B.pxt",
	"data/Sound/96.pxt",
	"data/Sound/96.pxt",
};

bool initDrumObject(unsigned int wave_no)
{
	if (wave_no >= _countof(drumLookup) || drumLookup[wave_no] == nullptr)
		return false;
	releaseDrumObject(wave_no); //Unload previous drum
	loadSound(drumLookup[wave_no], &orgDrums[wave_no].wave, &orgDrums[wave_no].length);
	return true;
}

// Load musicList from musicList.txt
void loadMusicList(const char *path)
{
	musicList = getLinesFromFile(path);
}

void initOrganya()
{
	if (!loadWave100())
		doCustomError("Couldn't open data/Wave100.dat");
	loadMusicList("data/Org/musicList.txt");
}

//Play melody functions
double freq_tbl[12] = { 261.62556530060, 277.18263097687, 293.66476791741, 311.12698372208, 329.62755691287, 349.22823143300, 369.99442271163, 391.99543598175, 415.30469757995, 440.00000000000, 466.16376151809, 493.88330125612 };

void changeOrganFrequency(unsigned char key, uint8_t track, int32_t a)
{
	double tmpDouble;
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 2; i++) {
			tmpDouble = (((double)oct_wave[j].wave_size * freq_tbl[key])*(double)oct_wave[j].oct_par) / 8.00f + ((double)a - 1000.0f);
			orgWaves[track][j][i].freq = (size_t)tmpDouble;
		}
	}
}

short pan_tbl[13] = { 0,43,86,129,172,215,256,297,340,383,426,469,512 };
unsigned char old_key[16] = { 255,255,255,255,255,255,255,255 };
unsigned char key_on[16] = { 0 };
unsigned char key_twin[16] = { 0 };

void changeOrganPan(unsigned char /*key*/, unsigned char pan, uint8_t track)
{
	if (old_key[track] != 0xFF)
	{
		orgWaves[track][old_key[track] / 12][key_twin[track]].volume_l = 1.0f;
		orgWaves[track][old_key[track] / 12][key_twin[track]].volume_r = 1.0f;

		const int pan_val = pan_tbl[pan];

		orgWaves[track][old_key[track] / 12][key_twin[track]].volume_l = (512.0f - (long double)pan_val) / 256.0f;
		orgWaves[track][old_key[track] / 12][key_twin[track]].volume_r = (long double)pan_val / 256.0f;

		if (orgWaves[track][old_key[track] / 12][key_twin[track]].volume_l > 1.0f) orgWaves[track][old_key[track] / 12][key_twin[track]].volume_l = 1.0f;
		if (orgWaves[track][old_key[track] / 12][key_twin[track]].volume_r > 1.0f) orgWaves[track][old_key[track] / 12][key_twin[track]].volume_r = 1.0f;
	}
}

constexpr long double orgVolumeMin = 0.204;
void changeOrganVolume(int /*no*/, long volume, uint8_t track)
{
	if (old_key[track] != 0xFF)
		orgWaves[track][old_key[track] / 12][key_twin[track]].volume = orgVolumeMin + ((long double)volume / 255.0f * (1.0 - orgVolumeMin));
}

void playOrganObject(unsigned char key, int play_mode, uint8_t track, int32_t freq)
{
	switch (play_mode) {
	case 0: //Stop
		orgWaves[track][old_key[track] / 12][key_twin[track]].playing = false;
		orgWaves[track][old_key[track] / 12][key_twin[track]].pos = 0;
		break;
	case 1:
		break;
	case 2: //Stop
		if (old_key[track] != 255) {
			orgWaves[track][old_key[track] / 12][key_twin[track]].playing = true;
			orgWaves[track][old_key[track] / 12][key_twin[track]].loops = false;
			old_key[track] = 255;
		}
		break;
	case -1: //Play
		if (old_key[track] == 255) //New note
		{
			changeOrganFrequency(key % 12, track, freq);
			orgWaves[track][key / 12][key_twin[track]].playing = true;
			orgWaves[track][key / 12][key_twin[track]].loops = true;
			old_key[track] = key;
			key_on[track] = 1;
		}
		else if (key_on[track] == 1 && old_key[track] == key) //Same note
		{
			orgWaves[track][old_key[track] / 12][key_twin[track]].playing = true;
			orgWaves[track][old_key[track] / 12][key_twin[track]].loops = false;
			key_twin[track]++;
			if (key_twin[track] == 2)key_twin[track] = 0;
			orgWaves[track][key / 12][key_twin[track]].playing = true;
			orgWaves[track][key / 12][key_twin[track]].loops = true;
		}
		else //Different note
		{
			orgWaves[track][old_key[track] / 12][key_twin[track]].playing = true;
			orgWaves[track][old_key[track] / 12][key_twin[track]].loops = false;
			key_twin[track]++;
			if (key_twin[track] == 2)key_twin[track] = 0;
			changeOrganFrequency(key % 12, track, freq);
			orgWaves[track][key / 12][key_twin[track]].playing = true;
			orgWaves[track][key / 12][key_twin[track]].loops = true;
			old_key[track] = key;
		}
		break;
	}
}
void playOrganObject2(unsigned char key, int play_mode, uint8_t track, int32_t freq)
{
	switch (play_mode) {
	case 0: //Stop
		orgWaves[track][old_key[track] / 12][key_twin[track]].playing = false;
		orgWaves[track][old_key[track] / 12][key_twin[track]].pos = 0;
		break;
	case 1:
		break;
	case 2: //Stop
		if (old_key[track] != 255)
			old_key[track] = 255;
		break;
	case -1:
		if (old_key[track] == 255) //New note
		{
			changeOrganFrequency(key % 12, track, freq);
			orgWaves[track][key / 12][key_twin[track]].playing = true;
			orgWaves[track][key / 12][key_twin[track]].loops = false;
			orgWaves[track][key / 12][key_twin[track]].pos = 0;
			old_key[track] = key;
			key_on[track] = 1;
		}
		else if (key_on[track] == 1 && old_key[track] == key) //Same note
		{
			key_twin[track]++;
			if (key_twin[track] == 2)
				key_twin[track] = 0;
			orgWaves[track][key / 12][key_twin[track]].playing = true;
			orgWaves[track][key / 12][key_twin[track]].loops = false;
			orgWaves[track][key / 12][key_twin[track]].pos = 0;
		}
		else //Different note
		{
			key_twin[track]++;
			if (key_twin[track] == 2)
				key_twin[track] = 0;
			changeOrganFrequency(key % 12, track, freq);
			orgWaves[track][key / 12][key_twin[track]].playing = true;
			orgWaves[track][key / 12][key_twin[track]].loops = false;
			orgWaves[track][key / 12][key_twin[track]].pos = 0;
			old_key[track] = key;
		}
		break;
	}
}

//Play drum functions
void changeDrumFrequency(unsigned char key, uint8_t track)
{
	orgDrums[track].freq = key * 800 + 100;
}

void changeDrumPan(unsigned char pan, uint8_t track)
{
	orgDrums[track].volume_l = 1.0f;
	orgDrums[track].volume_r = 1.0f;

	const int pan_val = pan_tbl[pan];

	orgDrums[track].volume_l = (512.0f - (long double)pan_val) / 256.0f;
	orgDrums[track].volume_r = (long double)pan_val / 256.0f;

	if (orgDrums[track].volume_l > 1.0f) orgDrums[track].volume_l = 1.0f;
	if (orgDrums[track].volume_r > 1.0f) orgDrums[track].volume_r = 1.0f;
}

void changeDrumVolume(long volume, uint8_t track)
{
	orgDrums[track].volume = orgVolumeMin + ((long double)volume / 255.0f * (1.0 - orgVolumeMin));
}

void playDrumObject(uint8_t key, int play_mode, uint8_t track)
{
	switch (play_mode) {
	case 0: //Stop
		orgDrums[track].playing = false;
		orgDrums[track].pos = 0;
		break;
	case 1: //Play
		changeDrumFrequency(key, track);
		orgDrums[track].playing = true;
		orgDrums[track].pos = 0;
		break;
	case 2: //Stop
		break;
	case -1:
		break;
	}
}

//Playing functions
long play_p; //Current playback position
NOTELIST *play_np[16]; //Currently ready to play notes
long now_leng[8] = { 0 };
int trackVol[16] = { 0 };

void clearPlayNp()
{
	memset(play_np, 0, sizeof(play_np));

	for (int i = 0; i < 16; i++)
		play_np[i] = (NOTELIST*)malloc(sizeof(NOTELIST));
}

void setPlayPointer(int32_t x)
{
	for (int i = 0; i < 16; i++)
	{
		play_np[i] = org.tdata[i].note_list;

		while (play_np[i] != nullptr && play_np[i]->x < x)
			play_np[i] = play_np[i]->to; //Set notes to watch
	}

	play_p = x;
}

void playData()
{
	if (!org.loaded)
		return;

	//Melody
	for (int i = 0; i < 8; i++)
	{
		if (org.loaded && play_np[i] != nullptr && play_p == play_np[i]->x)
		{
			if (play_np[i]->y != 0xFF)
			{
				if (org.tdata[i].pipi)
					playOrganObject2(play_np[i]->y, -1, i, org.tdata[i].freq);
				else
					playOrganObject(play_np[i]->y, -1, i, org.tdata[i].freq);
				now_leng[i] = play_np[i]->length;
			}

			if (play_np[i]->pan < 0xFF)
				changeOrganPan(play_np[i]->y, play_np[i]->pan, i);
			if (play_np[i]->volume < 0xFF)
				trackVol[i] = play_np[i]->volume; //changeOrganVolume(play_np[i]->y, play_np[i]->volume, i);

			play_np[i] = play_np[i]->to;
		}

		if (now_leng[i] == 0)
		{
			if (org.tdata[i].pipi)
				playOrganObject2(0, 2, i, org.tdata[i].freq);
			else
				playOrganObject(0, 2, i, org.tdata[i].freq);
		}

		if (now_leng[i] > 0)
			now_leng[i]--;

		if (play_np[i])
			changeOrganVolume(play_np[i]->y, orgVolume * trackVol[i] / 100, i);
	}

	//Drums
	for (int i = 8; i < 16; i++)
	{
		if (org.loaded && play_np[i] != nullptr && play_p == play_np[i]->x)
		{
			if (play_np[i]->y != 0xFF)
				playDrumObject(play_np[i]->y, 1, i - 8);

			if (play_np[i]->pan != 0xFF)
				changeDrumPan(play_np[i]->pan, i - 8);
			if (play_np[i]->volume != 0xFF)
				trackVol[i] = play_np[i]->volume; //changeDrumVolume(play_np[i]->volume, i - 8);

			play_np[i] = play_np[i]->to;
		}

		if (play_np[i])
			changeDrumVolume(orgVolume * trackVol[i] / 100, i - 8);
	}

	//Looping
	play_p++;

	if (play_p >= org.end_x)
		setPlayPointer(org.repeat_x);
}

//Load function
char pass[7] = "Org-01";
char pass2[7] = "Org-02"; //Pipi
char pass3[7] = "Org-03"; //New drums

void loadOrganya(const string& name)
{
	if (disableOrg)
		return;

    logDebug("Loading " + name);

	//Pause sound device
	//SDL_PauseAudioDevice(soundDev, -1);

	//Unload previous things
	org.loaded = false;
	org.playing = false;

	releaseNote();
	memset(&org, 0, sizeof(org));
	noteAlloc(0xFFFF);

	org.loaded = false;
	org.playing = false;

	//Stop currently playing notes
	clearPlayNp();
	memset(old_key, 0xFF, sizeof(old_key));
	memset(key_on, 0, sizeof(key_on));
	memset(key_twin, 0, sizeof(key_twin));
	memset(now_leng, 0, sizeof(now_leng));

	//Load
	SDL_RWops *fp = SDL_RWFromFile(name.c_str(), "rb");

	if (!fp) {
		string msg(name + "couldn't be accessed");
		doCustomError(msg);
		return;
	}

	//Version Check
	uint8_t ver = 0;
	char pass_check[6];

	SDL_RWread(fp, &pass_check[0], sizeof(char), 6);

	if (!memcmp(pass_check, pass, 6))ver = 1;
	if (!memcmp(pass_check, pass2, 6))ver = 2;
	//if (!memcmp(pass_check, pass3, 6))ver = 2;

	if (!ver) {
		SDL_RWclose(fp);
		doCustomError("File given is invalid version, or isn't a proper Organya file");
		return;
	}

	//Set song information
	org.wait = SDL_ReadLE16(fp);
	org.line = SDL_ReadU8(fp);
	org.dot = SDL_ReadU8(fp);
	org.repeat_x = SDL_ReadLE32(fp);
	org.end_x = SDL_ReadLE32(fp);

	for (int i = 0; i < 16; i++) {
		org.tdata[i].freq = SDL_ReadLE16(fp);
		org.tdata[i].wave_no = SDL_ReadU8(fp);
		org.tdata[i].pipi = SDL_ReadU8(fp);
		if (ver == 1)
			org.tdata[i].pipi = false;
		org.tdata[i].note_num = SDL_ReadLE16(fp);
	}

	//Load notes
	NOTELIST *np;

	for (int j = 0; j < 16; j++) {
		//The first note from is NULL
		if (org.tdata[j].note_num == 0) {
			org.tdata[j].note_list = nullptr;
			continue;
		}

		//Make note list
		np = org.tdata[j].note_p;
		org.tdata[j].note_list = org.tdata[j].note_p;
		np->from = nullptr;
		np->to = (np + 1);
		np++;

		for (int i = 1; i < org.tdata[j].note_num; i++) {
			np->from = (np - 1);
			np->to = (np + 1);
			np++;
		}

		//The last note to is NULL
		np--;
		np->to = nullptr;

		//Set note properties
		np = org.tdata[j].note_p; //X position
		for (int i = 0; i < org.tdata[j].note_num; i++) {
			np->x = SDL_ReadLE32(fp);
			np++;
		}

		np = org.tdata[j].note_p; //Y position
		for (int i = 0; i < org.tdata[j].note_num; i++) {
			np->y = SDL_ReadU8(fp);
			np++;
		}

		np = org.tdata[j].note_p; //Length
		for (int i = 0; i < org.tdata[j].note_num; i++) {
			np->length = SDL_ReadU8(fp);
			np++;
		}

		np = org.tdata[j].note_p; //Volume
		for (int i = 0; i < org.tdata[j].note_num; i++) {
			np->volume = SDL_ReadU8(fp);
			np++;
		}

		np = org.tdata[j].note_p; //Pan
		for (int i = 0; i < org.tdata[j].note_num; i++) {
			np->pan = SDL_ReadU8(fp);
			np++;
		}
	}

	SDL_RWclose(fp);

	//Create waves
	for (int j = 0; j < 8; j++)
		makeOrganyaWave(j, org.tdata[j].wave_no, org.tdata[j].pipi);

	for (unsigned int j = 8; j < 16; j++)
		initDrumObject(j - 8);

	//Reset position
	setPlayPointer(0);

	//Set as loaded
	org.loaded = true;
	org.playing = true;

	//Start up sound device again
	//SDL_PauseAudioDevice(soundDev, 0);
}

//Other functions
const char *orgFolder = "data/Org/";

void changeOrg(const uint32_t num)
{
	if (num == currentOrg)
		return;
	prevOrg = currentOrg;
	prevOrgPos = play_p;
	currentOrg = num;
	string path(orgFolder + musicList[num]);
	orgVolume = 100;
	orgFadeout = false;
	loadOrganya(path.c_str());
}

void resumeOrg()
{
	uint32_t temp = 0;
	temp = currentOrg;
	currentOrg = prevOrg;
	prevOrg = temp;
	string path(orgFolder + musicList[currentOrg]);
	temp = play_p;
	orgVolume = 100;
	orgFadeout = false;
	loadOrganya(path.c_str());
	setPlayPointer(prevOrgPos);
	prevOrgPos = temp;
}
