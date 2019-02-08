#pragma once
#include "../Shared/Utility.h"
#include "Vertex.h"

namespace QZL
{
	class VertexBuffer {
		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},

			{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}}
		};
	public:
		VertexBuffer(VkPhysicalDevice physicalDevice, VkDevice logicDevice, VkCommandPool commandPool, VkQueue queue);
		~VertexBuffer();
		VkBuffer get();
	private:
		VkBuffer buffer_;
		VkDeviceMemory memory_;
		VkDevice cLogicDevice_;
	};

	class IndexBuffer {
	public:
		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};
		IndexBuffer(VkPhysicalDevice physicalDevice, VkDevice logicDevice, VkCommandPool commandPool, VkQueue queue);
		~IndexBuffer();
		VkBuffer get();
	private:
		VkBuffer buffer_;
		VkDeviceMemory memory_;
		VkDevice cLogicDevice_;
	};
}
