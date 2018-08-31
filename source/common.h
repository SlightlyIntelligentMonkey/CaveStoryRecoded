#pragma once

#include <cstdint>
#include <string>

struct RECT
{
	int32_t left, top, right, bottom;
};

struct POINT
{
	int32_t x, y;
};

//Debug flags
extern int debugFlags;

enum debugFlagsEnum
{
	showSlots = 0x01,
	showNPCId = 0x02,
	showBULId = 0x04,
	showCARId = 0x08,
	notifyOnNotImplemented = 0x10,
	showNPCHealth = 0x20,
};

//Enums
enum Collisions
{
	leftWall = 0b0001, //Touching a left wall
	ceiling = 0b0010, //Touching a ceiling
	rightWall = 0b0100, //Touching a right wall
	ground = 0b1000, //Touching the ground
	slopeRight = 0x10, //On a slope that goes up to the right (used to keep Quote on the slope)
	slopeLeft = 0x20, //On a slope that goes up to the left (used to keep Quote on the slope)
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
	dirCenter,
};

//Struct
struct VIEW
{
	int x;
	int y;
	int *lookX;
	int *lookY;
	int speed;

	int quake;
	int quake2;
};

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;
struct SDL_Texture;
union SDL_Event;

//Drawing related variables
extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern SDL_Rect DrawRect;
extern SDL_Rect ImageRect;

extern VIEW viewport;

extern SDL_Texture* sprites[40];

//Event variable
extern SDL_Event events;

//Some game variables
extern int framerate;
extern unsigned int framerateTicks;

extern int mode;

extern bool exitGame;

extern int gameFlags;

//Functions
[[noreturn]] void doError() noexcept;
[[noreturn]] void doCustomError(const std::string& msg) noexcept;

//Macros
#ifdef __GNUC__
#define attrPure __attribute__((pure))
#define attrConst __attribute__((const))
#else
#define attrPure
#define attrConst
#endif
