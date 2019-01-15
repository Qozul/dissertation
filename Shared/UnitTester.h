/// Author: Ralph Ridley
/// Date: 15/01/19
/// Purpose: Test the shared library classes
#pragma once

namespace QZL
{
	namespace Shared
	{
		class UnitTester {
		public:
			bool TestAll();
			bool TestTransform();
			bool TestMesh();
			bool TestMeshLoader();
			bool TestPerfMeasurer();
		};
	}
}
