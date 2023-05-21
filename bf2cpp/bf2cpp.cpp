#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <algorithm>
#include <cstring>
#include <array>

// Init memory
constexpr std::size_t mem_size = 30000;
std::array<uint8_t, mem_size> mem = { 0 };
uint8_t* ptr = mem.data();

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "No input file supplied.\nUse \"bf2cpp --help\" for more information\n";
		return 1;
	}

	if (std::string(argv[1]) == "--help")
	{
		std::cout << "Available commands:\n\n\t--help\t\tDisplays this screen\n\t--version\tDisplays installed version\n";
		return 1;
	}

	if (std::string(argv[1]) == "--version")
	{
		std::cout << "bf2cpp 1.0\nhttps://github.com/stumburs\n";
		return 1;
	}

	// Create input file
	std::string file_path = argv[1];
	std::ifstream input_file(file_path);
	std::string output_file_path = file_path.substr(0, file_path.find_last_of('.')) + ".cpp";
	std::ofstream output_file(output_file_path);

	// Check if file successfully opened
	if (!input_file.is_open())
	{
		std::cout << "Failed to open file.\n";
		return 1;
	}

	// Preprocess file to remove non-Brainfuck characters
	std::string program((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	std::erase_if(program, [](char c)
		{
			return !strchr("><+-.,[]", c);
		});
	input_file.close();

	// Initialize boilerplate code
	output_file << "#include <iostream>\n";
	output_file << "#include <array>\n\n";
	output_file << "constexpr std::size_t mem_size = 30000;\n";
	output_file << "std::array<uint8_t, mem_size> mem = {0};\n";
	output_file << "uint8_t* ptr = mem.data();\n\n";
	output_file << "int main()\n";
	output_file << "{\n";

	// Transpile program and write to file
	std::size_t i = 0;
	std::size_t indent_level = 1;
	while (i < program.length())
	{
		switch (program[i])
		{
		case '>':
			output_file << std::string(indent_level, '\t') << "++ptr;\n";
			break;
		case '<':
			output_file << std::string(indent_level, '\t') << "--ptr;\n";
			break;
		case '+':
			output_file << std::string(indent_level, '\t') << "++*ptr;\n";
			break;
		case '-':
			output_file << std::string(indent_level, '\t') << "--*ptr;\n";
			break;
		case '.':
			output_file << std::string(indent_level, '\t') << "std::cout << *ptr;\n";
			break;
		case ',':
			output_file << std::string(indent_level, '\t') << "std::cin >> *ptr;\n";
			break;
		case '[':
			output_file << std::string(indent_level, '\t') << "while (*ptr != 0)\n" << std::string(indent_level, '\t') << "{\n";
			indent_level++;
			break;
		case ']':
			indent_level--;
			output_file << std::string(indent_level, '\t') << "}\n";
		default:
			break;
		}
		++i;
	}

	// Close main function
	output_file << "}\n";
	output_file.close();
	return 0;
}