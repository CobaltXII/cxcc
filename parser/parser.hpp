#pragma once
#include <string>
#include <vector>
#include <algorithm>

#include "../lexer/complete_token_stream.hpp"

// An identifier.
typedef std::string identifier_t;

#include "type.hpp"
#include "parameter.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "function.hpp"

// A program.
typedef std::vector<function_t> program_t;

// A parser.
struct parser_t {
	std::string filename;
	complete_token_stream_t input;

	// Default constructor.
	parser_t(std::string filename, std::string buffer) {
		this->filename = filename;
		input = complete_token_stream_t(filename, token_stream_t(filename, buffer));
	}

	// Print an error message, then exit.
	void die(std::string error) {
		token_t peek = input.peek();
		input.die(error, peek);
	}

	// Expect a certain type of token. If the next token is not of the
	// specified type, the program will die.
	token_t expect(token_type_t type) {
		token_t peek = input.peek();
		if (peek.type != type) {
			input.die("expected " + token_type_str[type] + ", encountered " + token_type_str[peek.type] + " instead", peek);
		}
		return input.next();
	}

	// Parse a type.
	type_t parse_type() {
		expect(tk_int);
		long pointer_depth = 0;
		while (input.peek().type == tk_asterisk) {
			input.next();
			pointer_depth++;
		}
		return {pointer_depth};
	}

	// Parse an identifier.
	identifier_t parse_identifier() {
		return expect(tk_identifier).text;
	}

	// Parse a parameter.
	parameter_t parse_parameter() {
		return {parse_type(), parse_identifier()};
	}

	// Parse a parameter array.
	std::vector<parameter_t> parse_parameters() {
		std::vector<parameter_t> parameters;
		expect(tk_left_parenthesis);
		while (input.peek().type != tk_right_parenthesis) {
			parameters.push_back(parse_parameter());
			if (input.peek().type != tk_right_parenthesis) {
				expect(tk_comma);
			}
		}
		expect(tk_right_parenthesis);
		return parameters;
	}

	// Parse a literal without a suffix (either an indexing suffix or a
	// post-increment/decrement suffix).
	expression_t* parse_literal_no_suffix() {
		token_t peek = input.peek();
		int lineno = peek.lineno;
		int colno = peek.colno;
		#define EXPRESSION_DEBUG lineno, colno - peek.text.length() - 1
		if (peek.type == tk_lit_integer) {
			return new expression_t(expect(tk_lit_integer).text, "int", EXPRESSION_DEBUG);
		} else if (peek.type == tk_lit_string) {
			return new expression_t(expect(tk_lit_string).text, "str", EXPRESSION_DEBUG - 2);
		} else if (peek.type == tk_lit_character) {
			return new expression_t(expect(tk_lit_character).text, "chr", EXPRESSION_DEBUG - 2);
		} else if (peek.type == tk_identifier) {
			identifier_t identifier = parse_identifier();
			peek = input.peek();
			if (peek.type == tk_left_parenthesis) {
				expect(tk_left_parenthesis);
				std::vector<expression_t*> parameters;
				while (input.peek().type != tk_right_parenthesis) {
					parameters.push_back(parse_expression());
					if (input.peek().type != tk_right_parenthesis) {
						expect(tk_comma);
					}
				}
				expect(tk_right_parenthesis);
				return new expression_t({identifier, parameters}, EXPRESSION_DEBUG);
			} else {
				return new expression_t(identifier, "id", EXPRESSION_DEBUG);
			}
		} else if (peek.type == tk_left_parenthesis) {
			expect(tk_left_parenthesis);
			expression_t* subexpression = parse_expression();
			expect(tk_right_parenthesis);
			return subexpression;
		} else if (peek.type == tk_asterisk) {
			expect(tk_asterisk);
			return new expression_t({parse_literal(), un_value_of}, EXPRESSION_DEBUG);
		} else if (peek.type == tk_plus) {
			expect(tk_plus);
			return new expression_t({parse_literal(), un_arithmetic_positive}, EXPRESSION_DEBUG);
		} else if (peek.type == tk_minus) {
			expect(tk_minus);
			return new expression_t({parse_literal(), un_arithmetic_negative}, EXPRESSION_DEBUG);
		} else if (peek.type == tk_ampersand) {
			expect(tk_ampersand);
			return new expression_t({parse_literal(), un_address_of}, EXPRESSION_DEBUG);
		} else if (peek.type == tk_un_logical_not) {
			expect(tk_un_logical_not);
			return new expression_t({parse_literal(), un_logical_not}, EXPRESSION_DEBUG);
		} else if (peek.type == tk_un_binary_not) {
			expect(tk_un_binary_not);
			return new expression_t({parse_literal(), un_binary_not}, EXPRESSION_DEBUG);
		} else if (peek.type == tk_int) {
			expect(tk_int);
			while (input.peek().type == tk_asterisk) {
				expect(tk_asterisk);
			}
			return new expression_t("0", "int", EXPRESSION_DEBUG);
		} else {
			die("expected literal");
			return nullptr;
		}
		#undef EXPRESSION_DEBUG
	}

