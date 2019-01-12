/// Author: Ralph Ridley
/// Date: 11/10/18

#include "QueueFamilies.h"

using namespace QZL;

QZL::QueueFamilies::QueueFamilies()
{
}

QueueFamilies::QueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface)
{
	family_indices_.resize(static_cast<unsigned int>(QueueFamilyType::kNumQueueFamilyTypes));
	for (auto& index : family_indices_) {
		index = -1;
	}
	find_indices(device, surface);
}

bool QueueFamilies::has_all_families()
{
	for (const auto& index : family_indices_) {
		if (index == kInvalidIndex)
			return false;
	}
	return true;
}

std::vector<VkDeviceQueueCreateInfo> QZL::QueueFamilies::get_create_infos(const float* queue_priority)
{
	EXPECTSM(queue_priority != nullptr, "Queue priority is nullptr");
	std::vector<uint32_t> unique_indices = family_indices_;
	unique_indices.erase(std::unique(unique_indices.begin(), unique_indices.end()), unique_indices.end());

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	queue_create_infos.reserve(unique_indices.size());

	for (const auto& index : unique_indices) {
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = index;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = queue_priority;
		queue_create_infos.push_back(queue_create_info);
	}
	return queue_create_infos;
}

VkQueue QueueFamilies::get_queue_handle(QueueFamilyType type, VkDevice& logic_device, uint32_t index)
{
	EXPECTSM(type != QueueFamilyType::kNumQueueFamilyTypes, "Do not index with kNumQueueFamilyTypes");
	VkQueue queue;
	uint32_t family_index = family_indices_[static_cast<unsigned int>(type)];
	ENSURESM(family_index != kInvalidIndex, "Invalid index for QueueFamilyType: " + std::to_string(int(type)));
	vkGetDeviceQueue(logic_device, family_index, 0, &queue);
	return queue;
}

const uint32_t QueueFamilies::get_family_index(QueueFamilyType type) const
{
	EXPECTSM(type != QueueFamilyType::kNumQueueFamilyTypes, "Do not index with kNumQueueFamilyTypes");
	return family_indices_[static_cast<unsigned int>(type)];
}

const std::vector<uint32_t>& QueueFamilies::get_all_indices() const noexcept
{
	return family_indices_;
}

void QueueFamilies::find_indices(VkPhysicalDevice& device, VkSurfaceKHR& surface)
{
	EXPECTSM(device != VK_NULL_HANDLE && surface != VK_NULL_HANDLE, "");
	auto queue_families = Shared::Vulk::obtainVkData<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, device);
	int i = 0;
	for (const auto& queueFamily : queue_families) {
		// Want a queue family that has the graphics bit
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			family_indices_[static_cast<unsigned int>(QueueFamilyType::kGraphicsQueue)] = i;
		}
		// And one with a present
		VkBool32 has_present = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &has_present);
		if (queueFamily.queueCount > 0 && has_present) {
			family_indices_[static_cast<unsigned int>(QueueFamilyType::kPresentationQueue)] = i;
		}

		if (has_all_families()) {
			return;
		}
		else {
			i++;
		}
	}
	throw std::runtime_error("Required queue families not present.");
}
