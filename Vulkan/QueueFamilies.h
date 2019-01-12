/// Author: Ralph Ridley
/// Date: 11/10/18

#pragma once

#include "../Shared/Utility.h"

namespace QZL
{
	enum class QueueFamilyType : unsigned int {
		kGraphicsQueue = 0,
		kPresentationQueue,
		kNumQueueFamilyTypes // Do not index with this, this is the size
	};
	/// This class encapsulates queue families for a given device and surface
	class QueueFamilies {
	public:
		QueueFamilies();
		QueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface);
		bool has_all_families();
		std::vector<VkDeviceQueueCreateInfo> get_create_infos(const float* queue_priority);
		VkQueue get_queue_handle(QueueFamilyType type, VkDevice& logic_device, uint32_t index);
		const uint32_t get_family_index(QueueFamilyType type) const;
		const std::vector<uint32_t>& get_all_indices() const noexcept;

	private:
		void find_indices(VkPhysicalDevice& device, VkSurfaceKHR& surface);

		// Use QueueFamilyType to index in to the appropriate family
		std::vector<uint32_t> family_indices_;
		bool is_suitable;

		static const uint32_t kInvalidIndex = std::numeric_limits<uint32_t>::max();
	};
}
