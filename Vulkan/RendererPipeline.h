#pragma once
#include "VkUtil.h"

namespace QZL
{
	class LogicDevice;
	class ElementBuffer;

	class RendererPipeline {
	public:
		RendererPipeline(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent,
			const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~RendererPipeline();
		virtual void recordFrame(const uint32_t idx, VkCommandBuffer cmdBuffer) = 0;
	protected:
		void beginFrame(VkCommandBuffer cmdBuffer);
		std::vector<ElementBuffer*> elementBuffers_;

		VkPipeline pipeline_;
		VkPipelineLayout layout_;
		const LogicDevice* logicDevice_;
	};
}
