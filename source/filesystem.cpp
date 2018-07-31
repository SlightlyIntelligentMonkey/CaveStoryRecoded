#include "filesystem.h"

//Read function stuff
uint16_t readLEshort(BYTE *data, unsigned int offset) {
	return ((data[offset + 1] << 8) + data[offset]);
}

uint32_t readLElong(BYTE *data, unsigned int offset) {
	return ((data[offset + 3] << 24) + (data[offset + 2] << 16) + (data[offset + 1] << 8) + data[offset]);
}

//Write stuff
void writeLEshort(BYTE *data, uint16_t input, unsigned int offset) {
	data[offset] = (BYTE)input;
	data[offset + 1] = (BYTE)(input >> 8);
}

void writeLElong(BYTE *data, uint32_t input, unsigned int offset) {
	data[offset] = (BYTE)input;
	data[offset + 1] = (BYTE)(input >> 8);
	data[offset + 2] = (BYTE)(input >> 16);
	data[offset + 3] = (BYTE)(input >> 24);
}

//Loading and writing functions
bool fileExists(const char *name)
{
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}

int loadFile(const char *name, BYTE **data) {
	int filesize = 0;

	//Open file
	FILE *file = fopen(name, "rb");
	if (file == nullptr)
		return -1;

	//Get filesize
	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	fseek(file, 0, 0);

	//Load data
	*data = (BYTE*)malloc(filesize);
	if (fread(*data, 1, filesize, file) == 0) 
	{
		fclose(file);
		return -1;
	}

	//Close file
	fclose(file);
	
	return filesize;
}

int writeFile(char *name, void *data, int amount)
{
	FILE *file = fopen(name, "wb");
	if (file == nullptr) 
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
	SDL_RWops *profile = SDL_RWFromFile(profileName, "rb");

	if (profile == nullptr)
		return;

	uint64_t code = SDL_ReadLE64(profile); //Code

	int level = SDL_ReadLE32(profile); //level
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
	
	//Flags
	SDL_RWseek(profile, 0x21C, 0);

	for (Sint64 i = 0; i < 1000; i++)
		SDL_RWread(profile, &tscFlags[i], 1, 1);

	//Now load level
	loadLevel(level);
}

void saveProfile() {
	BYTE *profile = (BYTE*)malloc(0x604);

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

	memcpy(profile + 0x218, "FLAG", 4);

	for (Sint64 i = 0; i < 1000; i++)
		profile[0x21C + i] = tscFlags[i];

	//Save to file
	writeFile((char*)profileName, profile, 0x604);

	//End
	free(profile);
}
