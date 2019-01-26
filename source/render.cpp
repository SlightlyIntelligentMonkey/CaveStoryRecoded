#include "render.h"

#include <string>
#include "SDL.h"
#include "lodepng/lodepng.h"
#include "game.h"
#include "main.h"
#include "common.h"
#include "input.h"
#include "log.h"
#include "filesystem.h"
#ifdef USE_ICONS_SDL2
#include "icon_mini.h"
#endif

using namespace std::string_literals;

SDL_Window *gWindow;
SDL_Renderer *gRenderer;

SDL_Rect gRcDraw = {0, 0, 0, 0};
SDL_Rect gImageRectangle = {0, 0, 0, 0};
SDL_Rect gDrawRectangle = {0, 0, 0, 0};
SDL_Rect gCliprect = {0, 0, 0, 0};

int gScreenWidth = 320;
int gScreenHeight = 240;
int gScreenScale = 2;

bool gDisplayFpsCounter = false;

int gPrevWidth = 0;
int gPrevHeight = 0;
int gPrevScale = 0;

int gCharWidth = 24;
int gCharHeight = 24;
int gCharScale = 2;

int gFramewait = 20; //17 for 60-ish fps

uint32_t gWindowFlags = 0;

static SDL_Surface *gCursorSurface;
static SDL_Cursor *gCursor;

static SDL_Surface* loadPNGToSurface(const std::string& path)
{
	SDL_Surface *surface = nullptr;

	unsigned char *pixel_buffer;
	unsigned int width;
	unsigned int height;
	if (const unsigned int error = lodepng_decode32_file(&pixel_buffer, &width, &height, path.c_str()))
		doCustomError("loadPNGToSurface failed!\n\nlodepng error: "s + lodepng_error_text(error) + "\nFile : " + path);
	else
	{
		// We don't use SDL_CreateRGBSurfaceWithFormatFrom because then the pixel data wouldn't
		// automatically be freed when the surface is destroyed, so we have to do this trickery instead.

		surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGBA32);

		if (surface == nullptr)
			doCustomError("loadPNGToSurface failed!\n\nSDL2 error: "s + SDL_GetError() + "\nFile : " + path);
		else
			for (unsigned int i = 0; i < height; ++i)
				memcpy(static_cast<unsigned char*>(surface->pixels) + (i * surface->pitch), pixel_buffer + (i * width * 4), width * 4);

		free(pixel_buffer);
	}

	return surface;
}

static SDL_Texture* loadPNGToTexture(SDL_Renderer *localRenderer, const std::string& path)
{
	SDL_Texture *texture = nullptr;

	SDL_Surface *surface = loadPNGToSurface(path);

	if (surface)
	{
		texture = SDL_CreateTextureFromSurface(localRenderer, surface);

		if (texture == nullptr)
			doCustomError("loadPNGToTexture failed!\n\nSDL2 error: "s + SDL_GetError());

		SDL_FreeSurface(surface);
	}

	return texture;
}

static SDL_Texture* loadBMPToTexture(SDL_Renderer *rend, const std::string& path)
{
	SDL_Texture *texture = nullptr;
	SDL_Surface *surf = SDL_LoadBMP(path.c_str());
	if (surf)
	{
		SDL_SetColorKey(surf, 1, 0);
		texture = SDL_CreateTextureFromSurface(rend, surf);

		if (texture == nullptr)
			doCustomError("loadBMPToTexture failed!\n\nSDL2 error: "s + SDL_GetError());

		SDL_FreeSurface(surf);
	}

	return texture;
}

//Create window
int createWindow(int width, int height, int scale)
{
	const int createWidth = width * scale;
	const int createHeight = height * scale;

	gScreenWidth = width;
	gScreenHeight = height;
	gScreenScale = scale;

	//Set window
	if (!gWindow)
	{
		gWindow = SDL_CreateWindow("Cave Story Recoded",
		                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		                          createWidth, createHeight,
		                          gWindowFlags);

#ifdef USE_ICONS_SDL2
		// Set the window icon.
		// Note that we skip this on Windows since we do it natively.
		unsigned char *pixel_buffer;
		unsigned int width;
		unsigned int height;
		if (!lodepng_decode32(&pixel_buffer, &width, &height, res_icon_mini_png, res_icon_mini_png_len))
		{
			SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(pixel_buffer, width, height, 0, width * 4, SDL_PIXELFORMAT_RGBA32);

			if (surface)
			{
				SDL_SetWindowIcon(gWindow, surface);
				SDL_FreeSurface(surface);
			}

			free(pixel_buffer);
		}
#endif
	}
	else
		SDL_SetWindowSize(gWindow, createWidth, createHeight);

	if (gWindowFlags == SDL_WINDOW_FULLSCREEN)
        SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN);

	//Set renderer
	if (!gRenderer)
	{
		gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
		SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

		if (gRenderer == nullptr)
			logError("Couldn't create renderer! SDL2 error: "s + SDL_GetError());
	}

	// TODO free these when closing-down
	gCursorSurface = loadPNGToSurface("data/Cursor/cursor_normal.png");
	if (gCursorSurface)
	{
		gCursor = SDL_CreateColorCursor(gCursorSurface, 0, 0);	// Don't worry, the hotspots are accurate to the original files
		if (gCursor)
			SDL_SetCursor(gCursor);
        else
            logError("Couldn't create cursor");
	}
	else
        logError("Couldn't load cursor image (is the file missing ?)");

	return 0;
}

