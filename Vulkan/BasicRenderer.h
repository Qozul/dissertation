#pragma once
#include "RendererBase.h"

namespace QZL
{
	class BasicRenderer : public RendererBase {
	public:
		BasicRenderer(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent,
			const std::string& vertexShader, const std::string& fragmentShader);
		void recordFrame(const uint32_t idx, VkCommandBuffer cmdBuffer) override;
	};
}
