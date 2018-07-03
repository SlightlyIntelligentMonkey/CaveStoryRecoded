#include "org.h"

ORGPLAYER orgPlayer;
BYTE *org_waves;

ORG *currentOrg;

int orgAlloc = 0x1000u;

int16_t octfreq[12] = { 1, 2, 4, 8, 16, 32, 64, 128, 0, 0, 0, 0 };
int16_t notefreq[12] = { 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494 };

//Music list
const char* musicList[42] = {
	"xxxx", "wanpaku", "anzen", "gameover", "gravity", "weed", "mdown2", "fireeye", "vivi", "mura", "fanfale1", "ginsuke", "cemetery", "plant", "kodou",
	"fanfale2", "fanfale3", "dr", "escape", "jenka", "maze", "access", "ironh", "grand", "curly", "oside", "requiem", "wanpak2", "quiet", "lastcave",
	"balcony", "lastbtl", "lastbt3", "ending", "zonbie", "bdown", "hell", "jenka2", "marine", "ballos", "toroko", "white",
};

int orgLerp(int16_t v1, int16_t v2, int16_t f) //only supports unsigned integers I guess
{
	uint64_t fv;

	if (!f)
		return (int)v1;

	fv = (v2 - v1) * f;
	return ((int)((fv >> 20) + fv) >> 12) + (uint16_t)v1;
}

ORG *loadORG(const char *path)
{
	ORG *org;

	BYTE *orgRaw = nullptr;
	int orgSize = loadFile(path,&orgRaw);

	if (orgSize < 0)
		doCustomError("Couldn't load .org");

	org = (ORG *)malloc(orgAlloc);
	memset(org, 0, orgAlloc);

	int currentStreamPos = 6; //yes, I faked streaming, I guess this can be changed

	//Tempo
	org->tempo = readLEshort(orgRaw, currentStreamPos);
	currentStreamPos += 2;

	//Time signature
	org->spb = orgRaw[8];
	currentStreamPos++;

	org->bps = orgRaw[9];
	currentStreamPos++;

	//Loop
	org->loopStart = readLElong(orgRaw, currentStreamPos);
	currentStreamPos += 4;

	org->loopEnd = readLElong(orgRaw, currentStreamPos);
	currentStreamPos += 4;

	//Set org to play at beginning with nothing doing anything
	org->samples = 0;
	org->pos = 0;

	//Get track instrument data
	for (int i = 0; i <= 15; ++i)
	{
		org->track[i].freq = readLEshort(orgRaw, currentStreamPos);
		currentStreamPos += 2;

		org->track[i].wave = orgRaw[currentStreamPos];
		currentStreamPos++;

		org->track[i].pi = orgRaw[currentStreamPos];
		currentStreamPos++;

		org->track[i].num_notes = readLEshort(orgRaw, currentStreamPos);
		currentStreamPos += 2;
		
		//Track doesn't play anything
		org->track[i].note = 0;
		org->track[i].pos = 0;
		org->track[i].playing = 0;
		org->track[i].samples = 0;
	}

	for (int i = 0; i <= 15; ++i) //Load events
	{
		if (org->track[i].num_notes)
		{
			org->track[i].note = (NOTE *)calloc(org->track[i].num_notes, 8u);

			for (int j = 0; org->track[i].num_notes > j; ++j) //When event is played
			{
				org->track[i].note[j].start = readLElong(orgRaw, currentStreamPos);
				currentStreamPos += 4;
			}

			for (int j = 0; org->track[i].num_notes > j; ++j) //Tone (if a note)
			{
				org->track[i].note[j].note = orgRaw[currentStreamPos];
				currentStreamPos++;
			}

			for (int j = 0; org->track[i].num_notes > j; ++j) //Length
			{
				org->track[i].note[j].length = orgRaw[currentStreamPos];
				currentStreamPos++;
			}

			for (int j = 0; org->track[i].num_notes > j; ++j) //Volume
			{
				org->track[i].note[j].volume = orgRaw[currentStreamPos];
				currentStreamPos++;
			}

			for (int j = 0; org->track[i].num_notes > j; ++j) //Panning
			{
				org->track[i].note[j].pan = orgRaw[currentStreamPos];
				currentStreamPos++;
			}

			if (!org->track[i].note->start) //Get currently playing note at start of org
			{
				if (org->track[i].note->note != 255)
				{
					int playLength;

					if (org->track[i].pi)
						playLength = 1;
					else
						playLength = org->track[i].note->length;

					org->track[i].playing = playLength;
				}

				org->track[i].pos = 1;

				NOTE *v3 = org->track[i].note;
				uint32_t *v4 = &org->loopEnd + 9 * i;
				uint32_t v5 = *(uint32_t *)&v3->note;
				v4[3] = v3->start;
				v4[4] = v5;
			}
		}
	}

	org->samples = 0;
	org->pos = 0;

	free(orgRaw);

	return org;
}

