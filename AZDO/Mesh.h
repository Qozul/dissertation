#pragma once
#include "../Shared/Transform.h"

namespace QZL
{
	namespace Shared
	{
		struct Vertex;
	}
	namespace AZDO
	{
		class Texture;

		/// BasicMesh needs to provide a transform and pointers to it's data
		struct BasicMesh {
			GLuint indexCount;
			GLuint indexOffset;
			GLuint vertexOffset;
		};
		struct MeshInstance {
			Shared::Transform transform;
		};
		struct TexturedMeshInstance {
			Texture* texture;
			Shared::Transform transform;
			//MeshInstance meshInst;
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
}
