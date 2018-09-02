#pragma once
#include "common.h"
#include <vector>
#include <string>

uint16_t readLEshort(const uint8_t *data, size_t offset) attrPure;
uint32_t readLElong(const uint8_t *data, size_t offset) attrPure;

void writeLEshort(uint8_t *data, uint16_t input, size_t offset);
void writeLElong(uint8_t *data, uint32_t input, size_t offset);

/// Returns whether the given filename points to a valid file
bool fileExists(const std::string& name) noexcept;

int loadFile(const char *name, uint8_t **data);
int writeFile(const char *name, const void *data, size_t amount) noexcept;

void loadProfile();
void saveProfile();

std::vector<std::string> getLinesFromFile(const std::string& fileName);