void freeORG(ORG *org)
{
	int i;

	if (org)
	{
		for (i = 0; i <= 15; ++i)
		{
			if (org->track[i].note)
				free(org->track[i].note);
		}
		free(org);
	}
}

uint32_t getORGPosition(ORG *org)
{
	uint32_t result;

	if (org)
		result = org->pos;
	else
		result = 0;

	return result;
}

void setORGPosition(ORG *org, uint32_t position)
{
	org->pos = position;

	for (int track = 0; track <= 15; ++track)
	{
		for (org->track[track].pos = 0; ; ++org->track[track].pos)
		{
			bool posCheck = org->track[track].pos >= org->track[track].num_notes || org->track[track].note[org->track[track].pos].start >= org->pos ? 0 : 1;

			if (!posCheck)
				break;
		}

		//Reset current playing and samples
		org->track[track].playing = 0;
		org->track[track].samples = 0;

		if (org->track[track].pos < org->track[track].num_notes && org->track[track].note[org->track[track].pos].start == org->pos)
		{
			if (org->track[track].note[org->track[track].pos].note != 255) //Determine currently playing note at position
			{
				int16_t noteLength;

				if (org->track[track].pi)
					noteLength = 1;
				else
					noteLength = org->track[track].note[org->track[track].pos].length;

				org->track[track].playing = noteLength;
			}
		}
	}
}

void playORG(int id)
{
	freeORG(currentOrg); //Remove old song from memory

	//Load new song
	char path[256];
	snprintf(path, 256, "data/Org/%s.org", musicList[id]);

	currentOrg = loadORG(path);

	SDL_LockAudio(); //Lock audio while changing song

	orgPlayer.org = currentOrg;
	orgPlayer.volume = 100;
	orgPlayer.fadeout = 0;

	SDL_UnlockAudio(); //Okay, we're done here.
}

void renderDrumWave(int32_t *buffer, int no, TRACK *instrument, int len)
{
	//DISABLED UNTIL PXT
	return;
}

void renderOrgWave(Sint32 *buffer, TRACK *instrument, int len) //This REALLY needs to get cleaned up.
{
	long double v3;
	int s_offset_1;
	Sint32 val;
	int i;
	__int16 pan;
	long double volume_f;
	long double left_f;
	long double right_f;
	Sint8 *org_wave;

	volume_f = (long double)instrument->cur_note.volume / 254.0;
	pan = instrument->cur_note.pan;
	left_f = volume_f;
	right_f = volume_f;
	if (pan > 5)
	{
		if (pan > 6)
			left_f = volume_f * ((long double)(12 - instrument->cur_note.pan) / 6.0);
	}
	else
	{
		right_f = volume_f * ((long double)(6 - instrument->cur_note.pan) / 6.0);
	}
	org_wave = (Sint8 *)&org_waves[256 * instrument->wave];
	v3 = ((long double)(octfreq[instrument->cur_note.note / 0xCu] * (signed int)notefreq[instrument->cur_note.note % 0xCu])
		* 32.0
		+ (long double)instrument->freq
		- 1000.0)
		/ audioFrequency
		* 4096.0;
	for (i = 0; i < len; ++i)
	{
		instrument->samples = ((unsigned __int64)(signed __int64)v3 + instrument->samples) & 0xFFFFF;
		s_offset_1 = instrument->samples >> 12;
		val = (signed __int16)orgLerp(
			org_wave[s_offset_1] << 7,
			org_wave[(unsigned __int8)(((unsigned int)((s_offset_1 + 1) >> 31) >> 24) + s_offset_1 + 1)
			- ((unsigned int)((s_offset_1 + 1) >> 31) >> 24)] << 7,
			instrument->samples & 0xFFF);
		buffer[2 * i] += (signed int)((long double)val * left_f);
		buffer[2 * i + 1] += (signed int)((long double)val * right_f);
	}
}

