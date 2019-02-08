#pragma once
#include "VkUtil.h"

namespace QZL
{
	class LogicDevice;
	struct DeviceSurfaceCapabilities;

	struct SwapChainDetails {
		VkSwapchainKHR swapChain;
		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;
		VkExtent2D extent;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
	};

	class SwapChain {
		friend class LogicDevice;
	private:
		SwapChain(GLFWwindow* window, VkSurfaceKHR surface, LogicDevice* logicDevice, DeviceSurfaceCapabilities& surfaceCapabilities);
		~SwapChain();

		void initSwapChain(GLFWwindow* window, DeviceSurfaceCapabilities& surfaceCapabilities);
		void initSwapChainImages(GLFWwindow* window, VkSurfaceKHR surface, DeviceSurfaceCapabilities& surfaceCapabilities);
		void initImageViews();

		VkSurfaceFormatKHR chooseFormat(std::vector<VkSurfaceFormatKHR>& formats);
		VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR>& presentModes);
		VkExtent2D chooseExtent(GLFWwindow* window, VkSurfaceCapabilitiesKHR& capabilities);

		SwapChainDetails details_;
		LogicDevice* logicDevice_;
	};
}
