#include "LoopRenderer.h"
#include "ElementBuffer.h"
#include "StorageBuffer.h"
#include "LogicDevice.h"
#include "Descriptor.h"
#include "RendererPipeline.h"

using namespace QZL;

LoopRenderer::LoopRenderer(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, Descriptor* descriptor,
	const std::string& vertexShader, const std::string& fragmentShader, const uint32_t entityCount)
	: RendererBase()
{
	// Setup uniform buffers
	StorageBuffer* mvpBuf = new StorageBuffer(logicDevice, MemoryAllocationPattern::kDynamicResource, 0, 0,
		sizeof(ElementData) * entityCount, VK_SHADER_STAGE_VERTEX_BIT);
	auto layout = descriptor->makeLayout({ mvpBuf->getBinding() });
	storageBuffers_.push_back(mvpBuf);
	size_t idx = descriptor->createSets({ layout, layout, layout });
	std::vector<VkWriteDescriptorSet> descWrites;
	for (int i = 0; i < 3; ++i) {
		descriptorSets_.push_back(descriptor->getSet(idx + i));
		descWrites.push_back(mvpBuf->descriptorWrite(descriptor->getSet(idx + i)));
	}
	descriptor->updateDescriptorSets(descWrites);

	// Pipeline
	createPipeline(logicDevice, renderPass, swapChainExtent, RendererPipeline::makeLayoutInfo(storageBuffers_.size(), &layout), vertexShader, fragmentShader);
}

void LoopRenderer::initialise(const glm::mat4& viewMatrix)
{
	if (Shared::kProjectionMatrix[1][1] >= 0)
		Shared::kProjectionMatrix[1][1] *= -1;
}

void LoopRenderer::recordFrame(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer)
{
	beginFrame(cmdBuffer);

	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->getLayout(), 0, 1, &descriptorSets_[idx], 0, nullptr);
	ElementData* eleDataPtr = static_cast<ElementData*>(storageBuffers_[0]->bindRange());
	for (auto& it : meshes_) {
		it.first->bind(cmdBuffer);

		uint32_t instanceCount = 0;
		for (auto& it2 : it.second) {
			for (int i = 0; i < it2.second.size(); ++i) {
				auto inst = it2.second[i];
				glm::mat4 model = inst->transform.toModelMatrix();
				eleDataPtr[instanceCount + i].modelMatrix = model;
				eleDataPtr[instanceCount + i].mvpMatrix = Shared::kProjectionMatrix * viewMatrix * model;
			}
			const BasicMesh* mesh = (*it.first)[it2.first];
			vkCmdDrawIndexed(cmdBuffer, mesh->indexCount, it2.second.size(), mesh->indexOffset, mesh->vertexOffset, instanceCount);
			instanceCount += it2.second.size();
		}
	}
	storageBuffers_[0]->unbindRange();
}
