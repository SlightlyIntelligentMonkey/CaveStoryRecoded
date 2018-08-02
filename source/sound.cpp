#include "sound.h"

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

Mix_Chunk* sounds[_countof(sfxList)];
const char *soundPath = "data/Sound/";

void initSound()
{
	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != -1)
	{
		Mix_AllocateChannels(_countof(sfxList));

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
		Mix_HaltChannel(sound_no); //Cancel sound last playing
		Mix_PlayChannel(sound_no, sounds[sound_no], 0);
	}
}
