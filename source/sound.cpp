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

Mix_Chunk* sounds[_countof(sfxList)];
const char *soundPath = "data/Sound/";

void initSound()
{
	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != -1)
	{
		//Load sounds
		char path[64];

		for (int s = 0; s < _countof(sounds); s++)
		{
			if (sfxList[s] != NULL)
			{
				strcpy(path, soundPath);
				strcat(path, sfxList[s]);

				sounds[s] = Mix_LoadWAV(path);

				if (sounds[s] == NULL)
				{
					char error[0x100];
					snprintf(error, 0x100, "Couldn't load sound effect %s.", path);
					doCustomError(error);
				}
			}
			else
			{
				sounds[s] = nullptr;
			}
		}
	}
	else
		doCustomError(Mix_GetError());
}

void freeSounds()
{
	for (int s = 0; s < _countof(sounds); s++)
	{
		if (sounds[s] != nullptr)
			Mix_FreeChunk(sounds[s]);
	}
}

void playSound(int sound_no)
{
	if (sounds[sound_no] != nullptr)
	{
		Mix_HaltChannel(-1);
		Mix_PlayChannel(-1, sounds[sound_no], 0);
	}
}