void switchScreenMode()
{
	gWindowFlags ^= SDL_WINDOW_FULLSCREEN_DESKTOP;

	//Unlike gPrevWidth and Height, this is used for fixing the view when going between fullscreen and windowed mode
	const int lastWidth = gScreenWidth;
	const int lastHeight = gScreenHeight;

	if (gWindowFlags & SDL_WINDOW_FULLSCREEN_DESKTOP)
	{
		SDL_DisplayMode dm;

		if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
			doError();

		gPrevWidth = gScreenWidth;
		gPrevHeight = gScreenHeight;
		gPrevScale = gScreenScale;

		gScreenWidth = (dm.w * 240) / dm.h;
		gScreenHeight = 240;
		gScreenScale = dm.h / 240;
	}
	else
	{
		gScreenWidth = gPrevWidth;
		gScreenHeight = gPrevHeight;
		gScreenScale = gPrevScale;
	}

	//Scale renderer to proper proportions
	SDL_RenderSetLogicalSize(gRenderer, gScreenWidth * gScreenScale, gScreenHeight * gScreenScale);

	//Ensure that the view is shifted properly
	gViewport.x += (lastWidth - gScreenWidth) * 0x100;
	gViewport.y += (lastHeight - gScreenHeight) * 0x100;

	//Set window properties
	SDL_SetWindowSize(gWindow, gScreenWidth * gScreenScale, gScreenHeight * gScreenScale);
	SDL_SetWindowFullscreen(gWindow, gWindowFlags);
}

uint32_t calculateFPS()
{
	static bool hasFunctionBeenExecuted = false;
	static uint32_t ticksAtStart = 0;

	if (!hasFunctionBeenExecuted)
	{
		ticksAtStart = SDL_GetTicks();
		hasFunctionBeenExecuted = true;
	}

	const uint32_t tickCount = SDL_GetTicks();
	static uint32_t timeElapsed = 0;
	++timeElapsed;

	static uint32_t currentRetVal;
	if (ticksAtStart + 1000 <= tickCount)
	{
		ticksAtStart += 1000;
		currentRetVal = timeElapsed;
		timeElapsed = 0;
	}
	return currentRetVal;
}

void drawFPS()
{
	if (gDisplayFpsCounter)
		drawNumber(calculateFPS(), gScreenWidth - 40, 8, false);
}

bool drawWindow()
{
	while (true)
	{
		if (!handleEvents())
			return false;

		//framerate limiter
		static uint32_t timePrev;
		const uint32_t timeNow = SDL_GetTicks();

		if (timeNow >= timePrev + gFramewait)
		{
			if (timeNow >= timePrev + 100)
				timePrev = timeNow;	// If the timer is freakishly out of sync, panic and reset it, instead of spamming frames for who-knows how long
			else
				timePrev += gFramewait;
			break;
		}

		SDL_Delay(1);
	}

	drawFPS();

	SDL_RenderPresent(gRenderer);

	return true;
}

void captureScreen(enum TextureNums texture_id)
{
	//Destroy previously existing texture and load new one
	if (gSprites[texture_id] != nullptr)
		SDL_DestroyTexture(gSprites[texture_id]);

	int width, height;
	SDL_GetRendererOutputSize(gRenderer, &width, &height);

	// The depth parameter here is unused. Be aware, it will be removed in SDL 2.1.
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(SDL_TEXTUREACCESS_TARGET, width, height, 0, SDL_PIXELFORMAT_RGB888);
	SDL_RenderReadPixels(gRenderer, nullptr, SDL_PIXELFORMAT_RGB888, surface->pixels, surface->pitch);

	gSprites[texture_id] = SDL_CreateTextureFromSurface(gRenderer, surface);

	SDL_FreeSurface(surface);
}

