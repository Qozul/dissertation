/// Author: Ralph Ridley
/// Date: 02/02/19
/// Purpose: Encapsulate and support loading meshes from .obj files.
#pragma once
#include "VaoWrapper.h"

namespace QZL
{
	namespace AZDO
	{
		class MeshLoader {
		public:
			static BasicMesh* loadMesh(const std::string& meshName, VaoWrapper& vao);
		private:
			static void loadMeshFromFile(const std::string& meshName, VaoWrapper& vao);
			static const std::string kPath;
			static const std::string kExt;
		};
	}
}