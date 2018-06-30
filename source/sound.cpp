#include "sound.h"

const char* musicList[42] = {
	"xxxx", "wanpaku", "anzen", "gameover", "gravity", "weed", "mdown2", "fireeye", "vivi", "mura", "fanfale1", "ginsuke", "cemetery", "plant", "kodou",
	"fanfale2", "fanfale3", "dr", "escape", "jenka", "maze", "access", "ironh", "grand", "curly", "oside", "requiem", "wanpak2", "quiet", "lastcave",
	"balcony", "lastbtl", "lastbt3", "ending", "zonbie", "bdown", "hell", "jenka2", "marine", "ballos", "toroko", "white",
};

void playOrg(int id) {
	stopOrganyaMusic();

	char orgPath[256];
	strcpy(orgPath, "./data/Org/");
	strcat(orgPath, musicList[id]);
	strcat(orgPath, ".org");

	loadOrganya(orgPath);
	playOrganyaMusic();
}