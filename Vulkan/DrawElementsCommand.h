#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	struct DrawElementsCommand {
		uint32_t indexCount;
		uint32_t instanceCount;
		uint32_t firstIndex;
		uint32_t baseVertex;
		uint32_t baseInstance; 
		DrawElementsCommand(uint32_t idxCount, uint32_t instCount, uint32_t firstIdx, uint32_t baseVert, GLuint baseInst)
			: indexCount(idxCount), instanceCount(instCount), firstIndex(firstIdx), baseVertex(baseVert), baseInstance(baseInst)
		{
		}
	};
}
