#include "Program.h"
#include <exception>
#include <iostream>

int main(int argumentCount, char** arguments)
{
	try
	{
		Program program;
		if (argumentCount > 1)
			program.LoadCircuit(arguments[1]);
		program.Run();
		return 0;
	}
	catch (const std::exception& error)
	{
		std::cerr << "Dewy failed to start: " << error.what() << '\n';
		return 1;
	}
}
