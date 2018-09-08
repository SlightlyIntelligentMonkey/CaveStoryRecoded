#pragma once
#include "common.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

//Below are Organya song data structures
struct NOTELIST {
	NOTELIST *from; //Previous address
	NOTELIST *to; //Next address

	int32_t x; //Position
	unsigned char length; //Sound length
	unsigned char y; //Sound height
	unsigned char volume; //Volume
	unsigned char pan;
};

//Track data * 8
struct TRACKDATA {
	uint16_t freq; //Frequency (1000 is default)
	uint8_t wave_no; //Waveform No.
	uint8_t pipi;
	uint16_t note_num; //Number of notes

	NOTELIST *note_p;
	NOTELIST *note_list;
};

//Unique information held in songs
struct MUSICINFO {
	uint16_t wait;
	bool loaded;
	bool playing;
	unsigned char line; //Number of lines in one measure
	unsigned char dot; //Number of dots per line
	uint16_t alloc_note; //Number of allocated notes
	int32_t repeat_x; //Repeat
	int32_t end_x; //End of song (Return to repeat)
	int samples;
	int samplesForFrame;
	TRACKDATA tdata[16];
};

//Wave struct
struct WAVE {
	int8_t *wave;
	size_t freq;
	size_t length;
	bool playing;
	bool loops;
	unsigned int pos;
	long double volume;
	long double volume_l;
	long double volume_r;
};

//Drum struct
struct DRUM {
	uint8_t *wave; //Dynamic size
	size_t length;
	size_t freq;
	bool playing;
	size_t pos;
	long double volume;
	long double volume_l;
	long double volume_r;
};

void mixOrg(int16_t *stream, int len);

void initOrganya();
void loadOrganya(const char *name);

bool makeOrganyaWave(uint8_t track, uint8_t wave_no, bool pipi);

void playData();

extern uint32_t currentOrg;
extern bool orgFadeout;

void changeOrg(const uint32_t num);
void resumeOrg();

extern bool disableOrg;

enum musics
{
	mus_Silence = 0x0,
	mus_MischievousRobot = 0x1,
	mus_Safety = 0x2,
	mus_GameOver = 0x3,
	mus_Gravity = 0x4,
	mus_OnToGrasstown = 0x5,
	mus_Meltdown2 = 0x6,
	mus_EyesOfFlame = 0x7,
	mus_Gestation = 0x8,
	mus_MimigaTown = 0x9,
	mus_GetItem = 0xA,
	mus_BalrogsTheme = 0xB,
	mus_Cemetary = 0xC,
	mus_Plant = 0xD,
	mus_Pulse = 0xE,
	mus_Victory = 0xF,
	mus_GetLifeCapsule = 0x10,
	mus_Tyrant = 0x11,
	mus_Run = 0x12,
	mus_Jenka1 = 0x13,
	mus_LabyrinthFight = 0x14,
	mus_Access = 0x15,
	mus_Oppression = 0x16,
	mus_Geothermal = 0x17,
	mus_CaveStory = 0x18,
	mus_Moonsong = 0x19,
	mus_Herosend = 0x1A,
	mus_ScorchingBack = 0x1B,
	mus_Quiet = 0x1C,
	mus_FinalCave = 0x1D,
	mus_Balcony = 0x1E,
	mus_Charge = 0x1F,
	mus_LastBattle = 0x20,
	mus_TheWayBackHome = 0x21,
	mus_Zombie = 0x22,
	mus_BreakDown = 0x23,
	mus_RunningHell = 0x24,
	mus_Jenka2 = 0x25,
	mus_LivingWaterway = 0x26,
	mus_SealChamber = 0x27,
	mus_TorokosTheme = 0x28,
	mus_White = 0x29,
};
