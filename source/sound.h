#pragma once
#include "common.h"

extern "C" {
	// sound.cpp includes liborganya, but sound.h does not!
	struct org_decoder_t;
}

namespace sound {
	class AudioTrack {
		public:
			virtual void audio(int16_t* stream, int len) = 0;
			int16_t volume = 75;
	};
	class OrganyaTrack : public AudioTrack {
		public:
			OrganyaTrack(const char * orgfn);
			~OrganyaTrack();
			// len is length in frames.
			// stream is signed 16-bit stereo 44100hz.
			void audio(int16_t* stream, int len) override;
		private:
			org_decoder_t * orgDecoder;
	};
	extern const char* musicList[];
	void playOrg(int id);
	void init();
	void quit();
}
