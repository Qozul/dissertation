/// Author: Ralph Ridley
/// Date: 31/12/18
#include "System.h"

constexpr auto kHoldConsole = false;

using namespace QZL;

int main()
{
	System system;
	system.loop();
	if (kHoldConsole) {
		int x;
		std::cout << "Enter any value to exit the program." << std::endl;
		std::cin >> x;
	}
	return EXIT_SUCCESS;
}
