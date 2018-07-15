#include "script.h"

// -- TSC Code --
// This is a string, not a fixed buffer.
static char *tsc = 0;

// -- TSC State --
int tscMode = 0;
enum modes
{
	parsingCheck = 0,
	defaultMode = 1,
	NOD = 2,
	dunno = 3,
	WAI = 4,
	FADE = 5,
	YNJ = 6,
	someCheck = 7
};

int tscPos = -1;
int tscWait = 0;
int tscCounter = 0;

int tscMsgOpen = -1;

// -- command variables --
unsigned int waitAmount = 0;
int yesnoSelect = 0;

// Decrypt into tsc buffer
void tscDecrypt(int start, BYTE * code, int size) {
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
	tscPos = -1;
	tscWait = 0;
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
	char *location;
	char eventChar[6] = "#0000";

	if (event_num > 9999) { event_num = 9999; }

	while (event_num > 0)
	{
		eventChar[position] = (char)((event_num % 10) + 0x30);
		position--;
		event_num /= 10;
	}

	location = strstr(tsc, eventChar);

	if (location == NULL)
		doCustomError("Event wasn't found.");

	tscMode = 1;
	tscPos = (location - tsc) + 7;
}

//Read numbers of a string
int getValue(char *str, int digits)
{
    int value = 0;

    for (int i = 0; i < digits; i++)
        value += (str[i] - 0x30) * std::pow(10, (digits - i) - 1);

    return value;
}

int updateTsc() {
	//I need working code here
	return 0;
}
