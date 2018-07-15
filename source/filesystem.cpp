#include "filesystem.h"

uint16_t readLEshort(BYTE *data, unsigned int offset) {
	return ((data[offset + 1] << 8) + data[offset]);
}

uint32_t readLElong(BYTE *data, unsigned int offset) {
	return ((data[offset + 3] << 24) + (data[offset + 2] << 16) + (data[offset + 1] << 8) + data[offset]);
}

int loadFile(const char *name, BYTE **data) {
	int filesize = 0;

	//Open file
	FILE *file = nullptr;
	file = fopen(name, "rb");
	if (file == nullptr) { return -1; }

	//Get filesize
	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	fseek(file, 0, 0);

	//Load data
	*data = (BYTE*)malloc(filesize);
	if (fread(*data, 1, filesize, file) == 0) { return -1; }

	//Close file
	fclose(file);
	
	return filesize;
}
