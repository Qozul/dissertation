#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	struct DeviceSwapChainDetails;

	enum class QueueFamilyType : size_t {
		kGraphicsQueue = 0,
		kPresentationQueue,
		kNumQueueFamilyTypes // Do not index with this, this is the size
	};
	class Device {
	public:
		Device(VkPhysicalDevice& device, VkSurfaceKHR& surface);
		~Device();
		std::vector<VkDeviceQueueCreateInfo> getCreateQueueInfos(const float* queue_priority);
		void printCapabilities();
		bool hasRequiredQueueFamilies();
		bool isSuitable(DeviceSwapChainDetails* swapChainDetails, VkSurfaceKHR surface);
		const uint32_t getFamilyIndex(QueueFamilyType type) const; // needed in swap chain
		const std::vector<uint32_t>& getAllIndices() const noexcept; // needed in swap chain
		VkDevice createLogicDevice(uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames);
		VkQueue getQueueHandle(QueueFamilyType type) const;
		VkCommandPool createCommandPool();

		VkPhysicalDevice getPhysicalDevice() const;
		VkDevice getLogicDevice() const;

	private:
		void findIndices(VkPhysicalDevice& device, VkSurfaceKHR& surface);
		VkQueue createQueueHandles(QueueFamilyType type);
		bool hasSuitableSwapchain(DeviceSwapChainDetails* swapChainDetails, VkSurfaceKHR surface);

		VkPhysicalDevice physicalDevice_;
		VkPhysicalDeviceFeatures features_;
		VkPhysicalDeviceProperties properties_;
		VkDevice logicDevice_;
		VkCommandPool commandPool_;
		std::vector<uint32_t> familyIndices_; // Use QueueFamilyType to index in to the appropriate family
		std::vector<VkQueue> queueHandles_;

		static const uint32_t kInvalidIndex = std::numeric_limits<uint32_t>::max();
		static const std::vector<const char*> kDeviceExtensions;
	};
}
