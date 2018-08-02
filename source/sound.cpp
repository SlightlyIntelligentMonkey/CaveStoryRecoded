#include "sound.h"

const char *sfxList[] =
{
	NULL,
	"menu_select.wav",
	"msg.wav",
	"quote_bonk.wav",
	"switch_weapon.wav",
	"menu_prompt.wav",
	"high_pitched_hop.wav",
	NULL,
	NULL,
	NULL,
	NULL,
	"door_open.wav",
	"destroy_block.wav",
	NULL,
	"get_xp.wav",
	"quote_jump.wav",
	"quote_hurt.wav",
	"quote_die.wav",
	"menu_confirm.wav",
	NULL,
	"health_refill.wav",
	"bubble.wav",
	"chest_open.wav",
	"thud.wav",
	"quote_walk.wav",
	"funny_explode.wav",
	"big_crash.wav",
	"level_up.wav",
	"shot_hit_wall.wav",
	"teleport.wav",
	"critter_hop.wav",
	"shot_bounce.wav",
	"polar_star_1_2.wav",
	"snake_shoot.wav",
	"fireball.wav",
	"explosion1.wav",
	NULL,
	"gun_click.wav",
	"get_item.wav",
	"enemy_shoot.wav",
	"stream1.wav",
	"stream1.wav",
	"get_missile.wav",
	"computer_beep.wav",
	"missile_hit.wav",
	"xp_bounce.wav",
	"ironh_shot_fly.wav",
	NULL,	//dunno what this is
	"bubble_pew.wav",
	"polar_star_3.wav",
	"mimiga_squeak.wav",
	"enemy_hurt_small.wav",
	"enemy_hurt_big.wav",
	"enemy_squeak2.wav",
	"enemy_hurt_cool.wav",
	NULL,	//dunno what this is
	"splash.wav",
	NULL,	//object hurt
	"heli_blade.wav",
	"spur_charge1.wav",
	"spur_charge2.wav",
	"spur_charge3.wav",
	"spur_fire2.wav",
	"spur_fire3.wav",
	"spur_fire_max.wav",
	"spur_charged.wav",
	NULL,
	NULL,
	NULL,
	NULL,
	"expl_small.wav",
	"little_crash.wav",
	NULL,	//some explosion
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
	NULL,	//bubbler lvl 3
	"lightning_strike.wav",
	"jaws.wav",
	"charge.wav",
	"hidden_squeak.wav",
	"puppy_bark.wav",
	"rotate.wav",
	"block_move.wav",
	"big_hop.wav",
	"critter_fly.wav",
	"big_critter_fly.wav",
	NULL,	//supposed to be some thud
	NULL,	//supposed to be a big thud
	"booster.wav",
	"core_hurt.wav",
	"core_thrust.wav",
	"core_charge.wav",
	"nemesis.wav",
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
	NULL,//"org_bass_01.wav",
	NULL,//"org_snare_01.wav",
	NULL,//"org_hi_close.wav",
	NULL,//"org_hi_open.wav",
	NULL,//"org_tom_01.wav",
	NULL,//"org_per_01.wav",
	NULL,
	NULL,
	NULL,
	NULL
};

struct SOUND_EFFECT
{
	BYTE *buf;
	Uint32 length;
};

SOUND_EFFECT sounds[_countof(sfxList)];

SDL_AudioStream *audioStream;
SDL_AudioDeviceID soundDev = 0;
SDL_AudioSpec soundSpec;
SDL_AudioSpec want;

//use int pointers for easy use
void mixAudio(int *dst, int *src, Uint32 len, Uint8 lVolume, Uint8 rVolume)
{
	//using 32 float point on native system 
	for (int i = 0; i <= (len / 4); i++)
	{
		//probably shouldn't leave it like this but I'll just leave it like this for now
		//technically the best algorithm if you have infinite values
		dst[i] = (dst[i] + src[i]);
	}
	return;
}

void audio_callback(void *userdata, Uint8 *stream, int len)
{
	int tempLength = 0;
	Uint8 *tempS = 0;
	AUDIO *audio = (AUDIO*)userdata;
	AUDIO *prevAudio = audio;
	audio = audio->next;

	memset(stream, 0, len);

	while (audio != nullptr)
	{
		if (audio->len > 0)
		{
			if (audio->len > len) { tempLength = len; }
			else { tempLength = audio->len; }
			mixAudio((int*)stream, (int*)audio->buf, tempLength, 100, 100);

			audio->len -= tempLength;
			audio->buf += tempLength;

			prevAudio = audio;
			audio = audio->next;
		}
		else
		{
			//clears out audio thats finished playing
			prevAudio->next = audio->next;
			free(audio);
			audio = prevAudio->next;
		}
	}

	return;
}

void ini_audio()
{
	want.channels = 2;
	want.freq = 44100;
	want.format = AUDIO_S32SYS;
	want.samples = 4096;
	want.callback = audio_callback;
	want.userdata = calloc(1, sizeof(AUDIO));

	soundDev = SDL_OpenAudioDevice(
		NULL,
		0,
		&want,
		&soundSpec,
		NULL);

	if (soundDev == 0)
		doError();

	SDL_PauseAudioDevice(soundDev, 0);

	return;
}

//since sdl doesn't actually get the loaded wav in the specified format,
//we have to do it ourselves
void loadSound(char *path, SDL_AudioSpec *spec, BYTE **buf, Uint32 *length)
{
	int view = 0;
	SDL_AudioSpec *lSpec = nullptr;

	lSpec = SDL_LoadWAV(path, spec, buf, length);

	if (lSpec == nullptr)
		doError();

	BYTE *pBuf = *buf;
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
	for (int i = 0; i < *length; i++)
	{
		realBuf[i << 1] = fakeFakeBuf[i];
		realBuf[(i << 1) + 1] = fakeFakeBuf[i];
	}

	free(fakeBuf);
	free(fakeFakeBuf);
	SDL_FreeWAV(*buf);

	*length *= (4 * dist * 2);
	*buf = (BYTE*)realBuf;
	return;
}

const char *pathToSounds = "data/Sound/";
void loadSounds()
{
	char path[64];

	for (int s = 0; s < _countof(sounds); s++)
	{
		if (sfxList[s] != NULL)
		{
			strcpy(path, pathToSounds);
			strcat(path, sfxList[s]);

			loadSound(path, &soundSpec, &sounds[s].buf, &sounds[s].length);

			if (sounds[s].buf == NULL)
				doError();
		}
		else
			sounds[s].buf = NULL;
	}

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
	return; //disabled
	AUDIO *sound = nullptr;
	AUDIO *lastSound = (AUDIO*)want.userdata;

	if (sound_no > _countof(sounds))
		return;

	sound = (AUDIO*)calloc(1, sizeof(AUDIO));

	if (sounds[sound_no].buf != NULL)
		sound->buf = sounds[sound_no].buf;
	else
		return;

	sound->len = sounds[sound_no].length;

	while (lastSound->next != nullptr)
		lastSound = lastSound->next;

	lastSound->next = sound;

	return;
}
