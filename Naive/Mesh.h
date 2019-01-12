/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once
//#include "Utility.h"
#include "../Shared/Utility.h"

namespace QZL
{
	struct Transform {
		glm::vec3 position;
		glm::vec3 rotation;
		float scale;
	};

	namespace Naive
	{
		struct Mesh {
			Transform transform;
			GLuint vaoId;
			GLint indexCount;
		};
	}
	
	namespace AZDO
	{
		struct DrawCommand {
			GLuint vertexCount;
			GLuint instanceCount;
			GLuint firstIndex;
			GLuint baseVertex;
			GLuint baseInstance;
		};

		struct Mesh {
			DrawCommand cmd;
			Transform transform;
		};
	}
}
