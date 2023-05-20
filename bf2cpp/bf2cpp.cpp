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
	//if (argc < 2)
	//{
	//	std::cout << "No input file supplied.\nUse \"bf2cpp --help\" for more information\n";
	//	return 1;
	//}

	//if (std::string(argv[1]) == "--help")
	//{
	//	std::cout << "Available commands:\n\n\t--help\t\tDisplays this screen\n\t--version\tDisplays installed version\n";
	//	return 1;
	//}

	//if (std::string(argv[1]) == "--version")
	//{
	//	std::cout << "bf2cpp 1.0\nhttps://github.com/stumburs\n";
	//	return 1;
	//}

	// Create input file
	//std::string file_path = argv[1];
	std::string file_path = "hello_world.bf";
	std::ifstream input_file(file_path);
	std::ofstream output_file("out.cpp");

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
	std::stack<std::streampos> loop_stack; // stack to keep track of loop positions
	std::size_t i = 0;
	while (i < program.length())
	{
		switch (program[i])
		{
		case '>':
			++ptr;
			output_file << "\t++ptr;\n";
			break;
		case '<':
			--ptr;
			output_file << "\t--ptr;\n";
			break;
		case '+':
			++ * ptr;
			output_file << "\t++*ptr;\n";
			break;
		case '-':
			-- * ptr;
			output_file << "\t--*ptr;\n";
			break;
		case '.':
			output_file << "\tstd::cout << *ptr;\n";
			break;
		case ',':
			output_file << "\tstd::cin >> *ptr;\n";
			break;
		case '[':
			if (*ptr == 0)
			{
				// Skip loop body
				int nested_loops = 1;
				while (nested_loops > 0)
				{
					++i;
					if (program[i] == '[')
						++nested_loops;
					else if (program[i] == ']')
						--nested_loops;
				}
			}
			else
				loop_stack.push(i);
			break;
		case ']':
			if (*ptr != 0)
				i = loop_stack.top();
			else
				loop_stack.pop();
			break;
		default:
			break;
		}
		++i;
		if (loop_stack.size() == 0 && i >= program.length())
			break; // exit loop if all loops have been exited
	}

	// Close main function
	output_file << "}\n";
	output_file.close();
	return 0;
}