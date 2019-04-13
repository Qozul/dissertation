#pragma once
#include "VkUtil.h"
#include "Mesh.h"

namespace QZL
{
	class Image2D;
	class LogicDevice;
	template<typename InstType>
	class RendererBase;
	class Descriptor;
	struct SwapChainDetails;
	class ElementBuffer;

	class RenderPass {
		friend class SwapChain;
	public:
		void doFrame(const uint32_t idx, VkCommandBuffer cmdBuffer);
	private:
		RenderPass(LogicDevice* logicDevice, const SwapChainDetails& swapChainDetails);
		~RenderPass();

		void createFramebuffers(LogicDevice* logicDevice, const SwapChainDetails& swapChainDetails);
		void createBackBuffer(LogicDevice* logicDevice, const SwapChainDetails& swapChainDetails);
		VkFormat createDepthBuffer(LogicDevice* logicDevice, const SwapChainDetails& swapChainDetails);
		void createRenderers();
		void createElementBuffers();

		VkRenderPass renderPass_;
		std::vector<VkFramebuffer> framebuffers_;
		const SwapChainDetails& swapChainDetails_;
		const LogicDevice* logicDevice_;

		Descriptor* descriptor_;

		std::vector<ElementBuffer*> elementBuffers_;

		RendererBase<MeshInstance>* basicRenderer_;
		RendererBase<MeshInstance>* texturedRenderer_;
		RendererBase<MeshInstance>* loopRenderer_;
		RendererBase<MeshInstance>* computeRenderer_;
		RendererBase<MeshInstance>* computeReadbackRenderer_;

		Image2D* backBuffer_;
		Image2D* depthBuffer_;

		glm::mat4 viewMatrix_;

		static const size_t kMaxRenderers = 1;
	};
}
