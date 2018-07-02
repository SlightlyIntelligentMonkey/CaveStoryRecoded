#include "smixer.h"

SDL_AudioSpec soundDevice;

int deviceFrequency;
uint16_t deviceFormat;
int deviceChannels;

MIXCHAN mixChannels[255];

uint32_t mixNextFree;

SOUND *chunks[160];
//SOUND *orgs[8][8][2];

//Some stuff
int interpolate(MIXCHAN *chan, bool left)
{
	int v2;

	if (left)
		v2 = chan->data->lvol;
	else
		v2 = chan->data->rvol;

	return (short)((short)((short)(((short)(1024 - chan->pos_comma)
		* (short)((chan->data->buf[chan->pos] - 128) << 7)
		+ (short)((chan->data->buf[(chan->pos + 2)
			% chan->data->len]
			- 128) << 7)
		* chan->pos_comma) >> 10)
		* chan->data->vol >> 10)
		* v2 >> 8);
}

//Advance
void advance(MIXCHAN *chan)
{
	Uint16 freq;

	freq = chan->data->freq;

	chan->pos += (BYTE)((int32_t)freq >> 10);
	chan->pos_comma += freq & 0x3FF;

	chan->pos += (int32_t)chan->pos_comma >> 10;
	chan->pos_comma &= 0x3FFu;
}

//Check repeat
int32_t checkRepeat(MIXCHAN *chan)
{
	if (chan->pos >= chan->data->len)
	{
		if (chan->repeat <= 0)
		{
			if (chan->repeat >= 0)
			{
				chan->pos = 0;
				return 1;
			}
			chan->pos = 0;
		}
		else
		{
			--chan->repeat;
			chan->pos = 0;
		}
	}
	return 0;
}

//Mix callback
void smixerCallback(void *userdata, Uint8 *stream, int len)
{
	int32_t accum;
	int8_t left;
	int i;
	uint32_t c;
	MIXCHAN *cur_chan;

	left = 1;
	for (i = 0; len / 2 > i; ++i)
	{
		accum = 0;
		for (c = 0; c < mixNextFree; ++c)
		{
			cur_chan = (MIXCHAN *)(16 * c + 136110560);
			if (mixChannels[c].data)
			{
				accum += (short)interpolate(cur_chan, left);
				advance(cur_chan);
				if ((BYTE)checkRepeat(cur_chan))
				{
					--mixNextFree;

					mixChannels[c].pos = mixChannels[mixNextFree].pos;
					mixChannels[c].pos_comma = mixChannels[mixNextFree].pos_comma;
					mixChannels[c].repeat = mixChannels[mixNextFree].repeat;
					mixChannels[c].data = mixChannels[mixNextFree].data;
				}
			}
		}

		if (accum <= 0x7FFF)
		{
			if (accum < -0x8000)
				accum = 0x8000;
		}
		else
		{
			accum = 0x7FFF;
		}

		*(uint16_t *)&stream[2 * i] += accum;
		left ^= 1u;
	}
	orgMixer(userdata, stream, len);
}

//Init, Start, and Quit
void smixerInit()
{
	SDL_zero(soundDevice);

	soundDevice.freq = 44100;
	soundDevice.samples = 1024;
	soundDevice.format = AUDIO_S16;
	soundDevice.channels = 2;
	soundDevice.callback = smixerCallback;

	if (SDL_OpenAudio(&soundDevice, NULL) != 0)
		doCustomError("Couldn't open audio device");
}

void smixerStart()
{
	SDL_PauseAudio(0);
}

void smixerQuit()
{
	SDL_CloseAudio();
}

//Play chunks
void smixerInitChunk(SOUND *c)
{
	c->vol = 1024;
	c->lvol = 256;
	c->rvol = 256;
	c->freq = 256;
}

void smixerPlayChunk(SOUND *c, Sint32 repeat)
{
	uint32_t v2;

	uint32_t i;

	SDL_LockAudio();

	for (i = 0; i < mixNextFree; ++i)
	{
		if (mixChannels[i].data == c)
		{
			mixChannels[i].repeat = repeat;
			mixChannels[i].pos = 0;
			break;
		}
	}

	if (i == mixNextFree)
	{
		if (mixNextFree > 0xFE)
			return;
		
		v2 = mixNextFree;

		mixChannels[v2].pos = 0;
		mixChannels[v2].pos_comma = 0;
		mixChannels[v2].repeat = repeat;
		mixChannels[v2].data = c;

		mixNextFree = v2 + 1;
	}

	SDL_UnlockAudio();
}

void smixerStopChunk(SOUND *c)
{
	uint32_t v1;
	//uint32_t *v2;
	uint32_t i;

	SDL_LockAudio();
	for (i = 0; i < mixNextFree; ++i)
	{
		if (mixChannels[i].data == c)
		{
			--mixNextFree;

			v1 = 16 * i;
			//v2 = (DWORD *)(16 * mix_nextfree);

			mixChannels[i].pos = mixChannels[mixNextFree].pos;
			mixChannels[i].pos_comma = mixChannels[mixNextFree].pos_comma;
			mixChannels[i].repeat = mixChannels[mixNextFree].repeat;
			mixChannels[i].data = mixChannels[mixNextFree].data;
		}
	}
	SDL_UnlockAudio();
}

void smixerReset(SOUND *c)
{
	unsigned int i;

	SDL_LockAudio();
	for (i = 0; i < mixNextFree; ++i)
	{
		if (mixChannels[i].data == c)
		{
			mixChannels[i].pos = 0;
			mixChannels[i].pos_comma = 0;
			break;
		}
	}
	SDL_UnlockAudio();
}

void smixerSetVolume(SOUND *c, Uint16 volume)
{
	SDL_LockAudio();
	c->vol = (volume << 10) / 300;
	SDL_UnlockAudio();
}

void smixerSetPan(SOUND *c, Uint16 pan)
{
	Sint16 lpan;
	Sint16 rpan;

	SDL_LockAudio();

	lpan = 256 - pan;
	rpan = pan - 256;

	c->lvol = 256;
	c->rvol = 256;

	if ((short)(256 - pan) > 0 && lpan <= 255)
		c->lvol = lpan;
	if (rpan > 0 && rpan <= 255)
		c->rvol = rpan;

	SDL_UnlockAudio();
}



void smixerSetFrequency(SOUND *c, Uint32 freq)
{
	SDL_LockAudio();
	c->freq = (freq << 10) / 0x15888;
	SDL_UnlockAudio();
}

//Free chunks
void smixerFreeChunks()
{
	size_t i;

	mixNextFree = 0;

	SDL_LockAudio();

	for (i = 0; i <= 0x9F; ++i)
	{
		if (chunks[i])
		{
			free(chunks[i]->buf);
			free(chunks[i]);

			chunks[i] = 0;
		}
	}

	SDL_UnlockAudio();
}