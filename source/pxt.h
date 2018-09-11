#pragma once

#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

void makeWaveTables();
int loadSound(const char *path, uint8_t **buf, size_t *length);
