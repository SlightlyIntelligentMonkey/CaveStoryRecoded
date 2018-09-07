#include "org.h"
#include "sound.h"
#include "render.h"
#include "input.h"
#include "filesystem.h"
#include "mathUtils.h"

#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <SDL.h>

using std::string;
using std::vector;

WAVE orgWaves[8];
DRUM orgDrums[8];

vector<string> musicList;

MUSICINFO org;
Uint32 currentOrg = 0;
Uint32 prevOrg = 0;
Uint32 prevOrgPos = 0;

uint8_t orgVolume = 100;
bool orgFadeout = false;

bool disableOrg = false;

const int filterWidth = 128;
double filter[filterWidth] = { 0 };

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
constexpr __int16 octfreq[12] = { 1, 2, 4, 8, 16, 32, 64, 128, 0, 0, 0, 0 };
constexpr double notefreq[12] = { 261.62556530060, 277.18263097687, 293.66476791741, 311.12698372208, 329.62755691287, 349.22823143300, 369.99442271163, 391.99543598175, 415.30469757995, 440.00000000000, 466.16376151809, 493.88330125612 };

/*typedef struct {
	short wave_size;
	short oct_par;
	short oct_size;
}OCTWAVE;

OCTWAVE oct_wave[8] = {
	{ 256,  1, 4 },//0 Oct
	{ 256,  2, 8 },//1 Oct
	{ 128,  4, 12 },//2 Oct
	{ 128,  8, 16 },//3 Oct
	{ 64, 16, 20 },//4 Oct
	{ 32, 32, 24 },
	{ 16, 64, 28 },
	{ 8,128, 32 },
};

double freq_tbl[12] = { 261.62556530060, 277.18263097687, 293.66476791741, 311.12698372208, 329.62755691287, 349.22823143300, 369.99442271163, 391.99543598175, 415.30469757995, 440.00000000000, 466.16376151809, 493.88330125612 };
*/

//simply generates an array to approximate rather then do it in real time
void genFilter()
{
	int halfWidth = filterWidth / 2;
	double windowApprox = 0;
	for (int i = 0; i < filterWidth; i++)
	{
		windowApprox = sinc(i - halfWidth) / sinc(i - halfWidth);
		filter[i] = sinc(i - halfWidth)*windowApprox;
	}
}
 
//multiplies it all out
int16_t lowPassFilter(int16_t *stream, int len)
{
	for (int s = 0; s < len; s++)
	{
		for (int i = 0; i < filterWidth; i++)
			stream[s] += (stream[s - ((filterWidth-i)/2)] * filter[i]);
	}
	return 1;
}

