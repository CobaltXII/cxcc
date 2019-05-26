#pragma once
#include <string>

#include "literals.hpp"
#include "operators.hpp"

struct expression_t;

// All expression types.
enum expression_type_t {
	et_integer_literal,
	et_string_literal,
	et_character_literal,
	et_identifier,
	et_indexing,
	et_function_call,
	et_binary,
	et_unary
};

// An indexing expression.
struct indexing_expression_t {
	identifier_t array;
	expression_t* index;
};

// A function call expression.
struct function_call_expression_t {
	identifier_t function;
	std::vector<expression_t*> arguments;
};

// A binary expression.
struct binary_expression_t {
	expression_t* left_operand;
	expression_t* right_operand;
	binary_operator_t binary_operator;
};

// A unary expression.
struct unary_expression_t {
	expression_t* operand;
	unary_operator_t unary_operator;
};

// An expression.
struct expression_t {
	expression_type_t			type;
	type_t						return_type;

	long						lineno;
	long						colno;

	string_t					string_literal;
	long						string_label;

	integer_t					integer_literal;
	character_t					character_literal;
	identifier_t				identifier;
	indexing_expression_t		indexing;
	function_call_expression_t	function_call;
	binary_expression_t			binary;
	unary_expression_t			unary;

	expression_t(std::string literal, std::string disambiguation, long lineno, long colno) {
		if (disambiguation == "int") {
			type = et_integer_literal;
			integer_literal = literal;
		} else if (disambiguation == "str") {
			type = et_string_literal;
			string_literal = literal;
		} else if (disambiguation == "chr") {
			type = et_character_literal;
			character_literal = literal;
		} else if (disambiguation == "id") {
			type = et_identifier;
			identifier = literal;
		}
		this->lineno = lineno;
		this->colno = colno;
	}

	expression_t(indexing_expression_t expr, long lineno, long colno) {
		type = et_indexing;
		indexing = expr;
		this->lineno = lineno;
		this->colno = colno;
	}

	expression_t(function_call_expression_t expr, long lineno, long colno) {
		type = et_function_call;
		function_call = expr;
		this->lineno = lineno;
		this->colno = colno;
	}

	expression_t(binary_expression_t expr, long lineno, long colno) {
		type = et_binary;
		binary = expr;
		this->lineno = lineno;
		this->colno = colno;
	}

	expression_t(unary_expression_t expr, long lineno, long colno) {
		type = et_unary;
		unary = expr;
		this->lineno = lineno;
		this->colno = colno;
	}
};