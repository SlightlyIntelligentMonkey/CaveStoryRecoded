#pragma once
#include "common.h"


//list of notes
typedef struct NOTELIST {
	NOTELIST *from;		//points to previous note
	NOTELIST *to;		//points to next note
	Uint32 start;
	Uint8 note;
	Uint8 length;
	Uint8 volume;
	Uint8 pan;
}NOTELIST;

//track stuff
typedef struct {
	Uint16 freq;			//1000 is default
	Uint16 num_notes;		//total number of notes in track
	Uint8 wave;				//instrument
	Uint8 pi;				//pizzacatto

	Uint8 length;			//length of note in steps

	Uint8 key;
	Uint8 keyOn;
	Uint8 keyTwin;

	int *noteBuf;
	Uint32 noteBufLen;
	//used for drums
	Uint32 bufPos;

	//calculated volume/pan amounts
	double volume;
	double lpan;
	double rpan;

	NOTELIST *note_p;		//pointer to current note
	NOTELIST *note_list;	//pointer to start of notes
}TRACKDATA;


const Uint32 MAXMELODY = 8;
const Uint32 MAXTRACK = 16;
class ORG
{
private:
	char header[6];			//describes which org version
	Uint16 wait;			//length of step in milliseconds
	Uint8 stepPerBar;
	Uint8 beatsPerStep;

	Uint32 loopStart;
	Uint32 loopEnd;

	TRACKDATA track[MAXTRACK];
	
public:
	int *stepBuf;
	Uint32 stepBufPos;
	Uint32 stepBufLen;
	Uint32 samplesPerStep;
	Uint32 pos;

	void load(const char *path);
	void playData();
	void freemem();

	//sets the play position
	void setPos(long x)
	{
		for (Uint32 i = 0; i < MAXTRACK; i++)
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
	Uint32 len;
};

void iniOrg();
void exitOrg();
void playWave(int wave, int *stream, Uint32 len, double frequency);

#define MAXWAVES 100
#define MAXDRUMS 9

extern ORG *org;
extern DRUM drumTbl[MAXDRUMS];