#pragma once
#include <string>
#include <vector>

// All token types.
enum token_type_t {
	// Special tokens.
	tk_identifier,
	tk_lit_integer,
	tk_lit_string,
	tk_lit_character,
	tk_eof,
	// Reserved words.
	tk_if,
	tk_int,
	tk_else,
	tk_while,
	tk_return,
	tk_break,
	tk_continue,
	// Punctuation.
	tk_left_parenthesis,
	tk_right_parenthesis,
	tk_left_bracket,
	tk_right_bracket,
	tk_left_brace,
	tk_right_brace,
	tk_comma,
	tk_semicolon,
	// Binary operators.
	tk_bi_division,
	tk_bi_modulo,
	tk_bi_assignment,
	tk_bi_addition_assignment,
	tk_bi_subtraction_assignment,
	tk_bi_multiplication_assignment,
	tk_bi_division_assignment,
	tk_bi_modulo_assignment,
	tk_bi_logical_and,
	tk_bi_logical_or,
	tk_bi_relational_equal,
	tk_bi_relational_non_equal,
	tk_bi_relational_greater_than,
	tk_bi_relational_lesser_than,
	tk_bi_relational_greater_than_or_equal_to,
	tk_bi_relational_lesser_than_or_equal_to,
	tk_bi_binary_or,
	tk_bi_binary_xor,
	tk_bi_binary_and_assignment,
	tk_bi_binary_or_assignment,
	tk_bi_binary_xor_assignment,
	// Unary operators.
	tk_un_logical_not,
	// Ambiguous operators.
	tk_plus,
	tk_minus,
	tk_asterisk,
	tk_ampersand
};

// All token types as strings.
std::string token_type_str[] = {
	// Special tokens.
	"identifier",
	"integer literal",
	"string literal",
	"character literal",
	"end-of-file",
	// Reserved words.
	"'if'",
	"'int'",
	"'else'",
	"'while'",
	"'return'",
	"'break'",
	"'continue'",
	// Punctuation.
	"'('",
	"')'",
	"'['",
	"']'",
	"'{'",
	"'}'",
	"','",
	"';'",
	// Binary operators.
	"'/'",
	"'%'",
	"'='",
	"'+='",
	"'-='",
	"'*='",
	"'/='",
	"'%='",
	"'&&'",
	"'||'",
	"'=='",
	"'!='",
	"'>'",
	"'<'",
	"'>='",
	"'<='",
	"'|'",
	"'^'",
	"'&='",
	"'|='",
	"'^='",
	// Unary operators.
	"'!'",
	// Ambiguous operators.
	"'+'",
	"'-'",
	"'*'",
	"'&'"
};

// All token types as strings, padded.
std::vector<std::string> make_token_type_str_pad() {
	std::vector<std::string> token_type_str_pad;
	long max_length = 0;
	for (int i = 0; i < sizeof(token_type_str) / sizeof(token_type_str[0]); i++) {
		if (token_type_str[i].length() > max_length) {
			max_length = token_type_str[i].length();
		}
	}
	for (int i = 0; i < sizeof(token_type_str) / sizeof(token_type_str[0]); i++) {
		std::string padding(max_length - token_type_str[i].length(), ' ');
		token_type_str_pad.push_back(token_type_str[i] + padding);
	}
	return token_type_str_pad;
}

// All token types as strings, padded.
std::vector<std::string> token_type_str_pad = make_token_type_str_pad();

// A token.
struct token_t {
	token_type_t type;
	std::string text;
	long lineno;
	long colno;
};