void createTextureBuffer(enum TextureNums texture_id, int width, int height)
{
	//Destroy previously existing texture and load new one
	if (gSprites[texture_id] != nullptr)
		SDL_DestroyTexture(gSprites[texture_id]);

	gSprites[texture_id] = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, width, height);
}

//Texture and drawing stuff
void loadImage(const std::string& file, SDL_Texture **tex)
{
	if (tex == nullptr)
		doCustomError("tex was nullptr in loadImage");

	//Destroy previously existing texture and load new one
	if (*tex != nullptr)
		SDL_DestroyTexture(*tex);

	//loads either a png or bmp
	if(fileExists("data/" + file + ".png"))
		*tex = loadPNGToTexture(gRenderer, "data/" + file + ".png");
	if (fileExists("data/" + file + ".bmp"))
		*tex = loadBMPToTexture(gRenderer, "data/" + file + ".bmp");
	

	//Error if anything failed
	if (*tex == nullptr)
		doError();

	//Set to transparent, error if failed
	if (SDL_SetTextureBlendMode(*tex, SDL_BLENDMODE_BLEND) != 0)
		doError();
}

//loads images with limited colors
uint8_t colorValTbl[] = { 0, 52, 87, 116, 144, 172, 206, 255 };
void loadImageBad(const std::string& file, SDL_Texture **tex)
{
	SDL_Surface *surface;

	if (tex == nullptr)
		doCustomError("tex was nullptr in loadImage");

	//Destroy previously existing texture and load new one
	if (*tex != nullptr)
		SDL_DestroyTexture(*tex);
	surface = loadPNGToSurface(file);
	if (surface->format->palette != nullptr)
	{
		SDL_Color *colors = new SDL_Color[4 * surface->format->palette->ncolors]();
		for (int c = 0; c < surface->format->palette->ncolors; c++)
		{
			colors[c].r = colorValTbl[(surface->format->palette->colors[c].r * (sizeof(colorValTbl) - 1)) / 0xFF];
			colors[c].g = colorValTbl[(surface->format->palette->colors[c].g * (sizeof(colorValTbl) - 1)) / 0xFF];
			colors[c].b = colorValTbl[(surface->format->palette->colors[c].b * (sizeof(colorValTbl) - 1)) / 0xFF];
			colors[c].a = surface->format->palette->colors[c].a;
		}
		SDL_SetPaletteColors(surface->format->palette, colors, 0, surface->format->palette->ncolors);
		delete[] colors;
	}
	else
	{
		uint8_t *pixel = static_cast<uint8_t*>(surface->pixels);
		for (int p = 0; p < surface->w*surface->h; p++)
			if (pixel[p])
				pixel[p] = colorValTbl[(pixel[p] * (sizeof(colorValTbl) - 1)) / 0xFF];
	}
	*tex = SDL_CreateTextureFromSurface(gRenderer, surface);

	//Error if anything failed
	if (*tex == nullptr)
		doError();

	//Set to transparent, error if failed
	if (SDL_SetTextureBlendMode(*tex, SDL_BLENDMODE_BLEND) != 0)
		doError();
}

//Drawing functions
void setCliprect(const RECT *rect)
{
	//All of this code should be pretty self explanatory
	if (rect != nullptr)
	{
		gCliprect = { rect->left * gScreenScale, rect->top * gScreenScale, (rect->right - rect->left) * gScreenScale, (rect->bottom - rect->top) * gScreenScale };
		SDL_RenderSetClipRect(gRenderer, &gCliprect);
	}
	else
		SDL_RenderSetClipRect(gRenderer, nullptr);
}

void drawTexture(SDL_Texture *texture, const RECT *rect, int x, int y)
{
	//Set framerect
	if (rect)
	{
		gImageRectangle = { rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top };

		//Set drawrect, with defined width and height
		gRcDraw.x = x * gScreenScale;
		gRcDraw.y = y * gScreenScale;
		gRcDraw.w = gImageRectangle.w * gScreenScale;
		gRcDraw.h = gImageRectangle.h * gScreenScale;

		//Draw to screen, error if failed
		if (SDL_RenderCopy(gRenderer, texture, &gImageRectangle, &gRcDraw) != 0)
			doError();
	}
	else
	{
		int w, h;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

		gRcDraw.x = (x-(w/2)) * gScreenScale;
		gRcDraw.y = (y-(h/2)) * gScreenScale;
		gRcDraw.w = w * gScreenScale;
		gRcDraw.h = h * gScreenScale;

		if (SDL_RenderCopy(gRenderer, texture, nullptr, &gRcDraw) != 0)
			doError();
	}
}

