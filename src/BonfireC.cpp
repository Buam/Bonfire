#include <iostream>

#include "FileUtils.h"
#include "PreProcessor.h"
#include "Lexer.h"
#include "Assembler.h"
#include "defs.h"
#include "Parser.h"

using namespace Bonfire;

#define ERRCODE_INVALID_ARGS 1
#define ERRCODE_INVALID_FILE 2
#define ERRCODE_COMPILE 3
#define ERRCODE_GCC 4

// Print a compile error into the std::cerr stream
void print_compile_error_exit(const char* message, int64_t line) {
	std::cerr << "Compile error: " << message << " at line " << line << std::endl;
	exit(ERRCODE_COMPILE);
}

// Start indices of every line, for debugging purposes
// With this, we can determine in which line the char at our cursor is
std::vector<uint64_t> line_start_indices;

// For finding the character index a token starts at. To determine the line
std::vector<uint64_t> token_indices;

void find_line_start_indices(std::string source) {
	line_start_indices.push_back(0);
	for (int64_t i = 0; i < source.size(); i++) {
		if (source[i] == '\n') {
			++i;
			if(i != source.size()) line_start_indices.push_back(i);
		}
	}
}

int64_t find_line_by_token(int64_t token_index) {
	int64_t curr_line = 0;
	for (int64_t i = 0; i < line_start_indices.size(); i++) {
		if (line_start_indices[i] > token_indices[token_index]) {
			return curr_line + 1;
		}
		curr_line = i;
	}
}

int64_t find_line_by_char(int64_t char_index) {
	int64_t curr_line = 0;
	for (int64_t i = 0; i < line_start_indices.size(); i++) {
		if (line_start_indices[i] > char_index) {
			return curr_line + 1;
		}
		curr_line = i;
	}
}

// Arguments:
// BonfireC [-gcc] <source-file>
int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Invalid Arguments" << std::endl;
		return ERRCODE_INVALID_ARGS;
	}

	bool gcc = argc > 2 && strcmp(argv[1], "-gcc") == 0;

	// Load source file
	char* file_contents;
	int64_t length;
	if (FileUtils::load_file(argv[argc - 1], file_contents, length) != 0 || !file_contents) {
		// An error occured while loading the file
		return ERRCODE_INVALID_FILE;
	}
	std::string source = PreProcessor::process(file_contents);
	
	find_line_start_indices(source);

	std::vector<Token> tokens;
	std::vector<FunctionDef> functions;
	try {
		// Tokenize
		Lexer::tokenize(source, tokens, token_indices);

		// Parse
		ProgramST* program = Parser::parse(tokens);

		// Assemble
		std::string assembly = Assembler::assemble(program);
		//std::cout << assembly << std::endl;

		// Output .s file
		std::string asm_file_name = argv[argc - 1];
		FileUtils::change_extension(asm_file_name, ".s");
		FileUtils::write_file(asm_file_name.c_str(), assembly);

		if (gcc) {
			if (!system(NULL)) {
				// We can't execute system calls
				std::cerr << "Assembly using GCC failed" << std::endl;
				return ERRCODE_GCC;
			}

			std::string exe_file_name = argv[argc - 1];
			FileUtils::change_extension(exe_file_name, ".exe");

			// Invoke gcc to assemble the .s file
			int err_gcc = system(string_format("gcc -o %s -m32 %s", exe_file_name.c_str(), asm_file_name.c_str()).c_str());
			// Check if assembly was successful
			if (err_gcc) {
				std::cerr << "Assembly using GCC failed: Code " << err_gcc << std::endl;
				return ERRCODE_GCC;
			}
		}
	}
	catch (Parser::unexpected_token e) {
		char buf[512];
		sprintf_s(buf, "Unexpected token: '%s'", tokens[e.index].to_string());
		print_compile_error_exit(buf, find_line_by_token(e.index));
	}
	catch (Parser::unexpected_c e) {
		char buf[512];
		sprintf_s(buf, "Unexpected character: '%s'", source[e.index]);
		print_compile_error_exit(buf, find_line_by_char(e.index));
	}

	return 0;
}