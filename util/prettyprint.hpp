#pragma once
#include <string>
#include <iostream>

#include "ansi_colors.hpp"

// Indentation character.
#define T ' '

// Prettyprint a binary operator.
std::string prettyprint_binary_operator(binary_operator_t binary_operator) {
	return binary_operator_str[binary_operator];
}

// Prettyprint a unary operator.
std::string prettyprint_unary_operator(unary_operator_t unary_operator) {
	return unary_operator_str[unary_operator];
}

// Prettyprint an expression.
void prettyprint_expression(expression_t* expression, long tab_count) {
	std::string tabs(tab_count, T);
	if (expression->type == et_integer_literal) {
		std::cout << tabs << set_color(red) << "integer literal expression" << set_color(reset) << ": " << set_color(magenta) << expression->integer_literal << set_color(reset);
	} else if (expression->type == et_string_literal) {
		std::cout << tabs << set_color(red) << "string literal expression" << set_color(reset) << ": " << set_color(cyan) << expression->string_literal << set_color(reset);
	} else if (expression->type == et_character_literal) {
		std::cout << tabs << set_color(red) << "character literal expression" << set_color(reset) << ": " << set_color(cyan) << expression->character_literal << set_color(reset);
	} else if (expression->type == et_identifier) {
		std::cout << tabs << set_color(red) << "identifier expression" << set_color(reset) << ": " << set_color(yellow) << expression->identifier << set_color(reset);
	} else if (expression->type == et_indexing) {
		std::cout << tabs << set_color(red) << "indexing expression" << set_color(reset) << " {" << std::endl;
		std::cout << tabs << T << "array: " << set_color(yellow) << expression->indexing.array << set_color(reset) << "," << std::endl;
		std::cout << tabs << T << "index: {" << std::endl;
		prettyprint_expression(expression->indexing.index, tab_count + 2);
		std::cout << std::endl;
		std::cout << tabs << T << "}" << std::endl;
		std::cout << tabs << "}";
	} else if (expression->type == et_function_call) {
		std::cout << tabs << set_color(red) << "function call expression" << set_color(reset) << " {" << std::endl;
		std::cout << tabs << T << "function: " << set_color(yellow) << expression->function_call.function << set_color(reset) << "," << std::endl;
		std::cout << tabs << T << "arguments: {" << std::endl;
		for (int i = 0; i < expression->function_call.arguments.size(); i++) {
			prettyprint_expression(expression->function_call.arguments[i], tab_count + 2);
			if (i != expression->function_call.arguments.size() - 1) {
				std::cout << ",";
			}
			std::cout << std::endl;
		}
		std::cout << tabs << T << "}" << std::endl;
		std::cout << tabs << "}";
	} else if (expression->type == et_binary) {
		std::cout << tabs << set_color(red) << "binary expression" << set_color(reset) << " {" << std::endl;
		std::cout << tabs << T << "left operand: {" << std::endl;
		prettyprint_expression(expression->binary.left_operand, tab_count + 2);
		std::cout << std::endl;
		std::cout << tabs << T << "}," << std::endl;
		std::cout << tabs << T << "right operand: {" << std::endl;
		prettyprint_expression(expression->binary.right_operand, tab_count + 2);
		std::cout << std::endl;
		std::cout << tabs << T << "}," << std::endl;
		std::cout << tabs << T << "operator: " << prettyprint_binary_operator(expression->binary.binary_operator) << std::endl;
		std::cout << tabs << "}";
	} else if (expression->type == et_unary) {
		std::cout << tabs << set_color(red) << "unary expression" << set_color(reset) << " {" << std::endl;
		std::cout << tabs << T << "operand: {" << std::endl;
		prettyprint_expression(expression->unary.operand, tab_count + 2);
		std::cout << std::endl;
		std::cout << tabs << T << "}," << std::endl;
		std::cout << tabs << T << "operator: " << prettyprint_unary_operator(expression->unary.unary_operator) << std::endl;
		std::cout << tabs << "}";
	}
}

