#pragma once
#include <string>

// All binary operators.
enum binary_operator_t {
	bi_addition,
	bi_subtraction,
	bi_multiplication,
	bi_division,
	bi_modulo,
	bi_assignment,
	bi_logical_and,
	bi_logical_or,
	bi_relational_equal,
	bi_relational_non_equal,
	bi_relational_greater_than,
	bi_relational_lesser_than,
	bi_error
};

// All binary operators as strings.
std::string binary_operator_str[] = {
	"addition",
	"subtraction",
	"multiplication",
	"division",
	"modulo",
	"assignment",
	"logical and",
	"logical or",
	"relational equal",
	"relational non-equal",
	"relational greater-than",
	"relational lesser-than"
};

// All unary operators.
enum unary_operator_t {
	un_value_of,
	un_arithmetic_positive,
	un_arithmetic_negative,
	un_address_of,
	un_logical_not
};

// All unary operators as strings.
std::string unary_operator_str[] = {
	"value-of",
	"arithmetic positive",
	"arithmetic negative",
	"address-of",
	"logical not"
};