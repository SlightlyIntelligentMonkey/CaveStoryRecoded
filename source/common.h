#pragma once

#include <cstdint>
#include <cstddef>

// Structs
struct RECT
{
	int32_t left, top, right, bottom;
};

struct POINT
{
	int32_t x, y;
};

// Enums
enum Collisions
{
	leftWall = 0x1, //Touching a left wall
	ceiling = 0x2, //Touching a ceiling
	rightWall = 0x4, //Touching a right wall
	ground = 0x8, //Touching the ground
	slopeRight = 0x10, //On a slope that goes up to the right (used to keep Quote on the slope)
	slopeLeft = 0x20, //On a slope that goes up to the left (used to keep Quote on the slope)
    solid = 0xFF,    //Touching some solid surface
	water = 0x100, //In water
	spike = 0x400, //In spike tile
	bloodWater = 0x800, //Set by water spikes, if wasn't in water, water particles will be blood (used in Last Cave)
	windLeft = 0x1000, //In wind going to the left
	windUp = 0x2000, //In wind going up
	windRight = 0x4000, //In wind going to the right
	windDown = 0x8000, //In wind going down
	slopeE = 0x10000, //A slope flag used to keep Quote on the slope
	slopeF = 0x20000, //A slope flag used to keep Quote on the slope
	slopeG = 0x40000, //A slope flag used to keep Quote on the slope
	slopeH = 0x80000, //A slope flag used to keep Quote on the slope
};

enum Direction
{
	dirLeft,
	dirUp,
	dirRight,
	dirDown,
	dirAuto,
};

#ifndef _countof
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
template <typename T, size_t N> constexpr size_t _countof(T const (&)[N]) noexcept
{
    return N;
}

// For dynamic containers
template <class C> size_t _countof(C const& arr)
{
    return arr.size();
}
#elif __cplusplus >= 199711L
template <typename T, size_t N> char (&COUNTOF_REQUIRES_ARRAY_ARGUMENT(T(&)[N]))[N];
#define _countof(arr) sizeof(COUNTOF_REQUIRES_ARRAY_ARGUMENT(x))
#else
#define _countof(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif
#endif

#include <sys/types.h>	// For ssize_t
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;	// For some demented reason MSVC doesn't have ssize_t
#endif

// Macros
#ifdef __GNUC__
#define attrPure __attribute__((pure))
#define attrConst __attribute__((const))
#else
#define attrPure
#define attrConst
#endif

