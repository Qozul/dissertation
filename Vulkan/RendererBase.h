#pragma once
#include "Mesh.h"
#include "RendererPipeline.h"
#include "UniformBuffer.h"

namespace QZL
{
	class LogicDevice;
	class ElementBuffer;
	class Descriptor;

	struct ElementData {
		glm::mat4 modelMatrix;
		glm::mat4 mvpMatrix;
	};

	template<typename InstType>
	class RendererBase {
	public:
		RendererBase() : pipeline_(nullptr) {
			// Vulkan origin is changed so need to flip axis
			Shared::kProjectionMatrix[1][1] *= -1;
		}
		virtual ~RendererBase();
		virtual void recordFrame(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer) = 0;
		std::vector<VkWriteDescriptorSet> getDescriptorWrites(uint32_t frameIdx);

		void addMesh(ElementBuffer* buf, const std::string& meshName, InstType* instance) {
			meshes_[buf][meshName].push_back(instance);
		}
	protected:
		void createPipeline(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, VkPipelineLayoutCreateInfo layoutInfo,
			const std::string& vertexShader, const std::string& fragmentShader);
		void beginFrame(VkCommandBuffer cmdBuffer);

		RendererPipeline* pipeline_;
		std::map<ElementBuffer*, std::map<std::string, std::vector<InstType*>>> meshes_;
		std::vector<UniformBuffer*> uniformBuffers_;
		std::vector<VkDescriptorSet> descriptorSets_;
	};

	template<typename InstType>
	inline RendererBase<InstType>::~RendererBase() {
		for (auto& buffer : uniformBuffers_) {
			SAFE_DELETE(buffer);
		}
		SAFE_DELETE(pipeline_);
	}

	template<typename InstType>
	inline std::vector<VkWriteDescriptorSet> RendererBase<InstType>::getDescriptorWrites(uint32_t frameIdx)
	{
		std::vector<VkWriteDescriptorSet> writes;
		for (auto& buf : uniformBuffers_)
			writes.push_back(buf->descriptorWrite(descriptorSets_[frameIdx]));
		return writes;
	}

	template<typename InstType>
	inline void RendererBase<InstType>::createPipeline(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent,
		VkPipelineLayoutCreateInfo layoutInfo, const std::string& vertexShader, const std::string& fragmentShader)
	{
		pipeline_ = new RendererPipeline(logicDevice, renderPass, swapChainExtent, layoutInfo, vertexShader, fragmentShader);
	}

	template<typename InstType>
	inline void RendererBase<InstType>::beginFrame(VkCommandBuffer cmdBuffer)
	{
		EXPECTS(pipeline_ != nullptr);
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->getPipeline());
	}
}
