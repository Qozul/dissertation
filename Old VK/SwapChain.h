#pragma once
#include "../Shared/Utility.h"

#include "ElementBuffer.h"
#include "UniformBuffer.h"
#include "Descriptor.h"

#define MAX_FRAMES_IN_FLIGHT 2

namespace QZL
{
	class Device;
	class RenderPass;

	struct SwapChainDetails {
		VkSwapchainKHR swapChain;
		VkFormat format;
		VkExtent2D extent;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
	};
	struct DeviceSwapChainDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	class SwapChain {
	public:
		SwapChain(const Device* device, VkSurfaceKHR surface, GLFWwindow* window, DeviceSwapChainDetails details, VkCommandPool primaryCommandPool);
		~SwapChain();
		void loop();
	private:
		void createSwapChain(const Device* device, VkSurfaceKHR surface, GLFWwindow* window, DeviceSwapChainDetails details);
		void createImageViews(VkDevice logicDevice);
		VkSurfaceFormatKHR chooseFormat(std::vector<VkSurfaceFormatKHR>& formats);
		VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR>& presentModes);
		VkExtent2D chooseExtent(GLFWwindow* window, VkSurfaceCapabilitiesKHR& capabilities);
		void allocateCommandBuffers(VkCommandPool primaryCommandPool);
		void createSyncObjects();
		void updateUniformBuffer(uint32_t currentImage);

		const Device* cDevice_;
		VkQueue cGraphicsQueue_;
		VkQueue cPresentQueue_;

		SwapChainDetails details_;
		RenderPass* renderPass_;
		VkCommandPool primaryCommandPool_;
		std::vector<VkCommandBuffer> commandBuffers_;

		VertexBuffer vertexBuffer_;
		IndexBuffer indexBuffer_;

		std::vector<UniformBuffer*> mvpBuffers_;
		Descriptor* descriptor_;

		std::vector<VkSemaphore> imageAvailableSemaphores_;
		std::vector<VkSemaphore> renderFinishedSemaphores_;
		std::vector<VkFence> inFlightFences_;
		size_t currentFrame_ = 0;
		
		VkDevice cLogicDevice_;
	};
}
