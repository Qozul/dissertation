#pragma once
#include "VkUtil.h"

namespace QZL
{
	class LogicDevice;
	class RendererPipeline;
	class ElementBuffer;

	class RendererBase {
	public:
		RendererBase() : pipeline_(nullptr) {}
		virtual ~RendererBase();
		virtual void recordFrame(const uint32_t idx, VkCommandBuffer cmdBuffer) = 0;
	protected:
		void createPipeline(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, VkPipelineLayoutCreateInfo layoutInfo,
			const std::string& vertexShader, const std::string& fragmentShader);
		void beginFrame(VkCommandBuffer cmdBuffer);

		RendererPipeline* pipeline_;
		std::vector<ElementBuffer*> elementBuffers_;
	};
}
