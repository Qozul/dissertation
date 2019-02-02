/// Author: Ralph Ridley
/// Date: 15/01/19
/// Purpose: Give all mesh types in one place for easy comparison and testing
///		Data order is important for optimal byte boundary alignment.
#pragma once
#include "Transform.h"
#include "Texture.h"

namespace QZL
{
	namespace Naive
	{
		/// BasicMesh needs to provide a transform and suitable access to its own buffer data
		struct BasicMesh {
			GLuint vaoId;
			GLsizei indexCount;
			Shared::Transform transform;
		};
		/// Only difference is it now stores a texture
		struct TexturedBasicMesh {
			GLuint vaoId;
			GLsizei indexCount;
			Texture* texture; // Just a single texture
			Shared::Transform transform;
		};

		// Input mesh is deleted by this function
		TexturedBasicMesh* basicToTextured(BasicMesh* bmesh, Texture* texture);
	}
	namespace AZDO
	{
		/// BasicMesh needs to provide a transform and pointers to it's data
		struct BasicMesh {
			GLuint indexCount;
			GLuint indexOffset;
			GLuint vertexOffset;
		};
		struct MeshInstance {
			std::string meshName;
			Shared::Transform transform;
		};
		struct TexturedMeshInstance {
			std::string meshName;
			Texture* texture;
			Shared::Transform transform;
		};
		template<typename InstType>
		inline InstType* makeMeshInstance(const std::string& meshName, const BasicMesh& mesh)
		{
			static_assert(std::is_same<InstType, AZDO::MeshInstance>::value ||
				std::is_same<InstType, AZDO::TexturedMeshInstance>::value,
				"Type not allowed");

			InstType* inst = new InstType();
			inst->meshName = meshName;
			return inst;
		}
	}
	namespace Vulk {
		struct Mesh {

		};
	}
}
