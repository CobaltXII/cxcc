#pragma once
#include <string>

#include "../util/char_stream.hpp"
#include "token.hpp"

// Debugging information for tokens. This includes the line number and the
// column number.
#define TOKEN_DEBUG input.lineno, input.colno

// A token stream.
struct token_stream_t {
	std::string filename;
	char_stream_t input;

	// Default constructor.
	token_stream_t(std::string filename = "", std::string buffer = "") {
		this->filename = filename;
		input = char_stream_t(filename, buffer);
	}

	// Checks if a character is a digit.
	static bool chr_digit(int ch) {
		return ch >= '0' && ch <= '9';
	}

	// Checks if a character can be the first character in an identifier.
	static bool chr_id_start(int ch) {
		return (ch >= 'a' && ch <= 'z') ||
			   (ch >= 'A' && ch <= 'Z') ||
			   ch == '_';
	}

	// Checks if a character can be used anywhere in an identifier besides the
	// first character. Any value x where chr_id_start(x) == true must also
	// satisfy chr_id(x) == true.
	static bool chr_id(int ch) {
		return (ch >= 'a' && ch <= 'z') ||
			   (ch >= 'A' && ch <= 'Z') ||
			   (ch >= '0' && ch <= '9') ||
			   ch == '_';
	}

	// Checks is a character is whitespace.
	static bool chr_whitespace(int ch) {
		return ch == ' ' || ch == '\t' || ch == '\n';
	}

	// Reads from the character stream until the predicate returns false or
	// the end-of-file has been reached.
	std::string read_while(bool (*predicate)(int)) {
		std::string str = "";
		while (!input.eof() && predicate(input.peek())) {
			str += input.next();
		}
		return str;
	}

	// Reads an identifier.
	token_t read_identifier() {
		std::string str = read_while(chr_id);
		if (str == "if") {
			return {tk_if, str, TOKEN_DEBUG};
		} else if (str == "int") {
			return {tk_int, str, TOKEN_DEBUG};
		} else if (str == "else") {
			return {tk_else, str, TOKEN_DEBUG};
		} else if (str == "while") {
			return {tk_while, str, TOKEN_DEBUG};
		} else if (str == "return") {
			return {tk_return, str, TOKEN_DEBUG};
		} else if (str == "break") {
			return {tk_break, str, TOKEN_DEBUG};
		} else if (str == "continue") {
			return {tk_continue, str, TOKEN_DEBUG};
		} else {
			return {tk_identifier, str, TOKEN_DEBUG};
		}
	}

	// Reads an integer literal.
	token_t read_lit_integer() {
		return {
			tk_lit_integer,
			read_while(chr_digit),
			TOKEN_DEBUG
		};
	}

	// Reads an escaped string/character literal. The start and end quotes are
	// not included in the token text.
	std::string read_escaped(int quote) {
		bool escaped = false;
		std::string str = "";
		input.next();
		while (!input.eof()) {
			int ch = input.next();
			if (ch == '\n') {
				break;
			} else if (escaped) {
				str += ch;
				escaped = false;
			} else if (ch == '\\') {
				str += ch;
				escaped = true;
			} else if (ch == quote) {
				break;
			} else {
				str += ch;
			}
		}
		return str;
	}

	// Reads a string literal.
	token_t read_lit_string() {
		return {
			tk_lit_string,
			read_escaped('"'),
			TOKEN_DEBUG
		};
	}

	// Reads a character literal.
	token_t read_lit_character() {
		return {
			tk_lit_character,
			read_escaped('\''),
			TOKEN_DEBUG
		};
	}

	// Skips whitespace.
	void skip_whitespace() {
		read_while(chr_whitespace);
	}

	// Checks if a character is not a newline character.
	static bool chr_newline(int ch) {
		return ch != '\n';
	}

	// Skips comments.
	void skip_comment() {
		read_while(chr_newline);
		input.next();
	}

