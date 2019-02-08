#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	class GfxPipeline;
	class Image2D;
	struct SwapChainDetails;

	class RenderPass {
	public:
		RenderPass(VkPhysicalDevice physicalDevice, VkDevice logicDevice, SwapChainDetails& swapChainDetails, VkCommandPool commandPool, VkQueue queue, 
			const VkDescriptorSetLayout descriptorSetLayout);
		~RenderPass();
		VkRenderPass get();
		std::vector<VkFramebuffer> framebuffers_;
		std::vector<GfxPipeline*> gfxPipelines_;
	private:
		void createFramebuffers(SwapChainDetails& swapChainDetails);
		void createColourImage(VkPhysicalDevice physicalDevice, VkDevice logicDevice, SwapChainDetails& swapChainDetails, VkCommandPool commandPool, VkQueue queue);
		VkFormat createDepthImage(VkPhysicalDevice physicalDevice, VkDevice logicDevice, SwapChainDetails& swapChainDetails, VkCommandPool commandPool, VkQueue queue);

		VkRenderPass renderPass_;

		Image2D* colourImage_;
		Image2D* depthImage_;

		VkDevice cLogicDevice_;
		SwapChainDetails& cSwapChainDetails_;
	};
}
