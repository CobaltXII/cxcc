#pragma once
#include <string>
#include <iostream>

// All available colors.
enum ansi_color_t {
	black,
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	white,
	bold_black,
	bold_red,
	bold_green,
	bold_yellow,
	bold_blue,
	bold_magenta,
	bold_cyan,
	bold_white,
	reset
};

// All available color escape codes.
std::string ansi_color_code[] = {
	"\u001b[30m",
	"\u001b[31m",
	"\u001b[32m",
	"\u001b[33m",
	"\u001b[34m",
	"\u001b[35m",
	"\u001b[36m",
	"\u001b[37m",
	"\u001b[30;1m",
	"\u001b[31;1m",
	"\u001b[32;1m",
	"\u001b[33;1m",
	"\u001b[34;1m",
	"\u001b[35;1m",
	"\u001b[36;1m",
	"\u001b[37;1m",
	"\u001b[0m"
};

// Set the terminal color.
std::string set_color(ansi_color_t color) {
	std::cerr << ansi_color_code[color];
	return "";
}