#pragma once
#include "MemoryAllocation.h"

namespace QZL
{
	class DeviceMemory;
	class LogicDevice;

	struct ImageParameters {
		VkImageAspectFlagBits aspectBits;
		VkImageLayout oldLayout;
		VkImageLayout newLayout;
	};

	class Image2D {
	public:
		Image2D(LogicDevice* logicDevice, DeviceMemory* deviceMemory, const VkImageCreateInfo createInfo, MemoryAllocationPattern pattern,
			ImageParameters imageParameters);
		~Image2D();
		static VkImageCreateInfo makeImageCreateInfo(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
			VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		const VkImageView& getImageView();
		const VkImage& getImage();

	private:
		MemoryAllocationDetails imageDetails_;
		VkImageView imageView_;

		DeviceMemory* deviceMemory_;
		LogicDevice* logicDevice_;
	};
}
