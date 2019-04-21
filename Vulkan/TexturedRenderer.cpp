#include "TexturedRenderer.h"
#include "ElementBuffer.h"
#include "StorageBuffer.h"
#include "LogicDevice.h"
#include "Descriptor.h"
#include "TextureSampler.h"
#include "TextureLoader.h"
#include "DeviceMemory.h"
#include "RendererPipeline.h"

#define NEW_TEXTURE0(str) new TextureSampler(logicDevice, textureLoader_->loadTexture(str), VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 8, 1)
#define NEW_TEXTURE1(str) new TextureSampler(logicDevice, textureLoader_->loadTexture(str), VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 8, 3)

using namespace QZL;

TexturedRenderer::TexturedRenderer(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, Descriptor* descriptor,
	const std::string& vertexShader, const std::string& fragmentShader, const uint32_t entityCount)
	: RendererBase(), textureLoader_(new TextureLoader(logicDevice, logicDevice->getDeviceMemory())), descriptor_(descriptor)
{
	if (entityCount > 0) {
		textures_.push_back({ NEW_TEXTURE0(Shared::kTextureNames[0].first), NEW_TEXTURE1(Shared::kTextureNames[0].second) });
		textures_.push_back({ NEW_TEXTURE0(Shared::kTextureNames[1].first), NEW_TEXTURE1(Shared::kTextureNames[1].second) });
		textures_.push_back({ NEW_TEXTURE0(Shared::kTextureNames[2].first), NEW_TEXTURE1(Shared::kTextureNames[2].second) });

		StorageBuffer* mvpBuf = new StorageBuffer(logicDevice, MemoryAllocationPattern::kDynamicResource, 0, 0,
			sizeof(ElementData) * entityCount, VK_SHADER_STAGE_VERTEX_BIT);
		auto layout = descriptor->makeLayout({ mvpBuf->getBinding(), textures_[0].first->getBinding(), textures_[0].second->getBinding() });
		storageBuffers_.push_back(mvpBuf);
		size_t idx = descriptor->createSets({ layout, layout, layout });
		std::vector<VkWriteDescriptorSet> descWrites;
		for (int i = 0; i < 3; ++i) {
			descriptorSets_.push_back(descriptor->getSet(idx + i));
			descWrites.push_back(mvpBuf->descriptorWrite(descriptor->getSet(idx + i)));
		}
		descriptor->updateDescriptorSets(descWrites);

		createPipeline(logicDevice, renderPass, swapChainExtent, RendererPipeline::makeLayoutInfo(storageBuffers_.size(), &layout), vertexShader, fragmentShader);
	}
	else {
		createPipeline(logicDevice, renderPass, swapChainExtent, RendererPipeline::makeLayoutInfo(0, nullptr), vertexShader, fragmentShader);
	}
}

TexturedRenderer::~TexturedRenderer()
{
	for (auto texPair : textures_) {
		SAFE_DELETE(texPair.first);
		SAFE_DELETE(texPair.second);
	}
	SAFE_DELETE(textureLoader_);
}

void TexturedRenderer::initialise(const glm::mat4& viewMatrix)
{
	if (renderStorage_->instanceCount() == 0)
		return;
	if (Shared::kProjectionMatrix[1][1] >= 0)
		Shared::kProjectionMatrix[1][1] *= -1;
	ElementData* eleDataPtr = static_cast<ElementData*>(storageBuffers_[0]->bindRange());
	auto instPtr = renderStorage_->instanceData();
	for (size_t i = 0; i < renderStorage_->instanceCount(); ++i) {
		glm::mat4 model = (instPtr + i)->transform.toModelMatrix();
		eleDataPtr[i] = {
			model, Shared::kProjectionMatrix * viewMatrix * model
		};
	}
	storageBuffers_[0]->unbindRange();
}

void TexturedRenderer::recordFrame(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer)
{
	if (renderStorage_->instanceCount() == 0)
		return;
	beginFrame(cmdBuffer);

	renderStorage_->buf()->bind(cmdBuffer);
	for (int i = 0; i < renderStorage_->meshCount(); ++i) {
		std::vector<VkWriteDescriptorSet> descWrites;
		descWrites.push_back(textures_[i].first->descriptorWrite(descriptorSets_[idx]));
		descWrites.push_back(textures_[i].second->descriptorWrite(descriptorSets_[idx]));
		descriptor_->updateDescriptorSets(descWrites);
		vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->getLayout(), 0, 1, &descriptorSets_[idx], 0, nullptr);

		const DrawElementsCommand& drawElementCmd = renderStorage_->meshData()[i];
		vkCmdDrawIndexed(cmdBuffer, drawElementCmd.indexCount, drawElementCmd.instanceCount, drawElementCmd.firstIndex, drawElementCmd.baseVertex, drawElementCmd.baseInstance);
	}
}
