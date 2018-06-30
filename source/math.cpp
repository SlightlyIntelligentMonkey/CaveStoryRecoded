#include "math.h"

int random(UINT mi, UINT ma) {
	return rand() % (ma - mi + 1) + mi;
}

int sign(int x) {
	if (x != 0) return x / std::abs(x);
	return 0;
}

int clamp(int x, int mi, int ma) {
	return std::max(std::min(ma, x), mi);
}