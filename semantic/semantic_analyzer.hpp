#pragma once
#include <string>
#include <sstream>
#include <iostream>

#include "../util/ansi_colors.hpp"
#include "symbol_table.hpp"

// A semantic analyzer.
struct semantic_analyzer_t {
	std::string filename;
	std::string buffer;

	// Default constructor.
	semantic_analyzer_t(std::string filename, std::string buffer) {
		this->filename = filename;
		this->buffer = buffer;
	}

	// Print an error message, then exit.
	void die(std::string error) {
		std::cerr << error << std::endl;
		exit(3);
	}

	// Print an error message, then exit.
	void die(std::string error, long lineno, long colno) {
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
		for (int i = 0; i < colno + 1; i++) {
			if (line[i] == '\t') {
				std::cerr << '\t';
			} else {
				std::cerr << ' ';
			}
		}
		std::cerr << set_color(bold_green) << '^' << set_color(reset) << std::endl;
		exit(3);
	}

	// Print an error message, then exit.
	void die(std::string error, expression_t* expression) {
		die(error, expression->lineno, expression->colno);
	}

	// Print an error message, then exit.
	void die(std::string error, function_t function) {
		die(error, function.lineno, function.colno);
	}

	// Expands a string or character literal.
	std::string expand_literal(std::string literal, expression_t* expression) {
		std::string expanded;
		bool escaped = false;
		for (int i = 0; i < literal.size(); i++) {
			if (literal[i] == '\\') {
				escaped = true;
			} else {
				if (escaped) {
					if (literal[i] == 'a') {
						expanded += '\a';
					} else if (literal[i] == 'b') {
						expanded += '\b';
					} else if (literal[i] == 'f') {
						expanded += '\f';
					} else if (literal[i] == 'n') {
						expanded += '\n';
					} else if (literal[i] == 'r') {
						expanded += '\r';
					} else if (literal[i] == 't') {
						expanded += '\t';
					} else if (literal[i] == 'v') {
						expanded += '\v';
					} else if (literal[i] == '\\') {
						expanded += '\\';
					} else if (literal[i] == '\'') {
						expanded += '\'';
					} else if (literal[i] == '"') {
						expanded += '"';
					} else {
						die("unknown escape sequence", expression->lineno, expression->colno + i - 2);
					}
					escaped = false;
					// TODO: null character literal
					// TODO: octal character literal
					// TODO: hexadecimal character literal
				} else {
					expanded += literal[i];
				}
			}
		}
		return expanded;
	}

	// Checks if an identifier is reserved.
	bool is_reserved(identifier_t identifier) {
		return identifier == "__return__";
	}

	// Check if an expression is an rvalue.
	bool is_rvalue(expression_t* expression, symbol_table_t symbols) {
		if (expression->type == et_unary) {
			return expression->unary.unary_operator != un_value_of;
		} else {
			return expression->type != et_identifier &&
				   expression->type != et_indexing;
		}
	}

