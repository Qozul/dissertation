/// Author: Ralph Ridley
/// Date: 11/10/18

#include "QueueFamilies.h"

using namespace QZL;

QZL::QueueFamilies::QueueFamilies()
{
}

QueueFamilies::QueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface)
{
	familyIndices_.resize(static_cast<unsigned int>(QueueFamilyType::kNumQueueFamilyTypes));
	for (auto& index : familyIndices_) {
		index = -1;
	}
	findIndices(device, surface);
}

bool QueueFamilies::hasAllFamilies()
{
	for (const auto& index : familyIndices_) {
		if (index == kInvalidIndex)
			return false;
	}
	return true;
}

std::vector<VkDeviceQueueCreateInfo> QZL::QueueFamilies::getCreateInfos(const float* queue_priority)
{
	EXPECTSM(queue_priority != nullptr, "Queue priority is nullptr");
	std::vector<uint32_t> uniqueIndices = familyIndices_;
	uniqueIndices.erase(std::unique(uniqueIndices.begin(), uniqueIndices.end()), uniqueIndices.end());

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.reserve(uniqueIndices.size());

	for (const auto& index : uniqueIndices) {
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = index;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = queue_priority;
		queueCreateInfos.push_back(queue_create_info);
	}
	return queueCreateInfos;
}

VkQueue QueueFamilies::getQueueHandle(QueueFamilyType type, VkDevice& logic_device, uint32_t index)
{
	EXPECTSM(type != QueueFamilyType::kNumQueueFamilyTypes, "Do not index with kNumQueueFamilyTypes");
	VkQueue queue;
	uint32_t familyIndex = familyIndices_[static_cast<unsigned int>(type)];
	ENSURESM(familyIndex != kInvalidIndex, "Invalid index for QueueFamilyType: " + std::to_string(int(type)));
	vkGetDeviceQueue(logic_device, familyIndex, 0, &queue);
	return queue;
}

const uint32_t QueueFamilies::getFamilyIndex(QueueFamilyType type) const
{
	EXPECTSM(type != QueueFamilyType::kNumQueueFamilyTypes, "Do not index with kNumQueueFamilyTypes");
	return familyIndices_[static_cast<unsigned int>(type)];
}

const std::vector<uint32_t>& QueueFamilies::getAllIndices() const noexcept
{
	return familyIndices_;
}

void QueueFamilies::findIndices(VkPhysicalDevice& device, VkSurfaceKHR& surface)
{
	EXPECTSM(device != VK_NULL_HANDLE && surface != VK_NULL_HANDLE, "");
	auto queueFamilies = Shared::Vulk::obtainVkData<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, device);
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		// Want a queue family that has the graphics bit
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			familyIndices_[static_cast<unsigned int>(QueueFamilyType::kGraphicsQueue)] = i;
		}
		// And one with a present
		VkBool32 has_present = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &has_present);
		if (queueFamily.queueCount > 0 && has_present) {
			familyIndices_[static_cast<unsigned int>(QueueFamilyType::kPresentationQueue)] = i;
		}

		if (hasAllFamilies()) {
			return;
		}
		else {
			i++;
		}
	}
	throw std::runtime_error("Required queue families not present.");
}
