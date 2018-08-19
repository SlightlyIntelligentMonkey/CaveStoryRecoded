#include "org.h"

#include <cstring>
#include <SDL_RWops.h>

using std::memset;

auto song = static_cast<ORG*>(malloc(sizeof(ORG)));

void loadOrg(const char *name)
{
	memset(song, 0, sizeof(ORG));

	SDL_RWops *file = SDL_RWFromFile(name, "rb");

	//Read header
	file->read(file, &song->header, 1, 6);
	song->tempo = SDL_ReadLE16(file);
	song->beatPerMeasure = SDL_ReadU8(file);
	song->stepPerBeat = SDL_ReadU8(file);
	song->loopStart = SDL_ReadLE32(file);
	song->loopEnd = SDL_ReadLE32(file);

	//Read track data
	for (int i = 0; i < 16; ++i)
	{
		song->track[i].freq = SDL_ReadLE16(file);
		song->track[i].wave = SDL_ReadU8(file);
		song->track[i].pipi = SDL_ReadU8(file);
		song->track[i].amountNotes = SDL_ReadLE16(file);
	}

	//Read notes

}
