#include "BasicRenderer.h"
#include "ElementBuffer.h"
#include "LogicDevice.h"

using namespace QZL;

BasicRenderer::BasicRenderer(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent,
	const std::string& vertexShader, const std::string& fragmentShader)
	: RendererPipeline(logicDevice, renderPass, swapChainExtent, vertexShader, fragmentShader)
{
	ElementBuffer* buf = new ElementBuffer(logicDevice->getDeviceMemory());
	std::vector<Vertex> vertices = { {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
									{0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
									{0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
									{-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f } };
	buf->addVertices(vertices.data(), vertices.size());
	std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
	};
	buf->addIndices(indices.data(), indices.size());
	buf->commit();
	elementBuffers_.push_back(buf);
}

void BasicRenderer::recordFrame(const uint32_t idx, VkCommandBuffer cmdBuffer)
{
	for (auto& elementBuffer : elementBuffers_) {
		VkBuffer vertexBuffers[] = { elementBuffer->getVertexBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(cmdBuffer, elementBuffer->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

		//vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_, 0, 1, descriptor_->getSet(i), 0, nullptr);

		vkCmdDrawIndexed(cmdBuffer, elementBuffer->indexCount(), 1, 0, 0, 0);
	}
}
