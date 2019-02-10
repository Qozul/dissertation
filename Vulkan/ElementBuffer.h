#pragma once
#include "VkUtil.h"
#include "MemoryAllocation.h"
#include "../Shared/Vertex.h"

namespace QZL
{
	class DeviceMemory;

	// Element buffer is immutable, once committed it cannot be changed
	class ElementBuffer {
	public:
		ElementBuffer(DeviceMemory& deviceMemory);
		~ElementBuffer();
		void commit();
		size_t addVertices(Shared::Vertex* data, const size_t size);
		size_t addIndices(uint16_t* data, const size_t size);
		VkBuffer getVertexBuffer();
		VkBuffer getIndexBuffer();
	private:
		MemoryAllocationDetails vertexBufferDetails_;
		MemoryAllocationDetails indexBufferDetails_;
		DeviceMemory& deviceMemory_;
		
		uint32_t indexCount_;
		std::vector<uint16_t> indices_;
		std::vector<Shared::Vertex> vertices_;
		bool isCommitted_;
	};
}
