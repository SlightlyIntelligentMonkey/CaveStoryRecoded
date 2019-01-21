#pragma once

#include <memory>
#include <cstddef>

struct CONFIG
{
	char proof[32];
	char font_name[64];
	int32_t move_button_mode;
	int32_t attack_button_mode;
	int32_t ok_button_mode;
	int32_t display_mode;
	int32_t bJoystick;
	int32_t joystick_button[8];
};

std::unique_ptr<CONFIG> loadConfigDat();

void loadConfigFiles();
