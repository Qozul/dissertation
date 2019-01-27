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
		bool hasAllFamilies();
		std::vector<VkDeviceQueueCreateInfo> getCreateInfos(const float* queuePriority);
		VkQueue getQueueHandle(QueueFamilyType type, VkDevice& logicDevice, uint32_t index);
		const uint32_t getFamilyIndex(QueueFamilyType type) const;
		const std::vector<uint32_t>& getAllIndices() const noexcept;

	private:
		void findIndices(VkPhysicalDevice& device, VkSurfaceKHR& surface);

		// Use QueueFamilyType to index in to the appropriate family
		std::vector<uint32_t> familyIndices_;
		bool isSuitable;

		static const uint32_t kInvalidIndex = std::numeric_limits<uint32_t>::max();
	};
}
