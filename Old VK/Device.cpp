#include "Device.h"
#include "SwapChain.h"

using namespace QZL;

const std::vector<const char*> Device::kDeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

Device::Device(VkPhysicalDevice& device, VkSurfaceKHR& surface)
	: physicalDevice_(device)
{
	familyIndices_.resize(static_cast<unsigned int>(QueueFamilyType::kNumQueueFamilyTypes));
	queueHandles_.resize(static_cast<unsigned int>(QueueFamilyType::kNumQueueFamilyTypes));
	for (auto& index : familyIndices_) {
		index = kInvalidIndex;
	}
	findIndices(device, surface);
}

Device::~Device()
{
	vkDestroyCommandPool(logicDevice_, commandPool_, nullptr);
	vkDestroyDevice(logicDevice_, nullptr);
}

std::vector<VkDeviceQueueCreateInfo> Device::getCreateQueueInfos(const float* queue_priority)
{
	EXPECTS(queue_priority != nullptr);
	std::vector<uint32_t> uniqueIndices = familyIndices_;
	uniqueIndices.erase(std::unique(uniqueIndices.begin(), uniqueIndices.end()), uniqueIndices.end());

	std::vector<VkDeviceQueueCreateInfo> createInfos;
	createInfos.reserve(uniqueIndices.size());

	for (const auto& index : uniqueIndices) {
		VkDeviceQueueCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = index;
		createInfo.queueCount = 1;
		createInfo.pQueuePriorities = queue_priority;
		createInfos.push_back(createInfo);
	}
	return createInfos;
}

void Device::printCapabilities()
{
	// TODO
}

bool Device::hasRequiredQueueFamilies()
{
	for (const auto& index : familyIndices_) {
		if (index == kInvalidIndex)
			return false;
	}
	return true;
}

bool Device::isSuitable(DeviceSwapChainDetails* swapChainDetails, VkSurfaceKHR surface)
{
	vkGetPhysicalDeviceProperties(physicalDevice_, &properties_);
	vkGetPhysicalDeviceFeatures(physicalDevice_, &features_);
	// Can expand this to see if features contains certain things if needed
	bool swapchain_adequate = hasSuitableSwapchain(swapChainDetails, surface);
	return properties_.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && swapchain_adequate;
}

const uint32_t Device::getFamilyIndex(QueueFamilyType type) const
{
	EXPECTS(type != QueueFamilyType::kNumQueueFamilyTypes);
	return familyIndices_[static_cast<unsigned int>(type)];
}

const std::vector<uint32_t>& Device::getAllIndices() const noexcept
{
	return familyIndices_;
}

VkDevice Device::createLogicDevice(uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames)
{
	const float queuePriority = 1.0f;
	auto createInfos = getCreateQueueInfos(&queuePriority);

	VkPhysicalDeviceFeatures deviceFeatures = {}; // TODO?

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = createInfos.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(createInfos.size());
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledLayerCount = enabledLayerCount;
	deviceCreateInfo.ppEnabledLayerNames = ppEnabledLayerNames;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = kDeviceExtensions.data();
	CHECK_VKRESULT(vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &logicDevice_));
	// Get handle to graphics device queue
	queueHandles_[static_cast<size_t>(QueueFamilyType::kGraphicsQueue)] = createQueueHandles(QueueFamilyType::kGraphicsQueue);
	queueHandles_[static_cast<size_t>(QueueFamilyType::kPresentationQueue)] = createQueueHandles(QueueFamilyType::kPresentationQueue);
	return logicDevice_;
}

VkQueue Device::getQueueHandle(QueueFamilyType type) const
{
	EXPECTS(type != QueueFamilyType::kNumQueueFamilyTypes);
	return queueHandles_[static_cast<size_t>(type)];
}

VkCommandPool Device::createCommandPool()
{

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = familyIndices_[static_cast<size_t>(QueueFamilyType::kGraphicsQueue)];

	CHECK_VKRESULT(vkCreateCommandPool(logicDevice_, &poolInfo, nullptr, &commandPool_));
	return commandPool_;
}

VkPhysicalDevice Device::getPhysicalDevice() const
{
	return physicalDevice_;
}

VkDevice Device::getLogicDevice() const
{
	return logicDevice_;
}

VkQueue Device::createQueueHandles(QueueFamilyType type)
{
	EXPECTS(type != QueueFamilyType::kNumQueueFamilyTypes);
	VkQueue queue;
	uint32_t familyIndex = familyIndices_[static_cast<size_t>(type)];
	ENSURES(familyIndex != kInvalidIndex);
	vkGetDeviceQueue(logicDevice_, familyIndex, 0, &queue);
	return queue;
}

bool Device::hasSuitableSwapchain(DeviceSwapChainDetails* swapChainDetails, VkSurfaceKHR surface)
{
	// Supports swapchain extension
	auto availableExts = Shared::Vulk::obtainVkData<VkExtensionProperties>(vkEnumerateDeviceExtensionProperties, physicalDevice_, nullptr);
	bool hasExt = false;
	for (const auto& aext : availableExts) {
		if (strcmp(aext.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
			hasExt = true;
	}
	if (hasExt) {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice_, surface, &swapChainDetails->capabilities);
		swapChainDetails->formats = Shared::Vulk::obtainVkData<VkSurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR, physicalDevice_, surface);
		swapChainDetails->presentModes = Shared::Vulk::obtainVkData<VkPresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR, physicalDevice_, surface);
		return !swapChainDetails->formats.empty() && !swapChainDetails->presentModes.empty();
	}
	else {
		return false;
	}
}

void Device::findIndices(VkPhysicalDevice& device, VkSurfaceKHR& surface)
{
	EXPECTS(device != VK_NULL_HANDLE && surface != VK_NULL_HANDLE);
	auto queueFamilies = Shared::Vulk::obtainVkData<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, device);
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		// Want a queue family that has the graphics bit
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			familyIndices_[static_cast<size_t>(QueueFamilyType::kGraphicsQueue)] = i;
		}
		// And one that can present
		VkBool32 has_present = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &has_present);
		if (queueFamily.queueCount > 0 && has_present) {
			familyIndices_[static_cast<size_t>(QueueFamilyType::kPresentationQueue)] = i;
		}

		if (hasRequiredQueueFamilies()) {
			return;
		}
		else {
			i++;
		}
	}
	throw std::runtime_error("Required queue families not present.");
}
