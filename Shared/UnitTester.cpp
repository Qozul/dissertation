#include "UnitTester.h"
#include "Mesh.h"

using namespace QZL;
using namespace QZL::Shared;

bool UnitTester::TestMesh()
{
	std::cout << "Naive BasicMesh size: " << sizeof(Naive::BasicMesh) << std::endl;
	std::cout << "Naive TexturedBasicMesh size: " << sizeof(Naive::TexturedBasicMesh) << std::endl;
	return true;
}