void drawTextureNoScale(SDL_Texture *texture, const RECT *rect, int x, int y)
{
	//Set framerect
	if (rect)
	{
		gImageRectangle = { rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top };

		//Set drawrect, with defined width and height
		gRcDraw.x = x;
		gRcDraw.y = y;
		gRcDraw.w = gImageRectangle.w;
		gRcDraw.h = gImageRectangle.h;

		//Draw to screen, error if failed
		if (SDL_RenderCopy(gRenderer, texture, &gImageRectangle, &gRcDraw) != 0)
			doError();
	}
	else
	{
		int w, h;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

		gRcDraw.x = x;
		gRcDraw.y = y;
		gRcDraw.w = w;
		gRcDraw.h = h;

		if (SDL_RenderCopy(gRenderer, texture, nullptr, &gRcDraw) != 0)
			doError();
	}
}

void drawTextureSize(SDL_Texture *texture, const RECT *rect, int x, int y, int w, int h)
{
	//Set framerect
	gImageRectangle = { rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top };

	//Set drawrect, with defined width and height
	gRcDraw.x = x * gScreenScale;
	gRcDraw.y = y * gScreenScale;
	gRcDraw.w = w * gScreenScale;
	gRcDraw.h = h * gScreenScale;

	//Draw to screen, error if failed
	if (SDL_RenderCopy(gRenderer, texture, &gImageRectangle, &gRcDraw) != 0)
		doError();
}


void drawNumber(int value, int x, int y, bool bZero)
{
	RECT numbRect;

	int offset = 0;
	int pos = 1000; //Replacing an array a day keeps The Doctor away

	//Cap value
	if (value > 9999)
		value = 9999;

	int count = 0;
	while (offset < 4)
	{
		int drawValue = 0;

		//Get value of this digit
		while (pos <= value)
		{
			value -= pos;

			++drawValue;
			++count;
		}

		if ((bZero && offset == 2) || count != 0 || offset == 3) //bZero just makes the second from the right character always draw
		{
			//Set rect and draw
			numbRect = { drawValue << 3, 56, (drawValue + 1) << 3, 64 };
			drawTexture(gSprites[26], &numbRect, x + (offset << 3), y);
		}

		//Change checking digit
		offset++;
		pos /= 10;
	}
}

bool isMultibyte(uint8_t c)  attrConst;

bool isMultibyte(uint8_t c)  //Shift-JIS
{
	if (c > 0x80u && c <= 0x9Fu)
		return true;
	return !(c <= 0xDFu || c > 0xEFu);
}

void drawString(int x, int y, const std::string& str, const uint8_t *flag)
{
	RECT rcChar;

	for (int i = 0; ; i++)
	{
		if (str[i]) //Go through string until reaching a terminator (0x00) character.
		{
			//Get separation value from flag array
			int sep = 6;

			if (flag != nullptr && flag[i] & 1)
				sep = 5;
			//Circle thing
			if (flag != nullptr && flag[i] & 2)
			{
				rcChar = { 64, 48, 72, 56 };
				drawTexture(gSprites[TEX_TEXTBOX], &rcChar, x + (i * sep), y + 2);
			}
			else
			{
				//Set framerect to what it's supposed to be
				const int drawIndex = i;

				if (isMultibyte(str[i]))
				{
					const int localChar = 0x81 + str[i + 1] + ((str[i] - 0x81) * 0x100);
					rcChar.left = ((localChar % 32) * gCharWidth);
					rcChar.top = ((localChar >> 5) * gCharHeight);
					rcChar.right = rcChar.left + gCharWidth;
					rcChar.bottom = rcChar.top + gCharHeight;
					i++;
				}
				else
				{
					rcChar.left = ((str[i] % 32) * gCharWidth);
					rcChar.top = ((str[i] >> 5) * gCharHeight);
					rcChar.right = rcChar.left + gCharWidth;
					rcChar.bottom = rcChar.top + gCharHeight;
				}

				//Draw to the screen
				drawTextureSize(gSprites[0x26], &rcChar, x + (drawIndex * sep), y, gCharWidth / gCharScale, gCharHeight / gCharScale);
			}
		}
		else
			break;
	}
}

void drawRect(int x, int y, int w, int h)
{
	//Map this onto an SDL_Rect
	gDrawRectangle.x = x * gScreenScale;
	gDrawRectangle.y = y * gScreenScale;
	gDrawRectangle.w = w * gScreenScale;
	gDrawRectangle.h = h * gScreenScale;

	//Render onto the screen
	SDL_RenderFillRect(gRenderer, &gDrawRectangle);
}
