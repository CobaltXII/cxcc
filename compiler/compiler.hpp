#pragma once

// A compiler.
struct compiler_t {
	// Default constructor.
	compiler_t(program_t program) {
		this->program = program;
	}

	// Emit a line of assembly.
	#define emit printf

	// Compile an expression.
	void compile_expression(expression_t* expression, symbol_table_t& symbols) {
		if (expression->type == et_integer_literal) {
			emit("    movq    $%s, %%rax\n", expression->integer_literal.c_str());
		} else if (expression->type == et_string_literal) {
			emit("    leaq    S%ld(%%rip), %%rax\n", expression->string_label);
		} else if (expression->type == et_character_literal) {
			emit("    movq    $%u, %%rax\n", expression->character_literal[0]);
		} else if (expression->type == et_identifier) {
			emit("    movq    %ld(%%rbp), %%rax\n", symbols.fetch(expression->identifier).offset);
		} else if (expression->type == et_function_call) {
			function_call_expression_t expr = expression->function_call;
			if (expr.function == "sizeof") {
				compile_expression(expr.arguments[0], symbols);
				if (expr.arguments[0]->type == et_string_literal) {
					emit("    movq    $%lu, %%rax\n", expr.arguments[0]->string_literal.size() * 8 + 8);
				} else {
					emit("    movq    $8, %%rax\n");
				}
			} else {
				const char* registers[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
				if (expr.arguments.size() <= 6) {
					for (int i = 0; i < expr.arguments.size(); i++) {
						compile_expression(expr.arguments[i], symbols);
						emit("    movq    %%rax, %s\n", registers[i]);
					}
					#ifdef __APPLE__
					emit("    callq   _%s\n", expr.function.c_str());
					#else
					emit("    callq   %s\n", expr.function.c_str());
					#endif
				} else {
					for (int i = 0; i < 6; i++) {
						compile_expression(expr.arguments[0], symbols);
						expr.arguments.erase(expr.arguments.begin());
						emit("    movq    %%rax, %s\n", registers[i]);
					}
					for (int i = expr.arguments.size() - 1; i >= 0; i--) {
						compile_expression(expr.arguments[i], symbols);
						emit("    pushq   %%rax\n");
					}
					#ifdef __APPLE__
					emit("    callq   _%s\n", expr.function.c_str());
					#else
					emit("    callq   %s\n", expr.function.c_str());
					#endif
					emit("    addq    $%ld, %%rsp\n", expr.arguments.size() * 8);
				}
			}
		} else if (expression->type == et_binary) {
			binary_expression_t expr = expression->binary;
			if (expr.binary_operator == bi_addition) {
				if (expr.left_operand->return_type.pointer_depth > 0 ||
					expr.right_operand->return_type.pointer_depth > 0)
				{
					compile_expression(expr.left_operand, symbols);
					if (expr.left_operand->return_type.pointer_depth == 0) {
						emit("    salq    $3, %%rax\n");
					}
					emit("    pushq   %%rax\n");
					compile_expression(expr.right_operand, symbols);
					if (expr.right_operand->return_type.pointer_depth == 0) {
						emit("    salq    $3, %%rax\n");
					}
					emit("    popq    %%rcx\n");
					emit("    addq    %%rcx, %%rax\n");
				} else {
					compile_expression(expr.left_operand, symbols);
					emit("    pushq   %%rax\n");
					compile_expression(expr.right_operand, symbols);
					emit("    popq    %%rcx\n");
					emit("    addq    %%rcx, %%rax\n");
				}
			} else if (expr.binary_operator == bi_subtraction) {
				compile_expression(expr.right_operand, symbols);
				emit("    pushq   %%rax\n");
				compile_expression(expr.left_operand, symbols);
				emit("    popq    %%rcx\n");
				emit("    subq    %%rcx, %%rax\n");
			} else if (expr.binary_operator == bi_multiplication) {
				compile_expression(expr.left_operand, symbols);
				emit("    pushq   %%rax\n");
				compile_expression(expr.right_operand, symbols);
				emit("    popq    %%rcx\n");
				emit("    imulq   %%rcx, %%rax\n");
			} else if (expr.binary_operator == bi_division) {
				compile_expression(expr.right_operand, symbols);
				emit("    pushq   %%rax\n");
				compile_expression(expr.left_operand, symbols);
				emit("    popq    %%rcx\n");
				emit("    cqto\n");
				emit("    idivq   %%rcx\n");
			} else if (expr.binary_operator == bi_modulo) {
				compile_expression(expr.right_operand, symbols);
				emit("    pushq   %%rax\n");
				compile_expression(expr.left_operand, symbols);
				emit("    popq    %%rcx\n");
				emit("    cqto\n");
				emit("    idivq   %%rcx\n");
				emit("    movq    %%rdx, %%rax\n");
			} else if (expr.binary_operator == bi_assignment) {
				if (expr.left_operand->type == et_identifier) {
					compile_expression(expr.right_operand, symbols);
					emit("    movq    %%rax, %ld(%%rbp)\n", symbols.fetch(expr.left_operand->identifier).offset);
				} else {
					compile_expression(expr.right_operand, symbols);
					emit("    pushq   %%rax\n");
					compile_expression(expr.left_operand->unary.operand, symbols);
					emit("    popq    %%rcx\n");
					emit("    movq    %%rcx, (%%rax)\n");
					emit("    movq    (%%rax), %%rax\n");
				}
			} else if (expr.binary_operator == bi_logical_and) {
				long l0 = label++;
				long l1 = label++;
				compile_expression(expr.left_operand, symbols);
				emit("    cmpq    $0, %%rax\n");
				emit("    jne     L%ld\n", l0);
				emit("    jmp     L%ld\n", l1);
				emit("L%ld:\n", l0);
				compile_expression(expr.right_operand, symbols);
				emit("    cmpq    $0, %%rax\n");
				emit("    movq    $0, %%rax\n");
				emit("    setne   %%al\n");
				emit("L%ld:\n", l1);
			} else if (expr.binary_operator == bi_logical_or) {
				long l0 = label++;
				long l1 = label++;
				compile_expression(expr.left_operand, symbols);
				emit("    cmpq    $0, %%rax\n");
				emit("    je      L%ld\n", l0);
				emit("    movq    $1, %%rax\n");
				emit("    jmp     L%ld\n", l1);
				emit("L%ld:\n", l0);
				compile_expression(expr.right_operand, symbols);
				emit("    cmpq    $0, %%rax\n");
				emit("    movq    $0, %%rax\n");
				emit("    setne   %%al\n");
				emit("L%ld:\n", l1);
			} else if (expr.binary_operator == bi_relational_equal) {
				compile_expression(expr.left_operand, symbols);
				emit("    pushq   %%rax\n");
				compile_expression(expr.right_operand, symbols);
				emit("    popq    %%rcx\n");
				emit("    cmpq    %%rcx, %%rax\n");
				emit("    sete    %%al\n");
				emit("    movzbq  %%al, %%rax\n");
			} else if (expr.binary_operator == bi_relational_non_equal) {
				compile_expression(expr.left_operand, symbols);
				emit("    pushq   %%rax\n");
				compile_expression(expr.right_operand, symbols);
				emit("    popq    %%rcx\n");
				emit("    cmpq    %%rcx, %%rax\n");
				emit("    setne   %%al\n");
				emit("    movzbq  %%al, %%rax\n");
			} else if (expr.binary_operator == bi_relational_greater_than) {
				compile_expression(expr.right_operand, symbols);
				emit("    pushq   %%rax\n");
				compile_expression(expr.left_operand, symbols);
				emit("    popq    %%rcx\n");
				emit("    cmpq    %%rcx, %%rax\n");
				emit("    setg    %%al\n");
				emit("    movzbq  %%al, %%rax\n");
			} else if (expr.binary_operator == bi_relational_lesser_than) {
				compile_expression(expr.right_operand, symbols);
				emit("    pushq   %%rax\n");
				compile_expression(expr.left_operand, symbols);
				emit("    popq    %%rcx\n");
				emit("    cmpq    %%rcx, %%rax\n");
				emit("    setl    %%al\n");
				emit("    movzbq  %%al, %%rax\n");
			}
		} else if (expression->type == et_unary) {
			unary_expression_t expr = expression->unary;
			if (expr.unary_operator == un_value_of) {
				compile_expression(expr.operand, symbols);
				emit("    movq    (%%rax), %%rax\n");
			} else if (expr.unary_operator == un_arithmetic_negative) {
				compile_expression(expr.operand, symbols);
				emit("    negq    %%rax\n");
			} else if (expr.unary_operator == un_address_of) {
				emit("    leaq    %ld(%%rbp), %%rax\n", symbols.fetch(expr.operand->identifier).offset);
			} else if (expr.unary_operator == un_logical_not) {
				compile_expression(expr.operand, symbols);
				emit("    cmpq    $0, %%rax\n");
				emit("    movq    $0, %%rax\n");
				emit("    sete    %%al\n");
			}
		}
	}

	// Compile a statement.
	void compile_statement(statement_t* statement, symbol_table_t& symbols) {
		if (statement->type == st_compound) {
			compound_statement_t stmt = statement->compound_stmt;
			symbol_table_t new_symbols(&symbols);
			for (int i = 0; i < stmt.statements.size(); i++) {
				compile_statement(stmt.statements[i], new_symbols);
			}
		} else if (statement->type == st_conditional) {
			conditional_statement_t stmt = statement->conditional_stmt;
			long l0 = label++;
			compile_expression(stmt.condition, symbols);
			emit("    cmpq    $0, %%rax\n");
			emit("    je      L%ld\n", l0);
			symbol_table_t new_symbols(&symbols);
			compile_statement(stmt.body, new_symbols);
			emit("L%ld:\n", l0);
		} else if (statement->type == st_while) {
			while_statement_t stmt = statement->while_stmt;
			long l0 = label++;
			long l1 = label++;
			emit("L%ld:\n", l0);
			compile_expression(stmt.condition, symbols);
			emit("    cmpq    $0, %%rax\n");
			emit("    je      L%ld\n", l1);
			symbol_table_t new_symbols(&symbols);
			new_symbols.loop_break_to = l1;
			new_symbols.loop_continue_to = l0;
			compile_statement(stmt.body, new_symbols);
			emit("    jmp     L%ld\n", l0);
			emit("L%ld:\n", l1);
		} else if (statement->type == st_return) {
			return_statement_t stmt = statement->return_stmt;
			compile_expression(stmt.value, symbols);
			emit("    movq    %%rbp, %%rsp\n");
			emit("    popq    %%rbp\n");
			emit("    retq\n");
		} else if (statement->type == st_variable_declaration) {
			variable_declaration_statement_t stmt = statement->variable_declaration_stmt;
			symbols.add_symbol(symbol_t(stmt.type, stmt.identifier, symbols.offset -= 8));
			if (stmt.initializer) {
				compile_expression(stmt.initializer, symbols);
				emit("    movq    %%rax, %ld(%%rbp)\n", symbols.fetch(stmt.identifier).offset);
			}
		} else if (statement->type == st_expression) {
			expression_statement_t stmt = statement->expression_stmt;
			compile_expression(stmt.expression, symbols);
		} else if (statement->type == st_break) {
			emit("    jmp     L%ld\n", symbols.loop_break_to);
		} else if (statement->type == st_continue) {
			emit("    jmp     L%ld\n", symbols.loop_continue_to);
		}
	}

	// Compile a function.
	void compile_function(function_t function, symbol_table_t& symbols) {
		#ifdef __APPLE__
		emit(".globl _%s\n", function.identifier.c_str());
		emit("_%s:\n", function.identifier.c_str());
		#else
		emit(".globl %s\n", function.identifier.c_str());
		emit("%s:\n", function.identifier.c_str());
		#endif

		emit("    pushq   %%rbp\n");
		emit("    movq    %%rsp, %%rbp\n");
		emit("    subq    $%ld, %%rsp\n", aligned_offset(function, symbols));
		emit("    andq    $-16, %%rsp\n");

		symbol_table_t new_symbols(&symbols);
		const char* registers[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
		for (int i = 0; i < function.parameters.size() && i < 6; i++) {
			emit("    movq    %s, %d(%%rbp)\n", registers[i], i * -8 - 8);
			new_symbols.add_symbol(symbol_t(
				function.parameters[i].type,
				function.parameters[i].identifier,
				i * -8 - 8
			));
		}
		if (function.parameters.size() > 6) {
			for (int i = 6; i < function.parameters.size(); i++) {
				new_symbols.add_symbol(symbol_t(
					function.parameters[i].type,
					function.parameters[i].identifier,
					(i - 6) * 8 + 16
				));
			}
		}
		for (int i = 0; i < function.body.size(); i++) {
			compile_statement(function.body[i], new_symbols);
		}
	}

	// Compile the program.
	void compile() {
		pack_strings();
		symbol_table_t symbols;
		for (int i = 0; i < program.size(); i++) {
			function_t function = program[i];
			symbols.add_symbol(symbol_t(
				function.type,
				function.identifier,
				function.parameters
			));
			compile_function(function, symbols);
		}
	}

	// Pack all the strings in an expression.
	void pack_strings(expression_t* expression) {
		if (expression->type == et_string_literal) {
			long string_label = label++;
			expression->string_label = string_label;
			emit("S%ld:\n", string_label);
			for (int i = 0; i < expression->string_literal.size(); i++) {
				emit("    .quad   %u\n", (unsigned char)expression->string_literal[i]);
			}
			emit("    .quad   0\n");
		} else if (expression->type == et_function_call) {
			function_call_expression_t expr = expression->function_call;
			for (int i = 0; i < expr.arguments.size(); i++) {
				pack_strings(expr.arguments[i]);
			}
		} else if (expression->type == et_binary) {
			pack_strings(expression->binary.left_operand);
			pack_strings(expression->binary.right_operand);
		} else if (expression->type == et_unary) {
			pack_strings(expression->unary.operand);
		}
	}

	// Pack all the strings in a statement.
	void pack_strings(statement_t* statement) {
		if (statement->type == st_compound) {
			compound_statement_t stmt = statement->compound_stmt;
			for (int i = 0; i < stmt.statements.size(); i++) {
				pack_strings(stmt.statements[i]);
			}
		} else if (statement->type == st_conditional) {
			pack_strings(statement->conditional_stmt.condition);
			pack_strings(statement->conditional_stmt.body);
		} else if (statement->type == st_while) {
			pack_strings(statement->while_stmt.condition);
			pack_strings(statement->while_stmt.body);
		} else if (statement->type == st_return) {
			pack_strings(statement->return_stmt.value);
		} else if (statement->type == st_variable_declaration) {
			variable_declaration_statement_t stmt = statement->variable_declaration_stmt;
			if (stmt.initializer) {
				pack_strings(stmt.initializer);
			}
		} else if (statement->type == st_expression) {
			pack_strings(statement->expression_stmt.expression);
		}
	}

	// Pack all the strings in a function.
	void pack_strings(function_t& function) {
		for (int i = 0; i < function.body.size(); i++) {
			pack_strings(function.body[i]);
		}
	}

	// Pack all the strings in the program.
	void pack_strings() {
		for (int i = 0; i < program.size(); i++) {
			pack_strings(program[i]);
		}
	}

	// Find the absolute value of the lowest RBP offset used in a statement,
	// rounded up to a multiple of 8.
	void aligned_offset_statement(statement_t* statement, symbol_table_t& symbols, long& lowest_offset) {
		if (statement->type == st_compound) {
			compound_statement_t stmt = statement->compound_stmt;
			symbol_table_t new_symbols(&symbols);
			for (int i = 0; i < stmt.statements.size(); i++) {
				aligned_offset_statement(stmt.statements[i], new_symbols, lowest_offset);
			}
		} else if (statement->type == st_conditional) {
			conditional_statement_t stmt = statement->conditional_stmt;
			symbol_table_t new_symbols(&symbols);
			aligned_offset_statement(stmt.body, new_symbols, lowest_offset);
		} else if (statement->type == st_while) {
			while_statement_t stmt = statement->while_stmt;
			symbol_table_t new_symbols(&symbols);
			aligned_offset_statement(stmt.body, new_symbols, lowest_offset);
		} else if (statement->type == st_variable_declaration) {
			variable_declaration_statement_t stmt = statement->variable_declaration_stmt;
			symbols.add_symbol(symbol_t(stmt.type, stmt.identifier, symbols.offset -= 8));
			if (symbols.offset < lowest_offset) {
				lowest_offset = symbols.offset;
			}
		}
	}

	// Find the absolute value of the lowest RBP offset used in a function,
	// rounded up to a multiple of 8.
	long aligned_offset(function_t function, symbol_table_t& symbols) {
		symbol_table_t new_symbols(&symbols);
		long lowest_offset = 0;
		for (int i = 0; i < function.body.size(); i++) {
			aligned_offset_statement(function.body[i], new_symbols, lowest_offset);
		}

		long params_offset;
		if (function.parameters.size() >= 6) {
			params_offset = 48;
		} else {
			params_offset = function.parameters.size() * 8;
		}

		long highest_offset = -lowest_offset + params_offset;
		int remainder = highest_offset % 8;
		if (!remainder) {
			return highest_offset;
		}
		return highest_offset + 8 - remainder;
	}

private:
	// The program to compile.
	program_t program;
	// The current label number.
	long label = 0;
};