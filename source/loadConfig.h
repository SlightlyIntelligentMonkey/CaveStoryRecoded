#pragma once

struct CONFIG
{
	char proof[32];
	char font_name[64];
	Sint32 move_button_mode;
	Sint32 attack_button_mode;
	Sint32 ok_button_mode;
	Sint32 display_mode;
	Sint32 bJoystick;
	Sint32 joystick_button[8];
};
CONFIG *loadConfigdat();

void loadConfigFiles();