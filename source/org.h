#pragma once
#include "common.h"

struct NOTE
{
	uint32_t start;
	uint8_t note;
	uint8_t length;
	uint8_t volume;
	uint8_t pan;
};

struct TRACK
{
	uint16_t freq;
	uint8_t wave;
	uint8_t pi;
	uint16_t num_notes;
	NOTE cur_note;
	NOTE *note;
	uint32_t pos;
	int16_t playing;
	uint32_t samples;
	uint16_t pos_comma;
};

struct NOTELIST
{
	NOTELIST *from;
	NOTELIST *to;
	int32_t x;
	uint8_t length;
	uint8_t y;
	uint8_t volume;
	uint8_t pan;
};

struct OCTWAVE
{
	int16_t wave_size;
	int16_t oct_par;
	int16_t oct_size;
};

struct ORG
{
	char header[6];
	Uint16 tempo;
	Uint8 spb;
	Uint8 bps;
	Uint32 loopStart;
	Uint32 loopEnd;
	TRACK track[16];
	Uint32 pos;
	Uint32 samples;
};

struct ORGPLAYER
{
	ORG *org;
	int16_t volume;
	int8_t fadeout;
};

ORG *loadORG(const char *path);
void playORG(int id);

void freeORG(ORG *org);

extern ORGPLAYER orgPlayer;

void orgMixer(void *param, uint8_t *stream, int len);

void initORG();
void stopORG();