void mixOrg(int16_t *stream, int len)
{
	if (stream == nullptr)
		doCustomError("stream was nullptr in mixOrg");

	for (int i = 0; i < len; i++)
	{
		//Update
		const int samplesPerBeat = sampleRate * org.wait / 1000;
		if (++org.samples > samplesPerBeat)
		{
			organyaPlayStep();
			org.samples = 0;
		}

		//Put current stream sample into temp samples (org's done first so this is typically 0 anyways)
		auto tempSampleL = (int32_t)stream[2 * i];
		auto tempSampleR = (int32_t)stream[2 * i + 1];

		//Play waves
		for (int wave = 0; wave < 8; wave++)
		{
			const int waveSamples = (int)(((long double)(octfreq[orgWaves[wave].key / 12] * (signed int)notefreq[orgWaves[wave].key 
				% 12])
				* 32.0
				+ (long double)org.tdata[wave].freq
				- 1000.0)
				/ 44100.0
				* 4096.0);
			
			if (orgWaves[wave].playing)
			{
				orgWaves[wave].pos = (orgWaves[wave].pos + waveSamples) & 0xFFFFF;
				const unsigned int s_offset_1 = orgWaves[wave].pos >> 12;

				const int sample1 = orgWaves[wave].wave[s_offset_1] << 7;
				const int sample2 = orgWaves[wave].wave[(s_offset_1 + 1) % 0x100] << 7;//(unsigned __int8)(((unsigned int)((s_offset_1 + 1) >> 31) >> 24) + s_offset_1 + 1) - ((unsigned int)((s_offset_1 + 1) >> 31) >> 24)];
				
				const auto val = (int)(sample1 + (sample2 - sample1) * ((double)(orgWaves[wave].pos & 0xFFF) / 4096.0));

				tempSampleL += (int32_t)((long double)val * orgWaves[wave].volume * orgWaves[wave].volume_l * (long double)orgVolume / 100.0);
				tempSampleR += (int32_t)((long double)val * orgWaves[wave].volume * orgWaves[wave].volume_r * (long double)orgVolume / 100.0);
			}
		}

		//Play Drums
		for (int wave = 0; wave < 8; wave++)
		{
			const size_t waveSamples = ((long double)(800 * orgDrums[wave].key + 100) / (double)sampleRate * 4096.0);

			int id;

			switch (org.tdata[wave + 8].wave_no)
			{
			case 0:
				id = 0x96;
				break;
			case 2:
				id = 0x97;
				break;
			case 4:
				id = 0x9A;
				break;
			case 5:
				id = 0x98;
				break;
			case 6:
				id = 0x99;
				break;
			case 9:
				id = 0x9B;
				break;
			default:
				id = 0;
				break;
			}

			if (id)
			{
				if (orgDrums[wave].playing)
				{
					orgDrums[wave].pos += waveSamples;

					if ((orgDrums[wave].pos >> 12) >= sounds[id].length)
						orgDrums[wave].playing = false;
					else
					{
						const size_t s_offset_1 = orgDrums[wave].pos >> 12;

						const int sample1 = (sounds[id].wave[s_offset_1] - 0x80) << 7;
						int sample2 = 0; //orgDrums[wave].wave[(s_offset_1 + 1) % 0x100] - 0x80; //(unsigned __int8)(((unsigned int)((s_offset_1 + 1) >> 31) >> 24) + s_offset_1 + 1) - ((unsigned int)((s_offset_1 + 1) >> 31) >> 24)];

						if ((orgDrums[wave].pos >> 12) < sounds[id].length - 1)
							sample2 = (sounds[id].wave[s_offset_1 + 1] - 0x80) << 7;

						const auto val = (int)(sample1 + (sample2 - sample1) * ((double)(orgDrums[wave].pos & 0xFFF) / 4096.0));

						tempSampleL += (int32_t)((long double)val * orgDrums[wave].volume * orgDrums[wave].volume_l * (long double)orgVolume / 100.0);
						tempSampleR += (int32_t)((long double)val * orgDrums[wave].volume * orgDrums[wave].volume_r * (long double)orgVolume / 100.0);
					}
				}
			}
		}

		//Put into main stream and clip
		stream[2 * i] = clamp(tempSampleL, -0x7FFF, 0x7FFF);
		stream[2 * i + 1] = clamp(tempSampleR, -0x7FFF, 0x7FFF);
	}
}

// Load musicList from musicList.txt
void loadMusicList(const char *path)
{
	musicList = getLinesFromFile(path);
}

void initOrganya()
{
	//Load music list
	loadMusicList("data/Org/musicList.txt");
	genFilter();
}

///////////////////////////
////==ORGANYA  MELODY==////
///////////////////////////

int32_t play_p; //Current playback position
NOTELIST *play_np[16]; //Currently ready to play notes
int32_t now_leng[16] = { 0 }; //Length of notes during playback

//Change frequency
void changeNoteFrequency(int key, size_t track, int32_t /*a*/)
{
	if (key != 0xFF)
		orgWaves[track].key = key;
}

unsigned char old_key[16] = { 255 }; //Sound during playback
unsigned char key_on[16] = { 0 }; //Key switch
unsigned char key_twin[16] = { 0 }; //Key used now

constexpr long double orgVolumeMin = 0.04;
constexpr int16_t pan_tbl[13] = { 0,43,86,129,172,215,256,297,340,383,426,469,512 };

void changeNotePan(unsigned char pan, size_t track)
{
	if (old_key[track] != 0xFF)
	{
		orgWaves[track].volume_l = 1.0f;
		orgWaves[track].volume_r = 1.0f;

		const int pan_val = pan_tbl[pan];

		orgWaves[track].volume_l = (512.0f - (long double)pan_val) / 256.0f;
		orgWaves[track].volume_r = (long double)pan_val / 256.0f;

		if (orgWaves[track].volume_l > 1.0f) orgWaves[track].volume_l = 1.0f;
		if (orgWaves[track].volume_r > 1.0f) orgWaves[track].volume_r = 1.0f;
	}
}

void changeNoteVolume(int32_t volume, size_t track)
{
	if (old_key[track] != 0xFF)
	{
		orgWaves[track].volume = orgVolumeMin + ((long double)volume / 255.0f * (1.0 - orgVolumeMin));
	}
}

