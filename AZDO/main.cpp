/// Author: Ralph Ridley
/// Date: 02/02/18
#include "System.h"

constexpr auto kHoldConsole = false;

using namespace QZL;
using namespace QZL::AZDO;

int main(int argc, char** argv)
{
	int x = 1;
	int y = 1;
	int z = 1;
	if (argc > 1) x = atoi(argv[1]);
	if (argc > 2) y = atoi(argv[2]);
	if (argc > 3) z = atoi(argv[3]);

	System system(x, y, z);
	system.loop();

	if (kHoldConsole) {
		int x;
		std::cout << "Enter any value to exit the program." << std::endl;
		std::cin >> x;
	}
	return EXIT_SUCCESS;
}