	// Get the return type of an expression.
	type_t expression_type(expression_t* expression, symbol_table_t symbols) {
		if (expression->type == et_integer_literal) {
			// An integer literal is of type int.
			return expression->return_type = {0};
		} else if (expression->type == et_string_literal) {
			// A string literal is of type int*.
			return expression->return_type = {1};
		} else if (expression->type == et_character_literal) {
			// A character literal is of type int.
			return expression->return_type = {0};
		} else if (expression->type == et_identifier) {
			// An identifier literal's type is equivalent to the type of the
			// entry in the symbol table under the name of the identifier.
			if (symbols.exists(expression->identifier)) {
				return expression->return_type = symbols.fetch(expression->identifier).type;
			} else {
				die("unknown identifier '" + expression->identifier + "'", expression);
				return expression->return_type = {0};
			}
		} else if (expression->type == et_indexing) {
			// An indexing expression's return type is the type of the array
			// expression with one less pointer-depth.
			type_t array_type;
			if (symbols.exists(expression->indexing.array)) {
				array_type = symbols.fetch(expression->indexing.array).type;
				return expression->return_type = {array_type.pointer_depth - 1};
			} else {
				die("unknown identifier '" + expression->indexing.array + "'", expression);
				return expression->return_type = {0};
			}
		} else if (expression->type == et_function_call) {
			// A function call's type is equivalent to the type of the entry
			// in the symbol table under the name of the function.
			function_call_expression_t function_call = expression->function_call;
			if (symbols.exists(function_call.function)) {
				return expression->return_type = symbols.fetch(function_call.function).type;
			} else {
				die("unknown identifier '" + function_call.function + "'", expression);
				return expression->return_type = {0};
			}
		} else if (expression->type == et_binary) {
			binary_expression_t binary = expression->binary;
			if (binary.binary_operator == bi_addition ||
				binary.binary_operator == bi_subtraction ||
				binary.binary_operator == bi_multiplication ||
				binary.binary_operator == bi_division ||
				binary.binary_operator == bi_modulo)
			{
				// The return type of these types of binary expressions is
				// equivalent to the return type of the operand with the
				// greatest pointer depth.
				type_t left_type = expression_type(binary.left_operand, symbols);
				type_t right_type = expression_type(binary.right_operand, symbols);
				return expression->return_type = {std::max(left_type.pointer_depth, right_type.pointer_depth)};
			} else if (binary.binary_operator == bi_assignment) {
				// The return type of this type of binary expression is
				// equivalent to the return type of the left-hand operand.
				return expression->return_type = expression_type(binary.left_operand, symbols);
			} else {
				// bi_logical_and,
				// bi_logical_or,
				// bi_relational_equal,
				// bi_relational_non_equal,
				// bi_relational_greater_than,
				// bi_relational_lesser_than,
				//
				// The return type of these types of binary expressions is
				// int.
				return expression->return_type = {0};
			}
		} else {
			unary_expression_t unary = expression->unary;
			if (unary.unary_operator == un_value_of) {
				// The return type of this type of unary expression is the
				// return type of the operand with one less pointer-depth.
				type_t operand_type = expression_type(unary.operand, symbols);
				return expression->return_type = {operand_type.pointer_depth - 1};
			} else if (unary.unary_operator == un_arithmetic_positive ||
					   unary.unary_operator == un_arithmetic_negative)
			{
				// The return type of these types of unary expressions is
				// equivalent to the return type of the operand.
				return expression->return_type = expression_type(unary.operand, symbols);
			} else if (unary.unary_operator == un_address_of) {
				// The return type of this type of unary expression is the
				// return type of the operand with one more pointer-depth.
				type_t operand_type = expression_type(unary.operand, symbols);
				return expression->return_type = {operand_type.pointer_depth + 1};
			} else {
				// un_logical_not
				//
				// The return type of this type of unary expression is int.
				return expression->return_type = {0};
			}
		}
	}