// Prettyprint a statement.
void prettyprint_statement(statement_t* statement, long tab_count) {
	std::string tabs(tab_count, T);
	if (statement->type == st_compound) {
		std::cout << tabs << set_color(red) << "compound statement" << set_color(reset) << " {" << std::endl;
		for (int i = 0; i < statement->compound_stmt.statements.size(); i++) {
			prettyprint_statement(statement->compound_stmt.statements[i], tab_count + 1);
			if (i != statement->compound_stmt.statements.size() - 1) {
				std::cout << ",";
			}
			std::cout << std::endl;
		}
		std::cout << tabs << "}";
	} else if (statement->type == st_conditional) {
		std::cout << tabs << set_color(red) << "conditional statement" << set_color(reset) << " {" << std::endl;
		std::cout << tabs << T << "condition: {" << std::endl;
		prettyprint_expression(statement->conditional_stmt.condition, tab_count + 2);
		std::cout << std::endl;
		std::cout << tabs << T << "}," << std::endl;
		std::cout << tabs << T << "body: {" << std::endl;
		prettyprint_statement(statement->conditional_stmt.body, tab_count + 2);
		std::cout << std::endl;
		std::cout << tabs << T << "}" << std::endl;
		std::cout << tabs << "}";
	} else if (statement->type == st_while) {
		std::cout << tabs << set_color(red) << "while statement" << set_color(reset) << " {" << std::endl;
		std::cout << tabs << T << "condition: {" << std::endl;
		prettyprint_expression(statement->while_stmt.condition, tab_count + 2);
		std::cout << std::endl;
		std::cout << tabs << T << "}," << std::endl;
		std::cout << tabs << T << "body: {" << std::endl;
		prettyprint_statement(statement->while_stmt.body, tab_count + 2);
		std::cout << std::endl;
		std::cout << tabs << T << "}" << std::endl;
		std::cout << tabs << "}";
	} else if (statement->type == st_return) {
		std::cout << tabs << set_color(red) << "return statement" << set_color(reset) << " {" << std::endl;
		std::cout << tabs << T << "value: {" << std::endl;
		prettyprint_expression(statement->return_stmt.value, tab_count + 2);
		std::cout << std::endl;
		std::cout << tabs << T << "}" << std::endl;
		std::cout << tabs << "}";
	} else if (statement->type == st_variable_declaration) {
		std::cout << tabs << set_color(red) << "variable declaration statement" << set_color(reset) << " {" << std::endl;
		std::cout << tabs << T << "type: " << set_color(green) << prettyprint_type(statement->variable_declaration_stmt.type) << set_color(reset) << "," << std::endl;
		std::cout << tabs << T << "identifier: " << set_color(yellow) << statement->variable_declaration_stmt.identifier << set_color(reset) << std::endl;
		if (statement->variable_declaration_stmt.initializer) {
			std::cout << tabs << T << "initializer: {" << std::endl;
			prettyprint_expression(statement->variable_declaration_stmt.initializer, tab_count + 2);
			std::cout << std::endl;
			std::cout << tabs << T << "}" << std::endl;
		}
		std::cout << tabs << "}";
	} else if (statement->type == st_no_op) {
		std::cout << tabs << set_color(red) << "no-op statement" << set_color(reset) << " {}";
	} else if (statement->type == st_expression) {
		std::cout << tabs << set_color(red) << "expression statement" << set_color(reset) << " {" << std::endl;
		std::cout << tabs << T << "expression: {" << std::endl;
		prettyprint_expression(statement->expression_stmt.expression, tab_count + 2);
		std::cout << std::endl;
		std::cout << tabs << T << "}" << std::endl;
		std::cout << tabs << "}";
	}
}

// Prettyprint a function.
void prettyprint_function(function_t function) {
	std::cout << set_color(red) << "function" << set_color(reset) << " {" << std::endl;
	std::cout << T << "type: " << set_color(green) << prettyprint_type(function.type) << set_color(reset) << "," << std::endl;
	std::cout << T << "identifier: " << set_color(yellow) << function.identifier << set_color(reset) << "," << std::endl;
	std::cout << T << "parameters: {" << std::endl;
	for (int i = 0; i < function.parameters.size(); i++) {
		std::cout << T << T << "{type: " << set_color(green) << prettyprint_type(function.parameters[i].type) << set_color(reset) << ", identifier: " << set_color(yellow) << function.parameters[i].identifier << set_color(reset) << "}";
		if (i != function.parameters.size() - 1) {
			std::cout << ",";
		}
		std::cout << std::endl;
	}
	std::cout << T << "}," << std::endl;
	std::cout << T << "body: {" << std::endl;
	for (int i = 0; i < function.body.size(); i++) {
		prettyprint_statement(function.body[i], 2);
		if (i != function.body.size() - 1) {
			std::cout << ",";
		}
		std::cout << std::endl;
	}
	std::cout << T << "}" << std::endl;
	std::cout << "}" << std::endl;
}

#undef T