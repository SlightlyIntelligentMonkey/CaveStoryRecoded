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

int orgLerp(int16_t v1, int16_t v2, uint16_t f)
{
	uint32_t v4;

	if (!f)
		return (uint16_t)v1;

	v4 = (v2 - v1) * f;
	return ((int)((v4 >> 20) + v4) >> 12) + (uint16_t)v1;
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

	int currentStreamPos = 6;

	org->tempo = readLEshort(orgRaw, currentStreamPos);
	currentStreamPos += 2;

	org->spb = orgRaw[8];
	currentStreamPos++;

	org->bps = orgRaw[9];
	currentStreamPos++;

	org->loopStart = readLElong(orgRaw, currentStreamPos);
	currentStreamPos += 4;

	org->loopEnd = readLElong(orgRaw, currentStreamPos);
	currentStreamPos += 4;

	org->samples = 0;
	org->pos = 0;

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
		
		org->track[i].note = 0;
		org->track[i].pos = 0;
		org->track[i].playing = 0;
		org->track[i].samples = 0;
	}

	uint32_t v1 = 0;
	uint32_t v2 = 0;
	NOTE *v3;
	uint32_t *v4;
	uint32_t v5;

	for (int i = 0; i <= 15; ++i)
	{
		if (org->track[i].num_notes)
		{
			v1 = i;

			org->track[v1].note = (NOTE *)calloc(org->track[i].num_notes, 8u);

			for (int j = 0; org->track[i].num_notes > j; ++j)
			{
				org->track[i].note[j].start = readLElong(orgRaw, currentStreamPos);
				currentStreamPos += 4;
			}

			for (int j = 0; org->track[i].num_notes > j; ++j)
			{
				org->track[i].note[j].note = orgRaw[currentStreamPos];
				currentStreamPos++;
			}

			for (int j = 0; org->track[i].num_notes > j; ++j)
			{
				org->track[i].note[j].length = orgRaw[currentStreamPos];
				currentStreamPos++;
			}

			for (int j = 0; org->track[i].num_notes > j; ++j)
			{
				org->track[i].note[j].volume = orgRaw[currentStreamPos];
				currentStreamPos++;
			}

			for (int j = 0; org->track[i].num_notes > j; ++j)
			{
				org->track[i].note[j].pan = orgRaw[currentStreamPos];
				currentStreamPos++;
			}

			if (!org->track[i].note->start)
			{
				if (org->track[i].note->note != -1)
				{
					if (org->track[i].pi)
						v2 = 1;
					else
						v2 = org->track[i].note->length;
					org->track[i].playing = v2;
				}

				org->track[i].pos = 1;
				v3 = org->track[i].note;
				v4 = &org->loopEnd + 9 * i;
				v5 = *(uint32_t *)&v3->note;

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

void playORG(int id)
{
	freeORG(currentOrg);

	char path[256];
	snprintf(path, 256, "data/Org/%s.org", musicList[id]);

	currentOrg = loadORG(path);
	SDL_LockAudio();
	orgPlayer.org = currentOrg;
	orgPlayer.volume = 100;
	orgPlayer.fadeout = 0;
	SDL_UnlockAudio();
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
	char v2;
	int16_t v3;
	NOTE *v4;
	uint32_t *v5;
	uint32_t v6;
	int i;

	org->pos = position;

	for (i = 0; i <= 15; ++i)
	{
		for (org->track[i].pos = 0; ; ++org->track[i].pos)
		{
			v2 = org->track[i].pos >= org->track[i].num_notes || org->track[i].note[org->track[i].pos].start >= org->pos ? 0 : 1;

			if (!v2)
				break;
		}

		org->track[i].playing = 0;
		org->track[i].samples = 0;

		if (org->track[i].pos < org->track[i].num_notes && org->track[i].note[org->track[i].pos].start == org->pos)
		{
			if (org->track[i].note[org->track[i].pos].note != -1)
			{
				if (org->track[i].pi)
					v3 = 1;
				else
					v3 = org->track[i].note[org->track[i].pos].length;

				org->track[i].playing = v3;
			}
		}
	}
}

void renderDrumWave(int32_t *buffer, int no, TRACK *instrument, int len)
{
	return;
}

void renderOrgWave(Sint32 *buffer, TRACK *instrument, int len)
{
	long double v3;
	int s_offset_1;
	Sint32 val;
	int i;
	__int16 pan;
	float volume_f;
	float left_f;
	float right_f;
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
		/ 44100.0
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

void orgMixer(void *param, uint8_t *stream, int len)
{
	Sint16 v3;
	unsigned int samples_per_beat;
	int samples;
	int samples_left;
	signed int render_samples;
	int samples_rendered;
	int i;
	int ia;
	int j;
	int ja;
	int track_pos;
	Sint32 *buffer;
	Uint8 b_notestart;
	float f_volume;

	samples = len / 4;

	if (orgPlayer.org)
	{
		samples_per_beat = 44100 * orgPlayer.org->tempo / 1000;
		buffer = (Sint32 *)malloc(8 * samples);
		memset(buffer, 0, 8 * samples);
		samples_left = len / 4;
		samples_rendered = 0;

		while (samples_left > 0)
		{
			render_samples = samples_per_beat - orgPlayer.org->samples % samples_per_beat;

			if (render_samples > samples_left)
				render_samples = samples_left;

			for (i = 0; i <= 15; ++i)
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

				for (ia = 0; ia <= 15; ++ia)
				{
					track_pos = orgPlayer.org->track[ia].pos;
					if (orgPlayer.org->track[ia].note)
					{
						b_notestart = 0;
						if (orgPlayer.org->track[ia].num_notes > track_pos
							&& orgPlayer.org->pos == orgPlayer.org->track[ia].note[track_pos].start)
						{
							++orgPlayer.org->track[ia].pos;
							if (orgPlayer.org->track[ia].note[track_pos].note != 255)
							{
								orgPlayer.org->track[ia].samples = 0;
								if (orgPlayer.org->track[ia].pi)
									v3 = 1;
								else
									v3 = orgPlayer.org->track[ia].note[track_pos].length;
								orgPlayer.org->track[ia].playing = v3;
								orgPlayer.org->track[ia].cur_note.note = orgPlayer.org->track[ia].note[track_pos].note;
								b_notestart = 1;
							}

							if (orgPlayer.org->track[ia].note[track_pos].pan != 255)
								orgPlayer.org->track[ia].cur_note.pan = orgPlayer.org->track[ia].note[track_pos].pan;

							if (orgPlayer.org->track[ia].note[track_pos].volume != 255)
								orgPlayer.org->track[ia].cur_note.volume = orgPlayer.org->track[ia].note[track_pos].volume;
						}
						if (!b_notestart && orgPlayer.org->track[ia].playing > 0 && ia <= 7)
							--orgPlayer.org->track[ia].playing;
					}
				}
			}
		}

		for (j = 0; 2 * samples > j; ++j)
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

		for (ja = 0; ja < samples; ++ja)
		{
			*(int16_t *)&stream[4 * ja] = (int16_t)((long double)buffer[2 * ja] * f_volume);
			*(int16_t *)&stream[2 * (2 * ja + 1)] = (int16_t)((long double)buffer[2 * ja + 1] * f_volume);
		}

		free(buffer);
	}
}

void initORG()
{
	orgPlayer.org = NULL;
	
	org_waves = nullptr;
	int orgWavesSize = loadFile("data/ORGWave.dat", &org_waves);

	if (orgWavesSize < 0)
		doCustomError("ORGWave.dat not found");
}

void stopORG()
{
	SDL_LockAudio();
	orgPlayer.org = NULL;
	SDL_UnlockAudio();
}