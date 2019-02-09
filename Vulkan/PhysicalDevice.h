#pragma once
#include "VkUtil.h"

namespace QZL
{
	struct DeviceSurfaceCapabilities;
	class LogicDevice;
	struct SystemDetails;

	enum class QueueFamilyType : size_t;

	/// Deals with Queue Families, Memory heap, and Memory types
	class PhysicalDevice {
		friend class System;
	public:
		bool isValid(DeviceSurfaceCapabilities& swapChainDetails, VkSurfaceKHR& surface);
		LogicDevice* createLogicDevice(const SystemDetails& sysDetails, DeviceSurfaceCapabilities& surfaceCapabilities,
			uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames);
		VkPhysicalDevice getPhysicalDevice() const;

	private:
		PhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface);

		bool findIndices(VkPhysicalDevice& device, VkSurfaceKHR& surface);
		bool hasRequiredQueueFamilies();
		bool hasRequiredSwapchain(DeviceSurfaceCapabilities& surfaceCapabilities, VkSurfaceKHR& surface);

		std::vector<VkDeviceQueueCreateInfo> getCreateQueueInfos(const float* queue_priority);
		VkQueue createQueueHandles(VkDevice logicDevice, QueueFamilyType type);
		VkQueue getQueueHandle(QueueFamilyType type);

		VkPhysicalDevice device_;
		VkPhysicalDeviceFeatures features_;
		VkPhysicalDeviceProperties properties_;

		// Queue families, use QueueFamilyType to index these
		std::vector<uint32_t> queueFamilyIndices_;
		std::vector<VkQueue> queueHandles_;

		static const uint32_t kInvalidIndex = std::numeric_limits<uint32_t>::max();
	};
}
