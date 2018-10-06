#include "render.h"

#include "SDL.h"
#include "lodepng/lodepng.h"
#include "game.h"
#include "main.h"
#include "common.h"
#include "input.h"
#include "log.h"

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Rect rcDraw = {0, 0, 0, 0};
SDL_Rect rcImage = {0, 0, 0, 0};

SDL_Rect drawRectangle = {0, 0, 0, 0};
SDL_Rect cliprect = {0, 0, 0, 0};

int screenWidth = 0;
int screenHeight = 0;
int screenScale = 0;

int prevWidth = 0;
int prevHeight = 0;
int prevScale = 0;

int windowFlags = 0;

int charWidth = 24;
int charHeight = 24;
int charScale = 2;

int framerate = 20; //17 for 60-ish fps

static SDL_Surface *cursor_surface;
static SDL_Cursor *cursor;

#ifdef USE_ICONS_SDL2
extern const unsigned char binary_res_icon_mini_png_start[];
extern const unsigned char binary_res_icon_mini_png_end[];
#endif

static SDL_Surface* loadPNGToSurface(const char *path)
{
	SDL_Surface *surface = nullptr;

	unsigned char *pixel_buffer;
	unsigned int width;
	unsigned int height;
	if (const unsigned int error = lodepng_decode32_file(&pixel_buffer, &width, &height, path))
	{
		doCustomError((std::string)"loadPNGToSurface failed!\n\nlodepng error: " + lodepng_error_text(error));
	}
	else
	{
		// We don't use SDL_CreateRGBSurfaceWithFormatFrom because then the pixel data wouldn't
		// automatically be freed when the surface is destroyed, so we have to do this trickery instead.

		surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGBA32);

		if (surface == nullptr)
		{
			doCustomError((std::string)"loadPNGToSurface failed!\n\nSDL2 error: " + SDL_GetError());
		}
		else
		{
			for (unsigned int i = 0; i < height; ++i)
				memcpy((unsigned char*)surface->pixels + (i * surface->pitch), pixel_buffer + (i * width * 4), width * 4);
		}

		free(pixel_buffer);
	}

	return surface;
}

static SDL_Texture* loadPNGToTexture(SDL_Renderer *localRenderer, const char *path)
{
	SDL_Texture *texture = nullptr;

	SDL_Surface *surface = loadPNGToSurface(path);

	if (surface)
	{
		texture = SDL_CreateTextureFromSurface(localRenderer, surface);

		if (texture == nullptr)
			doCustomError((std::string)"loadPNGToTexture failed!\n\nSDL2 error: " + SDL_GetError());

		SDL_FreeSurface(surface);
	}

	return texture;
}

//Create window
int createWindow(int width, int height, int scale, bool fullscreen)
{
	const int createWidth = width * scale;
	const int createHeight = height * scale;

	screenWidth = width;
	screenHeight = height;
	screenScale = scale;

	//Set window
	if (!window)
	{
		window = SDL_CreateWindow("Cave Story Engine",
		                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		                          createWidth, createHeight,
		                          0);

#ifdef USE_ICONS_SDL2
		// Set the window icon.
		// Note that we skip this on Windows since we do it natively.
		unsigned char *pixel_buffer;
		unsigned int width;
		unsigned int height;
		if (!lodepng_decode32(&pixel_buffer, &width, &height, binary_res_icon_mini_png_start, binary_res_icon_mini_png_end - binary_res_icon_mini_png_start))
		{
			SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(pixel_buffer, width, height, 0, width * 4, SDL_PIXELFORMAT_RGBA32);

			if (surface)
			{
				SDL_SetWindowIcon(window, surface);
				SDL_FreeSurface(surface);
			}

			free(pixel_buffer);
		}
#endif
	}
	else
		SDL_SetWindowSize(window, createWidth, createHeight);

	if (fullscreen)
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	//Set renderer
	if (!renderer)
	{
		renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		if (renderer == nullptr)
			logError((std::string)"Couldn't create renderer! SDL2 error: " + SDL_GetError());
	}

	// TODO free these when closing-down
	cursor_surface = loadPNGToSurface("data/Cursor/cursor_normal.png");
	if (cursor_surface)
	{
		cursor = SDL_CreateColorCursor(cursor_surface, 0, 0);	// Don't worry, the hotspots are accurate to the original files
		if (cursor)
			SDL_SetCursor(cursor);
        else
            logError("Couldn't create cursor");
	}
	else
        logError("Couldn't load cursor image (is the file missing ?)");

	return 0;
}

