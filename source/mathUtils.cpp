#include "mathUtils.h"

#include <random>
#include <chrono>
#include <cstddef>
#include <thread>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using std::mt19937;
using std::enable_if;
using std::seed_seq;
using std::uniform_int_distribution;
using std::random_device;

template<class T = mt19937, size_t N = T::state_size>
auto seededRandomEngine() -> typename enable_if<!!N, T>::type
{
	random_device rd;
	if (rd.entropy() != 0)
	{
		seed_seq seeds
		{
			rd(),
			rd(),
			rd(),
			rd(),
			rd(),
			rd(),
			rd(),
			rd(),
		};
		T seededEngine(seeds);
		return seededEngine;
	}
	srand(uint32_t(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
	seed_seq seeds
	{
		int(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
		int(std::chrono::high_resolution_clock::now().time_since_epoch().count() >> 32),
		rand(),
		rand(),
		rand(),
		rand(),
		rand(),
		rand(),
	};
	T seededEngine(seeds);
	return seededEngine;
}

thread_local mt19937 engine(seededRandomEngine());

// Distribution goes from 0 to TYPE_MAX by default

uniform_int_distribution<int32_t> distrInt;

//Not the original code, because it's better
int getSin(uint8_t deg)
{
	return static_cast<int>(sin(deg * (M_PI / 0x80)) * 512.0);
}

int getCos(uint8_t deg)
{
	return static_cast<int>(cos(deg * (M_PI / 0x80)) * 512.0);
}

uint8_t getAtan(int x, int y)
{
	return static_cast<uint8_t>(atan2(-y, -x) * 0x80 / M_PI);
}

//these are good functions
int random(int32_t mi, int32_t ma)
{
	return (mi + (distrInt(engine) % (ma - mi + 1)));
}

int sign(int x) {
	if (x != 0)
		return x / std::abs(x);

	return 0;
}

int clamp(int x, int mi, int ma) {
	return std::max(std::min(ma, x), mi);
}