	// Parse a literal.
	expression_t* parse_literal() {
		expression_t* node = parse_literal_no_suffix();
		token_t peek;
		while ((peek = input.peek()).type == tk_left_bracket) {
			expect(tk_left_bracket);
			expression_t* index = parse_expression();
			expect(tk_right_bracket);
			node = new expression_t((indexing_expression_t){node, index}, peek.lineno, peek.colno - peek.text.length() - 1);
		}
		return node;
	}

	#define EXPRESSION_DEBUG token.lineno, token.colno

	// Parse a multiplicative term.
	expression_t* parse_multiplicative_term() {
		expression_t* node = parse_literal();
		token_t token;
		while (input.peek().type == tk_asterisk ||
			   input.peek().type == tk_bi_division ||
			   input.peek().type == tk_bi_modulo)
		{
			token = input.peek();
			binary_operator_t binary_operator = bi_error;
			if (token.type == tk_asterisk) {
				expect(tk_asterisk);
				binary_operator = bi_multiplication;
			} else if (token.type == tk_bi_division) {
				expect(tk_bi_division);
				binary_operator = bi_division;
			} else if (token.type == tk_bi_modulo) {
				expect(tk_bi_modulo);
				binary_operator = bi_modulo;
			}
			node = new expression_t((binary_expression_t){node, parse_literal(), binary_operator}, EXPRESSION_DEBUG);
		}
		return node;
	}

	// Parse an additive term.
	expression_t* parse_additive_term() {
		expression_t* node = parse_multiplicative_term();
		token_t token;
		while (input.peek().type == tk_plus ||
			   input.peek().type == tk_minus)
		{
			token = input.peek();
			binary_operator_t binary_operator = bi_error;
			if (token.type == tk_plus) {
				expect(tk_plus);
				binary_operator = bi_addition;
			} else if (token.type == tk_minus) {
				expect(tk_minus);
				binary_operator = bi_subtraction;
			}
			node = new expression_t((binary_expression_t){node, parse_multiplicative_term(), binary_operator}, EXPRESSION_DEBUG);
		}
		return node;
	}

	// Parse a binary shift term.
	expression_t* parse_shift_term() {
		expression_t* node = parse_additive_term();
		token_t token;
		while (input.peek().type == tk_bi_binary_left_shift ||
			   input.peek().type == tk_bi_binary_right_shift)
		{
			token = input.peek();
			binary_operator_t binary_operator = bi_error;
			if (token.type == tk_bi_binary_left_shift) {
				expect(tk_bi_binary_left_shift);
				binary_operator = bi_binary_left_shift;
			} else if (token.type == tk_bi_binary_right_shift) {
				expect(tk_bi_binary_right_shift);
				binary_operator = bi_binary_right_shift;
			}
			node = new expression_t((binary_expression_t){node, parse_additive_term(), binary_operator}, EXPRESSION_DEBUG);
		}
		return node;
	}

	// Parse a relational term.
	expression_t* parse_relational_term() {
		expression_t* node = parse_shift_term();
		token_t token;
		while (input.peek().type == tk_bi_relational_greater_than ||
			   input.peek().type == tk_bi_relational_lesser_than ||
			   input.peek().type == tk_bi_relational_greater_than_or_equal_to ||
			   input.peek().type == tk_bi_relational_lesser_than_or_equal_to)
		{
			token = input.peek();
			binary_operator_t binary_operator = bi_error;
			if (token.type == tk_bi_relational_greater_than) {
				expect(tk_bi_relational_greater_than);
				binary_operator = bi_relational_greater_than;
			} else if (token.type == tk_bi_relational_lesser_than) {
				expect(tk_bi_relational_lesser_than);
				binary_operator = bi_relational_lesser_than;
			} else if (token.type == tk_bi_relational_greater_than_or_equal_to) {
				expect(tk_bi_relational_greater_than_or_equal_to);
				binary_operator = bi_relational_greater_than_or_equal_to;
			} else if (token.type == tk_bi_relational_lesser_than_or_equal_to) {
				expect(tk_bi_relational_lesser_than_or_equal_to);
				binary_operator = bi_relational_lesser_than_or_equal_to;
			}
			node = new expression_t((binary_expression_t){node, parse_shift_term(), binary_operator}, EXPRESSION_DEBUG);
		}
		return node;
	}