void switchScreenMode()
{
	windowFlags ^= SDL_WINDOW_FULLSCREEN_DESKTOP;

	//Unlike prevWidth and Height, this is used for fixing the view when going between fullscreen and windowed mode
	const int lastWidth = screenWidth;
	const int lastHeight = screenHeight;

	if (windowFlags & SDL_WINDOW_FULLSCREEN_DESKTOP)
	{
		SDL_DisplayMode dm;

		if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
			doError();

		prevWidth = screenWidth;
		prevHeight = screenHeight;
		prevScale = screenScale;

		screenWidth = (dm.w * 240) / dm.h;
		screenHeight = 240;
		screenScale = dm.h / 240;
	}
	else
	{
		screenWidth = prevWidth;
		screenHeight = prevHeight;
		screenScale = prevScale;
	}

	//Scale renderer to proper proportions
	SDL_RenderSetLogicalSize(renderer, screenWidth * screenScale, screenHeight * screenScale);

	//Ensure that the view is shifted properly
	viewport.x += (lastWidth - screenWidth) * 0x100;
	viewport.y += (lastHeight - screenHeight) * 0x100;

	//Set window properties
	SDL_SetWindowSize(window, screenWidth * screenScale, screenHeight * screenScale);
	SDL_SetWindowFullscreen(window, windowFlags);
}

bool drawWindow()
{
	while (true)
	{
		if (!handleEvents())
			return false;

		//Framerate limiter
		static uint32_t timePrev;
		const uint32_t timeNow = SDL_GetTicks();

		if (timeNow >= timePrev + framerate)
		{
			if (timeNow >= timePrev + 100)
				timePrev = timeNow;	// If the timer is freakishly out of sync, panic and reset it, instead of spamming frames for who-knows how long
			else
				timePrev += framerate;

			break;
		}

		SDL_Delay(1);
	}

	SDL_RenderPresent(renderer);

	return true;
}

void captureScreen(enum TextureNums texture_id)
{
	//Destroy previously existing texture and load new one
	if (sprites[texture_id] != nullptr)
		SDL_DestroyTexture(sprites[texture_id]);

	int width, height;
	SDL_GetRendererOutputSize(renderer, &width, &height);

	// The depth parameter here is unused. Be aware, it will be removed in SDL 2.1.
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(SDL_TEXTUREACCESS_TARGET, width, height, 0, SDL_PIXELFORMAT_RGB888);
	SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGB888, surface->pixels, surface->pitch);

	sprites[texture_id] = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);
}

void createTextureBuffer(enum TextureNums texture_id, int width, int height)
{
	//Destroy previously existing texture and load new one
	if (sprites[texture_id] != nullptr)
		SDL_DestroyTexture(sprites[texture_id]);

	sprites[texture_id] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, width, height);//SDL_CreateTextureFromSurface(renderer, surface);
}

//Texture and drawing stuff
void loadImage(const char *file, SDL_Texture **tex)
{
	if (tex == nullptr)
		doCustomError("tex was nullptr in loadImage");

	//Destroy previously existing texture and load new one
	if (*tex != nullptr)
	{
		SDL_DestroyTexture(*tex);
	}
	*tex = loadPNGToTexture(renderer, file);

	//Error if anything failed
	if (*tex == nullptr)
		doError();

	//Set to transparent, error if failed
	if (SDL_SetTextureBlendMode(*tex, SDL_BLENDMODE_BLEND) != 0)
		doError();
}

//loads images with limited colors
uint8_t colorValTbl[] = { 0, 52, 87, 116, 144, 172, 206, 255 };
void loadImageBad(const char *file, SDL_Texture **tex)
{
	SDL_Surface *surface;

	if (tex == nullptr)
		doCustomError("tex was nullptr in loadImage");

	//Destroy previously existing texture and load new one
	if (*tex != nullptr)
	{
		SDL_DestroyTexture(*tex);
	}
	surface = loadPNGToSurface(file);
	if (surface->format->palette != nullptr)
	{
		SDL_Color *colors = static_cast<SDL_Color*>(calloc(4, surface->format->palette->ncolors));
		for (int c = 0; c < surface->format->palette->ncolors; c++)
		{
			colors[c].r = colorValTbl[(surface->format->palette->colors[c].r * (sizeof(colorValTbl) - 1)) / 0xFF];
			colors[c].g = colorValTbl[(surface->format->palette->colors[c].g * (sizeof(colorValTbl) - 1)) / 0xFF];
			colors[c].b = colorValTbl[(surface->format->palette->colors[c].b * (sizeof(colorValTbl) - 1)) / 0xFF];
			colors[c].a = surface->format->palette->colors[c].a;
		}
		SDL_SetPaletteColors(surface->format->palette, colors, 0, surface->format->palette->ncolors);
		free(colors);
	}
	else
	{
		uint8_t *pixel = reinterpret_cast<uint8_t*>(surface->pixels);
		for (int p = 0; p < surface->w*surface->h; p++)
		{
			if (pixel[p])
				pixel[p] = colorValTbl[(pixel[p] * (sizeof(colorValTbl) - 1)) / 0xFF];
		}
	}
	*tex = SDL_CreateTextureFromSurface(renderer, surface);

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
		cliprect = { rect->left * screenScale, rect->top * screenScale, (rect->right - rect->left) * screenScale, (rect->bottom - rect->top) * screenScale };
		SDL_RenderSetClipRect(renderer, &cliprect);
	}
	else
	{
		SDL_RenderSetClipRect(renderer, nullptr);
	}
}

