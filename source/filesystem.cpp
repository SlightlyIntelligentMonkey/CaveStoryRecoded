#include "filesystem.h"
#include "fade.h"

//Read function stuff
uint16_t readLEshort(const BYTE * data, size_t offset) {
	return ((data[offset + 1] << 8) + data[offset]);
}

uint32_t readLElong(const BYTE * data, size_t offset) {
	return ((data[offset + 3] << 24) + (data[offset + 2] << 16) + (data[offset + 1] << 8) + data[offset]);
}

//Write stuff
void writeLEshort(BYTE *data, uint16_t input, size_t offset) {
	data[offset] = static_cast<BYTE>(input);
	data[offset + 1] = static_cast<BYTE>(input >> 8);
}

void writeLElong(BYTE *data, uint32_t input, size_t offset) {
	data[offset] = static_cast<BYTE>(input);
	data[offset + 1] = static_cast<BYTE>(input >> 8);
	data[offset + 2] = static_cast<BYTE>(input >> 16);
	data[offset + 3] = static_cast<BYTE>(input >> 24);
}

//Loading and writing functions
bool fileExists(const char *name)
{
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}

int loadFile(const char *name, BYTE **data) {
	size_t filesize = 0;

	//Open file
	FILE *file = fopen(name, "rb");
	if (file == nullptr)
		return -1;

	//Get filesize
	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	fseek(file, 0, 0);

	//Load data
	*data = static_cast<BYTE *>(malloc(filesize));
	if (fread(*data, 1, filesize, file) == 0) 
	{
		fclose(file);
		return -1;
	}

	//Close file
	fclose(file);
	
	return filesize;
}

int writeFile(const char *name, void *data, size_t amount)
{
	FILE *file;
	if ((file = fopen(name, "wb")) == NULL)
		return -1;

	if (fwrite(data, 1, amount, file) == 0) 
	{
		fclose(file);
		return -1;
	}

	fclose(file);
	return 0;
}

//Profile code
const char *profileName = "Profile.dat";
const char *profileCode = "Do041220";

void loadProfile()
{
	if (fileExists(profileName))
	{
		SDL_RWops *profile = SDL_RWFromFile(profileName, "rb");

		if (profile == nullptr)
			return;

		const uint64_t code = SDL_ReadLE64(profile); //Code
		if (memcmp(&code, "Do041220", sizeof(code)) != 0)
			doCustomError("Invalid profile (first 8 bytes aren't \"Do041120\"");

		const int level = SDL_ReadLE32(profile); //level
		SDL_ReadLE32(profile); //song

		currentPlayer.init();

		currentPlayer.x = SDL_ReadLE32(profile); //Player X
		currentPlayer.y = SDL_ReadLE32(profile); //Player Y
		currentPlayer.direct = SDL_ReadLE32(profile); //Player Direction

		currentPlayer.max_life = SDL_ReadLE16(profile); //max health
		currentPlayer.star = SDL_ReadLE16(profile); //whimsical star
		currentPlayer.life = SDL_ReadLE16(profile); //health

		SDL_ReadLE16(profile); // a?

		SDL_ReadLE32(profile); //current weapon
		SDL_ReadLE32(profile); //current item

		currentPlayer.equip = SDL_ReadLE32(profile); //equipped items
		currentPlayer.unit = SDL_ReadLE32(profile); //physics

		SDL_RWseek(profile, 0x158, 0);

		for (auto permitStageIterator : permitStage)
		{
			permitStageIterator.index = SDL_ReadLE32(profile);
			permitStageIterator.event = SDL_ReadLE32(profile);
		}

		for (auto mapFlagsIterator : mapFlags)
			SDL_RWread(profile, &mapFlagsIterator, 1, 1);

		SDL_ReadLE32(profile); //FLAG

		for (auto tscFlagsIterator : tscFlags)
			SDL_RWread(profile, &tscFlagsIterator, 1, 1);

		//Now load level
		loadLevel(level);
		startTscEvent(0);
		initFade();

		//Close RW
		SDL_RWclose(profile);
	}
	else
	{
		initGame();
	}
}

void saveProfile() {
	auto profile = static_cast<BYTE *>(malloc(0x604));

	if (profile == nullptr)
		doCustomError("Could not allocate memory for profile");

	//Set data
	memset(profile, 0, 0x604);
	memcpy(profile, profileCode, 0x08);

	writeLElong(profile, currentLevel, 0x08); //Level

	writeLElong(profile, currentPlayer.x, 0x10); //Player X
	writeLElong(profile, currentPlayer.y, 0x14); //Player Y
	writeLElong(profile, currentPlayer.direct, 0x18); //Player direction

	writeLEshort(profile, currentPlayer.max_life, 0x1C); //Player max health
	writeLEshort(profile, currentPlayer.star, 0x1E); //Whimsical star
	writeLEshort(profile, currentPlayer.life, 0x20); //Player health
	
	writeLElong(profile, currentPlayer.equip, 0x2C); //Equipped items
	writeLElong(profile, currentPlayer.unit, 0x30); //Current physics

	for (size_t i = 0; i < 8; i++)
	{
		writeLElong(profile, permitStage[i].index, 0x158 + i * 8);
		writeLElong(profile, permitStage[i].event, 0x15C + i * 8);
	}

	memcpy(profile + 0x198, mapFlags, 0x80);
	memcpy(profile + 0x218, "FLAG", 4);
	memcpy(profile + 0x21C, tscFlags, 1000);

	//Save to file
	writeFile(profileName, profile, 0x604);

	//End
	free(profile);
}
