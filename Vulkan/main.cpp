/// Author: Ralph Ridley
/// Date: 08/10/18
#include "System.h"
#include "../Shared/Utility.h"
#include "Shader.h"

constexpr auto kHoldConsole = false;

using namespace QZL;

int main()
{
	System system;
	system.loop();/*
	try {
#ifdef _DEBUG
		// If its debug mode make sure the shaders are up to date
		//Shader::runBatchCompilation();
#endif
		
	}
	catch (const std::exception& e) {
		std::cerr << e.what();
		int x;
		std::cin >> x;
		return EXIT_FAILURE;
	}*/
	if (kHoldConsole) {
		int x;
		std::cout << "Enter any value to exit the program." << std::endl;
		std::cin >> x;
	}
	return EXIT_SUCCESS;
}
