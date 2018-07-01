#include "filesystem.h"

uint16_t readLEshort(BYTE *data, unsigned int offset) {
	return (data[offset + 1] * 0x100 + data[offset]);
}

uint32_t readLElong(BYTE *data, unsigned int offset) {
	return (data[offset + 3] * 0x1000000 + data[offset + 2] * 0x10000 + data[offset + 1] * 0x100 + data[offset]);
}

int loadFile(const char *name, BYTE **data) {
	int amount = 0;
	FILE *file = nullptr;
	file = fopen(name, "rb");
	if (file == nullptr) { return -1; }
	fseek(file, 0, SEEK_END);
	amount = ftell(file);
	fseek(file, 0, 0);
	*data = (BYTE*)malloc(amount);
	if (fread(*data, 1, amount, file) == 0) { return -1; }
	fclose(file);
	return amount;
}