void drawTexture(SDL_Texture *texture, const RECT *rect, int x, int y)
{
	//Set framerect
	if (rect)
	{
		rcImage = { rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top };

		//Set drawrect, with defined width and height
		rcDraw.x = x * screenScale;
		rcDraw.y = y * screenScale;
		rcDraw.w = rcImage.w * screenScale;
		rcDraw.h = rcImage.h * screenScale;

		//Draw to screen, error if failed
		if (SDL_RenderCopy(renderer, texture, &rcImage, &rcDraw) != 0)
			doError();
	}
	else
	{
		int w, h;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

		rcDraw.x = x * screenScale;
		rcDraw.y = y * screenScale;
		rcDraw.w = w * screenScale;
		rcDraw.h = h * screenScale;

		if (SDL_RenderCopy(renderer, texture, nullptr, &rcDraw) != 0)
			doError();
	}
}

void drawTextureNoScale(SDL_Texture *texture, const RECT *rect, int x, int y)
{
	//Set framerect
	if (rect)
	{
		rcImage = { rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top };

		//Set drawrect, with defined width and height
		rcDraw.x = x;
		rcDraw.y = y;
		rcDraw.w = rcImage.w;
		rcDraw.h = rcImage.h;

		//Draw to screen, error if failed
		if (SDL_RenderCopy(renderer, texture, &rcImage, &rcDraw) != 0)
			doError();
	}
	else
	{
		int w, h;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

		rcDraw.x = x;
		rcDraw.y = y;
		rcDraw.w = w;
		rcDraw.h = h;

		if (SDL_RenderCopy(renderer, texture, nullptr, &rcDraw) != 0)
			doError();
	}
}

void drawTextureSize(SDL_Texture *texture, const RECT *rect, int x, int y, int w, int h)
{
	//Set framerect
	rcImage = { rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top };

	//Set drawrect, with defined width and height
	rcDraw.x = x * screenScale;
	rcDraw.y = y * screenScale;
	rcDraw.w = w * screenScale;
	rcDraw.h = h * screenScale;

	//Draw to screen, error if failed
	if (SDL_RenderCopy(renderer, texture, &rcImage, &rcDraw) != 0)
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
			drawTexture(sprites[26], &numbRect, x + (offset << 3), y);
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

void drawString(int x, int y, const char *str, const uint8_t *flag)
{
	if (str == nullptr)
		doCustomError("str was nullptr in drawString");

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
				drawTexture(sprites[TEX_TEXTBOX], &rcChar, x + (i * sep), y + 2);
			}
			else
			{
				//Set framerect to what it's supposed to be
				const int drawIndex = i;

				if (isMultibyte(str[i]))
				{
					const int localChar = 0x81 + str[i + 1] + ((str[i] - 0x81) * 0x100);
					rcChar.left = ((localChar % 32) * charWidth);
					rcChar.top = ((localChar >> 5) * charHeight);
					rcChar.right = rcChar.left + charWidth;
					rcChar.bottom = rcChar.top + charHeight;
					i++;
				}
				else
				{
					rcChar.left = ((str[i] % 32) * charWidth);
					rcChar.top = ((str[i] >> 5) * charHeight);
					rcChar.right = rcChar.left + charWidth;
					rcChar.bottom = rcChar.top + charHeight;
				}

				//Draw to the screen
				drawTextureSize(sprites[0x26], &rcChar, x + (drawIndex * sep), y, charWidth / charScale, charHeight / charScale);
			}
		}
		else
			break;
	}
}

void drawRect(int x, int y, int w, int h)
{
	//Map this onto an SDL_Rect
	drawRectangle.x = x * screenScale;
	drawRectangle.y = y * screenScale;
	drawRectangle.w = w * screenScale;
	drawRectangle.h = h * screenScale;

	//Render onto the screen
	SDL_RenderFillRect(renderer, &drawRectangle);
}
