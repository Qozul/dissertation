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
			template<typename InstType> 
			static InstType* loadMesh(const std::string& meshName, VaoWrapper& vao);
		private:
			static void loadMeshFromFile(const std::string& meshName, VaoWrapper& vao);
			static const std::string kPath;
			static const std::string kExt;
		};
		template<typename InstType>
		inline InstType* MeshLoader::loadMesh(const std::string& meshName, VaoWrapper& vao)
		{
			ENSURES(!vao.isCommitted());
			if (vao.contains(meshName))
				return vao.getNewMeshInstance<InstType>(meshName);

			loadMeshFromFile(meshName, vao);

			DEBUG_OUT("Loaded instance of AZDO BasicMesh " << kPath << meshName << kExt);
			return vao.getNewMeshInstance<InstType>(meshName);
		}
	}
}