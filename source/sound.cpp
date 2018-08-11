#include "sound.h"
#include <SDL.h>

const char *sfxList[] =
{
	NULL,
	"01 - Menu select.wav",
	"02 - Message typing.wav",
	"03 - Bonk head.wav",
	"04 - Switch weapon.wav",
	"05 - Yes no appear.wav",
	"06 - Tiny jump.wav",
	NULL,
	"08 - Charging.wav",
	NULL,
	NULL,
	"0B - Open door.wav",
	"0C - Block destroy.wav",
	NULL,
	"0E - Get experience.wav",
	"0F - Quote jump.wav",
	"10 - Hurt.wav",
	"11 - Death.wav",
	"12 - Menu confirm.wav",
	NULL,
	"14 - Health refill.wav",
	"15 - Bubble.wav",
	"16 - Chest open.wav",
	"17 - Thud.wav",
	"18 - Footstep.wav",
	"19 - Funny explode.wav",
	"1A - Quake.wav",
	"1B - Level up.wav",
	"1C - Shot hit.wav",
	"1D - Teleport.wav",
	"1E - Critter jump.wav",
	"1F - Shot bounce.wav",
	"20 - Polar Star shot 1.wav",
	"21 - Fireball shoot.wav",
	"22 - Fireball bounce.wav",
	"23 - Explosion 1.wav",
	NULL,
	"25 - Empty.wav",
	"26 - Get item.wav",
	"27 - Enemy shoot.wav",
	"28 - Stream.wav",
	NULL,
	"2A - Missile collect.wav",
	"2B - Computer beep.wav",
	"2C - Missile hit.wav",
	"2D - Experience bounce.wav",
	"2E - Ironhead sound.wav",
	"2F - Explosion 2.wav",
	"30 - Bubbline shoot 1.wav",
	"31 - Polar Star shot 3.wav",
	"32 - Mimiga squeak.wav",
	"33 - Enemy hurt.wav",
	"34 - Enemy hurt big.wav",
	"35 - Enemy hurt small.wav",
	"36 - Enemy hurt cool.wav",
	"37 - Enemy squeak.wav",
	"38 - Splash.wav",
	"39 - Enemy damage.wav",
	"3A - Click.wav",
	NULL,
	"3C - Spur charge 1.wav",
	"3D - Spur charge 2.wav",
	"3E - Spur fire 2.wav",
	"3F - Spur fire 3.wav",
	"40 - Spur fire max.wav",
	"41 - Spur charged.wav",
	NULL,
	NULL,
	NULL,
	NULL,
	"46 - Small explosion.wav",
	"47 - Small crash.wav",
	"48 - Big crash.wav",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"64 - Unknown.wav",
	"65 - Lightning strike.wav",
	"66 - Jaws.wav",
	"67 - Curly charge gun.wav",
	"68 - Hidden shot.wav",
	"69 - Puppy bark.wav",
	"6A - Unknown.wav",
	"6B - Block move.wav",
	"6C - Big jump.wav",
	"6D - Critter fly.wav",
	"6E - Unknown.wav",
	"6F - Unknown.wav",
	"70 - Unknown.wav",
	"71 - Boosting.wav",
	"72 - Core hurt.wav",
	"73 - Core thrust.wav",
	"74 - Unknown.wav",
	"75 - Unknown.wav",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"96 - Org drum.wav",
	"97 - Org drum.wav",
	"98 - Org drum.wav",
	"99 - Org drum.wav",
	"9A - Org drum.wav",
	"9B - Org drum.wav",
};

struct SOUND_EFFECT
{
	int *buf;
	Uint32 length;
	Uint32 pos;
};
SOUND_EFFECT sounds[_countof(sfxList)];

SDL_AudioStream *audioStream;
SDL_AudioDeviceID soundDev = 0;
SDL_AudioSpec soundSpec;
SDL_AudioSpec want;

