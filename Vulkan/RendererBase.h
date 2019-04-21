#pragma once
#include "Mesh.h"
#include "RendererPipeline.h"
#include "StorageBuffer.h"
#include "RenderStorage.h"

namespace QZL
{
	class LogicDevice;
	class Descriptor;

	struct ElementData {
		glm::mat4 modelMatrix;
		glm::mat4 mvpMatrix;
	};

	template<typename InstType>
	class RendererBase {
	public:
		RendererBase() : pipeline_(nullptr), renderStorage_(nullptr) {
			if (Shared::kProjectionMatrix[1][1] >= 0)
				Shared::kProjectionMatrix[1][1] *= -1;
		}
		virtual ~RendererBase();
		virtual void recordFrame(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer) = 0;
		virtual void recordCompute(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer);
		std::vector<VkWriteDescriptorSet> getDescriptorWrites(uint32_t frameIdx);
		virtual void initialise(const glm::mat4& viewMatrix) = 0;
		
		void createRenderStorage(ElementBuffer* buf) {
			renderStorage_ = new RenderStorage(buf);
		}
		void addMesh(const std::string& meshName, BasicMesh* mesh) {
			renderStorage_->addMesh(meshName, mesh);
		}
		void addMeshInstance(const std::string& meshName, InstType* instance) {
			renderStorage_->addInstance(meshName, instance);
		}
	protected:
		void createPipeline(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, VkPipelineLayoutCreateInfo layoutInfo,
			const std::string& vertexShader, const std::string& fragmentShader);
		void beginFrame(VkCommandBuffer cmdBuffer);

		RendererPipeline* pipeline_;
		RenderStorage* renderStorage_;
		std::vector<StorageBuffer*> storageBuffers_;
		std::vector<VkDescriptorSet> descriptorSets_;
	};

	template<typename InstType>
	inline RendererBase<InstType>::~RendererBase() {
		for (auto& buffer : storageBuffers_) {
			SAFE_DELETE(buffer);
		}
		SAFE_DELETE(renderStorage_);
		SAFE_DELETE(pipeline_);
	}

	template<typename InstType>
	inline void RendererBase<InstType>::recordCompute(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer)
	{
	}

	template<typename InstType>
	inline std::vector<VkWriteDescriptorSet> RendererBase<InstType>::getDescriptorWrites(uint32_t frameIdx)
	{
		std::vector<VkWriteDescriptorSet> writes;
		for (auto& buf : storageBuffers_)
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
