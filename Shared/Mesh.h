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
		};
	}
	namespace Vulk {
		struct Mesh {

		};
	}
}