//use int pointers for easy use
void mixAudioSFX(int *dst, Uint32 len, SOUND_EFFECT *sound, Uint8 lVolume, Uint8 rVolume)
{
	unsigned int currentPos = 0;
	int temp = 0;
	//using 32 float point on native system 
	while (sound->pos + currentPos < sound->length && (currentPos << 2) < len)
	{
		dst[currentPos] += sound->buf[sound->pos + currentPos];
		currentPos++;
	}
	sound->pos += currentPos;
	return;
}

void audio_callback(void *userdata, Uint8 *stream, int len)
{
	memset(stream, 0, len);

	for (int sfx = 0; sfx < _countof(sounds); sfx++)
	{
		if (sounds[sfx].pos < sounds[sfx].length)
		{
			mixAudioSFX((int*)stream, len, &sounds[sfx], 100, 100);
		}
	}

	return;
}

void ini_audio()
{
	want.channels = 2;
	want.freq = 44100;
	want.format = AUDIO_S32SYS;
	want.samples = 1024;
	want.callback = audio_callback;
	want.userdata = 0;

	soundDev = SDL_OpenAudioDevice(
		NULL,
		0,
		&want,
		&soundSpec,
		NULL);

	if (soundDev == 0) { doError(); }
	memset(sounds, 0, sizeof(sounds));

	return;
}

//since sdl doesn't actually get the loaded wav in the specified format,
//we have to do it ourselves
void loadSound(char *path, SDL_AudioSpec *spec, int **buf, Uint32 *length)
{
	int view = 0;
	BYTE *pBuf = nullptr;
	SDL_AudioSpec *lSpec = nullptr;
	lSpec = SDL_LoadWAV(path, spec, &pBuf, length);
	if (pBuf == NULL) { doError(); }
	int *fakeBuf = (int*)malloc(*length * 4);
	//number of data points to interolate
	int dist = 2;
	int *fakeFakeBuf = (int *)malloc(*length * 4 * 2);
	int *realBuf = (int*)calloc(dist, *length * 4 * 2);

	//converting to 32 signed int format from unsigned 8 bit
	for (int b = 0; b < *length; b++)
	{
		fakeBuf[b] = (0x7FFFFFFF / 0xFF) * (pBuf[b] - ((0xFF / 2) + 1));
	}

	//channels++
	for (int i = 0; i < *length; i++)
	{
		fakeFakeBuf[i << 1] = fakeBuf[i];
		fakeFakeBuf[(i << 1) + 1] = fakeBuf[i];
	}

	//interpolation
	for (int i = 0; i < *length << 1; i++)
	{
		realBuf[i << 1] = fakeFakeBuf[i];
		realBuf[(i << 1) + 1] = fakeFakeBuf[i];
	}

	free(fakeBuf);
	free(fakeFakeBuf);
	SDL_FreeWAV(pBuf);

	*length *= (2 + dist);
	*buf = realBuf;
	return;
}

void loadSounds()
{
	char path[64] = "data/Sound/";

	for (int s = 0; s < _countof(sounds); s++)
	{
		if (sfxList[s] != NULL)
		{
			strcat(path, sfxList[s]);
			loadSound(path, &soundSpec, &sounds[s].buf, &sounds[s].length);
			sounds[s].pos = sounds[s].length;
			strcpy(path, "data/Sound/");
		}
		else
		{
			sounds[s].buf = NULL;
		}
	}

	SDL_PauseAudioDevice(soundDev, 0);
	return;
}

void freeSounds()
{
	for (int s = 0; s < _countof(sounds); s++)
	{
		if (sounds[s].buf != NULL)
		{
			free(sounds[s].buf);
		}
	}
	return;
}

void playSound(int sound_no)
{
	if (sound_no > _countof(sounds)) { return; }
	if (sounds[sound_no].buf == NULL) { return; }
	sounds[sound_no].pos = 0;

	return;
}