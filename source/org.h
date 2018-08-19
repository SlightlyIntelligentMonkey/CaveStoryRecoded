#pragma once
#include "common.h"

struct NOTE
{
	uint32_t pos;
	uint8_t note;
	uint8_t length;
	uint8_t volume;
	uint8_t pan;
};

struct TRACK
{
	uint16_t freq;
	uint8_t wave;
	uint8_t pipi;

	uint16_t amountNotes;
	NOTE *note;
	
	uint32_t samples;
};

struct ORG
{
	char header[6];

	uint16_t tempo;

	uint8_t stepPerBeat;
	uint8_t beatPerMeasure;

	uint32_t loopStart;
	uint32_t loopEnd;

	TRACK track[16];

	uint32_t pos;
	uint32_t samples;
};
