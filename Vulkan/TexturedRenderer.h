#pragma once
#include "RendererBase.h"

namespace QZL
{
	class TextureSampler;
	class DeviceMemory;
	class TextureLoader;

	class TexturedRenderer : public RendererBase<MeshInstance> {
	public:
		TexturedRenderer(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, Descriptor* descriptor,
			const std::string& vertexShader, const std::string& fragmentShader, const uint32_t entityCount);
		~TexturedRenderer();
		void recordFrame(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer) override;
		void initialise(const glm::mat4& viewMatrix) override;
	private:
		TextureLoader* textureLoader_;
		std::vector<std::pair<TextureSampler*, TextureSampler*>> textures_;
		Descriptor* descriptor_;
	};
}
