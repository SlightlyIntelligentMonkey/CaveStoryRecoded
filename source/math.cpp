#include "math.h"

//Basically the original game's but instead of using an array it generates on the fly
int getSin(BYTE deg)
{
	return (int)(sin(deg * (M_PI / 0x80)) * 512.0);
}

int getCos(BYTE deg)
{
	return (int)(cos(deg * (M_PI / 0x80)) * 512.0);
}

//these are good functions
int random(uint32_t mi, uint32_t ma) {
	return rand() % (ma - mi + 1) + mi;
}

int sign(int x) {
	if (x != 0) return x / std::abs(x);
	return 0;
}

int clamp(int x, int mi, int ma) {
	return std::max(std::min(ma, x), mi);
}