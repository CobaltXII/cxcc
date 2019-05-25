#pragma once
#include <string>
#include <sstream>
#include <iostream>

#include "ansi_colors.hpp"

// A character stream.
struct char_stream_t {
	std::string filename;
	std::string buffer;
	long cursor = 0;
	long lineno = 0;
	long colno = 0;

	// Default constructor.
	char_stream_t(std::string filename = "", std::string buffer = "") {
		this->filename = filename;
		this->buffer = buffer;
	}

	// Get the next character in the stream and increment the cursor, line
	// number and column number.
	int next() {
		int ch = buffer[cursor++];
		if (ch == '\n') {
			lineno++;
			colno = 0;
		} else {
			colno++;
		}
		return ch;
	}

	// Peek the next character in the stream.
	int peek() {
		return buffer[cursor];
	}

	// Peek the character after the next character in the stream.
	int peek_two() {
		return buffer[cursor + 1];
	}

	// Check if the end-of-file has been reached.
	bool eof() {
		return cursor >= buffer.size();
	}

	// Print an error message along with the current line number and character
	// number of the character stream, then exit.
	void die(std::string error) {
		std::cerr << set_color(bold_white) << filename << ":";
		std::cerr << lineno + 1 << ":" << colno + 1 << ": ";
		std::cerr << set_color(bold_red) << "error: ";
		std::cerr << set_color(bold_white) << error << set_color(reset) << std::endl;
		// Print the line where the error occurred.
		std::stringstream in(buffer);
		std::string line;
		for (int i = 0; i < lineno; i++) {
			std::getline(in, line);
		}
		std::getline(in, line);
		std::cerr << line << std::endl;
		// Print an indicator pointing to the column where the error occurred.
		for (int i = 0; i < colno; i++) {
			if (line[i] == '\t') {
				std::cerr << '\t';
			} else {
				std::cerr << ' ';
			}
		}
		std::cerr << set_color(bold_green) << '^' << set_color(reset) << std::endl;
		exit(1);
	}
};