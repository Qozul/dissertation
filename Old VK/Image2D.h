#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	struct SwapChainDetails;
	
	struct ImageParameters {
		VkMemoryPropertyFlagBits memPropertyBits;
		VkImageAspectFlagBits aspectBits;
		VkImageLayout newLayout;
		VkImageLayout oldLayout;
		uint32_t mipLevels;
	};
	struct ImageDetails {
		VkImage image;
		VkDeviceMemory memory;
		VkImageView imageView;
		VkFormat format;
	};
	class Image2D {
	public:
		static void fillImageCreateInfo(VkImageCreateInfo& createInfo, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, 
			VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		Image2D(VkPhysicalDevice physicalDevice, VkDevice logicDevice, SwapChainDetails& swapChainDetails, VkImageCreateInfo createInfo, ImageParameters params, 
			VkCommandPool commandPool, VkQueue queue, VkFormat format);
		~Image2D();
		VkImageView getImageView();
	private:
		void createImage(VkPhysicalDevice physicalDevice, VkImageCreateInfo createInfo, VkMemoryPropertyFlags properties);
		void createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		void transitionLayout(VkCommandPool commandPool, VkQueue queue, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
		uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		bool hasStencilComponent(VkFormat format);

		ImageDetails details_;
		VkDevice cLogicDevice_;
		SwapChainDetails& cSwapChainDetails_;
	};
}
