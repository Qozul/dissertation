#include "ComputeRenderer.h"
#include "ElementBuffer.h"
#include "UniformBuffer.h"
#include "LogicDevice.h"
#include "Descriptor.h"
#include "RendererPipeline.h"
#include "ComputePipeline.h"

using namespace QZL;

ComputeRenderer::ComputeRenderer(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, Descriptor* descriptor,
	const std::string& vertexShader, const std::string& fragmentShader)
	: RendererBase()
{
	// Setup uniform buffers
	UniformBuffer* mvpBuf = new UniformBuffer(logicDevice, MemoryAllocationPattern::kDynamicResource, 0, 0,
		sizeof(ElementData) * 10, VK_SHADER_STAGE_VERTEX_BIT);

	// TODO create a storage buffer device local for ElementData
	// TODO create a storage buffer device local, transfer for Transform data (note: readback would persistent map)

	auto layout = descriptor->makeLayout({ mvpBuf->getBinding() });
	uniformBuffers_.push_back(mvpBuf);
	size_t idx = descriptor->createSets({ layout, layout, layout });
	std::vector<VkWriteDescriptorSet> descWrites;
	for (int i = 0; i < 3; ++i) {
		descriptorSets_.push_back(descriptor->getSet(idx + i));
		descWrites.push_back(mvpBuf->descriptorWrite(descriptor->getSet(idx + i)));
	}
	descriptor->updateDescriptorSets(descWrites);

	createPipeline(logicDevice, renderPass, swapChainExtent, RendererPipeline::makeLayoutInfo(uniformBuffers_.size(), &layout), vertexShader, fragmentShader);
	computePipeline_ = new ComputePipeline(logicDevice, ComputePipeline::makeLayoutInfo(uniformBuffers_.size(), &layout), "Compute");
}

void ComputeRenderer::initialise(const glm::mat4 & viewMatrix)
{
}

void ComputeRenderer::recordCompute(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer)
{
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline_->getPipeline());
	// TODO bind descriptor sets
	uint32_t instanceCount = 1;
	vkCmdDispatch(cmdBuffer, instanceCount, 1, 1);
}

void ComputeRenderer::recordFrame(const glm::mat4 & viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer)
{
	beginFrame(cmdBuffer);

	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->getLayout(), 0, 1, &descriptorSets_[idx], 0, nullptr);
	for (auto& it : meshes_) {
		it.first->bind(cmdBuffer);

		uint32_t instanceCount = 0;
		for (auto& it2 : it.second) {
			const BasicMesh* mesh = (*it.first)[it2.first];
			vkCmdDrawIndexed(cmdBuffer, mesh->indexCount, it2.second.size(), mesh->indexOffset, mesh->vertexOffset, instanceCount);
			instanceCount += it2.second.size();
		}
	}
}
