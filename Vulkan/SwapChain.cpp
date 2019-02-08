#include "SwapChain.h"
#include "LogicDevice.h"

using namespace QZL;

SwapChain::SwapChain(GLFWwindow* window, VkSurfaceKHR surface, LogicDevice* logicDevice, DeviceSurfaceCapabilities& surfaceCapabilities)
	: logicDevice_(logicDevice)
{
	initSwapChain(window, surfaceCapabilities);
	initSwapChainImages(window, surface, surfaceCapabilities);
	initImageViews();
}

SwapChain::~SwapChain()
{
	for (auto view : details_.imageViews) {
		vkDestroyImageView(logicDevice_->getLogicDevice(), view, nullptr);
	}
	vkDestroySwapchainKHR(logicDevice_->getLogicDevice(), details_.swapChain, nullptr);
}

void SwapChain::initSwapChain(GLFWwindow* window, DeviceSurfaceCapabilities& surfaceCapabilities)
{
	details_.surfaceFormat = chooseFormat(surfaceCapabilities.formats);
	VkPresentModeKHR presentMode = choosePresentMode(surfaceCapabilities.presentModes);
	details_.extent = chooseExtent(window, surfaceCapabilities.capabilities);
}

void SwapChain::initSwapChainImages(GLFWwindow* window, VkSurfaceKHR surface, DeviceSurfaceCapabilities& surfaceCapabilities)
{
	// Attempt to support triple buffering, request 3 images
	uint32_t imageCount = surfaceCapabilities.capabilities.minImageCount + 1;
	if (surfaceCapabilities.capabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.capabilities.maxImageCount)
		imageCount = surfaceCapabilities.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = details_.surfaceFormat.format;
	createInfo.imageColorSpace = details_.surfaceFormat.colorSpace;
	createInfo.imageExtent = details_.extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (logicDevice_->getFamilyIndex(QueueFamilyType::kGraphicsQueue) !=
		logicDevice_->getFamilyIndex(QueueFamilyType::kPresentationQueue)) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = static_cast<uint32_t>(QueueFamilyType::kNumQueueFamilyTypes);
		createInfo.pQueueFamilyIndices = logicDevice_->getAllIndices().data();
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = surfaceCapabilities.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = details_.presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE; // This NEEDS to be used if the swap chain has to be recreated

	CHECK_VKRESULT(vkCreateSwapchainKHR(logicDevice_->getLogicDevice(), &createInfo, nullptr, &details_.swapChain));

	details_.images = obtainVkData<VkImage>(vkGetSwapchainImagesKHR, logicDevice_->getLogicDevice(), details_.swapChain);
}

void SwapChain::initImageViews()
{
	// Need an image view for each image so that the image can be viewed
	details_.imageViews.resize(details_.images.size());
	for (auto i = 0; i < details_.images.size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = details_.images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = details_.surfaceFormat.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		CHECK_VKRESULT(vkCreateImageView(logicDevice_->getLogicDevice(), &createInfo, nullptr, &details_.imageViews[i]));
	}
}
VkSurfaceFormatKHR SwapChain::chooseFormat(std::vector<VkSurfaceFormatKHR>& formats)
{
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	for (const auto& format : formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}
	return formats[0];
}

VkPresentModeKHR SwapChain::choosePresentMode(std::vector<VkPresentModeKHR>& presentModes)
{
	for (const auto& mode : presentModes) {
		// Mailbox for triple buffering is best
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
			return mode;
	}
	// Only mode guaranteed to be available
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseExtent(GLFWwindow* window, VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
		};
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}