	// Validate an expression.
	bool validate_expression(expression_t* expression, symbol_table_t symbols) {
		if (expression->type == et_character_literal) {
			// Expand the character literal.
			character_t expanded_literal = expand_literal(expression->character_literal, expression);
			if (expanded_literal.size() != 1) {
				die("multi-character character literal", expression);
			}
			expression->character_literal = expanded_literal;
		} else if (expression->type == et_string_literal) {
			// TODO: handle newlines and whitespace in string literals
			//       appropriately
			expression->string_literal = expand_literal(expression->string_literal, expression);
		} else if (expression->type == et_identifier) {
			// Identifiers are invalid if they are undefined or reserved.
			if (is_reserved(expression->identifier)) {
				die("cannot refer to reserved identifier '" + expression->identifier + "'", expression);
				return false;
			}
			else if (!symbols.exists(expression->identifier)) {
				die("unknown identifier '" + expression->identifier + "'", expression);
				return false;
			}
		} else if (expression->type == et_indexing) {
			indexing_expression_t indexing = expression->indexing;
			// An indexing expression is invalid if the array is invalid or
			// the index is invalid.
			if (!symbols.exists(indexing.array)) {
				die("unknown identifier '" + indexing.array + "'", expression);
				return false;
			} else if (!validate_expression(indexing.index, symbols)) {
				return false;
			}
			// An indexing expression is invalid if the index expression's
			// return type cannot be converted to int.
			type_t index_type = expression_type(indexing.index, symbols);
			if (!can_convert(index_type, {0})) {
				die("cannot convert index expression of type '" + prettyprint_type(index_type) + "' to 'int'", expression);
				return false;
			}
		} else if (expression->type == et_function_call) {
			function_call_expression_t function_call = expression->function_call;
			// A function call expression is invalid if there is no symbol
			// under the function call's function identifier.
			if (!symbols.exists(function_call.function)) {
				die("unknown identifier '" + function_call.function + "'", expression);
				return false;
			}
			// A function call expression is invalid if there is a
			// non-function symbol under the function call's function
			// identifier.
			if (!symbols.fetch(function_call.function).is_function) {
				die("called variable '" + function_call.function + "' is not a function", expression);
				return false;
			}
			// A function call expression is invalid if it's parameter count
			// is not equal to the parameter count of it's registered symbol.
			symbol_t function = symbols.fetch(function_call.function);
			if (function.parameters.size() != function_call.arguments.size()) {
				die("no matching function call to '" + function_call.function + "'", expression);
				return false;
			}
			// A function call expression is invalid if any of it's parameter
			// expression types cannot be converted to it's corresponding type
			// as defined in the function's registered symbol.
			for (int i = 0; i < function_call.arguments.size(); i++) {
				type_t parameter_type = expression_type(function_call.arguments[i], symbols);
				type_t expected_type = function.parameters[i].type;
				if (!can_convert(parameter_type, expected_type)) {
					die("cannot convert parameter expression of type '" + prettyprint_type(parameter_type) + "' to '" + prettyprint_type(expected_type) + "'", function_call.arguments[i]);
					return false;
				}
			}
			// A function call expression is invalid if any of it's parameter
			// expressions are invalid.
			for (int i = 0; i < function_call.arguments.size(); i++) {
				if (!validate_expression(function_call.arguments[i], symbols)) {
					return false;
				}
			}
		} else if (expression->type == et_binary) {
			binary_expression_t binary = expression->binary;
			// Any binary expression is invalid if the operands are invalid.
			if (!validate_expression(binary.left_operand, symbols) ||
				!validate_expression(binary.right_operand, symbols))
			{
				return false;
			} else if (binary.binary_operator == bi_addition ||
					   binary.binary_operator == bi_subtraction ||
					   binary.binary_operator == bi_multiplication ||
					   binary.binary_operator == bi_division ||
					   binary.binary_operator == bi_modulo ||
					   binary.binary_operator == bi_assignment ||

					   binary.binary_operator == bi_relational_equal ||
					   binary.binary_operator == bi_relational_non_equal)
			{
				// A binary expression of this type is invalid if the return
				// type of the right-hand operand cannot be converted to the
				// return type of the left-hand operand.
				type_t left_type = expression_type(binary.left_operand, symbols);
				type_t right_type = expression_type(binary.right_operand, symbols);
				if (!can_convert(left_type, right_type)) {
					die("invalid operands to binary expression ('" + prettyprint_type(left_type) + "' and '" + prettyprint_type(right_type) + "')", expression);
					return false;
				}
				if (binary.binary_operator == bi_assignment) {
					// A binary expression of this type is invalid if the
					// left-hand operand is an rvalue.
					if (is_rvalue(binary.left_operand, symbols)) {
						die("expression is not assignable", binary.left_operand);
						return false;
					}
				}
				if (left_type.pointer_depth > 0) {
					if (binary.binary_operator == bi_multiplication ||
						binary.binary_operator == bi_division ||
						binary.binary_operator == bi_modulo) {
						// A binary expression of this type is invalid if the
						// left-hand operand is a pointer and the operator is
						// multiplication, division or modulo.
						die("invalid operands to binary expression ('" + prettyprint_type(left_type) + "' and '" + prettyprint_type(right_type) + "')", expression);
						return false;
					} else if (binary.binary_operator == bi_addition) {
						if (right_type.pointer_depth > 0) {
							// It is invalid to add pointers.
							die("invalid operands to binary expression ('" + prettyprint_type(left_type) + "' and '" + prettyprint_type(right_type) + "')", expression);
							return false;
						}
					}
				} else if (right_type.pointer_depth > 0) {
					if (binary.binary_operator == bi_subtraction ||
						binary.binary_operator == bi_multiplication ||
						binary.binary_operator == bi_division ||
						binary.binary_operator == bi_modulo) {
						// A binary expression of this type is invalid if the
						// right-hand operand is a pointer and the operator is
						// subtraction, multiplication, division or modulo.
						die("invalid operands to binary expression ('" + prettyprint_type(left_type) + "' and '" + prettyprint_type(right_type) + "')", expression);
						return false;
					}
				}
			} else {
				// bi_logical_and
				// bi_logical_or
				// bi_relational_greater_than
				// bi_relational_lesser_than
				//
				// A binary expression of this type is invalid if the return
				// type of either operand cannot be converted to int.
				type_t left_type = expression_type(binary.left_operand, symbols);
				type_t right_type = expression_type(binary.right_operand, symbols);
				if (!can_convert(left_type, {0}) ||
					!can_convert(right_type, {0}))
				{
					die("invalid operands to binary expression ('" + prettyprint_type(left_type) + "' and '" + prettyprint_type(right_type) + "')", expression);
					return false;
				}
			}
		} else if (expression->type == et_unary) {
			unary_expression_t unary = expression->unary;
			// Any unary expression is invalid if the operand is invalid.
			if (!validate_expression(unary.operand, symbols)) {
				return false;
			}
			else if (unary.unary_operator == un_value_of) {
				// A unary expression of this type is invalid if the operand's
				// pointer depth is less than 1.
				type_t operand_type = expression_type(unary.operand, symbols);
				if (operand_type.pointer_depth < 1) {
					die("cannot dereference expression of type '" + prettyprint_type(operand_type) + "'", expression);
					return false;
				}
			} else if (unary.unary_operator == un_arithmetic_positive ||
					   unary.unary_operator == un_arithmetic_negative)
			{
				// A unary expression of this type is invalid if the operand
				// is a pointer.
				type_t operand_type = expression_type(unary.operand, symbols);
				if (operand_type.pointer_depth > 0) {
					die("wrong type argument to unary operator ('" + prettyprint_type(operand_type) + "')", expression);
					return false;
				}
			} else {
				// un_address_of
				//
				// A unary expression of this type is invalid if the operand
				// is an rvalue.
				if (is_rvalue(unary.operand, symbols)) {
					die("cannot take the address of an rvalue of type '" + prettyprint_type(expression_type(unary.operand, symbols)) + "'", expression);
					return false;
				}
			}
		}
		return true;
	}

