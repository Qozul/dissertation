#pragma once
#include "RendererBase.h"

namespace QZL
{
	class ComputePipeline;

	class ComputeRenderer : public RendererBase<MeshInstance> {
	public:
		ComputeRenderer(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, Descriptor* descriptor,
			const std::string& vertexShader, const std::string& fragmentShader);
		void recordFrame(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer) override;
		void initialise(const glm::mat4& viewMatrix) override;
		void recordCompute(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer) override;
	private:
		ComputePipeline* computePipeline_;
	};
}
