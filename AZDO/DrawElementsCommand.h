#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace AZDO
	{
		struct DrawElementsCommand {
			GLuint indexCount; // Num indices of mesh
			GLuint instanceCount; // Number of instances of the mesh
			GLuint firstIndex; // The offset of the indices in the index buffer of the mesh
			GLuint baseVertex; // The offset of the vertices in the vertex buffer of the mesh
			GLuint baseInstance; // Encodes the index in to an array of instance data, the final index is gl_BaseInstance + gl_InstanceID
			// Note that baseInstance should be 0 if gl version is less than 4.2 TODO
			DrawElementsCommand(GLuint idxCount, GLuint instCount, GLuint firstIdx, GLuint baseVert, GLuint baseInst)
				: indexCount(idxCount), instanceCount(instCount), firstIndex(firstIdx), baseVertex(baseVert), baseInstance(baseInst) {}
		};
	}
}