//Play note
void playOrganyaNote(int key, int noteMode, size_t track, int32_t freq)
{
	switch (noteMode)
	{
	case -1:
		if (old_key[track] == 0xFF) //Play
		{
			changeNoteFrequency(key, track, freq); //Set the frequency
			orgWaves[track].playing = true;
			old_key[track] = key;
			key_on[track] = 1;
		}
		else if (key_on[track] == 1 && old_key[track] == key) //Same note
		{
			orgWaves[track].pos = 0; //Reset
			key_twin[track]++;
			if (key_twin[track] == 2)
				key_twin[track] = 0;
			orgWaves[track].playing = true;
		}
		else //Different note
		{
			orgWaves[track].pos = 0; //Reset
			key_twin[track]++;
			if (key_twin[track] == 2)
				key_twin[track] = 0;
			changeNoteFrequency(key, track, freq); //Set the frequency
			orgWaves[track].playing = true;
			old_key[track] = key;
		}
		break;

	case 0: //Stop
		orgWaves[track].playing = false;
		orgWaves[track].pos = 0;
		break;

	case 2: //Stop playing
		if (old_key[track] != 255)
		{
			orgWaves[track].playing = false;
			old_key[track] = 255;
		}
		break;

	default:
		break;
	}
}

///////////////////////////
////===ORGANYA DRUMS===////
///////////////////////////
void changeDrumFrequency(int key, size_t track)
{
	orgDrums[track].key = key;
}

void changeDrumPan(unsigned char pan, size_t track)
{
	orgDrums[track].volume_l = 1.0f;
	orgDrums[track].volume_r = 1.0f;

	const int pan_val = pan_tbl[pan];

	orgDrums[track].volume_l = (512.0f - (long double)pan_val) / 256.0f;
	orgDrums[track].volume_r = (long double)pan_val / 256.0f;

	if (orgDrums[track].volume_l > 1.0f) orgDrums[track].volume_l = 1.0f;
	if (orgDrums[track].volume_r > 1.0f) orgDrums[track].volume_r = 1.0f;
}

void changeDrumVolume(int32_t volume, size_t track)
{
	orgDrums[track].volume = orgVolumeMin + ((long double)volume / 255.0f * (1.0 - orgVolumeMin));
}

void playOrganyaDrum(int key, int drumMode, size_t track)
{
	switch (drumMode) {
	case 0: // Stop
		orgDrums[track].playing = false;
		orgDrums[track].pos = 0;
		break;

	case 1: // Play
		changeDrumFrequency(key, track); //Set the frequency
		orgDrums[track].pos = 0;
		orgDrums[track].playing = true;
		break;

	case 2: // Stop playing
		break;

	case -1:
		break;
	}
}

//Playing functions
void organyaSetPlayPosition(int32_t x)
{
	for (int i = 0; i < 16; i++) {
		play_np[i] = org.tdata[i].note_list;
		while (play_np[i] != nullptr && play_np[i]->x < x)
			play_np[i] = play_np[i]->to; //Set notes to watch
	}

	play_p = x;
}

void organyaPlayStep()
{
	//Handle fading out
	if (orgFadeout && orgVolume > 0)
		orgVolume -= 2;

	//Melody playback
	for (int i = 0; i < 8; i++)
	{
		if (play_np[i] != nullptr && play_p == play_np[i]->x) //The sound came.
		{
			if (play_np[i]->y != 0xFF) {
				playOrganyaNote(play_np[i]->y, -1, i, org.tdata[i].freq);
				now_leng[i] = play_np[i]->length;
			}

			if (play_np[i]->pan != 0xFF)
				changeNotePan(play_np[i]->pan, i);
			if (play_np[i]->volume != 0xFF)
				changeNoteVolume(play_np[i]->volume, i);

			play_np[i] = play_np[i]->to; //Point to the next note
		}

		if (now_leng[i] == 0)
			playOrganyaNote(0, 2, i, org.tdata[i].freq);

		if (now_leng[i] > 0)now_leng[i]--;
	}

	//Drum playback
	for (int i = 8; i < 16; i++)
	{
		if (play_np[i] != nullptr && play_p == play_np[i]->x) //The sound came.
		{
			if (play_np[i]->y != 0xFF)
				playOrganyaDrum(play_np[i]->y, 1, i - 8);

			if (play_np[i]->pan != 0xFF)
				changeDrumPan(play_np[i]->pan, i - 8);
			if (play_np[i]->volume != 0xFF)
				changeDrumVolume(play_np[i]->volume, i - 8);

			play_np[i] = play_np[i]->to; //Point to the next note
		}
	}

	play_p++;
	if (play_p >= org.end_x) {
		play_p = org.repeat_x; //Because it is ++
		organyaSetPlayPosition(play_p);
	}
}

