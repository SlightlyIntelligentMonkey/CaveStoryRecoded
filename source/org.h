#pragma once
#include "common.h"

/**

Org implementation requirements (names aren't strict) :

/// Loads a music from a given path
void loadOrg(const char *path);

/// Returns the current song's position
int32_t getOrgPosition();

/// The returned position from getOrgPosition is usable here. Position 0 is the start of the song
void setOrgPosition(int32_t position);

/// Playback starts when this function is called
void startOrgPlayback();

/// Playback ends when this function is called
void stopOrgPlayback();

/// Is set to false on loadOrg and setOrgPosition. Volume starts decreasing when it is set to true
bool musicFadeFlag;

*/

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

constexpr const char *songNames[42]
{
	"xxxx",
	"wanpaku",
	"anzen",
	"gameover",
	"gravity",
	"weed",
	"mdown2",
	"fireeye",
	"vivi",
	"mura",
	"fanfale1",
	"ginsuke",
	"cemetary",
	"plant",
	"kodou",
	"fanfale3",
	"fanfale2",
	"dr",
	"escape",
	"jenka",
	"maze",
	"access",
	"ironh",
	"grand",
	"curly",
	"oside",
	"requiem",
	"wanpak2",
	"quiet",
	"lastcave",
	"balcony",
	"lastbtl",
	"lastbtl3",
	"ending",
	"zonbie",
	"bdown",
	"hell",
	"jenka2",
	"marine",
	"ballos",
	"toroko",
	"white",
};