	// Parse an equality term.
	expression_t* parse_equality_term() {
		expression_t* node = parse_relational_term();
		token_t token;
		while (input.peek().type == tk_bi_relational_equal ||
			   input.peek().type == tk_bi_relational_non_equal)
		{
			token = input.peek();
			binary_operator_t binary_operator = bi_error;
			if (token.type == tk_bi_relational_equal) {
				expect(tk_bi_relational_equal);
				binary_operator = bi_relational_equal;
			} else if (token.type == tk_bi_relational_non_equal) {
				expect(tk_bi_relational_non_equal);
				binary_operator = bi_relational_non_equal;
			}
			node = new expression_t((binary_expression_t){node, parse_relational_term(), binary_operator}, EXPRESSION_DEBUG);
		}
		return node;
	}

	#undef EXPRESSION_DEBUG
	#define EXPRESSION_DEBUG peek.lineno, peek.colno

	// Parse a binary AND term.
	expression_t* parse_binary_and_term() {
		expression_t* node = parse_equality_term();
		while (input.peek().type == tk_ampersand) {
			token_t peek = input.peek();
			expect(tk_ampersand);
			node = new expression_t((binary_expression_t){node, parse_equality_term(), bi_binary_and}, EXPRESSION_DEBUG);
		}
		return node;
	}

	// Parse a binary XOR term.
	expression_t* parse_binary_xor_term() {
		expression_t* node = parse_binary_and_term();
		while (input.peek().type == tk_bi_binary_xor) {
			token_t peek = input.peek();
			expect(tk_bi_binary_xor);
			node = new expression_t((binary_expression_t){node, parse_binary_and_term(), bi_binary_xor}, EXPRESSION_DEBUG);
		}
		return node;
	}

	// Parse a binary OR term.
	expression_t* parse_binary_or_term() {
		expression_t* node = parse_binary_xor_term();
		while (input.peek().type == tk_bi_binary_or) {
			token_t peek = input.peek();
			expect(tk_bi_binary_or);
			node = new expression_t((binary_expression_t){node, parse_binary_xor_term(), bi_binary_or}, EXPRESSION_DEBUG);
		}
		return node;
	}

	// Parse a logical AND term.
	expression_t* parse_logical_and_term() {
		expression_t* node = parse_binary_or_term();
		while (input.peek().type == tk_bi_logical_and) {
			token_t peek = input.peek();
			expect(tk_bi_logical_and);
			node = new expression_t((binary_expression_t){node, parse_binary_or_term(), bi_logical_and}, EXPRESSION_DEBUG);
		}
		return node;
	}

	// Parse a logical OR term.
	expression_t* parse_logical_or_term() {
		expression_t* node = parse_logical_and_term();
		while (input.peek().type == tk_bi_logical_or) {
			token_t peek = input.peek();
			expect(tk_bi_logical_or);
			node = new expression_t((binary_expression_t){node, parse_logical_and_term(), bi_logical_or}, EXPRESSION_DEBUG);
		}
		return node;
	}

	// Parse an assignment term (right-associative).
	expression_t* parse_assignment_term() {
		std::vector<expression_t*> nodes;
		std::vector<binary_operator_t> operators;
		nodes.push_back(parse_logical_or_term());
		while (input.peek().type == tk_bi_assignment ||
			   input.peek().type == tk_bi_addition_assignment ||
			   input.peek().type == tk_bi_subtraction_assignment ||
			   input.peek().type == tk_bi_multiplication_assignment ||
			   input.peek().type == tk_bi_division_assignment ||
			   input.peek().type == tk_bi_modulo_assignment ||
			   input.peek().type == tk_bi_binary_and_assignment ||
			   input.peek().type == tk_bi_binary_or_assignment ||
			   input.peek().type == tk_bi_binary_xor_assignment ||
			   input.peek().type == tk_bi_binary_left_shift_assignment ||
			   input.peek().type == tk_bi_binary_right_shift_assignment)
		{
			token_t next = input.next();
			if (next.type == tk_bi_assignment) {
				operators.push_back(bi_assignment);
			} else if (next.type == tk_bi_addition_assignment) {
				operators.push_back(bi_addition_assignment);
			} else if (next.type == tk_bi_subtraction_assignment) {
				operators.push_back(bi_subtraction_assignment);
			} else if (next.type == tk_bi_multiplication_assignment) {
				operators.push_back(bi_multiplication_assignment);
			} else if (next.type == tk_bi_division_assignment) {
				operators.push_back(bi_division_assignment);
			} else if (next.type == tk_bi_modulo_assignment) {
				operators.push_back(bi_modulo_assignment);
			} else if (next.type == tk_bi_binary_and_assignment) {
				operators.push_back(bi_binary_and_assignment);
			} else if (next.type == tk_bi_binary_or_assignment) {
				operators.push_back(bi_binary_or_assignment);
			} else if (next.type == tk_bi_binary_xor_assignment) {
				operators.push_back(bi_binary_xor_assignment);
			} else if (next.type == tk_bi_binary_left_shift_assignment) {
				operators.push_back(bi_binary_left_shift_assignment);
			} else if (next.type == tk_bi_binary_right_shift_assignment) {
				operators.push_back(bi_binary_right_shift_assignment);
			}
			nodes.push_back(parse_logical_or_term());
		}
		if (nodes.size() == 1) {
			return nodes[0];
		} else {
			std::reverse(nodes.begin(), nodes.end());
			std::reverse(operators.begin(), operators.end());
			expression_t* node = new expression_t((binary_expression_t){nodes[1], nodes[0], operators[0]}, nodes[1]->lineno, nodes[1]->colno);
			nodes.erase(nodes.begin());
			nodes.erase(nodes.begin());
			operators.erase(operators.begin());
			while (nodes.size()) {
				expression_t* next = nodes[0];
				node = new expression_t((binary_expression_t){next, node, operators[0]}, next->lineno, next->colno);
				nodes.pop_back();
				operators.pop_back();
			}
			return node;
		}
	}

