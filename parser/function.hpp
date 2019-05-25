#pragma once
#include <vector>

// A function.
struct function_t {
	type_t						type;
	identifier_t				identifier;
	std::vector<parameter_t>	parameters;
	std::vector<statement_t*>	body;
	long						lineno;
	long						colno;
};