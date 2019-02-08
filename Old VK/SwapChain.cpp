#include "SwapChain.h"
#include "Device.h"
#include "RenderPass.h"
#include "GfxPipeline.h"

using namespace QZL;

SwapChain::SwapChain(const Device* device, VkSurfaceKHR surface, GLFWwindow* window, DeviceSwapChainDetails deviceDetails, VkCommandPool primaryCommandPool)
	: cLogicDevice_(device->getLogicDevice()), cDevice_(device), cGraphicsQueue_(device->getQueueHandle(QueueFamilyType::kGraphicsQueue)),
		vertexBuffer_(device->getPhysicalDevice(), device->getLogicDevice(), primaryCommandPool, cGraphicsQueue_), indexBuffer_(device->getPhysicalDevice(), 
		device->getLogicDevice(), primaryCommandPool, cGraphicsQueue_), primaryCommandPool_(primaryCommandPool), 
		cPresentQueue_(device->getQueueHandle(QueueFamilyType::kPresentationQueue))
{
	createSwapChain(device, surface, window, deviceDetails);
	createImageViews(cLogicDevice_);

	descriptor_ = new Descriptor(device->getLogicDevice(), details_.images.size());

	// Uniforms here temporarily
	mvpBuffers_.reserve(details_.images.size());
	for (size_t i = 0; i < details_.images.size(); i++) {
		mvpBuffers_.emplace_back(device->getPhysicalDevice(), device->getLogicDevice());
		mvpBuffers_[i]->updateDescriptorSets(*descriptor_->getSet(i));
	}

	renderPass_ = new RenderPass(device->getPhysicalDevice(), cLogicDevice_, details_, primaryCommandPool, device->getQueueHandle(QueueFamilyType::kGraphicsQueue), descriptor_->getLayout());
	allocateCommandBuffers(primaryCommandPool);
	createSyncObjects();
}

SwapChain::~SwapChain()
{
	for (auto& mvp : mvpBuffers_) {
		SAFE_DELETE(mvp);
	}
	vkFreeCommandBuffers(cLogicDevice_, primaryCommandPool_, static_cast<uint32_t>(commandBuffers_.size()), commandBuffers_.data());
	SAFE_DELETE(renderPass_);
	SAFE_DELETE(descriptor_);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(cLogicDevice_, renderFinishedSemaphores_[i], nullptr);
		vkDestroySemaphore(cLogicDevice_, imageAvailableSemaphores_[i], nullptr);
		vkDestroyFence(cLogicDevice_, inFlightFences_[i], nullptr);
	}
	for (auto view : details_.imageViews) {
		vkDestroyImageView(cLogicDevice_, view, nullptr);
	}
	vkDestroySwapchainKHR(cLogicDevice_, details_.swapChain, nullptr);
}

void SwapChain::loop()
{
	vkWaitForFences(cLogicDevice_, 1, &inFlightFences_[currentFrame_], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(cLogicDevice_, details_.swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &imageIndex);
	CHECK_VKRESULT(result);
	/*if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}*/

	updateUniformBuffer(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores_[currentFrame_] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers_[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores_[currentFrame_] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(cLogicDevice_, 1, &inFlightFences_[currentFrame_]);

	CHECK_VKRESULT(vkQueueSubmit(cGraphicsQueue_, 1, &submitInfo, inFlightFences_[currentFrame_]));

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { details_.swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(cPresentQueue_, &presentInfo);
	CHECK_VKRESULT(result);
	/*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}*/

	currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}

void SwapChain::createSwapChain(const Device* device, VkSurfaceKHR surface, GLFWwindow * window, DeviceSwapChainDetails deviceDetails)
{
	VkSurfaceFormatKHR surfaceFormat = chooseFormat(deviceDetails.formats);
	VkPresentModeKHR presentMode = choosePresentMode(deviceDetails.presentModes);
	details_.extent = chooseExtent(window, deviceDetails.capabilities);
	details_.format = surfaceFormat.format;

	// Attempt to support triple buffering, request 3 images
	uint32_t imageCount = deviceDetails.capabilities.minImageCount + 1;
	if (deviceDetails.capabilities.maxImageCount > 0 && imageCount > deviceDetails.capabilities.maxImageCount)
		imageCount = deviceDetails.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = details_.extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (device->getFamilyIndex(QueueFamilyType::kGraphicsQueue) !=
		device->getFamilyIndex(QueueFamilyType::kPresentationQueue)) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = static_cast<uint32_t>(QueueFamilyType::kNumQueueFamilyTypes);
		createInfo.pQueueFamilyIndices = device->getAllIndices().data();
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = deviceDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE; // This NEEDS to be used if the swap chain has to be recreated

	CHECK_VKRESULT(vkCreateSwapchainKHR(cLogicDevice_, &createInfo, nullptr, &details_.swapChain));

	details_.images = Shared::Vulk::obtainVkData<VkImage>(vkGetSwapchainImagesKHR, cLogicDevice_, details_.swapChain);
}

void SwapChain::createImageViews(VkDevice logicDevice)
{
	// Need an image view for each image so that the image can be viewed
	details_.imageViews.resize(details_.images.size());
	for (auto i = 0; i < details_.images.size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = details_.images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = details_.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		CHECK_VKRESULT(vkCreateImageView(logicDevice, &createInfo, nullptr, &details_.imageViews[i]));
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

void SwapChain::allocateCommandBuffers(VkCommandPool primaryCommandPool)
{
	commandBuffers_.resize(renderPass_->framebuffers_.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = primaryCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers_.size();

	CHECK_VKRESULT(vkAllocateCommandBuffers(cLogicDevice_, &allocInfo, commandBuffers_.data()));

	for (size_t i = 0; i < commandBuffers_.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		CHECK_VKRESULT(vkBeginCommandBuffer(commandBuffers_[i], &beginInfo));

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass_->get();
		renderPassInfo.framebuffer = renderPass_->framebuffers_[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = details_.extent;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers_[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass_->gfxPipelines_[0]->getPipeline());

			VkBuffer vertexBuffers[] = { vertexBuffer_.get() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffers_[i], 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffers_[i], indexBuffer_.get(), 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(commandBuffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass_->gfxPipelines_[0]->getLayout(), 0, 1, descriptor_->getSet(i), 0, nullptr);

			vkCmdDrawIndexed(commandBuffers_[i], static_cast<uint32_t>(indexBuffer_.indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers_[i]);

		CHECK_VKRESULT(vkEndCommandBuffer(commandBuffers_[i]));
	}
}

void SwapChain::createSyncObjects() {
	imageAvailableSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(cLogicDevice_, &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) != VK_SUCCESS ||
			vkCreateSemaphore(cLogicDevice_, &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != VK_SUCCESS ||
			vkCreateFence(cLogicDevice_, &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void SwapChain::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	MVP ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), details_.extent.width / (float)details_.extent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	void* data;
	vkMapMemory(cLogicDevice_, mvpBuffers_[currentImage]->getMemory(), 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(cLogicDevice_, mvpBuffers_[currentImage]->getMemory());
}
