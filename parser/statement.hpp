#pragma once
#include <vector>

struct statement_t;

// All types of statements.
enum statement_type_t {
	st_compound,
	st_conditional,
	st_while,
	st_return,
	st_variable_declaration,
	st_no_op,
	st_expression
};

// A compound statement.
struct compound_statement_t {
	std::vector<statement_t*> statements;
};

// A conditional statement.
struct conditional_statement_t {
	expression_t* condition;
	statement_t* body;
};

// A while statement.
struct while_statement_t {
	expression_t* condition;
	statement_t* body;
};

// A return statement.
struct return_statement_t {
	expression_t* value;
};

// A variable declaration statement.
struct variable_declaration_statement_t {
	type_t type;
	identifier_t identifier;
	expression_t* initializer;
};

// An expression statement.
struct expression_statement_t {
	expression_t* expression;
};

// A statement.
struct statement_t {
	statement_type_t					type;

	compound_statement_t				compound_stmt;
	conditional_statement_t				conditional_stmt;
	while_statement_t					while_stmt;
	return_statement_t					return_stmt;
	variable_declaration_statement_t	variable_declaration_stmt;
	expression_statement_t				expression_stmt;

	statement_t(compound_statement_t stmt) {
		type = st_compound;
		compound_stmt = stmt;
	}

	statement_t(conditional_statement_t stmt) {
		type = st_conditional;
		conditional_stmt = stmt;
	}

	statement_t(while_statement_t stmt) {
		type = st_while;
		while_stmt = stmt;
	}

	statement_t(return_statement_t stmt) {
		type = st_return;
		return_stmt = stmt;
	}

	statement_t(variable_declaration_statement_t stmt) {
		type = st_variable_declaration;
		variable_declaration_stmt = stmt;
	}

	statement_t() {
		type = st_no_op;
	}

	statement_t(expression_statement_t stmt) {
		type = st_expression;
		expression_stmt = stmt;
	}
};