//Load function
void loadOrganya(const char *name)
{
	if (disableOrg)
		return;
	SDL_PauseAudioDevice(soundDev, -1);

	NOTELIST *np;

	//Init some things
	orgVolume = 100;
	orgFadeout = false;

	organyaReleaseNote();
	organyaAllocNote(10000);

	memset(old_key, 0xFF, sizeof(old_key));
	memset(key_on, 0x00, sizeof(key_on));
	memset(key_twin, 0x00, sizeof(key_twin));

	//Load
	SDL_RWops *fp = SDL_RWFromFile(name, "rb");

	if (!fp)
		doCustomError("File cannot be accessed");

	//Password check
	char passCheck[6];

	SDL_RWread(fp, &passCheck[0], sizeof(char), 6);
	
	if (!memcmp(passCheck, "Org-01", 6) || !memcmp(passCheck, "Org-02", 6))
	{
		//Reading song information
		org.wait = SDL_ReadLE16(fp);
		org.line = SDL_ReadU8(fp);
		org.dot = SDL_ReadU8(fp);
		org.repeat_x = SDL_ReadLE32(fp);
		org.end_x = SDL_ReadLE32(fp);

		for (auto& i : org.tdata) {
			i.freq = SDL_ReadLE16(fp);
			i.wave_no = SDL_ReadU8(fp);
			i.pipi = SDL_ReadU8(fp);
			i.note_num = SDL_ReadLE16(fp);

			if (!memcmp(passCheck, "Org-01", 6))
				i.pipi = 0;
		}

		//Load notes
		for (auto& i : org.tdata)
		{
			//Check if there are no notes to load
			if (i.note_num == 0)
			{
				i.note_list = nullptr;
				continue;
			}

			//Load the notes
			np = i.note_p;
			i.note_list = i.note_p;
			np->from = nullptr;
			np->to = (np + 1);
			np++;

			for (size_t j = 1; j < i.note_num; j++) {
				np->from = (np - 1);
				np->to = (np + 1);
				np++;
			}

			//The last note to is NULL
			np--;
			np->to = nullptr;

			//Substitute content
			np = i.note_p; //X coordinate
			for (size_t j = 0; j < i.note_num; j++) {
				np->x = SDL_ReadLE32(fp);
				np++;
			}

			np = i.note_p; //Y coordinate
			for (size_t j = 0; j < i.note_num; j++) {
				np->y = SDL_ReadU8(fp);
				np++;
			}

			np = i.note_p; //Length
			for (size_t j = 0; j < i.note_num; j++) {
				np->length = SDL_ReadU8(fp);
				np++;
			}

			np = i.note_p; //Volume
			for (size_t j = 0; j < i.note_num; j++) {
				np->volume = SDL_ReadU8(fp);
				np++;
			}

			np = i.note_p; //Panning
			for (int j = 0; j < i.note_num; j++) {
				np->pan = SDL_ReadU8(fp);
				np++;
			}
		}

		SDL_RWclose(fp);

		//Load waves
		SDL_RWops *rawWave100 = SDL_RWFromFile("data/Wave100.dat", "rb");

		if (!rawWave100)
			doError();

		memset(orgWaves, 0, sizeof(orgWaves));
		
		
		for (int wave = 0; wave < 8; wave++)
		{
			SDL_RWseek(rawWave100, 0x100 * org.tdata[wave].wave_no, 0);
			for (auto& sample : orgWaves[wave].wave)
				sample = SDL_ReadU8(rawWave100);
		}

		SDL_RWclose(rawWave100);

		//Load drums
		memset(orgDrums, 0, sizeof(orgDrums));

		//Make sure position is at start
		organyaSetPlayPosition(0);
	}
	else
		doCustomError("File given either isn't a .org or isn't a valid version.");

	SDL_PauseAudioDevice(soundDev, 0);
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
	loadOrganya(path.c_str());
}

void resumeOrg()
{
	Uint32 temp = 0;
	temp = currentOrg;
	currentOrg = prevOrg;
	prevOrg = temp;
	string path(orgFolder + musicList[currentOrg]);
	temp = play_p;
	loadOrganya(path.c_str());
	organyaSetPlayPosition(prevOrgPos);
	prevOrgPos = temp;
}
