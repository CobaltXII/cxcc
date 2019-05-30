#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "parser/parser.hpp"
#include "semantic/semantic_analyzer.hpp"
#include "compiler/compiler.hpp"

// Print the usage text and exit.
void usage(char* exe) {
	std::cerr << "Usage: " << exe << " <in> [out [-o]]" << std::endl;
	std::cerr << "    Compiles C source <in> to x86-64 assembly and stores the    " << std::endl;
	std::cerr << "    output in [out]. If no value for [out] is provided, the     " << std::endl;
	std::cerr << "    output is stored in the file <in>.s. If the option -o is    " << std::endl;
	std::cerr << "    provided, gcc is used to assemble [out] and store the image " << std::endl;
	std::cerr << "    in [out].                                                   " << std::endl;
	exit(1);
}

// Entry point.
int main(int argc, char** argv) {
	if (argc != 2 && argc != 3 && argc != 4) {
		usage(argv[0]);
	} else if (argc == 4) {
		if (std::string(argv[3]) != "-o") {
			usage(argv[0]);
		}
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
	file_stream.close();

	// Parse the second argument.
	std::string outfile;
	if (argc == 3) {
		outfile = argv[2];
	} else if (argc == 4) {
		outfile = "tmp" + std::to_string(time(NULL)) + ".s";
	} else {
		outfile = std::string(argv[1]) + ".s";
	}

	// Open the output file.
	std::FILE* output_file = fopen(outfile.c_str(), "w");

	// Parse the file (implicity lexes the file).
	program_t program = parser_t(argv[1], file_content).parse();

	// Validate the program.
	semantic_analyzer_t semantic_analyzer(argv[1], file_content);
	semantic_analyzer.validate(program);

	// Compile the program.
	compiler_t compiler(program, output_file);
	compiler.compile();
	fclose(output_file);

	// Optionally assembly the output using gcc.
	if (argc == 4) {
		// Assemble the output.
		std::stringstream gcc_cmd;
		gcc_cmd << "gcc " << outfile << " -o " << argv[2];
		system(gcc_cmd.str().c_str());
		// Remove the temporary file.
		std::stringstream rm_cmd;
		rm_cmd << "rm -f " << outfile;
		system(rm_cmd.str().c_str());
	}

	// Exit successfully.
	exit(EXIT_SUCCESS);
}