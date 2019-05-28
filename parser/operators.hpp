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
	bi_addition_assignment,
	bi_subtraction_assignment,
	bi_multiplication_assignment,
	bi_division_assignment,
	bi_modulo_assignment,
	bi_logical_and,
	bi_logical_or,
	bi_relational_equal,
	bi_relational_non_equal,
	bi_relational_greater_than,
	bi_relational_lesser_than,
	bi_relational_greater_than_or_equal_to,
	bi_relational_lesser_than_or_equal_to,
	bi_binary_and,
	bi_binary_or,
	bi_binary_xor,
	bi_binary_and_assignment,
	bi_binary_or_assignment,
	bi_binary_xor_assignment,
	bi_binary_left_shift,
	bi_binary_right_shift,
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
	"addition assignment",
	"subtraction assignment",
	"multiplication assignment",
	"division assignment",
	"modulo assignment",
	"logical and",
	"logical or",
	"relational equal",
	"relational non-equal",
	"relational greater-than",
	"relational lesser-than",
	"relational greater-than or equal-to",
	"relational lesser-than or equal-to",
	"binary AND",
	"binary OR",
	"binary XOR",
	"binary AND assignment",
	"binary OR assignment",
	"binary XOR assignment"
	"binary left-shift",
	"binary right-shift"
};

// All unary operators.
enum unary_operator_t {
	un_value_of,
	un_arithmetic_positive,
	un_arithmetic_negative,
	un_address_of,
	un_logical_not,
	un_binary_not
};

// All unary operators as strings.
std::string unary_operator_str[] = {
	"value-of",
	"arithmetic positive",
	"arithmetic negative",
	"address-of",
	"logical NOT",
	"binary NOT"
};