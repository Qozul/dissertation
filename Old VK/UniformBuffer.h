#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	struct MVP {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
	class UniformBuffer {
	public:
		UniformBuffer(VkPhysicalDevice physicalDevice, VkDevice logicDevice);
		~UniformBuffer();
		UniformBuffer(UniformBuffer&& other) noexcept;

		void updateDescriptorSets(VkDescriptorSet set);
		VkDeviceMemory getMemory();
	private:

		VkBuffer buffer_;
		VkDeviceMemory memory_;
		VkDeviceSize size_;

		VkDevice cLogicDevice_;
	};
}