	// Validate a statement.
	bool validate_statement(statement_t* statement, symbol_table_t& symbols) {
		if (statement->type == st_compound) {
			compound_statement_t stmt = statement->compound_stmt;
			// A compound statement is invalid if any of it's child statements
			// are invalid.
			symbol_table_t new_symbols(&symbols);
			for (int i = 0; i < stmt.statements.size(); i++) {
				if (!validate_statement(stmt.statements[i], new_symbols)) {
					return false;
				}
			}
		} else if (statement->type == st_conditional) {
			conditional_statement_t stmt = statement->conditional_stmt;
			// A conditional statement is invalid if it's condition is
			// invalid.
			if (!validate_expression(stmt.condition, symbols)) {
				return false;
			}
			// A conditional statement is invalid if it's condition cannot be
			// converted to int.
			type_t condition_type = expression_type(stmt.condition, symbols);
			if (!can_convert(condition_type, {0})) {
				die("cannot convert expression of type '" + prettyprint_type(condition_type) + "' to 'int'", stmt.condition);
				return false;
			}
			// A conditional statement is invalid if it's body statement is
			// invalid.
			symbol_table_t new_symbols(&symbols);
			if (!validate_statement(stmt.body, new_symbols)) {
				return false;
			}
		} else if (statement->type == st_while) {
			while_statement_t stmt = statement->while_stmt;
			// A while statement is invalid if it's condition is invalid.
			if (!validate_expression(stmt.condition, symbols)) {
				return false;
			}
			// A while statement is invalid if it's condition cannot be
			// converted to int.
			type_t condition_type = expression_type(stmt.condition, symbols);
			if (!can_convert(condition_type, {0})) {
				die("cannot convert expression of type '" + prettyprint_type(condition_type) + "' to 'int'", stmt.condition);
				return false;
			}
			// A while statement is invalid if it's body statement is invalid.
			symbol_table_t new_symbols(&symbols);
			if (!validate_statement(stmt.body, new_symbols)) {
				return false;
			}
		} else if (statement->type == st_return) {
			return_statement_t stmt = statement->return_stmt;
			// A return statement is invalid if it's value's type cannot be
			// converted to the type of the return value of the function.
			type_t value_type = expression_type(stmt.value, symbols);
			type_t return_type = symbols.fetch("__return__").type;
			if (!can_convert(value_type, return_type)) {
				die("no conversion from value of type '" + prettyprint_type(value_type) + "' to function return type '" + prettyprint_type(return_type) + "'", stmt.value);
			}
			// A return statement is invalid if it's value is invalid.
			if (!validate_expression(stmt.value, symbols)) {
				return false;
			}
		} else if (statement->type == st_variable_declaration) {
			variable_declaration_statement_t stmt = statement->variable_declaration_stmt;
			// A variable declaration statement is invalid if it's identifier
			// is a reserved identifier.
			if (is_reserved(stmt.identifier)) {
				die("cannot declare variable with reserved identifier '" + stmt.identifier + "'");
				return false;
			}
			// A variable declaration statement is invalid if it's initializer
			// is invalid.
			if (stmt.initializer) {
				if (!validate_expression(stmt.initializer, symbols)) {
					return false;
				}
			}
			// A variable declaration statement is invalid if the type of it's
			// initializer cannot be converted to it's declared type.
			if (stmt.initializer) {
				type_t initializer_type = expression_type(stmt.initializer, symbols);
				if (!can_convert(initializer_type, stmt.type)) {
					die("no conversion from initializer value of type '" + prettyprint_type(initializer_type) + "' to variable type '" + prettyprint_type(stmt.type) + "'", stmt.initializer);
					return false;
				}
			}
			// A variable declaration statement is invalid if a variable under
			// the same identifier has already been declared in the current
			// scope.
			if (symbols.exists_locally(stmt.identifier)) {
				die("redefinition of '" + stmt.identifier + "'");
				return false;
			}
			// Add the variable to the current scope.
			symbols.add_symbol(symbol_t(stmt.type, stmt.identifier));
		} else if (statement->type == st_expression) {
			expression_statement_t stmt = statement->expression_stmt;
			// An expression statement is invalid if it's expression is
			// invalid.
			if (!validate_expression(stmt.expression, symbols)) {
				return false;
			}
		}
		return true;
	}

