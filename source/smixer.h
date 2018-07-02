#pragma once
#include "common.h"

//Structs
struct SOUND
{
	uint8_t *buf;
	uint32_t len;
	uint16_t vol;
	uint16_t lvol;
	uint16_t rvol;
	uint32_t freq;
};

struct MIXCHAN
{
	uint32_t pos;
	uint16_t pos_comma;
	int32_t repeat;
	SOUND *data;
};

//Functions
void smixerInit();

void smixerFreeChunks();

void smixerInitChunk(SOUND *c);

void smixerStart();
void smixerQuit();

extern SOUND *chunks[160]; // idb
//extern SOUND *orgs[8][8][2];