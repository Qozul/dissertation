#include "BasicRenderer.h"
#include "ElementBuffer.h"
#include "UniformBuffer.h"
#include "LogicDevice.h"
#include "Descriptor.h"
#include "RendererPipeline.h"

using namespace QZL;

BasicRenderer::BasicRenderer(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, Descriptor* descriptor,
	const std::string& vertexShader, const std::string& fragmentShader)
	: RendererBase()
{
	// Setup vertex and index (element) buffer
	ElementBuffer* buf = new ElementBuffer(logicDevice->getDeviceMemory());
	/*std::vector<Vertex> vertices = { {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
									{0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
									{0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
									{-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f } };*/
	std::vector<Vertex> vertices = { {0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
									{0.4f, -0.4f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f },
									{-0.4f, -0.4f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
									{-0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f } };
	buf->addVertices(vertices.data(), vertices.size());
	std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
	};
	buf->addIndices(indices.data(), indices.size());
	buf->commit();
	elementBuffers_.push_back(buf);

	// Setup uniform buffers
	UniformBuffer* mvpBuf = new UniformBuffer(logicDevice, MemoryAllocationPattern::kDynamicResource, 0, 0,
		sizeof(ElementData), VK_SHADER_STAGE_VERTEX_BIT);
	auto layout = descriptor->makeLayout({ mvpBuf->getBinding() });
	uniformBuffers_.push_back(mvpBuf);
	size_t idx = descriptor->createSets({ layout, layout, layout });
	std::vector<VkWriteDescriptorSet> descWrites;
	for (int i = 0; i < 3; ++i) {
		descriptorSets_.push_back(descriptor->getSet(idx + i));
		descWrites.push_back(mvpBuf->descriptorWrite(descriptor->getSet(idx + i)));
	}
	descriptor->updateDescriptorSets(descWrites);

	// Pipeline
	createPipeline(logicDevice, renderPass, swapChainExtent, RendererPipeline::makeLayoutInfo(uniformBuffers_.size(), &layout), vertexShader, fragmentShader);
}

void BasicRenderer::recordFrame(const uint32_t idx, VkCommandBuffer cmdBuffer)
{
	beginFrame(cmdBuffer);

	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	ElementData ubo = {};
	ubo.modelMatrix = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	auto view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.mvpMatrix = Shared::kProjectionMatrix * view * ubo.modelMatrix;
	uniformBuffers_[0]->uploadUniformRange<ElementData>(&ubo, sizeof(ubo), 0);

	for (auto& elementBuffer : elementBuffers_) {
		VkBuffer vertexBuffers[] = { elementBuffer->getVertexBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(cmdBuffer, elementBuffer->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

		// causing VK_ERROR_DEVICE_LOST
		vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->getLayout(), 0, 1, &descriptorSets_[idx], 0, nullptr);

		vkCmdDrawIndexed(cmdBuffer, elementBuffer->indexCount(), 1, 0, 0, 0);
	}
}
