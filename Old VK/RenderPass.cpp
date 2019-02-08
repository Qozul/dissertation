#include "RenderPass.h"
#include "SwapChain.h"
#include "Image2D.h"
#include "GfxPipeline.h"

using namespace QZL;
VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

RenderPass::RenderPass(VkPhysicalDevice physicalDevice, VkDevice logicDevice, SwapChainDetails& swapChainDetails, VkCommandPool commandPool, VkQueue queue, 
	const VkDescriptorSetLayout descriptorSetLayout)
	: cLogicDevice_(logicDevice), cSwapChainDetails_(swapChainDetails)
{
	createColourImage(physicalDevice, logicDevice, swapChainDetails, commandPool, queue);
	auto depthFormat = createDepthImage(physicalDevice, logicDevice, swapChainDetails, commandPool, queue);

	// Render pass
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainDetails.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = depthFormat;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	CHECK_VKRESULT(vkCreateRenderPass(logicDevice, &renderPassInfo, nullptr, &renderPass_));

	createFramebuffers(swapChainDetails);
	gfxPipelines_.push_back(new GfxPipeline(logicDevice, renderPass_, swapChainDetails.extent, descriptorSetLayout, "test_vert", "test_frag"));
}

RenderPass::~RenderPass()
{
	SAFE_DELETE(depthImage_);
	SAFE_DELETE(colourImage_);
	for (auto framebuffer : framebuffers_) {
		vkDestroyFramebuffer(cLogicDevice_, framebuffer, nullptr);
	}
	for (GfxPipeline* gfxPipeline : gfxPipelines_) {
		SAFE_DELETE(gfxPipeline);
	}
	vkDestroyRenderPass(cLogicDevice_, renderPass_, nullptr);
}

VkRenderPass RenderPass::get()
{
	return renderPass_;
}

void RenderPass::createFramebuffers(SwapChainDetails& swapChainDetails)
{
	framebuffers_.resize(swapChainDetails.imageViews.size());

	for (size_t i = 0; i < swapChainDetails.imageViews.size(); i++) {
		std::array<VkImageView, 2> attachments = {
			swapChainDetails.imageViews[i],
			depthImage_->getImageView()
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass_;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainDetails.extent.width;
		framebufferInfo.height = swapChainDetails.extent.height;
		framebufferInfo.layers = 1;

		CHECK_VKRESULT(vkCreateFramebuffer(cLogicDevice_, &framebufferInfo, nullptr, &framebuffers_[i]));
	}
}

void RenderPass::createColourImage(VkPhysicalDevice physicalDevice, VkDevice logicDevice, SwapChainDetails& swapChainDetails, VkCommandPool commandPool, VkQueue queue)
{
	ImageParameters params = {};
	params.aspectBits = VK_IMAGE_ASPECT_COLOR_BIT;
	params.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	params.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	params.memPropertyBits = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	params.mipLevels = 1;
	VkImageCreateInfo createInfo = {};
	Image2D::fillImageCreateInfo(createInfo, swapChainDetails.extent.width, swapChainDetails.extent.height, params.mipLevels, 
		msaaSamples, swapChainDetails.format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	colourImage_ = new Image2D(physicalDevice, logicDevice, swapChainDetails, createInfo, params, commandPool, queue, swapChainDetails.format);
}

VkFormat RenderPass::createDepthImage(VkPhysicalDevice physicalDevice, VkDevice logicDevice, SwapChainDetails& swapChainDetails, VkCommandPool commandPool, VkQueue queue)
{
	ImageParameters params = {};
	params.aspectBits = VK_IMAGE_ASPECT_DEPTH_BIT;
	params.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	params.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	params.memPropertyBits = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	params.mipLevels = 1;
	VkFormat format = Shared::Vulk::findSupportedImageFormat(physicalDevice, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	VkImageCreateInfo createInfo = {};
	Image2D::fillImageCreateInfo(createInfo, swapChainDetails.extent.width, swapChainDetails.extent.height, params.mipLevels,
		msaaSamples, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	depthImage_ = new Image2D(physicalDevice, logicDevice, swapChainDetails, createInfo, params, commandPool, queue, format);
	return format;
}
