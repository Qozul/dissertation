#pragma once
#include "Vertex.h"
#include "../Shared/Transform.h"

namespace QZL
{
	class Texture;

	/// BasicMesh needs to provide a transform and pointers to it's data
	struct BasicMesh {
		uint32_t indexCount;
		uint32_t indexOffset;
		uint32_t vertexOffset;
	};
	struct MeshInstance {
		Shared::Transform transform;
	};
	struct TexturedMeshInstance {
		Texture* texture;
		Shared::Transform transform;
	};
	template<typename InstType>
	inline InstType* makeMeshInstance(const std::string& meshName, const BasicMesh& mesh)
	{
		static_assert(std::is_same<InstType, MeshInstance>::value ||
			std::is_same<InstType, TexturedMeshInstance>::value,
			"Type not allowed");

		InstType* inst = new InstType();
		return inst;
	}
}