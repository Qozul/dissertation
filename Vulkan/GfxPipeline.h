#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	class GfxPipeline {
	public:
		GfxPipeline(VkDevice logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, const VkDescriptorSetLayout descriptorSetLayout, 
			const std::string& vname, const std::string& fname);
		~GfxPipeline();
		VkPipeline getPipeline();
		VkPipelineLayout getLayout();
	private:
		VkDevice cLogicDevice_;
		VkPipelineLayout layout_;
		VkPipeline pipeline_;
	};
}
