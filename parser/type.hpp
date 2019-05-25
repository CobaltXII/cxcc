#pragma once
#include <string>

// A type.
struct type_t {
	long	pointer_depth;
};

// Check if a type can be converted to another type.
bool can_convert(type_t a, type_t b) {
	return true;
}

// Prettyprint a type.
std::string prettyprint_type(type_t type) {
	return "int" + std::string(type.pointer_depth, '*');
}