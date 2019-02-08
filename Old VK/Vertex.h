/// Author: Ralph Ridley
/// Date: 29/10/18
/// Purpose: Define vertex data blocks and methods for their creation and usage
#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	/// Utility class for common vertex behaviour
	class VertexUtility {
		friend struct Vertex;
		friend struct VertexDataPTN;
	private:
		/// Preconditions, see https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkVertexInputBindingDescription.html
		static VkVertexInputBindingDescription fillBindDesc(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);
		/// Preconditions, see https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkVertexInputAttributeDescription.html
		static void fillAttribDescs(VkVertexInputAttributeDescription& attribDesc, uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);
	};

	struct Vertex {
		glm::vec3 position;
		//float padding;
		glm::vec3 colour;

		static VkVertexInputBindingDescription getBindDesc(uint32_t binding, VkVertexInputRate inputRate)
		{
			// Describes at which rate to load data from memory (instance/vertex) and number of bytes between data entries
			return VertexUtility::fillBindDesc(binding, sizeof(Vertex), inputRate);
		}
		static std::array<VkVertexInputAttributeDescription, 2> getAttribDescs(uint32_t binding)
		{
			// Describes how to extract a vertex attribute from a chunk of vertex data
			std::array<VkVertexInputAttributeDescription, 2> attribDescs;
			VertexUtility::fillAttribDescs(attribDescs[0], 0, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position));
			VertexUtility::fillAttribDescs(attribDescs[1], 1, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, colour));
			return attribDescs;
		}
	};
}
