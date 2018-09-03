#pragma once
#include "common.h"

#include <string>

/*

	Yet to be implemented :

    BOA, CIL, CLR, CPS, CRE, CSS, FLA, FOB, INP, NCJ, NUM, SIL, SPS, SSS, STC and XX1

*/


struct TSC
{
	std::string path;
	int size;
	uint8_t *data;
	char mode;
	char flags;
	int p_read;
	int p_write;
	int line;
	int ypos_line[4];
	int wait;
	int wait_next;
	int next_event;
	char select;
	int face;
	int face_x;
	int item;
	int item_y;
	RECT rcText;
	int offsetY;
	uint8_t wait_beam;
};

extern TSC tsc;

//Functions
bool initTsc();

void loadStageTsc(const char *name);
void loadTsc2(const char *name);

int startTscEvent(int no);
int jumpTscEvent(int no);
void stopTsc();

int updateTsc();
void drawTsc();
