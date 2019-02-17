#include "RendererBase.h"
#include "LogicDevice.h"
#include "ElementBuffer.h"
#include "UniformBuffer.h"
#include "RendererPipeline.h"

using namespace QZL;

RendererBase::~RendererBase() {
	for (auto& buffer : uniformBuffers_) {
		SAFE_DELETE(buffer);
	}
	for (auto& buffer : elementBuffers_) {
		SAFE_DELETE(buffer);
	}
	SAFE_DELETE(pipeline_);
}

std::vector<VkWriteDescriptorSet> RendererBase::getDescriptorWrites(uint32_t frameIdx)
{
	std::vector<VkWriteDescriptorSet> writes;
	for (auto& buf : uniformBuffers_)
		writes.push_back(buf->descriptorWrite(descriptorSets_[frameIdx]));
	return writes;
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