	#undef EXPRESSION_DEBUG

	// Parse an expression.
	expression_t* parse_expression() {
		return parse_assignment_term();
	}

	#define STATEMENT_DEBUG peek.lineno, peek.colno - peek.text.length() - 1

	// Parse a statement.
	statement_t* parse_statement() {
		token_t peek = input.peek();
		if (peek.type == tk_left_brace) {
			// Compound statement.
			std::vector<statement_t*> statements;
			expect(tk_left_brace);
			while (input.peek().type != tk_right_brace) {
				statements.push_back(parse_statement());
			}
			expect(tk_right_brace);
			return new statement_t({statements});
		} else if (peek.type == tk_if) {
			// Conditional statement.
			expect(tk_if);
			expect(tk_left_parenthesis);
			expression_t* condition = parse_expression();
			expect(tk_right_parenthesis);
			statement_t* body = parse_statement();
			return new statement_t((conditional_statement_t){condition, body});
		} else if (peek.type == tk_while) {
			// While statement.
			expect(tk_while);
			expect(tk_left_parenthesis);
			expression_t* condition = parse_expression();
			expect(tk_right_parenthesis);
			statement_t* body = parse_statement();
			return new statement_t((while_statement_t){condition, body});
		} else if (peek.type == tk_return) {
			// Return statement.
			expect(tk_return);
			expression_t* value = parse_expression();
			expect(tk_semicolon);
			return new statement_t((return_statement_t){value});
		} else if (peek.type == tk_int) {
			// Variable declaration statement.
			type_t type = parse_type();
			identifier_t identifier = parse_identifier();
			expression_t* initializer = nullptr;
			if (input.peek().type == tk_bi_assignment) {
				expect(tk_bi_assignment);
				initializer = parse_expression();
			}
			expect(tk_semicolon);
			return new statement_t({type, identifier, initializer});
		} else if (peek.type == tk_semicolon) {
			// No-op statement.
			expect(tk_semicolon);
			return new statement_t(st_no_op, STATEMENT_DEBUG);
		} else if (peek.type == tk_break) {
			// Break statement.
			expect(tk_break);
			return new statement_t(st_break, STATEMENT_DEBUG);
		} else if (peek.type == tk_continue) {
			// Continue statement.
			expect(tk_continue);
			return new statement_t(st_continue, STATEMENT_DEBUG);
		} else {
			// Expression statement.
			expression_t* expression = parse_expression();
			expect(tk_semicolon);
			return new statement_t((expression_statement_t){expression});
		}
	}

	#undef STATEMENT_DEBUG

	// Parse a list of statements.
	std::vector<statement_t*> parse_statements() {
		std::vector<statement_t*> statements;
		expect(tk_left_brace);
		while (input.peek().type != tk_right_brace) {
			statements.push_back(parse_statement());
		}
		expect(tk_right_brace);
		return statements;
	}

	// Parse the program.
	std::vector<function_t> parse() {
		std::vector<function_t> functions;
		while (!input.eof()) {
			token_t peek = input.peek();
			functions.push_back({
				parse_type(),
				parse_identifier(),
				parse_parameters(),
				parse_statements(),
				long(peek.lineno),
				long(peek.colno - peek.text.length() - 1)
			});
		}
		return functions;
	}
};