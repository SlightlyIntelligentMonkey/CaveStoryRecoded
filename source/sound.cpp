#include "sound.h"
#include <SDL.h>

char *sfxList[] =
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
	"bass_01.wav",
	"snare_01.wav",
	"hi_close.wav",
	"hi_open.wav",
	"tom_01.wav",
	"per_01.wav",
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

void mixAudio(BYTE *dst, BYTE *src, Uint32 len, Uint8 lVolume, Uint8 rVolume)
{
	//using 32 float point on native system 
	for (int f = 0; f <= len; f + 4)
	{
		//left pan
		if (f % 2 == 0)
		{
			dst[f] = (float)dst[f] + (float)src[f];
		}
		//right pan
		else
		{
			dst[f] = (float)dst[f] + (float)src[f];
		}
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
			mixAudio(stream, audio->buf, tempLength, 100, 100);

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
	want.freq = 22050;
	want.format = AUDIO_F32SYS;
	want.samples = 2048;
	want.callback = audio_callback;
	want.userdata = calloc(1, sizeof(AUDIO));

	soundDev = SDL_OpenAudioDevice(
		NULL,
		0,
		&want,
		&soundSpec,
		SDL_AUDIO_ALLOW_ANY_CHANGE);

	if (soundDev == 0) { doCustomError("Sound device didn't start."); }

	memset(sounds, 0, sizeof(sounds));

	return;
}

void loadSounds()
{
	char path[64] = "data/soundfx/";
	Uint32 silentLength = 0;
	BYTE *silentBuf = nullptr;
	SDL_LoadWAV("data/soundfx/silence.wav", &soundSpec, &silentBuf, &silentLength);

	for (int s = 0; s < _countof(sounds); s++)
	{
		if (sfxList[s] != NULL)
		{
			strcat(path, sfxList[s]);
			SDL_LoadWAV(path, &soundSpec, &sounds[s].buf, &sounds[s].length);
			if (sounds[s].buf == NULL)
			{
				strcpy(path, "Unable to load ");
				strcat(path, sfxList[s]);
				doCustomError(path);
			}
			strcpy(path, "data/soundfx/");
		}
		else
		{
			sounds[s].buf = silentBuf;
			sounds[s].length = silentLength;
		}
	}

	SDL_PauseAudioDevice(soundDev, 0);
	return;
}

void freeSounds()
{
	for (int s = 0; s < _countof(sounds); s++)
	{
		SDL_FreeWAV(sounds[s].buf);
	}
	return;
}

void playSound(int sound_no)
{
	AUDIO *sound = nullptr;
	AUDIO *lastSound = (AUDIO*)want.userdata;
	if (sound_no > _countof(sounds)) { return; }
	sound = (AUDIO*)calloc(1, sizeof(AUDIO));
	sound->buf = sounds[sound_no].buf;
	sound->len = sounds[sound_no].length;

	while (lastSound->next != nullptr)
	{
		lastSound = lastSound->next;
	}

	lastSound->next = sound;

	return;
}