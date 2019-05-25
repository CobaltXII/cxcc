#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include "parser/parser.hpp"
#include "semantic/semantic_analyzer.hpp"
#include "compiler/compiler.hpp"

// Entry point.
int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
		exit(1);
	}

	// Open the file.
	std::ifstream file_stream(argv[1]);
	if (!file_stream.is_open()) {
		std::cerr << "Could not open file \"" << argv[1] << "\"." << std::endl;
		exit(1);
	}

	// Load the file.
	std::string file_content(
		(std::istreambuf_iterator<char>(file_stream)),
		(std::istreambuf_iterator<char>())
	);

	// Parse the file (implicity lexes the file).
	program_t program = parser_t(argv[1], file_content).parse();

	// Validate the program.
	semantic_analyzer_t semantic_analyzer(argv[1], file_content);
	semantic_analyzer.validate(program);

	// Compile the program.
	compiler_t compiler(program);
	compiler.compile();

	// Exit successfully.
	exit(EXIT_SUCCESS);
}