	// Reads the next token.
	token_t next() {
		skip_whitespace();
		
		if (input.eof()) {
			return {tk_eof, "", TOKEN_DEBUG};
		}

		int ch = input.peek();

		// Skip comments.
		if (ch == '/' && input.peek_two() == '/') {
			skip_comment();
			return next();
		}

		// Check for identifiers.
		if (chr_id_start(ch)) {
			return read_identifier();
		}
		// Check for integer literals.
		else if (chr_digit(ch)) {
			return read_lit_integer();
		}
		// Check for string literals.
		else if (ch == '"') {
			return read_lit_string();
		}
		// Check for character literals.
		else if (ch == '\'') {
			return read_lit_character();
		}

		// Check for left parentheses.
		else if (ch == '(') {
			input.next();
			return {tk_left_parenthesis, "(", TOKEN_DEBUG};
		}
		// Check for right parentheses.
		else if (ch == ')') {
			input.next();
			return {tk_right_parenthesis, ")", TOKEN_DEBUG};
		}
		// Check for left brackets.
		else if (ch == '[') {
			input.next();
			return {tk_left_bracket, "[", TOKEN_DEBUG};
		}
		// Check for right brackets.
		else if (ch == ']') {
			input.next();
			return {tk_right_bracket, "]", TOKEN_DEBUG};
		}
		// Check for left braces.
		else if (ch == '{') {
			input.next();
			return {tk_left_brace, "{", TOKEN_DEBUG};
		}
		// Check for right braces.
		else if (ch == '}') {
			input.next();
			return {tk_right_brace, "}", TOKEN_DEBUG};
		}
		// Check for commas.
		else if (ch == ',') {
			input.next();
			return {tk_comma, ",", TOKEN_DEBUG};
		}
		// Check for semicolons.
		else if (ch == ';') {
			input.next();
			return {tk_semicolon, ";", TOKEN_DEBUG};
		}

		// Check for the ambiguous plus (+) operator.
		else if (ch == '+') {
			input.next();
			return {tk_plus, "+", TOKEN_DEBUG};
		}
		// Check for the ambiguous minus (-) operator.
		else if (ch == '-') {
			input.next();
			return {tk_minus, "-", TOKEN_DEBUG};
		}
		// Check for the ambiguous asterisk (*) operator.
		else if (ch == '*') {
			input.next();
			return {tk_asterisk, "*", TOKEN_DEBUG};
		}

		// Check for binary division.
		else if (ch == '/') {
			input.next();
			return {tk_bi_division, "/", TOKEN_DEBUG};
		}
		// Check for binary modulo.
		else if (ch == '%') {
			input.next();
			return {tk_bi_modulo, "%", TOKEN_DEBUG};
		}
		// Check for binary assignment and relational equals.
		else if (ch == '=') {
			input.next();
			if (input.eof()) {
				return {tk_bi_assignment, "=", TOKEN_DEBUG};
			} else if (input.peek() == '=') {
				input.next();
				return {tk_bi_relational_equal, "==", TOKEN_DEBUG};
			} else {
				return {tk_bi_assignment, "=", TOKEN_DEBUG};
			}
		}
		// Check for binary logical AND and unary address-of.
		else if (ch == '&') {
			input.next();
			if (input.eof()) {
				return {tk_un_address_of, "&", TOKEN_DEBUG};
			} else if (input.peek() == '&') {
				input.next();
				return {tk_bi_logical_and, "&&", TOKEN_DEBUG};
			} else {
				return {tk_un_address_of, "&", TOKEN_DEBUG};
			}
		}
		// Check for binary logical OR.
		else if (ch == '|') {
			input.next();
			if (input.eof()) {
				input.die("expected '|'");
				return {};
			}
			int ch = input.next();
			if (ch == '|') {
				return {tk_bi_logical_or, "||", TOKEN_DEBUG};
			} else {
				input.die("expected '|'");
				return {};
			}
		}
		// Check for binary relational non-equal and unary logical NOT.
		else if (ch == '!') {
			input.next();
			if (input.eof()) {
				return {tk_un_logical_not, "!", TOKEN_DEBUG};
			} else if (input.peek() == '=') {
				input.next();
				return {tk_bi_relational_non_equal, "!=", TOKEN_DEBUG};
			} else {
				return {tk_un_logical_not, "!", TOKEN_DEBUG};
			}
		}
		// Check for binary relational greater-than.
		else if (ch == '>') {
			input.next();
			return {tk_bi_relational_greater_than, ">", TOKEN_DEBUG};
		}
		// Check for binary relational lesser-than.
		else if (ch == '<') {
			input.next();
			return {tk_bi_relational_lesser_than, "<", TOKEN_DEBUG};
		}

		// Encountered an unexpected character.
		input.die("unexpected character");
		return {};
	}
};

#undef TOKEN_DEBUG