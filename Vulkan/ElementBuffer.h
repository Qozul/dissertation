#pragma once
#include "VkUtil.h"
#include "MemoryAllocation.h"
#include "Vertex.h"

namespace QZL
{
	class DeviceMemory;

	// Element buffer is immutable, once committed it cannot be changed
	class ElementBuffer {
	public:
		ElementBuffer(DeviceMemory* deviceMemory);
		~ElementBuffer();
		void commit();
		size_t addVertices(Vertex* data, const size_t size);
		size_t addIndices(uint16_t* data, const size_t size);
		VkBuffer getVertexBuffer();
		VkBuffer getIndexBuffer();
		uint32_t indexCount();
	private:
		MemoryAllocationDetails vertexBufferDetails_;
		MemoryAllocationDetails indexBufferDetails_;
		DeviceMemory* deviceMemory_;
		
		uint32_t indexCount_;
		std::vector<uint16_t> indices_;
		std::vector<Vertex> vertices_;
		bool isCommitted_;
	};
}
