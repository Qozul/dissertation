/// Author: Ralph Ridley
/// Date: 19/02/19
/// Purpose: Encapsulate and support loading meshes from .obj files.
#pragma once
#include "ElementBuffer.h"

namespace QZL
{
	class MeshLoader {
	public:
		template<typename InstType>
		static InstType* loadMesh(const std::string& meshName, ElementBuffer& eleBuf);
	private:
		static void loadMeshFromFile(const std::string& meshName, ElementBuffer& eleBuf);
		static const std::string kPath;
		static const std::string kExt;
	};
	template<typename InstType>
	inline InstType* MeshLoader::loadMesh(const std::string& meshName, ElementBuffer& eleBuf)
	{
		ENSURES(!eleBuf.isCommitted());
		if (eleBuf.contains(meshName))
			return eleBuf.getNewMeshInstance<InstType>(meshName);

		loadMeshFromFile(meshName, eleBuf);

		DEBUG_OUT("Loaded instance of Vulkan BasicMesh " << kPath << meshName << kExt);
		return eleBuf.getNewMeshInstance<InstType>(meshName);
	}
}