void orgMixer(void *param, uint8_t *stream, int len) //This too, but not as bad as above
{
	Sint16 noteLength;
	unsigned int samples_per_beat;
	int samples;
	int samples_left;
	signed int render_samples;
	int samples_rendered;
	int trackPosition;
	Sint32 *buffer;
	Uint8 noteStart;
	long double f_volume;

	samples = len / 4;

	if (orgPlayer.org)
	{
		samples_per_beat = audioFrequency * orgPlayer.org->tempo / 1000;
		buffer = (Sint32 *)malloc(8 * samples);
		memset(buffer, 0, 8 * samples);
		samples_left = len / 4;
		samples_rendered = 0;

		while (samples_left > 0)
		{
			render_samples = samples_per_beat - orgPlayer.org->samples % samples_per_beat;

			if (render_samples > samples_left)
				render_samples = samples_left;

			for (int i = 0; i <= 15; ++i)
			{
				if (orgPlayer.org->track[i].note && orgPlayer.org->track[i].playing > 0)
				{
					if (i > 7)
						renderDrumWave(&buffer[2 * samples_rendered], i - 8, &orgPlayer.org->track[i], render_samples);
					else
						renderOrgWave(&buffer[2 * samples_rendered], &orgPlayer.org->track[i], render_samples);
				}
			}

			samples_left -= render_samples;
			samples_rendered += render_samples;
			orgPlayer.org->samples += render_samples;

			if (!(orgPlayer.org->samples % samples_per_beat))
			{
				if (orgPlayer.fadeout)
					orgPlayer.volume -= 2;
				if (orgPlayer.volume < 0)
					orgPlayer.volume = 0;

				if (++orgPlayer.org->pos >= orgPlayer.org->loopEnd)
					setORGPosition(orgPlayer.org, orgPlayer.org->loopStart);

				for (int ia = 0; ia <= 15; ++ia)
				{
					trackPosition = orgPlayer.org->track[ia].pos;

					if (orgPlayer.org->track[ia].note)
					{
						noteStart = 0;

						if (orgPlayer.org->track[ia].num_notes > trackPosition
							&& orgPlayer.org->pos == orgPlayer.org->track[ia].note[trackPosition].start)
						{
							//Move track along
							++orgPlayer.org->track[ia].pos;

							if (orgPlayer.org->track[ia].note[trackPosition].note != 255)
							{
								orgPlayer.org->track[ia].samples = 0;

								//Get note's playing length
								if (orgPlayer.org->track[ia].pi)
									noteLength = 1; //I'm not so sure about this?
								else
									noteLength = orgPlayer.org->track[ia].note[trackPosition].length;
								
								orgPlayer.org->track[ia].playing = noteLength;

								//Set note to this note
								orgPlayer.org->track[ia].cur_note.note = orgPlayer.org->track[ia].note[trackPosition].note;

								noteStart = 1;
							}

							//Pan event
							if (orgPlayer.org->track[ia].note[trackPosition].pan != 255)
								orgPlayer.org->track[ia].cur_note.pan = orgPlayer.org->track[ia].note[trackPosition].pan;

							//Volume event
							if (orgPlayer.org->track[ia].note[trackPosition].volume != 255)
								orgPlayer.org->track[ia].cur_note.volume = orgPlayer.org->track[ia].note[trackPosition].volume;
						}

						//End note (and go through a lengthened one)
						if (!noteStart && orgPlayer.org->track[ia].playing > 0 && ia <= 7)
							--orgPlayer.org->track[ia].playing;
					}
				}
			}
		}

		for (int j = 0; 2 * samples > j; ++j)
		{
			if (buffer[j] <= 0x7FFF)
			{
				if (!(buffer[j] >= -0x7FFF))
				{
					//fprintf(stderr, "%d! ", 0x8000 - buffer[j]);
					buffer[j] = -32768;
				}
			}
			else
			{
				//fprintf(stderr, "%d! ", buffer[j] - 0x7FFF);
				buffer[j] = 0x7FFF;
			}
		}

		f_volume = (long double)orgPlayer.volume / 192.0;

		for (int ja = 0; ja < samples; ++ja)
		{
			*(int16_t *)&stream[4 * ja] = (int16_t)((long double)buffer[2 * ja] * f_volume);
			*(int16_t *)&stream[2 * (2 * ja + 1)] = (int16_t)((long double)buffer[2 * ja + 1] * f_volume);
		}

		free(buffer);
	}
}

void initORG()
{
	//Clear org
	if (orgPlayer.org)
		freeORG(orgPlayer.org);

	orgPlayer.org = NULL;
	
	//Load wave100
	int orgWavesSize = loadFile("data/Wave100.dat", &org_waves);

	if (orgWavesSize < 0)
		doCustomError("Wave100.dat not found");
}

void stopORG()
{
	SDL_LockAudio();
	orgPlayer.org = NULL;
	SDL_UnlockAudio();
}