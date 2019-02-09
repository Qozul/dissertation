#include "LogicDevice.h"
#include "SwapChain.h"
#include "vk_mem_alloc.h"
#include "System.h"
#include "PhysicalDevice.h"

using namespace QZL;

LogicDevice::LogicDevice(VkDevice device, const SystemDetails& sysDetails, DeviceSurfaceCapabilities& surfaceCapabilities,
	std::vector<uint32_t> indices, std::vector<VkQueue> handles)
	: device_(device), queueFamilyIndices_(indices), queueHandles_(handles)
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices_[static_cast<size_t>(QueueFamilyType::kGraphicsQueue)];

	CHECK_VKRESULT(vkCreateCommandPool(device_, &poolInfo, nullptr, &primaryCommandPool_));

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = sysDetails.physicalDevice->getPhysicalDevice();
	allocatorInfo.device = device;

	vmaCreateAllocator(&allocatorInfo, allocator_);

	swapChain_ = new SwapChain(sysDetails.window, sysDetails.surface, this, surfaceCapabilities);
}

LogicDevice::~LogicDevice()
{
	vkDeviceWaitIdle(device_);
	SAFE_DELETE(swapChain_);
	vkDestroyCommandPool(device_, primaryCommandPool_, nullptr);
	vkDestroyDevice(device_, nullptr);
}

VkDevice LogicDevice::getLogicDevice() const
{
	return device_;
}

const uint32_t LogicDevice::getFamilyIndex(QueueFamilyType type) const
{
	EXPECTS(type != QueueFamilyType::kNumQueueFamilyTypes);
	return queueFamilyIndices_[static_cast<unsigned int>(type)];
}

const std::vector<uint32_t>& LogicDevice::getAllIndices() const noexcept
{
	return queueFamilyIndices_;
}

VkQueue LogicDevice::getQueueHandle(QueueFamilyType type)
{
	EXPECTS(type != QueueFamilyType::kNumQueueFamilyTypes);
	return queueHandles_[static_cast<size_t>(type)];
}
