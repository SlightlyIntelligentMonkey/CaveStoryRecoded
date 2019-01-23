#pragma once

#include <vector>
#include <string>
#include <memory>
#include <utility>
#include "common.h"

uint16_t readLEshort(const uint8_t *data, size_t offset) attrPure;
uint32_t readLElong(const uint8_t *data, size_t offset) attrPure;

void writeLEshort(uint8_t *data, uint16_t input, size_t offset);
void writeLElong(uint8_t *data, uint32_t input, size_t offset);

/// Returns whether the given filename points to a valid file
bool fileExists(const std::string& name);

std::pair<size_t, std::vector<uint8_t>> loadFile(const std::string& name);
void writeFile(const std::string& name, const void *data, size_t amount);

void loadProfile();
void saveProfile();

std::vector<std::string> getLinesFromFile(const std::string& fileName);

extern std::string gProfileName;
extern std::string gProfileCode;
