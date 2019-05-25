#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "../util/ansi_colors.hpp"
#include "token_stream.hpp"

// A completed token stream.
struct complete_token_stream_t {
	std::string filename;
	token_stream_t input;
	std::vector<token_t> tokens;
	long cursor = 0;

	// Default constructor.
	complete_token_stream_t(std::string filename = "", token_stream_t buffer = token_stream_t("")) {
		this->filename = filename;
		input = buffer;
		token_t token;
		while ((token = input.next()).type != tk_eof) {
			tokens.push_back(token);
		}
		tokens.push_back(token);
	}

	// Get the next token in the stream and increment the cursor.
	token_t next() {
		return tokens[cursor++];
	}

	// Peek the next token in the stream.
	token_t peek() {
		return tokens[cursor];
	}

	// Check if the end-of-file has been reached.
	bool eof() {
		return tokens[cursor].type == tk_eof;
	}

	// Print an error message, then exit.
	void die(std::string error, token_t token) {
		std::cerr << set_color(bold_white) << filename << ":";
		std::cerr << token.lineno + 1 << ":" << token.colno + 1 - token.text.length() << ": ";
		std::cerr << set_color(bold_red) << "error: ";
		std::cerr << set_color(bold_white) << error << set_color(reset) << std::endl;
		// Print the line where the error occurred.
		std::stringstream in(input.input.buffer);
		std::string line;
		for (int i = 0; i < token.lineno; i++) {
			std::getline(in, line);
		}
		std::getline(in, line);
		std::cerr << line << std::endl;
		// Print an indicator pointing to the column where the error occurred.
		for (int i = 0; i < token.colno - token.text.length(); i++) {
			if (line[i] == '\t') {
				std::cerr << '\t';
			} else {
				std::cerr << ' ';
			}
		}
		std::cerr << set_color(bold_green) << '^' << set_color(reset) << std::endl;
		exit(2);
	}
};