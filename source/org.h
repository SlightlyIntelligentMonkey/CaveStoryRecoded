#pragma once
#include "common.h"

struct NOTE
{
	Uint32 pos;
	Uint8 note;
	Uint8 length;
	Uint8 volume;
	Uint8 pan;
};

struct TRACK
{
	Uint16 freq;
	Uint8 wave;
	Uint8 pipi;

	Uint16 amountNotes;
	NOTE *note;
	
	Uint32 samples;
};

struct ORG
{
	char header[6];

	Uint16 tempo;

	Uint8 stepPerBeat;
	Uint8 beatPerMeasure;

	Uint32 loopStart;
	Uint32 loopEnd;

	TRACK track[16];

	Uint32 pos;
	Uint32 samples;
};
