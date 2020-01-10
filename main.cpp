#include <iostream>

#include "Sectioner.h"
#include "Mediator.h"
#include "opcodes.h"


int main(int argc, char** argv)
{
	// Start by getting the wasm file from the input
	// For now only the name of the file itself is being passed.

	if (argc != 2)
	{
		std::cout << "Incorrect parameter passing. Try again with just the wasm filename." << std::endl;
		exit(1);
	}
	std::string filename = argv[1];
	std::cout << filename << std::endl;

	// TODO: Verify the file is a well-formed.
	Sectioner reader = Sectioner(filename);
	Mediator middle = Mediator(&reader);
}
