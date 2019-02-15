#include "RendererBase.h"
#include "LogicDevice.h"
#include "ElementBuffer.h"
#include "RendererPipeline.h"

using namespace QZL;

RendererBase::~RendererBase() {
	for (auto& buffer : elementBuffers_) {
		SAFE_DELETE(buffer);
	}
	SAFE_DELETE(pipeline_);
}

void RendererBase::createPipeline(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent,
	VkPipelineLayoutCreateInfo layoutInfo, const std::string& vertexShader, const std::string& fragmentShader)
{
	pipeline_ = new RendererPipeline(logicDevice, renderPass, swapChainExtent, layoutInfo, vertexShader, fragmentShader);
}

void RendererBase::beginFrame(VkCommandBuffer cmdBuffer)
{
	EXPECTS(pipeline_ != nullptr);
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->getPipeline());
}