	// Validate a function.
	bool validate_function(function_t function, symbol_table_t& global_symbols) {
		symbol_table_t symbols(&global_symbols);
		// Load the function parameters as symbols.
		for (int i = 0; i < function.parameters.size(); i++) {
			symbols.add_symbol(symbol_t(
				function.parameters[i].type,
				function.parameters[i].identifier
			));
		}
		// Add the function return value as a special symbol with the
		// identifier __return__.
		symbols.add_symbol(symbol_t(function.type, "__return__"));
		// Iterate through each statement in the function body.
		bool had_return_stmt = false;
		for (int i = 0; i < function.body.size(); i++) {
			statement_t* statement = function.body[i];
			if (!validate_statement(statement, symbols)) {
				return false;
			}
			if (statement->type == st_return) {
				had_return_stmt = true;
			}
		}
		// The function is invalid if it has no return statement.
		if (!had_return_stmt) {
			die("function '" + function.identifier + "' has no return statement", function);
			return false;
		}
		return true;
	}

	// Validate a program.
	bool validate(program_t program) {
		symbol_table_t global_symbols;
		for (int i = 0; i < program.size(); i++) {
			function_t function = program[i];
			// The function is invalid if a function already exists under the
			// same identifier.
			if (global_symbols.exists(function.identifier)) {
				die("redefinition of function '" + function.identifier + "'", function);
				return false;
			}
			// Add the function to the symbol table.
			global_symbols.add_symbol(symbol_t(
				function.type,
				function.identifier,
				function.parameters
			));
			if (!validate_function(function, global_symbols)) {
				return false;
			}
		}
		return true;
	}
};