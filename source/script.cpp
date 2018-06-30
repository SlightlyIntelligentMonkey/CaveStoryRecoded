#include "script.h"

int tscAlloc = 0x20000;

char *tsc = (char*)malloc(tscAlloc);
int tsc_pos = -1;
int tsc_wait = 0;

int tscMsgOpen = -1;

BYTE *tscHead = nullptr;
int tscHeadSize = loadFile((char*)"data/Head.tsc", &tscHead);

void loadTsc(char* path)
{
	tsc_pos = -1;
	tsc_wait = 0;
	tscMsgOpen = -1;

	memset((char*)tsc, 0, tscAlloc);

	//Header didn't load?
	if (tscHeadSize < 0)
		doCustomError("Head.tsc failed to load.");

	//Load the header
	int headHalf = tscHeadSize >> 1;
	int headCipher = tscHead[headHalf];

	for (int i = 0; i < tscHeadSize; i++) {
		int ret = tscHead[i];

		if (i != headHalf)
			ret -= headCipher;

		tsc[i] = ret;
	}

	//Load stage tsc
	BYTE *stageTsc = nullptr;
	int tscSize = loadFile(path, &stageTsc);

	int tscHalf = tscSize >> 1;
	int stageCipher = stageTsc[tscHalf];

	for (int i = 0; i < tscSize; i++) {
		int ret = stageTsc[i];

		if (i != tscHalf)
			ret -= stageCipher;

		tsc[i + tscHeadSize] = ret;
	}

	free(stageTsc);

	return;
}

void runScriptEvent(int event_num) {
	int position = 4;
	int rolamount = 0;
	char *pLocation;
	const int ascii_offset = 0x30;
	char event_char[6] = "#0000";

	if (event_num > 9999) { event_num = 9999; }
	while (event_num > 0)
	{
		event_char[position] = (char)((event_num % 10) + ascii_offset);
		position--;
		event_num /= 10;
	}

	pLocation = strstr(tsc, event_char);

	if (pLocation == NULL)
		doCustomError("Event wasn't found.");

	tsc_pos = (pLocation - tsc) + 7;

	//Parser of sorts?

	return;
}

void updateTsc() {
	return;
}