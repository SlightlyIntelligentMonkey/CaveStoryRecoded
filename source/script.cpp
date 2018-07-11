#include "script.h"

// -- TSC Code --
// This is a string, not a fixed buffer.
static char *tsc = 0;

// -- TSC State --
static int tsc_pos = -1;
static int tsc_wait = 0;

static int tscMsgOpen = -1;

// Decrypt into tsc buffer
static void tscDecrypt(int start, BYTE * code, int size) {
	int headHalf = size >> 1;
	int headCipher = code[headHalf];

	for (int i = 0; i < size; i++) {
		int ret = code[i];

		if (i != headHalf)
			ret -= headCipher;

		tsc[start + i] = (char) ret;
	}
}

void loadTsc(const char * path) {
	tsc_pos = -1;
	tsc_wait = 0;
	tscMsgOpen = -1;

	// Load head tsc
	BYTE *headTsc = nullptr;
	int tscHeadSize = loadFile("data/Head.tsc", &headTsc);
	if (tscHeadSize < 0) {
		doCustomError("TSC: Failed to load data/Head.tsc");
		return;
	}

	// Load stage tsc
	BYTE *stageTsc = nullptr;
	int tscSize = loadFile(path, &stageTsc);
	if (tscSize < 0) {
		free(headTsc);
		char msg[256];
		snprintf(msg, 256, "TSC: %s failed to load.", path);
		doCustomError(msg);
		return;
	}

	if (tsc)
		free(tsc);

	tsc = (char *) malloc(tscSize + tscHeadSize + 1);
	tscDecrypt(0, headTsc, tscHeadSize);
	tscDecrypt(tscHeadSize, stageTsc, tscSize);
	tsc[tscSize + tscHeadSize] = 0;

	free(headTsc);
	free(stageTsc);
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
}

void updateTsc() {
	
}
