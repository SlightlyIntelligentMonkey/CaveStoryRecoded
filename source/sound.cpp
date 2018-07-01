#include <set>
#include "sound.h"

#include "../liborganya/organya.h"

// -- Generic Audio --

static std::set<sound::AudioTrack*> tracks;

// -- Organya --

const char* sound::musicList[42] = {
	"xxxx", "wanpaku", "anzen", "gameover", "gravity", "weed", "mdown2", "fireeye", "vivi", "mura", "fanfale1", "ginsuke", "cemetery", "plant", "kodou",
	"fanfale2", "fanfale3", "dr", "escape", "jenka", "maze", "access", "ironh", "grand", "curly", "oside", "requiem", "wanpak2", "quiet", "lastcave",
	"balcony", "lastbtl", "lastbt3", "ending", "zonbie", "bdown", "hell", "jenka2", "marine", "ballos", "toroko", "white",
};

sound::OrganyaTrack::OrganyaTrack(const char * fn) {
	orgDecoder = org_decoder_create(fn, "liborganya/samples", 0);
}
sound::OrganyaTrack::~OrganyaTrack() {
	if (orgDecoder)
		org_decoder_destroy(orgDecoder);
}
void sound::OrganyaTrack::audio(int16_t * stream, int len) {
	if (orgDecoder)
		org_decode_samples(orgDecoder, stream, len);
}

static sound::OrganyaTrack * orgTrack = 0;
static int currentTrackId = 0;

void sound::playOrg(int id) {
	if (currentTrackId == id)
		return;
	currentTrackId = id;
	SDL_LockAudio();
	if (orgTrack) {
		tracks.erase(orgTrack);
		delete orgTrack;
	}
	char orgPath[256];
	strcpy(orgPath, "./data/Org/");
	strcat(orgPath, musicList[id]);
	strcat(orgPath, ".org");
	orgTrack = new OrganyaTrack(orgPath);
	tracks.insert(orgTrack);
	SDL_UnlockAudio();
}

// -- Mixer, Init, Quit --

static void soundMixer(void * userdata, Uint8 * stream, int len) {
	len /= sizeof(int16_t);
	int16_t * samples = (int16_t *) stream;
	int16_t * temp = (int16_t *) alloca(len * 4);
	memset(samples, 0, sizeof(int16_t) * len);
	for (sound::AudioTrack * trk : tracks) {
		trk->audio(temp, len / 2);
		for (int i = 0; i < len; i++) {
			int32_t p = samples[i] + (int32_t) (temp[i] * trk->volume);
			if (p > 0x7FFF)
				p = 0x7FFF;
			if (p < -0x8000)
				p = -0x8000;
			samples[i] = (int32_t) p;
		}
	}
}

void sound::init() {
	SDL_AudioSpec want;
	SDL_zero(want);
	want.freq = 44100;
	want.format = AUDIO_S16;
	want.channels = 2;
	want.samples = 4096;
	want.callback = soundMixer;
	SDL_OpenAudio(&want, NULL);
	SDL_PauseAudio(0);
}

void sound::quit() {
	SDL_CloseAudio();
	tracks.clear();
	if (orgTrack)
		delete orgTrack;
	orgTrack = 0;
}
