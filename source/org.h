#pragma once
#include "common.h"

//list of notes
typedef struct NOTELIST
{
	NOTELIST *from;		//points to previous note
	NOTELIST *to;		//points to next note
	uint32_t start;
	uint8_t note;
	uint8_t length;
	uint8_t volume;
	uint8_t pan;
}NOTELIST;

//track stuff
typedef struct
{
	uint16_t freq;			//1000 is default
	uint16_t num_notes;		//total number of notes in track
	uint8_t wave;				//instrument
	uint8_t pi;				//pizzacatto
	uint8_t length;			//length of note in steps
	uint8_t key;
	uint8_t keyOn;
	uint8_t keyTwin;
	int *noteBuf;
	uint32_t noteBufLen;
	//used for drums
	uint32_t bufPos;
	//calculated volume/pan amounts
	double volume;
	double lpan;
	double rpan;
	NOTELIST *note_p;		//pointer to current note
	NOTELIST *note_list;	//pointer to start of notes
}TRACKDATA;

constexpr uint32_t MAXMELODY = 8;
constexpr uint32_t MAXTRACK = 16;

class ORG
{
private:
	char header[6];			//describes which org version
	uint16_t wait;			//length of step in milliseconds
	uint8_t stepPerBar;
	uint8_t beatsPerStep;
	uint32_t loopStart;
	uint32_t loopEnd;
	TRACKDATA track[MAXTRACK];

public:
	int *stepBuf;
	uint32_t stepBufPos;
	uint32_t stepBufLen;
	uint32_t samplesPerStep;
	uint32_t pos;
	void load(const char *path);
	void playData();
	void freemem();
	//sets the play position
	void setPos(uint32_t x)
	{
		for (uint32_t i = 0; i < MAXTRACK; i++)
		{
			track[i].note_p = track[i].note_list;
			while (track[i].note_p != nullptr && track[i].note_p->start < x)
				track[i].note_p = track[i].note_p->to;
		}
		pos = x;
	}
};

struct DRUM
{
	int *buf;
	uint32_t len;
};

void iniOrg();
void exitOrg();
void playWave(int wave, int *stream, uint32_t len, double frequency);

void changeOrg(const int num);
void resumeOrg();

extern uint32_t currentOrg;

constexpr uint32_t MAXWAVES = 100;
constexpr uint32_t MAXDRUMS = 9;

extern ORG *org;
extern DRUM drumTbl[MAXDRUMS];

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
